/*
 * Copyright 2017 André Hentschel
 * Copyright 2018 Stefan Dösinger for CodeWeavers
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

/* NOTE: The guest side uses mingw's headers. The host side uses Wine's headers. */

#define COBJMACROS

#include <windows.h>
#include <stdio.h>

#include "windows-user-services.h"
#include "dll_list.h"

#ifdef QEMU_DLL_GUEST
#include <initguid.h>

#include <dxgi1_2.h>
#include <debug.h>

#else

#include <dxgi1_5.h>
#include <wine/debug.h>

#endif

#include "qemu_dxgi.h"

WINE_DEFAULT_DEBUG_CHANNEL(qemu_dxgi);

struct qemu_dxgi_adapter_QueryInterface
{
    struct qemu_syscall super;
    uint64_t iface;
    uint64_t iid;
    uint64_t out;
};

#ifdef QEMU_DLL_GUEST

static inline struct qemu_dxgi_adapter *impl_from_IDXGIAdapter3(IDXGIAdapter3 *iface)
{
    return CONTAINING_RECORD(iface, struct qemu_dxgi_adapter, IDXGIAdapter3_iface);
}

static HRESULT STDMETHODCALLTYPE dxgi_adapter_QueryInterface(IDXGIAdapter3 *iface, REFIID iid, void **out)
{
    struct qemu_dxgi_adapter_QueryInterface call;
    struct qemu_dxgi_adapter *adapter = impl_from_IDXGIAdapter3(iface);

    call.super.id = QEMU_SYSCALL_ID(CALL_DXGI_ADAPTER_QUERYINTERFACE);
    call.iface = (ULONG_PTR)adapter;
    call.iid = (ULONG_PTR)iid;
    call.out = (ULONG_PTR)out;

    qemu_syscall(&call.super);

    return call.super.iret;
}

#else

void qemu_dxgi_adapter_QueryInterface(struct qemu_syscall *call)
{
    struct qemu_dxgi_adapter_QueryInterface *c = (struct qemu_dxgi_adapter_QueryInterface *)call;
    struct qemu_dxgi_adapter *adapter;

    WINE_FIXME("Unverified!\n");
    adapter = QEMU_G2H(c->iface);

    c->super.iret = IDXGIAdapter3_QueryInterface(adapter->host, QEMU_G2H(c->iid), QEMU_G2H(c->out));
}

#endif

struct qemu_dxgi_adapter_AddRef
{
    struct qemu_syscall super;
    uint64_t iface;
};

#ifdef QEMU_DLL_GUEST

static ULONG STDMETHODCALLTYPE dxgi_adapter_AddRef(IDXGIAdapter3 *iface)
{
    struct qemu_dxgi_adapter_AddRef call;
    struct qemu_dxgi_adapter *adapter = impl_from_IDXGIAdapter3(iface);

    call.super.id = QEMU_SYSCALL_ID(CALL_DXGI_ADAPTER_ADDREF);
    call.iface = (ULONG_PTR)adapter;

    qemu_syscall(&call.super);

    return call.super.iret;
}

#else

void qemu_dxgi_adapter_AddRef(struct qemu_syscall *call)
{
    struct qemu_dxgi_adapter_AddRef *c = (struct qemu_dxgi_adapter_AddRef *)call;
    struct qemu_dxgi_adapter *adapter;

    WINE_TRACE("\n");
    adapter = QEMU_G2H(c->iface);

    c->super.iret = IDXGIAdapter3_AddRef(adapter->host);
}

#endif

struct qemu_dxgi_adapter_Release
{
    struct qemu_syscall super;
    uint64_t iface;
};

#ifdef QEMU_DLL_GUEST

static ULONG STDMETHODCALLTYPE dxgi_adapter_Release(IDXGIAdapter3 *iface)
{
    struct qemu_dxgi_adapter_Release call;
    struct qemu_dxgi_adapter *adapter = impl_from_IDXGIAdapter3(iface);

    call.super.id = QEMU_SYSCALL_ID(CALL_DXGI_ADAPTER_RELEASE);
    call.iface = (ULONG_PTR)adapter;

    qemu_syscall(&call.super);

    return call.super.iret;
}

