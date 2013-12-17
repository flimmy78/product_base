/*
 * Copyright (c) 2011 Patrick McHardy <kaber@trash.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Based on Rusty Russell's IPv4 NAT code. Development of IPv6 NAT
 * funded by Astaro.
 */

#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv6.h>
#include <linux/netfilter_ipv6/ip6_tables.h>
#include <linux/ipv6.h>
#include <net/ipv6.h>

#include <net/netfilter/nf_nat.h>
#include <net/netfilter/nf_nat_core.h>
#include <net/netfilter/nf_nat_l3proto.h>

/*huangjing##add for debug*/
int ip6table_nat_debug = 0;
module_param(ip6table_nat_debug,int,0644);

/*huangjing##add*/
/* Set up the info structure to map into this range. */
extern unsigned int nf_nat_setup_info_ipv6(struct nf_conn *ct,
				      const struct nf_nat_range *range,
				      enum nf_nat_manip_type maniptype);

static const struct xt_table nf_nat_ipv6_table = {
	.name		= "nat",
	.valid_hooks	= (1 << NF_INET_PRE_ROUTING) |
			  (1 << NF_INET_POST_ROUTING) |
			  (1 << NF_INET_LOCAL_OUT) |
			  (1 << NF_INET_LOCAL_IN),
	.me		= THIS_MODULE,
	.af		= NFPROTO_IPV6,
};

/*huangjing##add*/
#define NAT_VALID_HOOKS ((1 << NF_INET_PRE_ROUTING) | \
			    (1 << NF_INET_POST_ROUTING) | \
			    (1 << NF_INET_LOCAL_OUT) | \
			    (1 << NF_INET_LOCAL_IN))

static struct
{
	struct ip6t_replace repl;
	struct ip6t_standard entries[4];
	struct ip6t_error term;
} initial_table __net_initdata = {
	.repl = {
		.name = "nat",
		.valid_hooks = NAT_VALID_HOOKS,
		.num_entries = 5,
		.size = sizeof(struct ip6t_standard) * 4 + sizeof(struct ip6t_error),
		.hook_entry = {
			[NF_INET_PRE_ROUTING] = 0,
			[NF_INET_POST_ROUTING] = sizeof(struct ip6t_standard),
			[NF_INET_LOCAL_OUT] = sizeof(struct ip6t_standard) * 2,
			[NF_INET_LOCAL_IN] = sizeof(struct ip6t_standard) * 3,
		},
		.underflow = {
			[NF_INET_PRE_ROUTING] = 0,
			[NF_INET_POST_ROUTING] = sizeof(struct ip6t_standard),
			[NF_INET_LOCAL_OUT] = sizeof(struct ip6t_standard) * 2,
			[NF_INET_LOCAL_IN] = sizeof(struct ip6t_standard) * 3,
		},
	},
	.entries = {
		IP6T_STANDARD_INIT(NF_ACCEPT),	/* PRE_ROUTING */
		IP6T_STANDARD_INIT(NF_ACCEPT),	/* POST_ROUTING */
		IP6T_STANDARD_INIT(NF_ACCEPT),	/* LOCAL_OUT */
		IP6T_STANDARD_INIT(NF_ACCEPT),  /*LOCAL_IN*/
	},
	.term = IP6T_ERROR_INIT,		/* ERROR */
};

 /**
  * Display a buffer in hex
  *
  * @param buffer	  Data buffer to display
  * @param buffer_len Length of the buffer
  */
 static void hex_dump(const void *buffer, int buffer_len)
 {
	 const uint8_t *ptr = (const uint8_t*)buffer;
	 int i;
	 printk(KERN_DEBUG"SKB BUFFER:==========================\n");
	 for (i=0;i<buffer_len;i++) 
	 	{
//		 if (i % 16 == 0) printk("\n%04x: ", i);
		 if (i % 16 == 0) printk("\n");
		   printk("%02X ", (unsigned int)*ptr);
		   ptr++;
		}
	 printk(KERN_DEBUG"\n");
	 printk(KERN_DEBUG"SKB BUFFER:==========================\n");
 }

/*huangjing##add end*/

