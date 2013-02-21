/*
 * imagecapture.DS functions
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


#define NONAMELESSUNION
#define NONAMELESSSTRUCT

#include "config.h"
#include "wine/port.h"

#include <stdarg.h>
#include <stdio.h>
#include <math.h>

#ifdef HAVE_CARBON_CARBON_H
#define LoadResource __carbon_LoadResource
#define CompareString __carbon_CompareString
#define GetCurrentThread __carbon_GetCurrentThread
#define GetCurrentProcess __carbon_GetCurrentProcess
#define AnimatePalette __carbon_AnimatePalette
#define EqualRgn __carbon_EqualRgn
#define FillRgn __carbon_FillRgn
#define FrameRgn __carbon_FrameRgn
#define GetPixel __carbon_GetPixel
#define InvertRgn __carbon_InvertRgn
#define LineTo __carbon_LineTo
#define OffsetRgn __carbon_OffsetRgn
#define PaintRgn __carbon_PaintRgn
#define Polygon __carbon_Polygon
#define ResizePalette __carbon_ResizePalette
#define SetRectRgn __carbon_SetRectRgn
#define ULONG __carbon_ULONG
#define E_INVALIDARG __carbon_E_INVALIDARG
#define E_OUTOFMEMORY __carbon_E_OUTOFMEMORY
#define E_HANDLE __carbon_E_HANDLE
#define E_ACCESSDENIED __carbon_E_ACCESSDENIED
#define E_UNEXPECTED __carbon_E_UNEXPECTED
#define E_FAIL __carbon_E_FAIL
#define E_ABORT __carbon_E_ABORT
#define E_POINTER __carbon_E_POINTER
#define E_NOINTERFACE __carbon_E_NOINTERFACE
#define E_NOTIMPL __carbon_E_NOTIMPL
#define S_FALSE __carbon_S_FALSE
#define S_OK __carbon_S_OK
#define HRESULT_FACILITY __carbon_HRESULT_FACILITY
#define IS_ERROR __carbon_IS_ERROR
#define FAILED __carbon_FAILED
#define SUCCEEDED __carbon_SUCCEEDED
#define MAKE_HRESULT __carbon_MAKE_HRESULT
#define HRESULT __carbon_HRESULT
#define STDMETHODCALLTYPE __carbon_STDMETHODCALLTYPE
#include <Carbon/Carbon.h>
#undef LoadResource
#undef CompareString
#undef GetCurrentThread
#undef _CDECL
#undef DPRINTF
#undef GetCurrentProcess
#undef AnimatePalette
#undef EqualRgn
#undef FillRgn
#undef FrameRgn
#undef GetPixel
#undef InvertRgn
#undef LineTo
#undef OffsetRgn
#undef PaintRgn
#undef Polygon
#undef ResizePalette
#undef SetRectRgn
#undef ULONG
#undef E_INVALIDARG
#undef E_OUTOFMEMORY
#undef E_HANDLE
#undef E_ACCESSDENIED
#undef E_UNEXPECTED
#undef E_FAIL
#undef E_ABORT
#undef E_POINTER
#undef E_NOINTERFACE
#undef E_NOTIMPL
#undef S_FALSE
#undef S_OK
#undef HRESULT_FACILITY
#undef IS_ERROR
#undef FAILED
#undef SUCCEEDED
#undef MAKE_HRESULT
#undef HRESULT
#undef STDMETHODCALLTYPE
#endif /* HAVE_CARBON_CARBON_H */


#include "windef.h"
#include "winbase.h"
#include "twain.h"
#include "cic_i.h"
#include "winnls.h"
#include "wine/debug.h"
#include "wine/library.h"

WINE_DEFAULT_DEBUG_CHANNEL(twain);

#ifdef HAVE_CARBON_CARBON_H
static TW_UINT16 CIC_GetIdentity( pTW_IDENTITY pOrigin, pTW_IDENTITY self);
static TW_UINT16 CIC_OpenDS( pTW_IDENTITY pOrigin, pTW_IDENTITY self);
static TW_UINT16 CIC_CloseCIC( ICASessionID cameraSession, ICAScannerSessionID scannerSession);
static void CICNotificationCallback(CFStringRef notificationType, 
          CFDictionaryRef notificationDictionary);
static void CICRegisterForEventNotificationCallback(ICAHeader* header);
#endif

