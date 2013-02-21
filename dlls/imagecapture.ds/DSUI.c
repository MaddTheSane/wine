/*
 * imagecapture.DS UI functions
 *
 * Copyright 2010 C.W. Betts <computers57@hotmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include "config.h"

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#define NONAMELESSUNION
#define NONAMELESSSTRUCT
#include "windef.h"
#include "winbase.h"
#include "winuser.h"
#include "winnls.h"
#include "wingdi.h"
#include "prsht.h"
#include "twain.h"
#include "cic_i.h"
#include "wine/debug.h"
#include "resource.h"
#include "wine/unicode.h"

WINE_DEFAULT_DEBUG_CHANNEL(twain);

static const char settings_key[] = "Software\\Wine\\ImageCapture";
static const char settings_value[] = "SkipUI";
static BOOL disable_dialog;
static HBITMAP static_bitmap;

TW_UINT16 CIC_DisableDSUserInterface(pTW_IDENTITY pOrigin, pTW_IDENTITY self)
{
	return TWRC_FAILURE;
}

TW_UINT16 CIC_EnableDSUserInterface(pTW_IDENTITY pOrigin, pTW_IDENTITY self)
{
	return TWRC_FAILURE;
}

TW_UINT16 CIC_EnableDSUIOnly(pTW_IDENTITY pOrigin, pTW_IDENTITY self)
{
	return TWRC_FAILURE;
}
