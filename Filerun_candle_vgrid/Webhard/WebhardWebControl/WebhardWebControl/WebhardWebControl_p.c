

/* this ALWAYS GENERATED file contains the proxy stub code */


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

#if !defined(_M_IA64) && !defined(_M_AMD64)


#pragma warning( disable: 4049 )  /* more than 64k source lines */
#if _MSC_VER >= 1200
#pragma warning(push)
#endif
#pragma warning( disable: 4100 ) /* unreferenced arguments in x86 call */
#pragma warning( disable: 4211 )  /* redefine extent to static */
#pragma warning( disable: 4232 )  /* dllimport identity*/
#pragma optimize("", off ) 

#define USE_STUBLESS_PROXY


/* verify that the <rpcproxy.h> version is high enough to compile this file*/
#ifndef __REDQ_RPCPROXY_H_VERSION__
#define __REQUIRED_RPCPROXY_H_VERSION__ 440
#endif


#include "rpcproxy.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
#endif // __RPCPROXY_H_VERSION__


#include "WebhardWebControl.h"

#define TYPE_FORMAT_STRING_SIZE   55                                
#define PROC_FORMAT_STRING_SIZE   723                               
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   1            

typedef struct _MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } MIDL_TYPE_FORMAT_STRING;

typedef struct _MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } MIDL_PROC_FORMAT_STRING;


static RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax = 
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};


extern const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString;
extern const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IWebBBS_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IWebBBS_ProxyInfo;


extern const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ];

#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

#if !(TARGET_IS_NT40_OR_LATER)
#error You need a Windows NT 4.0 or later to run this stub because it uses these features:
#error   -Oif or -Oicf, [wire_marshal] or [user_marshal] attribute.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will die there with the RPC_X_WRONG_STUB_VERSION error.
#endif


static const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure CheckInstall */

			0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x7 ),	/* 7 */
/*  8 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 10 */	NdrFcShort( 0x0 ),	/* 0 */
/* 12 */	NdrFcShort( 0x8 ),	/* 8 */
/* 14 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */

/* 16 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 18 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 20 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure AllowPopUp */

/* 22 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 24 */	NdrFcLong( 0x0 ),	/* 0 */
/* 28 */	NdrFcShort( 0x8 ),	/* 8 */
/* 30 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 32 */	NdrFcShort( 0x0 ),	/* 0 */
/* 34 */	NdrFcShort( 0x8 ),	/* 8 */
/* 36 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter strTemp */

/* 38 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 40 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 42 */	NdrFcShort( 0x1a ),	/* Type Offset=26 */

	/* Return value */

/* 44 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 46 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 48 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure MakeShortCut */

/* 50 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 52 */	NdrFcLong( 0x0 ),	/* 0 */
/* 56 */	NdrFcShort( 0x9 ),	/* 9 */
/* 58 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 60 */	NdrFcShort( 0x0 ),	/* 0 */
/* 62 */	NdrFcShort( 0x8 ),	/* 8 */
/* 64 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter strURL */

/* 66 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 68 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 70 */	NdrFcShort( 0x1a ),	/* Type Offset=26 */

	/* Return value */

/* 72 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 74 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 76 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ShowDlgFile */

/* 78 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 80 */	NdrFcLong( 0x0 ),	/* 0 */
/* 84 */	NdrFcShort( 0xa ),	/* 10 */
/* 86 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 88 */	NdrFcShort( 0x0 ),	/* 0 */
/* 90 */	NdrFcShort( 0x8 ),	/* 8 */
/* 92 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */

/* 94 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 96 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 98 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ShowDlgFolder */

/* 100 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 102 */	NdrFcLong( 0x0 ),	/* 0 */
/* 106 */	NdrFcShort( 0xb ),	/* 11 */
/* 108 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 110 */	NdrFcShort( 0x0 ),	/* 0 */
/* 112 */	NdrFcShort( 0x8 ),	/* 8 */
/* 114 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */

/* 116 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 118 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 120 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SelectComplete */

/* 122 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 124 */	NdrFcLong( 0x0 ),	/* 0 */
/* 128 */	NdrFcShort( 0xc ),	/* 12 */
/* 130 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 132 */	NdrFcShort( 0x0 ),	/* 0 */
/* 134 */	NdrFcShort( 0x8 ),	/* 8 */
/* 136 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x5,		/* 5 */

	/* Parameter strPath */

/* 138 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 140 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 142 */	NdrFcShort( 0x1a ),	/* Type Offset=26 */

	/* Parameter strSize */

/* 144 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 146 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 148 */	NdrFcShort( 0x1a ),	/* Type Offset=26 */

	/* Parameter strMode */

