/*
 * imagecapture.DS functions
 *
 * Copyright 2000 Shi Quan He <shiquan@cyberdude.com>
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
#include "wine/port.h"

#include <stdarg.h>
#include <stdio.h>

#include "windef.h"
#include "winbase.h"
#include "twain.h"
#include "cic_i.h"
#include "wine/debug.h"
#include "wine/library.h"

WINE_DEFAULT_DEBUG_CHANNEL(twain);

HINSTANCE CIC_instance;

BOOL WINAPI DllMain (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    TRACE("%p,%x,%p\n", hinstDLL, fdwReason, lpvReserved);

    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH: {
	    CIC_instance = hinstDLL;
            DisableThreadLibraryCalls(hinstDLL);
            break;
	}
        case DLL_PROCESS_DETACH:
	    CIC_instance = NULL;
            break;
    }

    return TRUE;
}

/* Main entry point for the TWAIN library */
TW_UINT16 WINAPI
DS_Entry ( pTW_IDENTITY pOrigin,
           TW_UINT32    DG,
           TW_UINT16    DAT,
           TW_UINT16    MSG,
           TW_MEMREF    pData)
{
#ifdef HAVE_CARBON_CARBON_H
    TW_UINT16 twRC = TWRC_SUCCESS;  /* Return Code */

    TRACE("(DG=%d DAT=%d MSG=%d)\n", DG, DAT, MSG);

    switch (DG)
    {
        case DG_CONTROL:
            twRC = CIC_SourceControlHandler (pOrigin,DAT,MSG,pData);
            break;
        case DG_IMAGE:
            twRC = CIC_ImageGroupHandler (pOrigin,DAT,MSG,pData);
            break;
        case DG_AUDIO:
            WARN("Audio group of controls not implemented yet.\n");
            twRC = TWRC_FAILURE;
            activeDS.twCC = TWCC_CAPUNSUPPORTED;
            break;
        default:
            activeDS.twCC = TWCC_BADPROTOCOL;
            twRC = TWRC_FAILURE;
    }

    return twRC;
    
#else
    return TWRC_FAILURE;
#endif /* HAVE_CARBON_CARBON_H */
}
