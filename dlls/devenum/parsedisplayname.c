/*
 *	IParseDisplayName implementation for DEVENUM.dll
 *
 * Copyright (C) 2002 Robert Shearman
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
 *
 * NOTES ON THIS FILE:
 * - Implements IParseDisplayName interface which creates a moniker
 *   from a string in a special format
 */
#include "devenum_private.h"

#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(devenum);

static HRESULT WINAPI DEVENUM_IParseDisplayName_QueryInterface(IParseDisplayName *iface,
        REFIID riid, void **ppv)
{
    TRACE("\n\tIID:\t%s\n",debugstr_guid(riid));

    if (!ppv)
        return E_POINTER;

    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IParseDisplayName))
    {
        *ppv = iface;
        IParseDisplayName_AddRef(iface);
        return S_OK;
    }

    FIXME("- no interface IID: %s\n", debugstr_guid(riid));
    *ppv = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI DEVENUM_IParseDisplayName_AddRef(IParseDisplayName *iface)
{
    TRACE("\n");

    DEVENUM_LockModule();

    return 2; /* non-heap based object */
}

static ULONG WINAPI DEVENUM_IParseDisplayName_Release(IParseDisplayName *iface)
{
    TRACE("\n");

    DEVENUM_UnlockModule();

    return 1; /* non-heap based object */
}

/**********************************************************************
 * DEVENUM_IParseDisplayName_ParseDisplayName
 *
 *  Creates a moniker referenced to by the display string argument
 *
 * POSSIBLE BUGS:
 *  Might not handle more complicated strings properly (ie anything
 *  not in "@device:sw:{CLSID1}\<filter name or CLSID>" format
 */
static HRESULT WINAPI DEVENUM_IParseDisplayName_ParseDisplayName(IParseDisplayName *iface,
        IBindCtx *pbc, LPOLESTR name, ULONG *eaten, IMoniker **ret)
{
    WCHAR buffer[MAX_PATH];
    enum device_type type;
    MediaCatMoniker *mon;
    HKEY hbasekey;
    CLSID class;
    LRESULT res;

    TRACE("(%p, %s, %p, %p)\n", pbc, debugstr_w(name), eaten, ret);

    *ret = NULL;
    if (eaten)
        *eaten = strlenW(name);

    name = strchrW(name, ':') + 1;

    if (name[0] == 's' && name[1] == 'w' && name[2] == ':')
    {
        type = DEVICE_FILTER;
        if ((res = RegOpenKeyExW(HKEY_CLASSES_ROOT, clsidW, 0, 0, &hbasekey)))
            return HRESULT_FROM_WIN32(res);
        name += 3;
    }
    else if (name[0] == 'c' && name[1] == 'm' && name[2] == ':')
    {
        type = DEVICE_CODEC;
        if ((res = RegOpenKeyExW(HKEY_CURRENT_USER, wszActiveMovieKey, 0, 0, &hbasekey)))
            return HRESULT_FROM_WIN32(res);
        name += 3;
    }
    else
    {
        FIXME("unhandled device type %s\n", debugstr_w(name));
        return MK_E_SYNTAX;
    }

    if (!(mon = DEVENUM_IMediaCatMoniker_Construct()))
        return E_OUTOFMEMORY;

    lstrcpynW(buffer, name, CHARS_IN_GUID);
    if (CLSIDFromString(buffer, &class) == S_OK)
    {
        mon->has_class = TRUE;
        mon->class = class;
        name += CHARS_IN_GUID;
    }

    mon->type = type;

    if (!(mon->name = CoTaskMemAlloc((strlenW(name) + 1) * sizeof(WCHAR))))
    {
        IMoniker_Release(&mon->IMoniker_iface);
        return E_OUTOFMEMORY;
    }
    strcpyW(mon->name, name);

    buffer[0] = 0;
    if (mon->has_class)
    {
        StringFromGUID2(&mon->class, buffer, CHARS_IN_GUID);
        if (mon->type == DEVICE_FILTER)
            strcatW(buffer, instanceW);
        strcatW(buffer, backslashW);
    }
    strcatW(buffer, mon->name);

    if (RegCreateKeyW(hbasekey, buffer, &mon->hkey))
    {
        IMoniker_Release(&mon->IMoniker_iface);
        return MK_E_NOOBJECT;
    }
    *ret = &mon->IMoniker_iface;

    return S_OK;
}

/**********************************************************************
 * IParseDisplayName_Vtbl
 */
static const IParseDisplayNameVtbl IParseDisplayName_Vtbl =
{
    DEVENUM_IParseDisplayName_QueryInterface,
    DEVENUM_IParseDisplayName_AddRef,
    DEVENUM_IParseDisplayName_Release,
    DEVENUM_IParseDisplayName_ParseDisplayName
};

/* The one instance of this class */
IParseDisplayName DEVENUM_ParseDisplayName = { &IParseDisplayName_Vtbl };
