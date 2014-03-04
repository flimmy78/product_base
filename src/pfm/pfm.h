#ifndef __PFM_H__
#define __PFM_H__



//#define MAX_PROTOCOL				((sizeof(protocol_type))/(sizeof(int)))
#define MAX_PROTOCOL				2
#define PFM_ENTRY_STRUCT			1

#define MAX_PORT					0xffff
#define TREE_ELEM_LEN				(sizeof(struct tree_elem))
#define DEST_IP_TREE_LEN			(sizeof(struct dest_ip_tree))
#define IFINDEX_DEST_IP_TREE_LEN	(sizeof(struct ifindex_dest_ip_tree))
#define NEW_PFM_STRUCT 				1
#define HAVE_IPV6_ADDR				0

///////////////////////forward option////////////////
#define FOWD_OPT_INIT					-1
#define FOWD_OPT_FOWD_ALL_IP			-2
//////////////////////////////////////////////////////	

///////////////////////hook_of_forward_opt///////////
#define NOTHING 						-1

////////////////////////////////////////////////////

///////////////////////netlink_transport_info_opt///////////
#define ADD 						0
#define DEL							1
#define SET_LOG_OPT					2
#define SET_PFM_DISABLE				3
#define SET_PFM_ENABLE				4
#define SET_PFM_PID					8


////////////////////////////////////////////////////
#define RECORD_MAX 64
#if 1
#define GET_LIFT_CHILD_IFINDEX(root) 		((ifindex_dest_ip_tree*)((root) -> tree -> lc))
#define GET_RIGHT_CHILD_IFINDEX(root)		((ifindex_dest_ip_tree*)((root) -> tree -> rc))
#define GET_PARENT_IFINDEX(root)			((ifindex_dest_ip_tree*)((root) -> tree -> parent))


#define GET_LIFT_CHILD_DEST_IP(root) 		((dest_ip_tree*)((root) -> tree -> lc))
#define GET_RIGHT_CHILD_DEST_IP(root)		((dest_ip_tree*)((root) -> tree -> rc))
#define GET_PARENT_DEST_IP(root)			((dest_ip_tree*)((root) -> tree -> parent))

#endif

typedef unsigned short _u16;
typedef unsigned int _u32;


enum protocol_type
{
	tcp_type = 0,
	udp_type = 1,
}protocol_type;



///////////////////////tree_elem//////////////////////
//
//struct tree_elem *rc    right child of tree
//struct tree_elem *lc    lift child of tree
//int		 deep;		  deep of tree(used for balanced tree)
///////////////////////////////////////////////////////
struct tree_elem
{
	struct	tree_elem *rc;
	struct	tree_elem *lc;
	struct	tree_elem *parent;
	int				deep;
};



//////////////////////struct dest_ip_tree////////////////
//
//struct tree_elem tree    element of tree			 
//_u32	 dest_ip					 if dest_ip == packet.dest_ip packet will forward
//												 to forward_slot
//int		 forward_slot			 same as above
//////////////////////////////////////////////////////////
struct dest_ip_tree
{
	struct tree_elem	 *tree;
	_u32				dest_ip;
	int				forward_slot;
};




///////////////struct ifindex_dest_ip_tree////////////////
//
//struct tree_elem tree				element of tree
//int	ifindex									if packet->dev->ifindex == ifindex then
//														continue contrast with dest_ip in dest_ip
//														tree
//struct dest_ip_tree ip_tree same as above
//int		 forward_opt					option of worward
//int		 forward_opt_para			option may be need parameter
//////////////////////////////////////////////////////////
struct ifindex_dest_ip_tree
{
	struct	tree_elem 		*tree;
	struct	dest_ip_tree 	*ip_tree;
	int					ifindex;
	int					forward_opt;
	int					forward_opt_para;
};


////////////////netlink transport struct////////////////
//
//
//
//
//
//
////////////////////////////////////////////////////////
struct netlink_transport_info
{
	int 			opt;
	int 			opt_para;
	int				src_port;
	int 			dest_port;
	_u16			protocol;
	int				ifindex;
	int				src_ipaddr;
	int			 	dest_ipaddr;
	int 			forward_opt;
	int				forward_slot;
	int 			src_ipmask;
	int 			dest_ipmask;
};

//#if NEW_PFM_STRUCT


struct date
{
	struct tree_elem *tree;
	#if HAVE_IPV6_ADDR
	struct in6_addr 	date;
	#else
	int date;
	#endif
	int 	opt;
	int 	opt_para;
	struct date* next_keyword;
};
//char *key[6]={"proto","ifindex","d_addr","s_addr","d_port","s_port"};

struct entry_info
{
	int opt;
	int flag;
	int proto;
	int ifindex;
	#if HAVE_IPV6_ADDR
	struct in6_addr d_addr;
	struct in6_addr s_addr;
	#else
	int d_addr;
	int s_addr;
	#endif
	int d_port;
	int s_port;
	int d_mask;
	int	s_mask;
	int count;
};

#define set_value(entry,value_name,value)				set_##value_name(entry,value)
#define get_value(entry,value_name)							get_##value_name(entry)

#define set_opt(entry,opt_value)					((entry) -> opt = (opt_value))
#define set_flag(entry,flag_value)				((entry) -> flag = (flag_value))
#define set_proto(entry,proto_value)			((entry) -> proto = (proto_value))
#define set_ifindex(entry,ifindex_value)	((entry) -> ifindex = (ifindex_value))
#define set_d_addr(entry,d_addr_value)		((entry) -> d_addr = (d_addr_value))
#define set_s_addr(entry,s_addr_value)		((entry) -> s_addr = (s_addr_value))
#define set_d_port(entry,d_port_value)		((entry) -> d_port = (d_port_value))
#define set_s_port(entry,s_port_value)		((entry) -> s_port = (s_port_value))
#define set_d_mask(entry,d_mask_value)		((entry) -> d_mask = (d_mask_value))
#define set_s_mask(entry,s_mask_value)		((entry) -> s_mask = (s_mask_value))


#define set_count(entry,count_valut)			((entry) -> count = (count_valut))
#define get_opt(entry)										((entry) -> opt)
#define get_flag(entry)										((entry) -> flag)
#define get_proto(entry)									((entry) -> proto)
#define get_ifindex(entry)								((entry) -> ifindex)
#define get_d_addr(entry)									((entry) -> d_addr)
#define get_s_addr(entry)									((entry) -> s_addr)
#define get_d_port(entry)									((entry) -> d_port)
#define get_s_port(entry)									((entry) -> s_port)
#define get_count(entry)									((entry) -> count)
#define get_d_mask(entry)									((entry) -> d_mask)
#define get_s_mask(entry)									((entry) -> s_mask)
//#endif



//////////////////////////typedef/////////////////////////
typedef struct ifindex_dest_ip_tree 		ifindex_dest_ip_tree;
typedef struct tree_elem					tree_elem;
typedef struct dest_ip_tree					dest_ip_tree;
typedef struct netlink_transport_info		netlink_transport_info;
typedef struct date							date;
typedef struct entry_info					entry_info;

//////////////////////////////////////////////////////////





#endif
