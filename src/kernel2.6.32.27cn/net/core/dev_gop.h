/*******************************************************************************
Copyright (C) Autelan Technology

This software file is owned and distributed by Autelan Technology
********************************************************************************

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************************/
#ifndef _DEV_GOP_H_
#define _DEV_GOP_H_

#include <linux/types.h>
#include <linux/netdevice.h>
#include <linux/list.h>

/**
  *  error number base on <asm-generic/errno-base.h> and
  *	<asm/errno.h>
  */
/* Defined for net device global operation, for Autelan privacy */
#define EGINDEX_ALREADY_INUSE	200

/**
  *  external function declaration
  */
extern size_t strlen(const char *s);
extern struct net_device *__dev_get_by_index(struct net * net, int ifindex);

/**
  *  internal function declaration
  */
int dev_new_gindex(struct net *net);
int dev_update_gindex
(
	struct net * net,
	char *name,
	int gindex
);
int dev_get_gindex
(
	struct net * net,
	char *name,
	int *gindex
);
/*
void dev_gindex_hash_init();
*/
/**
 *	dev_gindex_init - initialize gindex hash chain
 */
void dev_gindex_hash_remove
(
	struct net_device* dev
);
/**
 *	dev_gindex_add_head - add device to gindex head chain
 */
void dev_gindex_hash_add_head
(
	struct net * net,
	struct net_device* dev
);
#endif