static unsigned int alloc_null_binding(struct nf_conn *ct, unsigned int hooknum)
{
	/* Force range to this IP; let proto decide mapping for
	 * per-proto parts (hence not IP_NAT_RANGE_PROTO_SPECIFIED).
	 */
	struct nf_nat_ipv6_range range;

	range.flags = 0;

	pr_debug("Allocating NULL binding for %p (%pI6)\n", ct,
		 HOOK2MANIP(hooknum) == IP_NAT_MANIP_SRC ?
		 &ct->tuplehash[IP_CT_DIR_REPLY].tuple.dst.u3.ip6 :
		 &ct->tuplehash[IP_CT_DIR_REPLY].tuple.src.u3.ip6);

	return nf_nat_setup_info_ipv6(ct, &range, HOOK2MANIP(hooknum));
}

static unsigned int ipv6_nf_nat_rule_find(struct sk_buff *skb, unsigned int hooknum,
				     const struct net_device *in,
				     const struct net_device *out,
				     struct nf_conn *ct)
{
	struct net *net = nf_ct_net(ct);
	unsigned int ret;

	ret = ip6t_do_table(skb, hooknum, in, out, net->ipv6.ip6table_nat);
	if (ret == NF_ACCEPT) {
		if (!nf_nat_initialized(ct, HOOK2MANIP(hooknum)))
			ret = alloc_null_binding(ct, hooknum);
	}

	return ret;
}

static unsigned int
nf_nat_ipv6_fn(unsigned int hooknum,
	       struct sk_buff *skb,
	       const struct net_device *in,
	       const struct net_device *out,
	       int (*okfn)(struct sk_buff *))
{
	struct nf_conn *ct;
	enum ip_conntrack_info ctinfo;
	struct nf_conn_nat *nat;
	enum nf_nat_manip_type maniptype = HOOK2MANIP(hooknum);
/*huangjing##
    __be16 frag_off;
*/
	int hdrlen;
	u8 nexthdr;

	ct = nf_ct_get(skb, &ctinfo);

	/* Can't track?  It's not due to stress, or conntrack would
	 * have dropped it.  Hence it's the user's responsibilty to
	 * packet filter it out, or implement conntrack/NAT for that
	 * protocol. 8) --RR
	 */
	if (!ct)
		return NF_ACCEPT;

	/* Don't try to NAT if this packet is not conntracked */
/*
    if (nf_ct_is_untracked(ct))
		return NF_ACCEPT;
huangjing##2013-9-23
*/
	if (nf_ct_is_untracked(skb))
		return NF_ACCEPT;

	nat = nfct_nat(ct);
	if (!nat) {
		/* NAT module was loaded late. */
		if (nf_ct_is_confirmed(ct))
			return NF_ACCEPT;
		nat = nf_ct_ext_add(ct, NF_CT_EXT_NAT, GFP_ATOMIC);
		if (nat == NULL) {
			pr_debug("failed to add NAT extension\n");
			return NF_ACCEPT;
		}
	}
	/*huangjing##add for debug*/
    if(ip6table_nat_debug)
    {
		printk(KERN_DEBUG"Function:nf_nat_ipv6_fn;ctinfo=%d\n",ctinfo);
    }
	
	switch (ctinfo) {
	case IP_CT_RELATED:
	case IP_CT_RELATED_REPLY:
		nexthdr = ipv6_hdr(skb)->nexthdr;
/*huangjing##
		hdrlen = ipv6_skip_exthdr(skb, sizeof(struct ipv6hdr),
					  &nexthdr, &frag_off);
*/
		hdrlen = ipv6_skip_exthdr(skb, sizeof(struct ipv6hdr),
					  &nexthdr);
/*huangjing##
		if (hdrlen >= 0 && nexthdr == IPPROTO_ICMPV6) {
			if (!nf_nat_icmpv6_reply_translation(skb, ct, ctinfo,
							     hooknum, hdrlen))
				return NF_DROP;
			else

				return NF_ACCEPT;
		}
*/
        return NF_ACCEPT;
		/* Fall thru... (Only ICMPs can be IP_CT_IS_REPLY) */
	case IP_CT_NEW:
		/* Seen it before?  This can happen for loopback, retrans,
		 * or local packets.
		 */
		if (!nf_nat_initialized(ct, maniptype)) {
			unsigned int ret;

			ret = ipv6_nf_nat_rule_find(skb, hooknum, in, out, ct);
			if (ret != NF_ACCEPT)
				return ret;
		} else {
			pr_debug("Already setup manip %s for ct %p\n",
				 maniptype == IP_NAT_MANIP_SRC ? "SRC" : "DST",
				 ct);
			if (nf_nat_oif_changed(hooknum, ctinfo, nat, out))
				goto oif_changed;
		}
		break;

	default:
		/* ESTABLISHED */
		NF_CT_ASSERT(ctinfo == IP_CT_ESTABLISHED ||
			     ctinfo == IP_CT_ESTABLISHED_REPLY);
		if (nf_nat_oif_changed(hooknum, ctinfo, nat, out))
			goto oif_changed;
	}
   
	return nf_nat_packet(ct, ctinfo, hooknum, skb);

oif_changed:
	nf_ct_kill_acct(ct, ctinfo, skb);
	return NF_DROP;
}