/* 150 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 152 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 154 */	NdrFcShort( 0x1a ),	/* Type Offset=26 */

	/* Parameter strMD5 */

/* 156 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 158 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 160 */	NdrFcShort( 0x1a ),	/* Type Offset=26 */

	/* Return value */

/* 162 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 164 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 166 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure UpLoad */

/* 168 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 170 */	NdrFcLong( 0x0 ),	/* 0 */
/* 174 */	NdrFcShort( 0xd ),	/* 13 */
/* 176 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 178 */	NdrFcShort( 0x0 ),	/* 0 */
/* 180 */	NdrFcShort( 0x8 ),	/* 8 */
/* 182 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x5,		/* 5 */

	/* Parameter strUserID */

/* 184 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 186 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 188 */	NdrFcShort( 0x1a ),	/* Type Offset=26 */

	/* Parameter strUserPW */

/* 190 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 192 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 194 */	NdrFcShort( 0x1a ),	/* Type Offset=26 */

	/* Parameter strType */

/* 196 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 198 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 200 */	NdrFcShort( 0x1a ),	/* Type Offset=26 */

	/* Parameter strIdx */

/* 202 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 204 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 206 */	NdrFcShort( 0x1a ),	/* Type Offset=26 */

	/* Return value */

/* 208 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 210 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 212 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DownLoad */

/* 214 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 216 */	NdrFcLong( 0x0 ),	/* 0 */
/* 220 */	NdrFcShort( 0xe ),	/* 14 */
/* 222 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 224 */	NdrFcShort( 0x0 ),	/* 0 */
/* 226 */	NdrFcShort( 0x8 ),	/* 8 */
/* 228 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x8,		/* 8 */

	/* Parameter strUserID */

/* 230 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 232 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 234 */	NdrFcShort( 0x1a ),	/* Type Offset=26 */

	/* Parameter strUserPW */

/* 236 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 238 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 240 */	NdrFcShort( 0x1a ),	/* Type Offset=26 */

	/* Parameter strType */

/* 242 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 244 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 246 */	NdrFcShort( 0x1a ),	/* Type Offset=26 */

	/* Parameter strIdx */

/* 248 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 250 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 252 */	NdrFcShort( 0x1a ),	/* Type Offset=26 */

	/* Parameter strIdx_ */

/* 254 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 256 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 258 */	NdrFcShort( 0x1a ),	/* Type Offset=26 */

	/* Parameter strSeverIdx */

/* 260 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 262 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 264 */	NdrFcShort( 0x1a ),	/* Type Offset=26 */

	/* Parameter strNameAuth */

/* 266 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 268 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 270 */	NdrFcShort( 0x1a ),	/* Type Offset=26 */

	/* Return value */

/* 272 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 274 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 276 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OpenUpLoad */

/* 278 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 280 */	NdrFcLong( 0x0 ),	/* 0 */
/* 284 */	NdrFcShort( 0xf ),	/* 15 */
/* 286 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 288 */	NdrFcShort( 0x0 ),	/* 0 */
/* 290 */	NdrFcShort( 0x8 ),	/* 8 */
/* 292 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter file_name */

/* 294 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 296 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 298 */	NdrFcShort( 0x1a ),	/* Type Offset=26 */

	/* Return value */

/* 300 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 302 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 304 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OpenDownLoad */

/* 306 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 308 */	NdrFcLong( 0x0 ),	/* 0 */
/* 312 */	NdrFcShort( 0x10 ),	/* 16 */
/* 314 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 316 */	NdrFcShort( 0x0 ),	/* 0 */
/* 318 */	NdrFcShort( 0x8 ),	/* 8 */
/* 320 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x2,		/* 2 */

	/* Parameter file_name */

/* 322 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 324 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 326 */	NdrFcShort( 0x1a ),	/* Type Offset=26 */

	/* Return value */

/* 328 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 330 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 332 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ShowDlgOpenFile */

/* 334 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 336 */	NdrFcLong( 0x0 ),	/* 0 */
/* 340 */	NdrFcShort( 0x11 ),	/* 17 */
/* 342 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 344 */	NdrFcShort( 0x0 ),	/* 0 */
/* 346 */	NdrFcShort( 0x8 ),	/* 8 */
/* 348 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */

/* 350 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 352 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 354 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure UpdateOpenCheck */

/* 356 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 358 */	NdrFcLong( 0x0 ),	/* 0 */
/* 362 */	NdrFcShort( 0x12 ),	/* 18 */
/* 364 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 366 */	NdrFcShort( 0x0 ),	/* 0 */
/* 368 */	NdrFcShort( 0x8 ),	/* 8 */
/* 370 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */

