/*
 * Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 * Alternatively, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") version 2 as published by the Free
 * Software Foundation.
 */

#include <common.h>
#include <mmc.h>

#include "boot_device.h"

#include <vboot_api.h>

static int boot_device_mmc_start(uint32_t disk_flags)
{
	/* We expect to have at least one MMC device */
	return 1;
}

static int boot_device_mmc_scan(block_dev_desc_t **desc, int max_devs,
			 uint32_t disk_flags)
{
	int index, found;

	for (index = found = 0; index < max_devs; index++) {
		struct mmc *mmc;
		uint32_t flags;

		mmc = find_mmc_device(index);
		if (!mmc)
			break;

		/* Skip device that don't match or cannot be initialized */
		if (boot_device_matches(&mmc->block_dev, disk_flags, &flags) &&
				mmc_init(mmc) == 0) {
			/*
			* find_mmc_device() returns a pointer from a global
			* linked list. So &mmc->block_dev is not a dangling
			* pointer after this function is returned.
			*/
			desc[found++] = &mmc->block_dev;
		}
	}
	return found;
}

static struct boot_interface mmc_interface = {
	.name = "mmc",
	.type = IF_TYPE_MMC,
	.start = boot_device_mmc_start,
	.scan = boot_device_mmc_scan,
};

int boot_device_mmc_probe(void)
{
	return boot_device_register_interface(&mmc_interface);
}