static unsigned int
nf_nat_ipv6_in(unsigned int hooknum,
	       struct sk_buff *skb,
	       const struct net_device *in,
	       const struct net_device *out,
	       int (*okfn)(struct sk_buff *))
{
	unsigned int ret;
	struct in6_addr daddr = ipv6_hdr(skb)->daddr;
	
    if(ip6table_nat_debug)
    {
		printk(KERN_DEBUG"Function:nf_nat_ipv6_in;hooknum=%d\n",hooknum);
        hex_dump(skb->data, skb->len);
    }
	
	ret = nf_nat_ipv6_fn(hooknum, skb, in, out, okfn);
	if (ret != NF_DROP && ret != NF_STOLEN &&
	    ipv6_addr_cmp(&daddr, &ipv6_hdr(skb)->daddr))
		skb_dst_drop(skb);

	return ret;
}

static unsigned int
nf_nat_ipv6_out(unsigned int hooknum,
		struct sk_buff *skb,
		const struct net_device *in,
		const struct net_device *out,
		int (*okfn)(struct sk_buff *))
{
#ifdef CONFIG_XFRM
    #if 0
	const struct nf_conn *ct;
	enum ip_conntrack_info ctinfo;
	int err;
	#endif
#endif
	unsigned int ret;
    //hex_dump(skb->data, skb->len);
	/* root is playing with raw sockets. */
	if (skb->len < sizeof(struct ipv6hdr))
		return NF_ACCEPT;
    
	ret = nf_nat_ipv6_fn(hooknum, skb, in, out, okfn);
#ifdef CONFIG_XFRM
    /*huangjing##*/
    #if 0
	if (ret != NF_DROP && ret != NF_STOLEN &&
	    !(IP6CB(skb)->flags & IP6SKB_XFRM_TRANSFORMED) &&
	    (ct = nf_ct_get(skb, &ctinfo)) != NULL) {
		enum ip_conntrack_dir dir = CTINFO2DIR(ctinfo);

		if (!nf_inet_addr_cmp(&ct->tuplehash[dir].tuple.src.u3,
				      &ct->tuplehash[!dir].tuple.dst.u3) ||
		    (ct->tuplehash[dir].tuple.dst.protonum != IPPROTO_ICMPV6 &&
		     ct->tuplehash[dir].tuple.src.u.all !=
		     ct->tuplehash[!dir].tuple.dst.u.all)) {
			err = nf_xfrm_me_harder(skb, AF_INET6);
			if (err < 0)
				ret = NF_DROP_ERR(err);
		}
	}
	#endif
#endif
	return ret;
}

#define NF_DROP_ERR(x) (((-x) << 16) | NF_DROP)