/* 372 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 374 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 376 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ShowDlgOpenFolder */

/* 378 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 380 */	NdrFcLong( 0x0 ),	/* 0 */
/* 384 */	NdrFcShort( 0x13 ),	/* 19 */
/* 386 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 388 */	NdrFcShort( 0x0 ),	/* 0 */
/* 390 */	NdrFcShort( 0x8 ),	/* 8 */
/* 392 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */

/* 394 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 396 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 398 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure UpLoadHash */

/* 400 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 402 */	NdrFcLong( 0x0 ),	/* 0 */
/* 406 */	NdrFcShort( 0x14 ),	/* 20 */
/* 408 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 410 */	NdrFcShort( 0x0 ),	/* 0 */
/* 412 */	NdrFcShort( 0x8 ),	/* 8 */
/* 414 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x6,		/* 6 */

	/* Parameter strUserID */

/* 416 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 418 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 420 */	NdrFcShort( 0x1a ),	/* Type Offset=26 */

	/* Parameter strUserPW */

/* 422 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 424 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 426 */	NdrFcShort( 0x1a ),	/* Type Offset=26 */

	/* Parameter strType */

/* 428 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 430 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 432 */	NdrFcShort( 0x1a ),	/* Type Offset=26 */

	/* Parameter strIdx */

/* 434 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 436 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 438 */	NdrFcShort( 0x1a ),	/* Type Offset=26 */

	/* Parameter strHashIdx */

/* 440 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 442 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 444 */	NdrFcShort( 0x1a ),	/* Type Offset=26 */

	/* Return value */

/* 446 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 448 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 450 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure VersionCheck */

/* 452 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 454 */	NdrFcLong( 0x0 ),	/* 0 */
/* 458 */	NdrFcShort( 0x15 ),	/* 21 */
/* 460 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 462 */	NdrFcShort( 0x0 ),	/* 0 */
/* 464 */	NdrFcShort( 0x8 ),	/* 8 */
/* 466 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */

/* 468 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 470 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 472 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure FileVersionCheckVista */

/* 474 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 476 */	NdrFcLong( 0x0 ),	/* 0 */
/* 480 */	NdrFcShort( 0x16 ),	/* 22 */
/* 482 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 484 */	NdrFcShort( 0x0 ),	/* 0 */
/* 486 */	NdrFcShort( 0x8 ),	/* 8 */
/* 488 */	0x4,		/* Oi2 Flags:  has return, */
			0x1,		/* 1 */

	/* Return value */

/* 490 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 492 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 494 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Movie_to_Images */

/* 496 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 498 */	NdrFcLong( 0x0 ),	/* 0 */
/* 502 */	NdrFcShort( 0x17 ),	/* 23 */
/* 504 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 506 */	NdrFcShort( 0x6 ),	/* 6 */
/* 508 */	NdrFcShort( 0x8 ),	/* 8 */
/* 510 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x5,		/* 5 */

	/* Parameter load_count */

/* 512 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 514 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 516 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Parameter url_domain */

/* 518 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 520 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 522 */	NdrFcShort( 0x1a ),	/* Type Offset=26 */

	/* Parameter url_path */

/* 524 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 526 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 528 */	NdrFcShort( 0x1a ),	/* Type Offset=26 */

	/* Parameter Save_files_data */

/* 530 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 532 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 534 */	NdrFcShort( 0x2c ),	/* Type Offset=44 */

	/* Return value */

/* 536 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 538 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 540 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure MtoI_exe */

/* 542 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 544 */	NdrFcLong( 0x0 ),	/* 0 */
/* 548 */	NdrFcShort( 0x18 ),	/* 24 */
/* 550 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 552 */	NdrFcShort( 0x6 ),	/* 6 */
/* 554 */	NdrFcShort( 0x8 ),	/* 8 */
/* 556 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x5,		/* 5 */

	/* Parameter Movie_filename */

/* 558 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 560 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 562 */	NdrFcShort( 0x1a ),	/* Type Offset=26 */

	/* Parameter load_count */

/* 564 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 566 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 568 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Parameter url_domain */

/* 570 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 572 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 574 */	NdrFcShort( 0x1a ),	/* Type Offset=26 */

	/* Parameter url_path */

/* 576 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 578 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 580 */	NdrFcShort( 0x1a ),	/* Type Offset=26 */

	/* Return value */

/* 582 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 584 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 586 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Movie_Capture */

