/*
 * Copyright 2000 Corel Corporation
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

#ifndef _TWAIN32_H
#define _TWAIN32_H

#ifndef __WINE_CONFIG_H
# error You must include config.h first
#endif

#ifndef __CARBON__
#if __LP64__
typedef unsigned int	UInt32_mac;
typedef signed int		SInt32_mac;
#else
typedef unsigned long	UInt32_mac;
typedef signed long		SInt32_mac;
#endif

typedef UInt32_mac		ICASessionID;
typedef UInt32_mac		ICAScannerSessionID;
#endif

/* internal information about an active data source */
struct tagActiveDS
{
    struct tagActiveDS	*next;			/* next active DS */
    TW_IDENTITY			identity;		/* identity */
    TW_UINT16			currentState;	/* current state */
    UINT				windowMessage;	/* message to use to send status */
    TW_UINT16			twCC;			/* condition code */
    HWND				hwndOwner;		/* window handle of the app */
    HWND				progressWnd;	/* window handle of the scanning window */
#ifdef HAVE_CARBON_CARBON_H
    ICASessionID		Camera_Session;
	ICAScannerSessionID	Scanner_Session;
    BOOL				sane_param_valid;	/* true if valid sane_param*/
    BOOL				CIC_started;		/* If sane_start has been called */
    INT					deviceIndex;		/* index of the current device */
#endif
    /* Capabilities */
    TW_UINT16			capXferMech;			/* ICAP_XFERMECH */
    BOOL				PixelTypeSet;
    TW_UINT16			defaultPixelType;		/* ICAP_PIXELTYPE */
    BOOL				XResolutionSet;
    TW_FIX32			defaultXResolution;
    BOOL				YResolutionSet;
    TW_FIX32			defaultYResolution;
} activeDS;

TW_UINT16 CIC_SourceControlHandler(
           pTW_IDENTITY pOrigin,
           TW_UINT16    DAT,
           TW_UINT16    MSG,
           TW_MEMREF    pData);
TW_UINT16 CIC_ImageGroupHandler(pTW_IDENTITY pOrigin,
           TW_UINT16    DAT,
           TW_UINT16    MSG,
           TW_MEMREF    pData);

TW_UINT16 CIC_CapabilityGet (pTW_IDENTITY pOrigin, pTW_IDENTITY self);
TW_UINT16 CIC_CapabilityGetCurrent (pTW_IDENTITY pOrigin, pTW_IDENTITY self);
TW_UINT16 CIC_CapabilityGetDefault (pTW_IDENTITY pOrigin, pTW_IDENTITY self);
TW_UINT16 CIC_CapabilityQuerySupport (pTW_IDENTITY pOrigin, pTW_IDENTITY self);
TW_UINT16 CIC_CapabilityReset (pTW_IDENTITY pOrigin, pTW_IDENTITY self);
TW_UINT16 CIC_CapabilitySet (pTW_IDENTITY pOrigin, pTW_IDENTITY self);
TW_UINT16 CIC_ProcessEvent(pTW_IDENTITY pOrigin, pTW_IDENTITY self);
TW_UINT16 CIC_PendingXfersEndXfer(pTW_IDENTITY pOrigin, pTW_IDENTITY self);
TW_UINT16 CIC_PendingXfersGet(pTW_IDENTITY pOrigin, pTW_IDENTITY self);
TW_UINT16 CIC_PendingXfersReset(pTW_IDENTITY pOrigin, pTW_IDENTITY self);
TW_UINT16 CIC_SetupMemXferGet(pTW_IDENTITY pOrigin, pTW_IDENTITY self);
TW_UINT16 CIC_GetDSStatus(pTW_IDENTITY pOrigin, pTW_IDENTITY self);
TW_UINT16 CIC_DisableDSUserInterface(pTW_IDENTITY pOrigin, pTW_IDENTITY self);
TW_UINT16 CIC_EnableDSUserInterface(pTW_IDENTITY pOrigin, pTW_IDENTITY self);
TW_UINT16 CIC_EnableDSUIOnly(pTW_IDENTITY pOrigin, pTW_IDENTITY self);
TW_UINT16 CIC_XferGroupGet(pTW_IDENTITY pOrigin, pTW_IDENTITY self);
TW_UINT16 CIC_XferGroupSet(pTW_IDENTITY pOrigin, pTW_IDENTITY self);

#endif