TW_UINT16 CIC_SourceControlHandler(
           pTW_IDENTITY pOrigin,
           TW_UINT16    DAT,
           TW_UINT16    MSG,
           TW_MEMREF    pData)
{
    TW_UINT16 twRC = TWRC_SUCCESS;
	
    switch (DAT)
    {
		case DAT_IDENTITY:
			switch (MSG)
	    {
			case MSG_CLOSEDS:
#ifdef HAVE_CARBON_CARBON_H
				twRC = CIC_CloseCIC(activeDS.Camera_Session, activeDS.Scanner_Session);
				activeDS.Camera_Session = 0;
				activeDS.Scanner_Session = 0;

#else
				twRC = TWRC_FAILURE;
				activeDS.twCC = TWCC_CAPUNSUPPORTED;
#endif
				break;
			case MSG_OPENDS:
#ifdef HAVE_CARBON_CARBON_H
				twRC = CIC_OpenDS( pOrigin, (pTW_IDENTITY)pData);
#else
				twRC = TWRC_FAILURE;
				activeDS.twCC = TWCC_CAPUNSUPPORTED;
#endif
				break;
			case MSG_GET:
#ifdef HAVE_CARBON_CARBON_H
				twRC = CIC_GetIdentity( pOrigin, (pTW_IDENTITY)pData);
#else
				twRC = TWRC_FAILURE;
				activeDS.twCC = TWCC_CAPUNSUPPORTED;
#endif
				break;
	    }
			break;
        case DAT_CAPABILITY:
            switch (MSG)
		{
			case MSG_GET:
				twRC = CIC_CapabilityGet (pOrigin, pData);
				break;
			case MSG_GETCURRENT:
				twRC = CIC_CapabilityGetCurrent (pOrigin, pData);
				break;
			case MSG_GETDEFAULT:
				twRC = CIC_CapabilityGetDefault (pOrigin, pData);
				break;
			case MSG_QUERYSUPPORT:
				twRC = CIC_CapabilityQuerySupport (pOrigin, pData);
				break;
			case MSG_RESET:
				twRC = CIC_CapabilityReset (pOrigin, pData);
				break;
			case MSG_SET:
				twRC = CIC_CapabilitySet (pOrigin, pData);
				break;
			default:
				twRC = TWRC_FAILURE;
				activeDS.twCC = TWCC_CAPBADOPERATION;
				FIXME("unrecognized operation triplet\n");
				break;
		}
            break;
			
        case DAT_EVENT:
            if (MSG == MSG_PROCESSEVENT)
                twRC = CIC_ProcessEvent (pOrigin, pData);
            else
            {
                activeDS.twCC = TWCC_CAPBADOPERATION;
                twRC = TWRC_FAILURE;
            }
            break;
			
        case DAT_PENDINGXFERS:
            switch (MSG)
		{
			case MSG_ENDXFER:
				twRC = CIC_PendingXfersEndXfer (pOrigin, pData);
				break;
			case MSG_GET:
				twRC = CIC_PendingXfersGet (pOrigin, pData);
				break;
			case MSG_RESET:
				twRC = CIC_PendingXfersReset (pOrigin, pData);
				break;
			default:
				activeDS.twCC = TWCC_CAPBADOPERATION;
				twRC = TWRC_FAILURE;
		}
            break;
			
        case DAT_SETUPMEMXFER:
            if (MSG == MSG_GET)
                twRC = CIC_SetupMemXferGet (pOrigin, pData);
            else
            {
                activeDS.twCC = TWCC_CAPBADOPERATION;
                twRC = TWRC_FAILURE;
            }
            break;
			
        case DAT_STATUS:
            if (MSG == MSG_GET)
                twRC = CIC_GetDSStatus (pOrigin, pData);
            else
            {
                activeDS.twCC = TWCC_CAPBADOPERATION;
                twRC = TWRC_FAILURE;
            }
            break;
			
        case DAT_USERINTERFACE:
            switch (MSG)
		{
			case MSG_DISABLEDS:
				twRC = CIC_DisableDSUserInterface (pOrigin, pData);
				break;
			case MSG_ENABLEDS:
				twRC = CIC_EnableDSUserInterface (pOrigin, pData);
				break;
			case MSG_ENABLEDSUIONLY:
				twRC = CIC_EnableDSUIOnly (pOrigin, pData);
				break;
			default:
				activeDS.twCC = TWCC_CAPBADOPERATION;
				twRC = TWRC_FAILURE;
				break;
		}
            break;
			
        case DAT_XFERGROUP:
            switch (MSG)
		{
			case MSG_GET:
				twRC = CIC_XferGroupGet (pOrigin, pData);
				break;
			case MSG_SET:
				twRC = CIC_XferGroupSet (pOrigin, pData);
				break;
			default:
				activeDS.twCC = TWCC_CAPBADOPERATION;
				twRC = TWRC_FAILURE;
				break;
		}
            break;
			
        default:
			WARN("code unsupported: %d\n", DAT);
            activeDS.twCC = TWCC_CAPUNSUPPORTED;
            twRC = TWRC_FAILURE;
            break;
    }
	
    return twRC;
}

TW_UINT16 CIC_ImageGroupHandler(pTW_IDENTITY pOrigin,
           TW_UINT16    DAT,
           TW_UINT16    MSG,
           TW_MEMREF    pData)
{
    return TWRC_FAILURE;
}

