/*
 * Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef __configs_chromeos_coreboot_h__
#define __configs_chromeos_coreboot_h__

/* So far all our x86-based boards share the coreboot config. */
#include <configs/coreboot.h>

/* Support USB booting */
#define CONFIG_CHROMEOS_USB

#include "chromeos.h"

#endif /* __configs_chromeos_tegra2_twostop_h__ */