/* 588 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 590 */	NdrFcLong( 0x0 ),	/* 0 */
/* 594 */	NdrFcShort( 0x19 ),	/* 25 */
/* 596 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 598 */	NdrFcShort( 0x12 ),	/* 18 */
/* 600 */	NdrFcShort( 0x8 ),	/* 8 */
/* 602 */	0x7,		/* Oi2 Flags:  srv must size, clt must size, has return, */
			0x8,		/* 8 */

	/* Parameter Movie_filename */

/* 604 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 606 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 608 */	NdrFcShort( 0x1a ),	/* Type Offset=26 */

	/* Parameter load_count */

/* 610 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 612 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 614 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Parameter url_domain */

/* 616 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 618 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 620 */	NdrFcShort( 0x1a ),	/* Type Offset=26 */

	/* Parameter url_path */

/* 622 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 624 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 626 */	NdrFcShort( 0x1a ),	/* Type Offset=26 */

	/* Parameter Image_show_info */

/* 628 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 630 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 632 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Parameter Image_show_size */

/* 634 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 636 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 638 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Parameter Save_files_data */

/* 640 */	NdrFcShort( 0x2113 ),	/* Flags:  must size, must free, out, simple ref, srv alloc size=8 */
/* 642 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 644 */	NdrFcShort( 0x2c ),	/* Type Offset=44 */

	/* Return value */

/* 646 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 648 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 650 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DownLoads */

/* 652 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 654 */	NdrFcLong( 0x0 ),	/* 0 */
/* 658 */	NdrFcShort( 0x1a ),	/* 26 */
/* 660 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
/* 662 */	NdrFcShort( 0x0 ),	/* 0 */
/* 664 */	NdrFcShort( 0x8 ),	/* 8 */
/* 666 */	0x6,		/* Oi2 Flags:  clt must size, has return, */
			0x9,		/* 9 */

	/* Parameter strUserID */

/* 668 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 670 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 672 */	NdrFcShort( 0x1a ),	/* Type Offset=26 */

	/* Parameter strUserPW */

/* 674 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 676 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 678 */	NdrFcShort( 0x1a ),	/* Type Offset=26 */

	/* Parameter strType */

/* 680 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 682 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 684 */	NdrFcShort( 0x1a ),	/* Type Offset=26 */

	/* Parameter strIdx */

/* 686 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 688 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 690 */	NdrFcShort( 0x1a ),	/* Type Offset=26 */

	/* Parameter strIdx_ */

/* 692 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 694 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 696 */	NdrFcShort( 0x1a ),	/* Type Offset=26 */

	/* Parameter strSeverIdx */

/* 698 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 700 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 702 */	NdrFcShort( 0x1a ),	/* Type Offset=26 */

	/* Parameter strNameAuth */

/* 704 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 706 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 708 */	NdrFcShort( 0x1a ),	/* Type Offset=26 */

	/* Parameter strFileAuth */

/* 710 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 712 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 714 */	NdrFcShort( 0x1a ),	/* Type Offset=26 */

	/* Return value */

/* 716 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 718 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 720 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

			0x0
        }
    };

static const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */
/*  2 */	
			0x12, 0x0,	/* FC_UP */
/*  4 */	NdrFcShort( 0xc ),	/* Offset= 12 (16) */
/*  6 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/*  8 */	NdrFcShort( 0x2 ),	/* 2 */
/* 10 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 12 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 14 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 16 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 18 */	NdrFcShort( 0x8 ),	/* 8 */
/* 20 */	NdrFcShort( 0xfff2 ),	/* Offset= -14 (6) */
/* 22 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 24 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 26 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 28 */	NdrFcShort( 0x0 ),	/* 0 */
/* 30 */	NdrFcShort( 0x4 ),	/* 4 */
/* 32 */	NdrFcShort( 0x0 ),	/* 0 */
/* 34 */	NdrFcShort( 0xffe0 ),	/* Offset= -32 (2) */
/* 36 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 38 */	NdrFcShort( 0x6 ),	/* Offset= 6 (44) */
/* 40 */	
			0x13, 0x0,	/* FC_OP */
/* 42 */	NdrFcShort( 0xffe6 ),	/* Offset= -26 (16) */
/* 44 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 46 */	NdrFcShort( 0x0 ),	/* 0 */
/* 48 */	NdrFcShort( 0x4 ),	/* 4 */
/* 50 */	NdrFcShort( 0x0 ),	/* 0 */
/* 52 */	NdrFcShort( 0xfff4 ),	/* Offset= -12 (40) */

			0x0
        }
    };

static const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ] = 
        {
            
            {
            BSTR_UserSize
            ,BSTR_UserMarshal
            ,BSTR_UserUnmarshal
            ,BSTR_UserFree
            }

        };