#else

ULONG qemu_dxgi_adapter_Release_internal(struct qemu_dxgi_adapter *adapter)
{
    struct qemu_dxgi_factory *factory = adapter->factory;
    ULONG ret;

    /* AddRef the parent factory and release it afterwards to make sure the last ref gets released through
     * our code so we destroy the wrapper. */
    IDXGIFactory5_AddRef(factory->host);
    ret = IDXGIAdapter3_Release(adapter->host);
    if (!ret)
    {
        WINE_TRACE("Destroying dxgi adapter wrapper %p (host adapter %p).\n", adapter, adapter->host);
        HeapFree(GetProcessHeap(), 0, adapter);
    }
    qemu_dxgi_factory_Release_internal(factory);
    return ret;
}

void qemu_dxgi_adapter_Release(struct qemu_syscall *call)
{
    struct qemu_dxgi_adapter_Release *c = (struct qemu_dxgi_adapter_Release *)call;
    struct qemu_dxgi_adapter *adapter;

    WINE_TRACE("\n");
    adapter = QEMU_G2H(c->iface);

    c->super.iret = qemu_dxgi_adapter_Release_internal(adapter);
}

#endif

struct qemu_dxgi_adapter_SetPrivateData
{
    struct qemu_syscall super;
    uint64_t iface;
    uint64_t guid;
    uint64_t data_size;
    uint64_t data;
};

#ifdef QEMU_DLL_GUEST

static HRESULT STDMETHODCALLTYPE dxgi_adapter_SetPrivateData(IDXGIAdapter3 *iface, REFGUID guid, UINT data_size, const void *data)
{
    struct qemu_dxgi_adapter_SetPrivateData call;
    struct qemu_dxgi_adapter *adapter = impl_from_IDXGIAdapter3(iface);

    call.super.id = QEMU_SYSCALL_ID(CALL_DXGI_ADAPTER_SETPRIVATEDATA);
    call.iface = (ULONG_PTR)adapter;
    call.guid = (ULONG_PTR)guid;
    call.data_size = data_size;
    call.data = (ULONG_PTR)data;

    qemu_syscall(&call.super);

    return call.super.iret;
}

#else

void qemu_dxgi_adapter_SetPrivateData(struct qemu_syscall *call)
{
    struct qemu_dxgi_adapter_SetPrivateData *c = (struct qemu_dxgi_adapter_SetPrivateData *)call;
    struct qemu_dxgi_adapter *adapter;

    WINE_FIXME("Unverified!\n");
    adapter = QEMU_G2H(c->iface);

    c->super.iret = IDXGIAdapter3_SetPrivateData(adapter->host, QEMU_G2H(c->guid), c->data_size, QEMU_G2H(c->data));
}

#endif

struct qemu_dxgi_adapter_SetPrivateDataInterface
{
    struct qemu_syscall super;
    uint64_t iface;
    uint64_t guid;
    uint64_t object;
};

#ifdef QEMU_DLL_GUEST

static HRESULT STDMETHODCALLTYPE dxgi_adapter_SetPrivateDataInterface(IDXGIAdapter3 *iface, REFGUID guid, const IUnknown *object)
{
    struct qemu_dxgi_adapter_SetPrivateDataInterface call;
    struct qemu_dxgi_adapter *adapter = impl_from_IDXGIAdapter3(iface);

    call.super.id = QEMU_SYSCALL_ID(CALL_DXGI_ADAPTER_SETPRIVATEDATAINTERFACE);
    call.iface = (ULONG_PTR)adapter;
    call.guid = (ULONG_PTR)guid;
    call.object = (ULONG_PTR)object;

    qemu_syscall(&call.super);

    return call.super.iret;
}

#else

