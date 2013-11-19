/*
 *
 *	Generic internet FLOW.
 *
 */

#ifndef _NET_FLOW_H
#define _NET_FLOW_H

#include <linux/in6.h>
#include <asm/atomic.h>

struct flowi {
	int	oif;
	int	iif;
	__u32	mark;

	union {
		struct {
			__be32			daddr;
			__be32			saddr;
			__u8			tos;
			__u8			scope;
		} ip4_u;
		
		struct {
			struct in6_addr		daddr;
			struct in6_addr		saddr;
			__be32			flowlabel;
		} ip6_u;

		struct {
			__le16			daddr;
			__le16			saddr;
			__u8			scope;
		} dn_u;
	} nl_u;
#define fld_dst		nl_u.dn_u.daddr
#define fld_src		nl_u.dn_u.saddr
#define fld_scope	nl_u.dn_u.scope
#define fl6_dst		nl_u.ip6_u.daddr
#define fl6_src		nl_u.ip6_u.saddr
#define fl6_flowlabel	nl_u.ip6_u.flowlabel
#define fl4_dst		nl_u.ip4_u.daddr
#define fl4_src		nl_u.ip4_u.saddr
#define fl4_tos		nl_u.ip4_u.tos
#define fl4_scope	nl_u.ip4_u.scope

	__u8	proto;
	__u8	flags;
#define FLOWI_FLAG_ANYSRC 0x01
	union {
		struct {
			__be16	sport;
			__be16	dport;
		} ports;

		struct {
			__u8	type;
			__u8	code;
		} icmpt;

		struct {
			__le16	sport;
			__le16	dport;
		} dnports;

		__be32		spi;

		struct {
			__u8	type;
		} mht;
	} uli_u;
#define fl_ip_sport	uli_u.ports.sport
#define fl_ip_dport	uli_u.ports.dport
#define fl_icmp_type	uli_u.icmpt.type
#define fl_icmp_code	uli_u.icmpt.code
#define fl_ipsec_spi	uli_u.spi
#define fl_mh_type	uli_u.mht.type
	__u32           secid;	/* used by xfrm; see secid.txt */
} __attribute__((__aligned__(BITS_PER_LONG/8)));

/*huangjing##add*/
struct flowi_common {
	int	flowic_oif;
	int	flowic_iif;
	__u32	flowic_mark;
	__u8	flowic_tos;
	__u8	flowic_scope;
	__u8	flowic_proto;
	__u8	flowic_flags;
#define FLOWI_FLAG_ANYSRC		0x01
#define FLOWI_FLAG_CAN_SLEEP		0x02
#define FLOWI_FLAG_KNOWN_NH		0x04
	__u32	flowic_secid;
};

union flowi_uli {
	struct {
		__be16	dport;
		__be16	sport;
	} ports;

	struct {
		__u8	type;
		__u8	code;
	} icmpt;

	struct {
		__le16	dport;
		__le16	sport;
	} dnports;

	__be32		spi;
	__be32		gre_key;

	struct {
		__u8	type;
	} mht;
};


struct flowi4 {
	struct flowi_common	__fl_common;
#define flowi4_oif		__fl_common.flowic_oif
#define flowi4_iif		__fl_common.flowic_iif
#define flowi4_mark		__fl_common.flowic_mark
#define flowi4_tos		__fl_common.flowic_tos
#define flowi4_scope		__fl_common.flowic_scope
#define flowi4_proto		__fl_common.flowic_proto
#define flowi4_flags		__fl_common.flowic_flags
#define flowi4_secid		__fl_common.flowic_secid

	/* (saddr,daddr) must be grouped, same order as in IP header */
	__be32			saddr;
	__be32			daddr;

	union flowi_uli		uli;
#define fl4_sport		uli.ports.sport
#define fl4_dport		uli.ports.dport
#define fl4_icmp_type		uli.icmpt.type
#define fl4_icmp_code		uli.icmpt.code
#define fl4_ipsec_spi		uli.spi
#define fl4_mh_type		uli.mht.type
#define fl4_gre_key		uli.gre_key
} __attribute__((__aligned__(BITS_PER_LONG/8)));

struct flowi6 {
	struct flowi_common	__fl_common;
#define flowi6_oif		__fl_common.flowic_oif
#define flowi6_iif		__fl_common.flowic_iif
#define flowi6_mark		__fl_common.flowic_mark
#define flowi6_tos		__fl_common.flowic_tos
#define flowi6_scope		__fl_common.flowic_scope
#define flowi6_proto		__fl_common.flowic_proto
#define flowi6_flags		__fl_common.flowic_flags
#define flowi6_secid		__fl_common.flowic_secid
	struct in6_addr		daddr;
	struct in6_addr		saddr;
	__be32			flowlabel;
	union flowi_uli		uli;
#define fl6_sport		uli.ports.sport
#define fl6_dport		uli.ports.dport
#define fl6_icmp_type		uli.icmpt.type
#define fl6_icmp_code		uli.icmpt.code
#define fl6_ipsec_spi		uli.spi
#define fl6_mh_type		uli.mht.type
#define fl6_gre_key		uli.gre_key
} __attribute__((__aligned__(BITS_PER_LONG/8)));


struct flowidn {
	struct flowi_common	__fl_common;
#define flowidn_oif		__fl_common.flowic_oif
#define flowidn_iif		__fl_common.flowic_iif
#define flowidn_mark		__fl_common.flowic_mark
#define flowidn_scope		__fl_common.flowic_scope
#define flowidn_proto		__fl_common.flowic_proto
#define flowidn_flags		__fl_common.flowic_flags
	__le16			daddr;
	__le16			saddr;
	union flowi_uli		uli;
#define fld_sport		uli.ports.sport
#define fld_dport		uli.ports.dport
} __attribute__((__aligned__(BITS_PER_LONG/8)));
/*huangjing## change struct flowi to struct flowi_change*/
struct flowi_change {
	union {
		struct flowi_common	__fl_common;
		struct flowi4		ip4;
		struct flowi6		ip6;
		struct flowidn		dn;
	} u;
#define flowi_oif	u.__fl_common.flowic_oif
#define flowi_iif	u.__fl_common.flowic_iif
#define flowi_mark	u.__fl_common.flowic_mark
#define flowi_tos	u.__fl_common.flowic_tos
#define flowi_scope	u.__fl_common.flowic_scope
#define flowi_proto	u.__fl_common.flowic_proto
#define flowi_flags	u.__fl_common.flowic_flags
#define flowi_secid	u.__fl_common.flowic_secid
} __attribute__((__aligned__(BITS_PER_LONG/8)));
/*huangjing## add end*/


#define FLOW_DIR_IN	0
#define FLOW_DIR_OUT	1
#define FLOW_DIR_FWD	2

struct net;
struct sock;
typedef int (*flow_resolve_t)(struct net *net, struct flowi *key, u16 family,
			      u8 dir, void **objp, atomic_t **obj_refp);

extern void *flow_cache_lookup(struct net *net, struct flowi *key, u16 family,
			       u8 dir, flow_resolve_t resolver);
extern void flow_cache_flush(void);
extern atomic_t flow_cache_genid;

static inline int flow_cache_uli_match(struct flowi *fl1, struct flowi *fl2)
{
	return (fl1->proto == fl2->proto &&
		!memcmp(&fl1->uli_u, &fl2->uli_u, sizeof(fl1->uli_u)));
}

#endif