#ifdef HAVE_CARBON_CARBON_H
static TW_UINT16 CIC_GetIdentity( pTW_IDENTITY pOrigin, pTW_IDENTITY self)
{
//	static int cursanedev = 0;
	
//    detect_ImageCapture_devices();
//    if (!sane_devlist[cursanedev])
		return TWRC_FAILURE;
    self->ProtocolMajor = TWON_PROTOCOLMAJOR;
    self->ProtocolMinor = TWON_PROTOCOLMINOR;
    self->SupportedGroups = DG_CONTROL | DG_IMAGE;
	
	return TWRC_SUCCESS;
}

static TW_UINT16 CIC_OpenDS( pTW_IDENTITY pOrigin, pTW_IDENTITY self)
{
	OSErr							   err = noErr;
    ICARegisterForEventNotificationPB  pb = {};
    CFStringRef NotificationTypes[] = {                       
		kICANotificationTypeDeviceRemoved,
		kICANotificationTypeDeviceInfoChanged,
		kICANotificationTypeDeviceWasReset,
		
		kICANotificationTypeTransactionCanceled,
		
		kICANotificationTypeObjectAdded,
		kICANotificationTypeObjectRemoved,
		kICANotificationTypeObjectInfoChanged,
		
		kICANotificationTypeScannerSessionClosed,
		kICANotificationTypeScannerScanDone,
		kICANotificationTypeScannerPageDone,
		kICANotificationTypeScannerButtonPressed,
		kICANotificationTypeScanProgressStatus,
		NULL
	};
	
    CFArrayRef array = CFArrayCreate(kCFAllocatorDefault, NotificationTypes, 12, &kCFTypeArrayCallBacks);
    
    
    pb.header.refcon    = 0; //(uintptr_t)self;							// refcon
    pb.objectOfInterest = 0;								// scanner
    pb.eventsOfInterest	= array;    
    pb.notificationProc = CICNotificationCallback;
    pb.options = NULL;
    err = ICARegisterForEventNotification (&pb, CICRegisterForEventNotificationCallback);
	
	CFRelease(array);
	
	return TWRC_FAILURE;
}

static void CICRegisterForEventNotificationCallback(ICAHeader* header)
{
	
}

static void CICNotificationCallback(CFStringRef notificationType, 
          CFDictionaryRef notificationDictionary)
{

}
#endif

TW_UINT16 CIC_CapabilityGet (pTW_IDENTITY pOrigin, pTW_IDENTITY self)
{
	return TWRC_FAILURE;
}

TW_UINT16 CIC_CapabilityGetCurrent (pTW_IDENTITY pOrigin, pTW_IDENTITY self)
{
	return TWRC_FAILURE;
}

TW_UINT16 CIC_CapabilityGetDefault (pTW_IDENTITY pOrigin, pTW_IDENTITY self)
{
	return TWRC_FAILURE;
}

TW_UINT16 CIC_CapabilityQuerySupport (pTW_IDENTITY pOrigin, pTW_IDENTITY self)
{
	return TWRC_FAILURE;
}

TW_UINT16 CIC_CapabilityReset (pTW_IDENTITY pOrigin, pTW_IDENTITY self)
{
	return TWRC_FAILURE;
}

TW_UINT16 CIC_CapabilitySet (pTW_IDENTITY pOrigin, pTW_IDENTITY self)
{
	return TWRC_FAILURE;
}

TW_UINT16 CIC_ProcessEvent(pTW_IDENTITY pOrigin, pTW_IDENTITY self)
{
	return TWRC_FAILURE;
}

TW_UINT16 CIC_PendingXfersEndXfer(pTW_IDENTITY pOrigin, pTW_IDENTITY self)
{
	return TWRC_FAILURE;
}

TW_UINT16 CIC_PendingXfersGet(pTW_IDENTITY pOrigin, pTW_IDENTITY self)
{
	return TWRC_FAILURE;
}

TW_UINT16 CIC_PendingXfersReset(pTW_IDENTITY pOrigin, pTW_IDENTITY self)
{
	return TWRC_FAILURE;
}

TW_UINT16 CIC_SetupMemXferGet(pTW_IDENTITY pOrigin, pTW_IDENTITY self)
{
	return TWRC_FAILURE;
}

TW_UINT16 CIC_GetDSStatus(pTW_IDENTITY pOrigin, pTW_IDENTITY self)
{
	return TWRC_FAILURE;
}

TW_UINT16 CIC_XferGroupGet(pTW_IDENTITY pOrigin, pTW_IDENTITY self)
{
	return TWRC_FAILURE;
}

TW_UINT16 CIC_XferGroupSet(pTW_IDENTITY pOrigin, pTW_IDENTITY self)
{
	return TWRC_FAILURE;
}

static TW_UINT16 CIC_CloseCIC( ICASessionID cameraSession, ICAScannerSessionID scannerSession)
{
    if (cameraSession)
    {
       	ICACloseSessionPB pb = {};
        pb.sessionID = cameraSession;
        ICACloseSession(&pb, NULL);
    }
	else if(scannerSession)
	{
		ICAScannerCloseSessionPB pb = {};
		pb.sessionID = scannerSession;
		ICAScannerCloseSession(&pb,	NULL);
	}
	else return TWRC_FAILURE;
	
	return TWRC_SUCCESS;
}