void qemu_dxgi_adapter_SetPrivateDataInterface(struct qemu_syscall *call)
{
    struct qemu_dxgi_adapter_SetPrivateDataInterface *c = (struct qemu_dxgi_adapter_SetPrivateDataInterface *)call;
    struct qemu_dxgi_adapter *adapter;

    WINE_FIXME("Unverified!\n");
    adapter = QEMU_G2H(c->iface);

    c->super.iret = IDXGIAdapter3_SetPrivateDataInterface(adapter->host, QEMU_G2H(c->guid), QEMU_G2H(c->object));
}

#endif

struct qemu_dxgi_adapter_GetPrivateData
{
    struct qemu_syscall super;
    uint64_t iface;
    uint64_t guid;
    uint64_t data_size;
    uint64_t data;
};

#ifdef QEMU_DLL_GUEST

static HRESULT STDMETHODCALLTYPE dxgi_adapter_GetPrivateData(IDXGIAdapter3 *iface, REFGUID guid, UINT *data_size, void *data)
{
    struct qemu_dxgi_adapter_GetPrivateData call;
    struct qemu_dxgi_adapter *adapter = impl_from_IDXGIAdapter3(iface);

    call.super.id = QEMU_SYSCALL_ID(CALL_DXGI_ADAPTER_GETPRIVATEDATA);
    call.iface = (ULONG_PTR)adapter;
    call.guid = (ULONG_PTR)guid;
    call.data_size = (ULONG_PTR)data_size;
    call.data = (ULONG_PTR)data;

    qemu_syscall(&call.super);

    return call.super.iret;
}

#else

void qemu_dxgi_adapter_GetPrivateData(struct qemu_syscall *call)
{
    struct qemu_dxgi_adapter_GetPrivateData *c = (struct qemu_dxgi_adapter_GetPrivateData *)call;
    struct qemu_dxgi_adapter *adapter;

    WINE_FIXME("Unverified!\n");
    adapter = QEMU_G2H(c->iface);

    c->super.iret = IDXGIAdapter3_GetPrivateData(adapter->host, QEMU_G2H(c->guid), QEMU_G2H(c->data_size), QEMU_G2H(c->data));
}

#endif

struct qemu_dxgi_adapter_GetParent
{
    struct qemu_syscall super;
    uint64_t iface;
    uint64_t iid;
    uint64_t parent;
};

#ifdef QEMU_DLL_GUEST

static HRESULT STDMETHODCALLTYPE dxgi_adapter_GetParent(IDXGIAdapter3 *iface, REFIID iid, void **parent)
{
    struct qemu_dxgi_adapter_GetParent call;
    struct qemu_dxgi_adapter *adapter = impl_from_IDXGIAdapter3(iface);
    struct qemu_dxgi_factory *factory;

    call.super.id = QEMU_SYSCALL_ID(CALL_DXGI_ADAPTER_GETPARENT);
    call.iface = (ULONG_PTR)adapter;

    qemu_syscall(&call.super);
    factory = (struct qemu_dxgi_factory *)(ULONG_PTR)call.parent;

    return IDXGIFactory_QueryInterface(&factory->IDXGIFactory5_iface, iid, parent);
}

#else

void qemu_dxgi_adapter_GetParent(struct qemu_syscall *call)
{
    struct qemu_dxgi_adapter_GetParent *c = (struct qemu_dxgi_adapter_GetParent *)call;
    struct qemu_dxgi_adapter *adapter;

    WINE_TRACE("\n");
    adapter = QEMU_G2H(c->iface);
    c->parent = QEMU_H2G(adapter->factory);
}

#endif

struct qemu_dxgi_adapter_EnumOutputs
{
    struct qemu_syscall super;
    uint64_t iface;
    uint64_t output_idx;
    uint64_t output;
};

#ifdef QEMU_DLL_GUEST