/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IDispatch, ver. 0.0,
   GUID={0x00020400,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IWebBBS, ver. 0.0,
   GUID={0xC037A761,0x74D9,0x4faf,{0xB8,0xE0,0x6F,0x2C,0xBE,0xAB,0xA6,0xCB}} */

#pragma code_seg(".orpc")
static const unsigned short IWebBBS_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    22,
    50,
    78,
    100,
    122,
    168,
    214,
    278,
    306,
    334,
    356,
    378,
    400,
    452,
    474,
    496,
    542,
    588,
    652
    };

static const MIDL_STUBLESS_PROXY_INFO IWebBBS_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IWebBBS_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IWebBBS_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IWebBBS_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(27) _IWebBBSProxyVtbl = 
{
    &IWebBBS_ProxyInfo,
    &IID_IWebBBS,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IWebBBS::CheckInstall */ ,
    (void *) (INT_PTR) -1 /* IWebBBS::AllowPopUp */ ,
    (void *) (INT_PTR) -1 /* IWebBBS::MakeShortCut */ ,
    (void *) (INT_PTR) -1 /* IWebBBS::ShowDlgFile */ ,
    (void *) (INT_PTR) -1 /* IWebBBS::ShowDlgFolder */ ,
    (void *) (INT_PTR) -1 /* IWebBBS::SelectComplete */ ,
    (void *) (INT_PTR) -1 /* IWebBBS::UpLoad */ ,
    (void *) (INT_PTR) -1 /* IWebBBS::DownLoad */ ,
    (void *) (INT_PTR) -1 /* IWebBBS::OpenUpLoad */ ,
    (void *) (INT_PTR) -1 /* IWebBBS::OpenDownLoad */ ,
    (void *) (INT_PTR) -1 /* IWebBBS::ShowDlgOpenFile */ ,
    (void *) (INT_PTR) -1 /* IWebBBS::UpdateOpenCheck */ ,
    (void *) (INT_PTR) -1 /* IWebBBS::ShowDlgOpenFolder */ ,
    (void *) (INT_PTR) -1 /* IWebBBS::UpLoadHash */ ,
    (void *) (INT_PTR) -1 /* IWebBBS::VersionCheck */ ,
    (void *) (INT_PTR) -1 /* IWebBBS::FileVersionCheckVista */ ,
    (void *) (INT_PTR) -1 /* IWebBBS::Movie_to_Images */ ,
    (void *) (INT_PTR) -1 /* IWebBBS::MtoI_exe */ ,
    (void *) (INT_PTR) -1 /* IWebBBS::Movie_Capture */ ,
    (void *) (INT_PTR) -1 /* IWebBBS::DownLoads */
};


static const PRPC_STUB_FUNCTION IWebBBS_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IWebBBSStubVtbl =
{
    &IID_IWebBBS,
    &IWebBBS_ServerInfo,
    27,
    &IWebBBS_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};

static const MIDL_STUB_DESC Object_StubDesc = 
    {
    0,
    NdrOleAllocate,
    NdrOleFree,
    0,
    0,
    0,
    0,
    0,
    __MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x20000, /* Ndr library version */
    0,
    0x600016e, /* MIDL Version 6.0.366 */
    0,
    UserMarshalRoutines,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0   /* Reserved5 */
    };

const CInterfaceProxyVtbl * _WebhardWebControl_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_IWebBBSProxyVtbl,
    0
};

const CInterfaceStubVtbl * _WebhardWebControl_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_IWebBBSStubVtbl,
    0
};

PCInterfaceName const _WebhardWebControl_InterfaceNamesList[] = 
{
    "IWebBBS",
    0
};

const IID *  _WebhardWebControl_BaseIIDList[] = 
{
    &IID_IDispatch,
    0
};


#define _WebhardWebControl_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _WebhardWebControl, pIID, n)

int __stdcall _WebhardWebControl_IID_Lookup( const IID * pIID, int * pIndex )
{
    
    if(!_WebhardWebControl_CHECK_IID(0))
        {
        *pIndex = 0;
        return 1;
        }

    return 0;
}

const ExtendedProxyFileInfo WebhardWebControl_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _WebhardWebControl_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _WebhardWebControl_StubVtblList,
    (const PCInterfaceName * ) & _WebhardWebControl_InterfaceNamesList,
    (const IID ** ) & _WebhardWebControl_BaseIIDList,
    & _WebhardWebControl_IID_Lookup, 
    1,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};
#pragma optimize("", on )
#if _MSC_VER >= 1200
#pragma warning(pop)
#endif


#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