static unsigned int
nf_nat_ipv6_local_fn(unsigned int hooknum,
		     struct sk_buff *skb,
		     const struct net_device *in,
		     const struct net_device *out,
		     int (*okfn)(struct sk_buff *))
{
	const struct nf_conn *ct;
	enum ip_conntrack_info ctinfo;
	unsigned int ret;
	int err;

	/* root is playing with raw sockets. */
	if (skb->len < sizeof(struct ipv6hdr))
		return NF_ACCEPT;

	ret = nf_nat_ipv6_fn(hooknum, skb, in, out, okfn);
	if (ret != NF_DROP && ret != NF_STOLEN &&
	    (ct = nf_ct_get(skb, &ctinfo)) != NULL) {
		enum ip_conntrack_dir dir = CTINFO2DIR(ctinfo);

		if (!nf_inet_addr_cmp(&ct->tuplehash[dir].tuple.dst.u3,
				      &ct->tuplehash[!dir].tuple.src.u3)) {
			err = ip6_route_me_harder(skb);
			if (err < 0)
				ret = NF_DROP_ERR(err);
		}
#ifdef CONFIG_XFRM
    /*huangjing##*/
    #if 0
		else if (!(IP6CB(skb)->flags & IP6SKB_XFRM_TRANSFORMED) &&
			 ct->tuplehash[dir].tuple.dst.protonum != IPPROTO_ICMPV6 &&
			 ct->tuplehash[dir].tuple.dst.u.all !=
			 ct->tuplehash[!dir].tuple.src.u.all) {
			err = nf_xfrm_me_harder(skb, AF_INET6);
			if (err < 0)
				ret = NF_DROP_ERR(err);
		}
	#endif
#endif
	}

	return ret;
}

static struct nf_hook_ops nf_nat_ipv6_ops[] __read_mostly = {
	/* Before packet filtering, change destination */
	{
		.hook		= nf_nat_ipv6_in,
		.owner		= THIS_MODULE,
		.pf		= NFPROTO_IPV6,
		.hooknum	= NF_INET_PRE_ROUTING,
		.priority	= NF_IP6_PRI_NAT_DST,
	},
	/* After packet filtering, change source */
	{
		.hook		= nf_nat_ipv6_out,
		.owner		= THIS_MODULE,
		.pf		= NFPROTO_IPV6,
		.hooknum	= NF_INET_POST_ROUTING,
		.priority	= NF_IP6_PRI_NAT_SRC,
	},
	/* Before packet filtering, change destination */
	{
		.hook		= nf_nat_ipv6_local_fn,
		.owner		= THIS_MODULE,
		.pf		= NFPROTO_IPV6,
		.hooknum	= NF_INET_LOCAL_OUT,
		.priority	= NF_IP6_PRI_NAT_DST,
	},
	/* After packet filtering, change source */
	{
		.hook		= nf_nat_ipv6_fn,
		.owner		= THIS_MODULE,
		.pf		= NFPROTO_IPV6,
		.hooknum	= NF_INET_LOCAL_IN,
		.priority	= NF_IP6_PRI_NAT_SRC,
	},
};

static int __net_init ip6table_nat_net_init(struct net *net)
{
	/* Register table */
	net->ipv6.ip6table_nat =
		ip6t_register_table(net, &nf_nat_ipv6_table, &initial_table.repl);
	if (IS_ERR(net->ipv6.ip6table_nat))
		return PTR_ERR(net->ipv6.ip6table_nat);
	return 0;

	/*huangjing##
	struct ip6t_replace *repl;

	repl = ip6t_alloc_initial_table(&nf_nat_ipv6_table);
	if (repl == NULL)
		return -ENOMEM;
	net->ipv6.ip6table_nat = ip6t_register_table(net, &nf_nat_ipv6_table, repl);
	kfree(repl);
	return PTR_RET(net->ipv6.ip6table_nat);
	*/
}

static void __net_exit ip6table_nat_net_exit(struct net *net)
{
	ip6t_unregister_table(net->ipv6.ip6table_nat);
	/*huangjing##delete
	ip6t_unregister_table(net, net->ipv6.ip6table_nat);
	*/
}

static struct pernet_operations ip6table_nat_net_ops = {
	.init	= ip6table_nat_net_init,
	.exit	= ip6table_nat_net_exit,
};

static int __init ip6table_nat_init(void)
{
	int err;
	err = register_pernet_subsys(&ip6table_nat_net_ops);
	if (err < 0)
		goto err1;

	err = nf_register_hooks(nf_nat_ipv6_ops, ARRAY_SIZE(nf_nat_ipv6_ops));
	if (err < 0)
		goto err2;
	return 0;

err2:
	unregister_pernet_subsys(&ip6table_nat_net_ops);
err1:
	return err;
}

static void __exit ip6table_nat_exit(void)
{
	nf_unregister_hooks(nf_nat_ipv6_ops, ARRAY_SIZE(nf_nat_ipv6_ops));
	unregister_pernet_subsys(&ip6table_nat_net_ops);
}

module_init(ip6table_nat_init);
module_exit(ip6table_nat_exit);

MODULE_LICENSE("GPL");