static HRESULT STDMETHODCALLTYPE dxgi_adapter_EnumOutputs(IDXGIAdapter3 *iface, UINT output_idx, IDXGIOutput **output)
{
    struct qemu_dxgi_adapter_EnumOutputs call;
    struct qemu_dxgi_adapter *adapter = impl_from_IDXGIAdapter3(iface);
    struct qemu_dxgi_output *obj;

    call.super.id = QEMU_SYSCALL_ID(CALL_DXGI_ADAPTER_ENUMOUTPUTS);
    call.iface = (ULONG_PTR)adapter;
    call.output_idx = output_idx;

    qemu_syscall(&call.super);
    obj = (struct qemu_dxgi_output *)(ULONG_PTR)call.output;
    if (obj)
    {
        qemu_dxgi_output_guest_init(obj);
        *output = (IDXGIOutput *)&obj->IDXGIOutput4_iface;
    }
    else
    {
        *output = NULL;
    }

    return call.super.iret;
}

#else

void qemu_dxgi_adapter_EnumOutputs(struct qemu_syscall *call)
{
    struct qemu_dxgi_adapter_EnumOutputs *c = (struct qemu_dxgi_adapter_EnumOutputs *)call;
    struct qemu_dxgi_adapter *adapter;
    struct qemu_dxgi_output *output;

    WINE_TRACE("\n");
    adapter = QEMU_G2H(c->iface);

    c->super.iret = qemu_dxgi_output_create(adapter, c->output_idx, &output);
    c->output = QEMU_H2G(output);
}

#endif

struct qemu_dxgi_adapter_GetDesc1
{
    struct qemu_syscall super;
    uint64_t iface;
    uint64_t desc;
};

#ifdef QEMU_DLL_GUEST

static HRESULT STDMETHODCALLTYPE dxgi_adapter_GetDesc1(IDXGIAdapter3 *iface, DXGI_ADAPTER_DESC1 *desc)
{
    struct qemu_dxgi_adapter_GetDesc1 call;
    struct qemu_dxgi_adapter *adapter = impl_from_IDXGIAdapter3(iface);

    call.super.id = QEMU_SYSCALL_ID(CALL_DXGI_ADAPTER_GETDESC1);
    call.iface = (ULONG_PTR)adapter;
    call.desc = (ULONG_PTR)desc;

    qemu_syscall(&call.super);

    return call.super.iret;
}

#else

void qemu_dxgi_adapter_GetDesc1(struct qemu_syscall *call)
{
    struct qemu_dxgi_adapter_GetDesc1 *c = (struct qemu_dxgi_adapter_GetDesc1 *)call;
    struct qemu_dxgi_adapter *adapter;

    WINE_FIXME("Unverified!\n");
    adapter = QEMU_G2H(c->iface);

    c->super.iret = IDXGIAdapter3_GetDesc1(adapter->host, QEMU_G2H(c->desc));
}

#endif

struct qemu_dxgi_adapter_GetDesc
{
    struct qemu_syscall super;
    uint64_t iface;
    uint64_t desc;
};

#ifdef QEMU_DLL_GUEST

static HRESULT STDMETHODCALLTYPE dxgi_adapter_GetDesc(IDXGIAdapter3 *iface, DXGI_ADAPTER_DESC *desc)
{
    struct qemu_dxgi_adapter_GetDesc call;
    struct qemu_dxgi_adapter *adapter = impl_from_IDXGIAdapter3(iface);

    call.super.id = QEMU_SYSCALL_ID(CALL_DXGI_ADAPTER_GETDESC);
    call.iface = (ULONG_PTR)adapter;
    call.desc = (ULONG_PTR)desc;

    qemu_syscall(&call.super);

    return call.super.iret;
}

#else

void qemu_dxgi_adapter_GetDesc(struct qemu_syscall *call)
{
    struct qemu_dxgi_adapter_GetDesc *c = (struct qemu_dxgi_adapter_GetDesc *)call;
    struct qemu_dxgi_adapter *adapter;

    WINE_FIXME("Unverified!\n");
    adapter = QEMU_G2H(c->iface);

    c->super.iret = IDXGIAdapter3_GetDesc(adapter->host, QEMU_G2H(c->desc));
}

#endif

