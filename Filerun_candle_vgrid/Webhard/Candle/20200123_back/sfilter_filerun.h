#ifndef _SFILTER_FILERUN_
#define _SFILTER_FILERUN_

#include "sfilter_sitecommon.h"

#define SFILTER_FILERUN_DLL_NAME	_T("sffilerun.dll")

// DLL export functions
typedef int  (WINAPI* SFILTERInit)(int unzip_enable, int unzip_site_status_dlg);
typedef int  (WINAPI* SFILTERUpload)(SFILTER_RET** pp_sfilter_ret, SFILTER_RET_MUSIC** pp_sfilter_ret_music, SFILTER_RET_VIDEO** pp_sfilter_ret_video, char* uploaid, char* filepath, int bbs_idx, char *bbs_title);
typedef int  (WINAPI* SFILTERDownload)(SFILTER_RET** pp_sfilter_ret, SFILTER_RET_MUSIC** pp_sfilter_ret_music, SFILTER_RET_VIDEO** pp_sfilter_ret_video, char* downloadid, char* sfilter_hash, int bbs_idx, char* sfilter_1hash, char* sfilter_rehash, char *bbs_title);
typedef int  (WINAPI* SFILTERDownCancel)(char* downloadid, char* sfilter_hash);
typedef int  (WINAPI* SFILTERVideoPaidReport)(char* content_id, char* site_paid_datetime, char* site_paid_price, char* site_paid_userid, char* site_paid_no , char* site_sell_userid, char* filtering_key, char* ch_content_id, int bbs_idx, char* bbs_title/* 선택 */ );
typedef int  (WINAPI* SFILTERUnzipStatus)(char* unzip_current_filename);
typedef void (WINAPI* SFILTERUnzipCancel)();
typedef int  (WINAPI* SFILTERGetFileFormat)(char* filepath, SFILTER_RET_FILEFORMAT* p_sfilter_ret_fileformat);
typedef int  (WINAPI* SFILTEREnd)(SFILTER_RET* p_sfilte_ret, SFILTER_RET_MUSIC* p_sfilter_ret_music, SFILTER_RET_VIDEO* p_sfilter_ret_video);

// 바이러스검사관련(터보)
typedef int  (WINAPI* SFILTERGetUnzipDirPath)(TCHAR* szUnzipDirPath);
typedef int  (WINAPI* SFILTERRemoveUnzipDir)();
typedef int  (WINAPI* SFILTERVirusScan_Init)(BOOL bWarningMsgOn);
typedef int  (WINAPI* SFILTERVirusScan_Scan)(TCHAR* strFilePath,TCHAR* strVirusCode);
typedef int  (WINAPI* SFILTERVirusScan_Uninit)();	
typedef int  (WINAPI* SFILTERVirusUpdate_SetVUpdateCycle)(INT nCycle, INT nValue);
typedef int  (WINAPI* SFILTERVirusUpdate_VUpdate)();
typedef int  (WINAPI* SFILTERVirusUpdateNow)();


#endif // _SFILTER_FILERUN_