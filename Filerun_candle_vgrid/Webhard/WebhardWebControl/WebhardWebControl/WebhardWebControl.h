

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* at Tue Oct 22 11:26:28 2019
 */
/* Compiler settings for .\WebhardWebControl.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __WebhardWebControl_h__
#define __WebhardWebControl_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IWebBBS_FWD_DEFINED__
#define __IWebBBS_FWD_DEFINED__
typedef interface IWebBBS IWebBBS;
#endif 	/* __IWebBBS_FWD_DEFINED__ */


#ifndef ___IWebBBSEvents_FWD_DEFINED__
#define ___IWebBBSEvents_FWD_DEFINED__
typedef interface _IWebBBSEvents _IWebBBSEvents;
#endif 	/* ___IWebBBSEvents_FWD_DEFINED__ */


#ifndef __WebBBS_FWD_DEFINED__
#define __WebBBS_FWD_DEFINED__

#ifdef __cplusplus
typedef class WebBBS WebBBS;
#else
typedef struct WebBBS WebBBS;
#endif /* __cplusplus */

#endif 	/* __WebBBS_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __IWebBBS_INTERFACE_DEFINED__
#define __IWebBBS_INTERFACE_DEFINED__

/* interface IWebBBS */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IWebBBS;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C037A761-74D9-4faf-B8E0-6F2CBEABA6CB")
    IWebBBS : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CheckInstall( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AllowPopUp( 
            /* [in] */ BSTR strTemp) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE MakeShortCut( 
            /* [in] */ BSTR strURL) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ShowDlgFile( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ShowDlgFolder( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SelectComplete( 
            BSTR strPath,
            BSTR strSize,
            BSTR strMode,
            BSTR strMD5) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE UpLoad( 
            BSTR strUserID,
            BSTR strUserPW,
            BSTR strType,
            BSTR strIdx) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DownLoad( 
            BSTR strUserID,
            BSTR strUserPW,
            BSTR strType,
            BSTR strIdx,
            BSTR strIdx_,
            BSTR strSeverIdx,
            BSTR strNameAuth) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OpenUpLoad( 
            BSTR file_name) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OpenDownLoad( 
            BSTR file_name) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ShowDlgOpenFile( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE UpdateOpenCheck( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ShowDlgOpenFolder( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE UpLoadHash( 
            BSTR strUserID,
            BSTR strUserPW,
            BSTR strType,
            BSTR strIdx,
            BSTR strHashIdx) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE VersionCheck( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FileVersionCheckVista( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Movie_to_Images( 
            SHORT load_count,
            BSTR url_domain,
            BSTR url_path,
            /* [retval][out] */ BSTR *Save_files_data) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE MtoI_exe( 
            BSTR Movie_filename,
            SHORT load_count,
            BSTR url_domain,
            BSTR url_path) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Movie_Capture( 
            BSTR Movie_filename,
            SHORT load_count,
            BSTR url_domain,
            BSTR url_path,
            SHORT Image_show_info,
            SHORT Image_show_size,
            /* [retval][out] */ BSTR *Save_files_data) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DownLoads( 
            BSTR strUserID,
            BSTR strUserPW,
            BSTR strType,
            BSTR strIdx,
            BSTR strIdx_,
            BSTR strSeverIdx,
            BSTR strNameAuth,
            BSTR strFileAuth) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWebBBSVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebBBS * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebBBS * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebBBS * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IWebBBS * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IWebBBS * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IWebBBS * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IWebBBS * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CheckInstall )( 
            IWebBBS * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AllowPopUp )( 
            IWebBBS * This,
            /* [in] */ BSTR strTemp);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *MakeShortCut )( 
            IWebBBS * This,
            /* [in] */ BSTR strURL);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ShowDlgFile )( 
            IWebBBS * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ShowDlgFolder )( 
            IWebBBS * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SelectComplete )( 
            IWebBBS * This,
            BSTR strPath,
            BSTR strSize,
            BSTR strMode,
            BSTR strMD5);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *UpLoad )( 
            IWebBBS * This,
            BSTR strUserID,
            BSTR strUserPW,
            BSTR strType,
            BSTR strIdx);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DownLoad )( 
            IWebBBS * This,
            BSTR strUserID,
            BSTR strUserPW,
            BSTR strType,
            BSTR strIdx,
            BSTR strIdx_,
            BSTR strSeverIdx,
            BSTR strNameAuth);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OpenUpLoad )( 
            IWebBBS * This,
            BSTR file_name);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OpenDownLoad )( 
            IWebBBS * This,
            BSTR file_name);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ShowDlgOpenFile )( 
            IWebBBS * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *UpdateOpenCheck )( 
            IWebBBS * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ShowDlgOpenFolder )( 
            IWebBBS * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *UpLoadHash )( 
            IWebBBS * This,
            BSTR strUserID,
            BSTR strUserPW,
            BSTR strType,
            BSTR strIdx,
            BSTR strHashIdx);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *VersionCheck )( 
            IWebBBS * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FileVersionCheckVista )( 
            IWebBBS * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Movie_to_Images )( 
            IWebBBS * This,
            SHORT load_count,
            BSTR url_domain,
            BSTR url_path,
            /* [retval][out] */ BSTR *Save_files_data);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *MtoI_exe )( 
            IWebBBS * This,
            BSTR Movie_filename,
            SHORT load_count,
            BSTR url_domain,
            BSTR url_path);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Movie_Capture )( 
            IWebBBS * This,
            BSTR Movie_filename,
            SHORT load_count,
            BSTR url_domain,
            BSTR url_path,
            SHORT Image_show_info,
            SHORT Image_show_size,
            /* [retval][out] */ BSTR *Save_files_data);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DownLoads )( 
            IWebBBS * This,
            BSTR strUserID,
            BSTR strUserPW,
            BSTR strType,
            BSTR strIdx,
            BSTR strIdx_,
            BSTR strSeverIdx,
            BSTR strNameAuth,
            BSTR strFileAuth);
        
        END_INTERFACE
    } IWebBBSVtbl;

    interface IWebBBS
    {
        CONST_VTBL struct IWebBBSVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebBBS_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWebBBS_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWebBBS_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWebBBS_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IWebBBS_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IWebBBS_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IWebBBS_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IWebBBS_CheckInstall(This)	\
    (This)->lpVtbl -> CheckInstall(This)

#define IWebBBS_AllowPopUp(This,strTemp)	\
    (This)->lpVtbl -> AllowPopUp(This,strTemp)

#define IWebBBS_MakeShortCut(This,strURL)	\
    (This)->lpVtbl -> MakeShortCut(This,strURL)

#define IWebBBS_ShowDlgFile(This)	\
    (This)->lpVtbl -> ShowDlgFile(This)

#define IWebBBS_ShowDlgFolder(This)	\
    (This)->lpVtbl -> ShowDlgFolder(This)

#define IWebBBS_SelectComplete(This,strPath,strSize,strMode,strMD5)	\
    (This)->lpVtbl -> SelectComplete(This,strPath,strSize,strMode,strMD5)

#define IWebBBS_UpLoad(This,strUserID,strUserPW,strType,strIdx)	\
    (This)->lpVtbl -> UpLoad(This,strUserID,strUserPW,strType,strIdx)

#define IWebBBS_DownLoad(This,strUserID,strUserPW,strType,strIdx,strIdx_,strSeverIdx,strNameAuth)	\
    (This)->lpVtbl -> DownLoad(This,strUserID,strUserPW,strType,strIdx,strIdx_,strSeverIdx,strNameAuth)

#define IWebBBS_OpenUpLoad(This,file_name)	\
    (This)->lpVtbl -> OpenUpLoad(This,file_name)

#define IWebBBS_OpenDownLoad(This,file_name)	\
    (This)->lpVtbl -> OpenDownLoad(This,file_name)

#define IWebBBS_ShowDlgOpenFile(This)	\
    (This)->lpVtbl -> ShowDlgOpenFile(This)

#define IWebBBS_UpdateOpenCheck(This)	\
    (This)->lpVtbl -> UpdateOpenCheck(This)

#define IWebBBS_ShowDlgOpenFolder(This)	\
    (This)->lpVtbl -> ShowDlgOpenFolder(This)

#define IWebBBS_UpLoadHash(This,strUserID,strUserPW,strType,strIdx,strHashIdx)	\
    (This)->lpVtbl -> UpLoadHash(This,strUserID,strUserPW,strType,strIdx,strHashIdx)

#define IWebBBS_VersionCheck(This)	\
    (This)->lpVtbl -> VersionCheck(This)

#define IWebBBS_FileVersionCheckVista(This)	\
    (This)->lpVtbl -> FileVersionCheckVista(This)

#define IWebBBS_Movie_to_Images(This,load_count,url_domain,url_path,Save_files_data)	\
    (This)->lpVtbl -> Movie_to_Images(This,load_count,url_domain,url_path,Save_files_data)

#define IWebBBS_MtoI_exe(This,Movie_filename,load_count,url_domain,url_path)	\
    (This)->lpVtbl -> MtoI_exe(This,Movie_filename,load_count,url_domain,url_path)

#define IWebBBS_Movie_Capture(This,Movie_filename,load_count,url_domain,url_path,Image_show_info,Image_show_size,Save_files_data)	\
    (This)->lpVtbl -> Movie_Capture(This,Movie_filename,load_count,url_domain,url_path,Image_show_info,Image_show_size,Save_files_data)

#define IWebBBS_DownLoads(This,strUserID,strUserPW,strType,strIdx,strIdx_,strSeverIdx,strNameAuth,strFileAuth)	\
    (This)->lpVtbl -> DownLoads(This,strUserID,strUserPW,strType,strIdx,strIdx_,strSeverIdx,strNameAuth,strFileAuth)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWebBBS_CheckInstall_Proxy( 
    IWebBBS * This);


void __RPC_STUB IWebBBS_CheckInstall_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWebBBS_AllowPopUp_Proxy( 
    IWebBBS * This,
    /* [in] */ BSTR strTemp);


void __RPC_STUB IWebBBS_AllowPopUp_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWebBBS_MakeShortCut_Proxy( 
    IWebBBS * This,
    /* [in] */ BSTR strURL);


void __RPC_STUB IWebBBS_MakeShortCut_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWebBBS_ShowDlgFile_Proxy( 
    IWebBBS * This);


void __RPC_STUB IWebBBS_ShowDlgFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWebBBS_ShowDlgFolder_Proxy( 
    IWebBBS * This);


void __RPC_STUB IWebBBS_ShowDlgFolder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWebBBS_SelectComplete_Proxy( 
    IWebBBS * This,
    BSTR strPath,
    BSTR strSize,
    BSTR strMode,
    BSTR strMD5);


void __RPC_STUB IWebBBS_SelectComplete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWebBBS_UpLoad_Proxy( 
    IWebBBS * This,
    BSTR strUserID,
    BSTR strUserPW,
    BSTR strType,
    BSTR strIdx);


void __RPC_STUB IWebBBS_UpLoad_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWebBBS_DownLoad_Proxy( 
    IWebBBS * This,
    BSTR strUserID,
    BSTR strUserPW,
    BSTR strType,
    BSTR strIdx,
    BSTR strIdx_,
    BSTR strSeverIdx,
    BSTR strNameAuth);


void __RPC_STUB IWebBBS_DownLoad_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWebBBS_OpenUpLoad_Proxy( 
    IWebBBS * This,
    BSTR file_name);


void __RPC_STUB IWebBBS_OpenUpLoad_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWebBBS_OpenDownLoad_Proxy( 
    IWebBBS * This,
    BSTR file_name);


void __RPC_STUB IWebBBS_OpenDownLoad_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWebBBS_ShowDlgOpenFile_Proxy( 
    IWebBBS * This);


void __RPC_STUB IWebBBS_ShowDlgOpenFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWebBBS_UpdateOpenCheck_Proxy( 
    IWebBBS * This);


void __RPC_STUB IWebBBS_UpdateOpenCheck_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWebBBS_ShowDlgOpenFolder_Proxy( 
    IWebBBS * This);


void __RPC_STUB IWebBBS_ShowDlgOpenFolder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWebBBS_UpLoadHash_Proxy( 
    IWebBBS * This,
    BSTR strUserID,
    BSTR strUserPW,
    BSTR strType,
    BSTR strIdx,
    BSTR strHashIdx);


void __RPC_STUB IWebBBS_UpLoadHash_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWebBBS_VersionCheck_Proxy( 
    IWebBBS * This);


void __RPC_STUB IWebBBS_VersionCheck_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWebBBS_FileVersionCheckVista_Proxy( 
    IWebBBS * This);


void __RPC_STUB IWebBBS_FileVersionCheckVista_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWebBBS_Movie_to_Images_Proxy( 
    IWebBBS * This,
    SHORT load_count,
    BSTR url_domain,
    BSTR url_path,
    /* [retval][out] */ BSTR *Save_files_data);


void __RPC_STUB IWebBBS_Movie_to_Images_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWebBBS_MtoI_exe_Proxy( 
    IWebBBS * This,
    BSTR Movie_filename,
    SHORT load_count,
    BSTR url_domain,
    BSTR url_path);


void __RPC_STUB IWebBBS_MtoI_exe_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWebBBS_Movie_Capture_Proxy( 
    IWebBBS * This,
    BSTR Movie_filename,
    SHORT load_count,
    BSTR url_domain,
    BSTR url_path,
    SHORT Image_show_info,
    SHORT Image_show_size,
    /* [retval][out] */ BSTR *Save_files_data);


void __RPC_STUB IWebBBS_Movie_Capture_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWebBBS_DownLoads_Proxy( 
    IWebBBS * This,
    BSTR strUserID,
    BSTR strUserPW,
    BSTR strType,
    BSTR strIdx,
    BSTR strIdx_,
    BSTR strSeverIdx,
    BSTR strNameAuth,
    BSTR strFileAuth);


void __RPC_STUB IWebBBS_DownLoads_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWebBBS_INTERFACE_DEFINED__ */



#ifndef __WebhardWebControlLib_LIBRARY_DEFINED__
#define __WebhardWebControlLib_LIBRARY_DEFINED__

/* library WebhardWebControlLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_WebhardWebControlLib;

#ifndef ___IWebBBSEvents_DISPINTERFACE_DEFINED__
#define ___IWebBBSEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IWebBBSEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IWebBBSEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("C037A763-74D9-4faf-B8E0-6F2CBEABA6CB")
    _IWebBBSEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IWebBBSEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IWebBBSEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IWebBBSEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IWebBBSEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IWebBBSEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IWebBBSEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IWebBBSEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IWebBBSEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IWebBBSEventsVtbl;

    interface _IWebBBSEvents
    {
        CONST_VTBL struct _IWebBBSEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IWebBBSEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _IWebBBSEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _IWebBBSEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _IWebBBSEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _IWebBBSEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _IWebBBSEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _IWebBBSEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IWebBBSEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_WebBBS;

#ifdef __cplusplus

class DECLSPEC_UUID("C037A764-74D9-4faf-B8E0-6F2CBEABA6CB")
WebBBS;
#endif
#endif /* __WebhardWebControlLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