struct qemu_dxgi_adapter_CheckInterfaceSupport
{
    struct qemu_syscall super;
    uint64_t iface;
    uint64_t guid;
    uint64_t umd_version;
};

#ifdef QEMU_DLL_GUEST

static HRESULT STDMETHODCALLTYPE dxgi_adapter_CheckInterfaceSupport(IDXGIAdapter3 *iface, REFGUID guid, LARGE_INTEGER *umd_version)
{
    struct qemu_dxgi_adapter_CheckInterfaceSupport call;
    struct qemu_dxgi_adapter *adapter = impl_from_IDXGIAdapter3(iface);

    call.super.id = QEMU_SYSCALL_ID(CALL_DXGI_ADAPTER_CHECKINTERFACESUPPORT);
    call.iface = (ULONG_PTR)adapter;
    call.guid = (ULONG_PTR)guid;
    call.umd_version = (ULONG_PTR)umd_version;

    qemu_syscall(&call.super);

    return call.super.iret;
}

#else

void qemu_dxgi_adapter_CheckInterfaceSupport(struct qemu_syscall *call)
{
    struct qemu_dxgi_adapter_CheckInterfaceSupport *c = (struct qemu_dxgi_adapter_CheckInterfaceSupport *)call;
    struct qemu_dxgi_adapter *adapter;

    WINE_FIXME("Unverified!\n");
    adapter = QEMU_G2H(c->iface);

    c->super.iret = IDXGIAdapter3_CheckInterfaceSupport(adapter->host, QEMU_G2H(c->guid), QEMU_G2H(c->umd_version));
}

#endif

struct qemu_dxgi_adapter_GetDesc2
{
    struct qemu_syscall super;
    uint64_t iface;
    uint64_t desc;
};

#ifdef QEMU_DLL_GUEST

static HRESULT STDMETHODCALLTYPE dxgi_adapter_GetDesc2(IDXGIAdapter3 *iface, DXGI_ADAPTER_DESC2 *desc)
{
    struct qemu_dxgi_adapter_GetDesc2 call;
    struct qemu_dxgi_adapter *adapter = impl_from_IDXGIAdapter3(iface);

    call.super.id = QEMU_SYSCALL_ID(CALL_DXGI_ADAPTER_GETDESC2);
    call.iface = (ULONG_PTR)adapter;
    call.desc = (ULONG_PTR)desc;

    qemu_syscall(&call.super);

    return call.super.iret;
}

#else

void qemu_dxgi_adapter_GetDesc2(struct qemu_syscall *call)
{
    struct qemu_dxgi_adapter_GetDesc2 *c = (struct qemu_dxgi_adapter_GetDesc2 *)call;
    struct qemu_dxgi_adapter *adapter;

    WINE_FIXME("Unverified!\n");
    adapter = QEMU_G2H(c->iface);

    c->super.iret = IDXGIAdapter3_GetDesc2(adapter->host, QEMU_G2H(c->desc));
}

#endif

struct qemu_dxgi_adapter_RegisterHardwareContentProtectionTeardownStatusEvent
{
    struct qemu_syscall super;
    uint64_t iface;
    uint64_t event;
    uint64_t cookie;
};

#ifdef QEMU_DLL_GUEST

static HRESULT STDMETHODCALLTYPE dxgi_adapter_RegisterHardwareContentProtectionTeardownStatusEvent(IDXGIAdapter3 *iface, HANDLE event, DWORD *cookie)
{
    struct qemu_dxgi_adapter_RegisterHardwareContentProtectionTeardownStatusEvent call;
    struct qemu_dxgi_adapter *adapter = impl_from_IDXGIAdapter3(iface);

    call.super.id = QEMU_SYSCALL_ID(CALL_DXGI_ADAPTER_REGISTERHARDWARECONTENTPROTECTIONTEARDOWNSTATUSEVENT);
    call.iface = (ULONG_PTR)adapter;
    call.event = (ULONG_PTR)event;
    call.cookie = (ULONG_PTR)cookie;

    qemu_syscall(&call.super);

    return call.super.iret;
}

