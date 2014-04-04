#ifndef _AAT_IOCTL_H
#define _AAT_IOCTL_H
struct sta_info* aat_add_sta(struct io_info * ioinfo);
int aat_del_sta(struct io_info * ioinfo);
struct sta_info *aat_get_stav2(struct aat_info *if_aat, unsigned char *sta, unsigned vrrid);
struct sta_info *aat_get_sta(struct aat_info *if_aat, unsigned char *sta);
int aat_clean_hansi_stas(struct sta_info **stalist, unsigned int vrrid);

/* get sta info by vip */
struct sta_info *aat_get_sta_by_vip
(
	struct aat_info *if_aat,
	unsigned int sta_vip
);

#endif
