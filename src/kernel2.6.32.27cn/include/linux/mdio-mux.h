/*
 * MDIO bus multiplexer framwork.
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2011 Cavium Networks
 */
#ifndef __LINUX_MDIO_MUX_H
#define __LINUX_MDIO_MUX_H
#include <linux/platform_device.h>

int mdio_mux_probe(struct platform_device *pdev,
		   int (*switch_fn) (int cur, int desired, void *data),
		   void *data);

#endif /* __LINUX_MDIO_MUX_H */