#else

void qemu_dxgi_adapter_RegisterHardwareContentProtectionTeardownStatusEvent(struct qemu_syscall *call)
{
    struct qemu_dxgi_adapter_RegisterHardwareContentProtectionTeardownStatusEvent *c = (struct qemu_dxgi_adapter_RegisterHardwareContentProtectionTeardownStatusEvent *)call;
    struct qemu_dxgi_adapter *adapter;

    WINE_FIXME("Unverified!\n");
    adapter = QEMU_G2H(c->iface);

    c->super.iret = IDXGIAdapter3_RegisterHardwareContentProtectionTeardownStatusEvent(adapter->host, QEMU_G2H(c->event), QEMU_G2H(c->cookie));
}

#endif

struct qemu_dxgi_adapter_UnregisterHardwareContentProtectionTeardownStatus
{
    struct qemu_syscall super;
    uint64_t iface;
    uint64_t cookie;
};

#ifdef QEMU_DLL_GUEST

static void STDMETHODCALLTYPE dxgi_adapter_UnregisterHardwareContentProtectionTeardownStatus(IDXGIAdapter3 *iface, DWORD cookie)
{
    struct qemu_dxgi_adapter_UnregisterHardwareContentProtectionTeardownStatus call;
    struct qemu_dxgi_adapter *adapter = impl_from_IDXGIAdapter3(iface);

    call.super.id = QEMU_SYSCALL_ID(CALL_DXGI_ADAPTER_UNREGISTERHARDWARECONTENTPROTECTIONTEARDOWNSTATUS);
    call.iface = (ULONG_PTR)adapter;
    call.cookie = cookie;

    qemu_syscall(&call.super);
}

#else

void qemu_dxgi_adapter_UnregisterHardwareContentProtectionTeardownStatus(struct qemu_syscall *call)
{
    struct qemu_dxgi_adapter_UnregisterHardwareContentProtectionTeardownStatus *c = (struct qemu_dxgi_adapter_UnregisterHardwareContentProtectionTeardownStatus *)call;
    struct qemu_dxgi_adapter *adapter;

    WINE_FIXME("Unverified!\n");
    adapter = QEMU_G2H(c->iface);

    IDXGIAdapter3_UnregisterHardwareContentProtectionTeardownStatus(adapter->host, c->cookie);
}

#endif

struct qemu_dxgi_adapter_QueryVideoMemoryInfo
{
    struct qemu_syscall super;
    uint64_t iface;
    uint64_t node_index;
    uint64_t segment_group;
    uint64_t memory_info;
};

#ifdef QEMU_DLL_GUEST

static HRESULT STDMETHODCALLTYPE dxgi_adapter_QueryVideoMemoryInfo(IDXGIAdapter3 *iface, UINT node_index, DXGI_MEMORY_SEGMENT_GROUP segment_group, DXGI_QUERY_VIDEO_MEMORY_INFO *memory_info)
{
    struct qemu_dxgi_adapter_QueryVideoMemoryInfo call;
    struct qemu_dxgi_adapter *adapter = impl_from_IDXGIAdapter3(iface);

    call.super.id = QEMU_SYSCALL_ID(CALL_DXGI_ADAPTER_QUERYVIDEOMEMORYINFO);
    call.iface = (ULONG_PTR)adapter;
    call.node_index = node_index;
    call.segment_group = segment_group;
    call.memory_info = (ULONG_PTR)memory_info;

    qemu_syscall(&call.super);

    return call.super.iret;
}

#else

void qemu_dxgi_adapter_QueryVideoMemoryInfo(struct qemu_syscall *call)
{
    struct qemu_dxgi_adapter_QueryVideoMemoryInfo *c = (struct qemu_dxgi_adapter_QueryVideoMemoryInfo *)call;
    struct qemu_dxgi_adapter *adapter;

    WINE_FIXME("Unverified!\n");
    adapter = QEMU_G2H(c->iface);

    c->super.iret = IDXGIAdapter3_QueryVideoMemoryInfo(adapter->host, c->node_index, c->segment_group, QEMU_G2H(c->memory_info));
}

#endif

struct qemu_dxgi_adapter_SetVideoMemoryReservation
{
    struct qemu_syscall super;
    uint64_t iface;
    uint64_t node_index;
    uint64_t segment_group;
    uint64_t reservation;
};

#ifdef QEMU_DLL_GUEST

static HRESULT STDMETHODCALLTYPE dxgi_adapter_SetVideoMemoryReservation(IDXGIAdapter3 *iface, UINT node_index, DXGI_MEMORY_SEGMENT_GROUP segment_group, UINT64 reservation)
{
    struct qemu_dxgi_adapter_SetVideoMemoryReservation call;
    struct qemu_dxgi_adapter *adapter = impl_from_IDXGIAdapter3(iface);

    call.super.id = QEMU_SYSCALL_ID(CALL_DXGI_ADAPTER_SETVIDEOMEMORYRESERVATION);
    call.iface = (ULONG_PTR)adapter;
    call.node_index = node_index;
    call.segment_group = segment_group;
    call.reservation = reservation;

    qemu_syscall(&call.super);

    return call.super.iret;
}

#else

void qemu_dxgi_adapter_SetVideoMemoryReservation(struct qemu_syscall *call)
{
    struct qemu_dxgi_adapter_SetVideoMemoryReservation *c = (struct qemu_dxgi_adapter_SetVideoMemoryReservation *)call;
    struct qemu_dxgi_adapter *adapter;

    WINE_FIXME("Unverified!\n");
    adapter = QEMU_G2H(c->iface);

    c->super.iret = IDXGIAdapter3_SetVideoMemoryReservation(adapter->host, c->node_index, c->segment_group, c->reservation);
}

#endif

struct qemu_dxgi_adapter_RegisterVideoMemoryBudgetChangeNotificationEvent
{
    struct qemu_syscall super;
    uint64_t iface;
    uint64_t event;
    uint64_t cookie;
};

#ifdef QEMU_DLL_GUEST

static HRESULT STDMETHODCALLTYPE dxgi_adapter_RegisterVideoMemoryBudgetChangeNotificationEvent(IDXGIAdapter3 *iface, HANDLE event, DWORD *cookie)
{
    struct qemu_dxgi_adapter_RegisterVideoMemoryBudgetChangeNotificationEvent call;
    struct qemu_dxgi_adapter *adapter = impl_from_IDXGIAdapter3(iface);

    call.super.id = QEMU_SYSCALL_ID(CALL_DXGI_ADAPTER_REGISTERVIDEOMEMORYBUDGETCHANGENOTIFICATIONEVENT);
    call.iface = (ULONG_PTR)adapter;
    call.event = (ULONG_PTR)event;
    call.cookie = (ULONG_PTR)cookie;

    qemu_syscall(&call.super);

    return call.super.iret;
}

#else

void qemu_dxgi_adapter_RegisterVideoMemoryBudgetChangeNotificationEvent(struct qemu_syscall *call)
{
    struct qemu_dxgi_adapter_RegisterVideoMemoryBudgetChangeNotificationEvent *c = (struct qemu_dxgi_adapter_RegisterVideoMemoryBudgetChangeNotificationEvent *)call;
    struct qemu_dxgi_adapter *adapter;

    WINE_FIXME("Unverified!\n");
    adapter = QEMU_G2H(c->iface);

    c->super.iret = IDXGIAdapter3_RegisterVideoMemoryBudgetChangeNotificationEvent(adapter->host, QEMU_G2H(c->event), QEMU_G2H(c->cookie));
}

#endif

struct qemu_dxgi_adapter_UnregisterVideoMemoryBudgetChangeNotification
{
    struct qemu_syscall super;
    uint64_t iface;
    uint64_t cookie;
};

#ifdef QEMU_DLL_GUEST

static void STDMETHODCALLTYPE dxgi_adapter_UnregisterVideoMemoryBudgetChangeNotification(IDXGIAdapter3 *iface, DWORD cookie)
{
    struct qemu_dxgi_adapter_UnregisterVideoMemoryBudgetChangeNotification call;
    struct qemu_dxgi_adapter *adapter = impl_from_IDXGIAdapter3(iface);

    call.super.id = QEMU_SYSCALL_ID(CALL_DXGI_ADAPTER_UNREGISTERVIDEOMEMORYBUDGETCHANGENOTIFICATION);
    call.iface = (ULONG_PTR)adapter;
    call.cookie = cookie;

    qemu_syscall(&call.super);
}

#else

void qemu_dxgi_adapter_UnregisterVideoMemoryBudgetChangeNotification(struct qemu_syscall *call)
{
    struct qemu_dxgi_adapter_UnregisterVideoMemoryBudgetChangeNotification *c = (struct qemu_dxgi_adapter_UnregisterVideoMemoryBudgetChangeNotification *)call;
    struct qemu_dxgi_adapter *adapter;

    WINE_FIXME("Unverified!\n");
    adapter = QEMU_G2H(c->iface);

    IDXGIAdapter3_UnregisterVideoMemoryBudgetChangeNotification(adapter->host, c->cookie);
}

#endif

#ifdef QEMU_DLL_GUEST

static const struct
{
    IDXGIAdapter2Vtbl vtbl2;
    void *RegisterHardwareContentProtectionTeardownStatusEvent;
    void *UnregisterHardwareContentProtectionTeardownStatus;
    void *QueryVideoMemoryInfo;
    void *SetVideoMemoryReservation;
    void *RegisterVideoMemoryBudgetChangeNotificationEvent;
    void *UnregisterVideoMemoryBudgetChangeNotification;

}
dxgi_adapter_vtbl =
{
    {
        dxgi_adapter_QueryInterface,
        dxgi_adapter_AddRef,
        dxgi_adapter_Release,
        dxgi_adapter_SetPrivateData,
        dxgi_adapter_SetPrivateDataInterface,
        dxgi_adapter_GetPrivateData,
        dxgi_adapter_GetParent,
        /* IDXGIAdapter methods */
        dxgi_adapter_EnumOutputs,
        dxgi_adapter_GetDesc,
        dxgi_adapter_CheckInterfaceSupport,
        /* IDXGIAdapter1 methods */
        dxgi_adapter_GetDesc1,
        /* IDXGIAdapter2 methods */
        dxgi_adapter_GetDesc2,
    },
    /* IDXGIAdapter3 methods */
    dxgi_adapter_RegisterHardwareContentProtectionTeardownStatusEvent,
    dxgi_adapter_UnregisterHardwareContentProtectionTeardownStatus,
    dxgi_adapter_QueryVideoMemoryInfo,
    dxgi_adapter_SetVideoMemoryReservation,
    dxgi_adapter_RegisterVideoMemoryBudgetChangeNotificationEvent,
    dxgi_adapter_UnregisterVideoMemoryBudgetChangeNotification,
};

void qemu_dxgi_adapter_guest_init(struct qemu_dxgi_adapter *factory)
{
    factory->IDXGIAdapter3_iface.lpVtbl = &dxgi_adapter_vtbl.vtbl2;
}

#else

HRESULT qemu_dxgi_adapter_create(struct qemu_dxgi_factory *factory, UINT idx, struct qemu_dxgi_adapter **adapter)
{
    struct qemu_dxgi_adapter *obj;
    HRESULT hr;

    obj = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*obj));
    if (!obj)
    {
        WINE_WARN("Out of memory\n");
        return E_OUTOFMEMORY;
    }

    obj->factory = factory;

    hr = IDXGIFactory5_EnumAdapters1(factory->host, idx, (IDXGIAdapter1 **)&obj->host);
    if (FAILED(hr))
        HeapFree(GetProcessHeap(), 0, obj);

    *adapter = obj;
    return hr;
}

#endif
