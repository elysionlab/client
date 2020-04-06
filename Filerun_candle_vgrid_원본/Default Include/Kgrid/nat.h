int READY_GRID       = 0;  // 준비
int CONNECTING_GRID  = 1;  // 연결중
int SENDING_GRID     = 2;  // 전송중
int COMPLETE_GRID    = 3;  // 완료
int ABORT_GRID       = 4;  // 중지
int SERVER_FAIL_GRID = 5;  // 서버연결실패
int FILE_EXIST_GRID  = 6;  // 받은파일
int FILE_WRITE_GRID  = 7;  // 파일쓰기 에러
int SPACELACK_GRID   = 8;  // 디스트 공간부족
int FILE_INIT_GRID   = 9;  // 파일 초기화 실패(디렉토리가 없거나, 잘못된경우)
int NOT_PEER_GRID    = 10;
int CUTSPPED_GRID    = 11;


//_StartDownload Return Value => Error State
int ERR_DISK_LACK    = -1;  //디스크 공간부족
int ERR_FILE_EXIST   = -2;  //파일이 존재할때
int ERR_FILE_INIT    = -3;  //파일 초기화 실패
int ERR_DOWN_FAILE   = -4;  //다운로드 실패
int ERR_WANT_PEER    = -5;  //원하는 피어수가 안될때.


extern "C" {
void __declspec(dllimport) _Init();
void __declspec(dllimport) _UnInit();
int __declspec(dllimport) _StartDownload(char* ServerIP, char* ServerFilename, char* SiteName, __int64 SiteFileID, char* Filename, __int64 Filesize, int WantPeerCount );
int __declspec(dllimport) _StartDownload2(char* SiteName, __int64 SiteFileID, char* Filename, __int64 Filesize, int WantPeerCount);
int __declspec(dllimport) _StartDownload3(char* SiteName, __int64 SiteFileID, char* Filename, __int64 Filesize, int WantPeerCount);
void __declspec(dllimport) _StopDownload();
double __declspec(dllimport) _Speed(int* RemainTime);
int __declspec(dllimport) _Status();
__int64 __declspec(dllimport) _ReceiveSize();
__int64 __declspec(dllimport) _GetCurrentFileSize();
bool __declspec(dllimport) _DownloadCompleteFile(char* SiteID, __int64 SiteFileID, char* LocalFilename, __int64 Filesize);
void __declspec(dllimport) _SetFileServerPeerCount(int PeerCnt);
bool __declspec(dllimport) _AddServer(char* ServerIP, int Port, char* LocalFilename, __int64 Filesize);
void __declspec(dllimport) _GetTraffic(__int64* ServerTraffic, __int64* PeerTraffic);
int __declspec(dllimport) _ExistFile(char* ServerIP, char* LocalFilename, __int64 Filesize);
void __declspec(dllimport) _UseDebugView(bool use);
bool __declspec(dllimport) _SetDownloadSpeed(int MaxSpeed); // default=0, 40=40K, 50=50KByte/sec, 100=100KByte/sec
void __declspec(dllimport) _NotUseServiceApp();
void __declspec(dllimport) _AutoSetupServiceApp(bool use);
void __declspec(dllimport) _SetServerConnectCount(int count);
int __declspec(dllimport) _GetLivePeerCount(char* ServerIP, char* ServerFilename, char* SiteName, __int64 SiteFileID, __int64 Filesize);
int __declspec(dllimport) _StartDownloadForLivePeer(char* Filename, __int64 Filesize);
bool __declspec(dllimport) _AppendServer(char* ServerIP, int Port, char* LocalFilename, __int64 Filesize);
void __declspec(dllimport) _GetCurrentPeerInfo(int* ServerPeerCnt, double* ServerPeerSpeed,int* PeerCnt, double* PeerSpeed);
void __declspec(dllimport) _SuspendDownload();
void __declspec(dllimport) _ResumeDownload();
void __declspec(dllimport) _SetPort(int DownPort);

//2016-02-28 NeoHash
int __declspec(dllimport) _StartDownloadW_Neo(WCHAR* SiteName, WCHAR* ServerIP, WCHAR* ServerFilename,  __int64 SiteFileID, WCHAR* Filename, __int64 Filesize, WCHAR* NeoHash, int WantPeerCount );

// WCHAR Support Function
int __declspec(dllimport) _StartDownloadW(WCHAR* ServerIP, WCHAR* ServerFilename, WCHAR* SiteName, __int64 SiteFileID, WCHAR* Filename, __int64 Filesize, int WantPeerCount );
int __declspec(dllimport) _StartDownload2W(WCHAR* SiteName, __int64 SiteFileID, WCHAR* Filename, __int64 Filesize, int WantPeerCount);
bool __declspec(dllimport) _AddServerW(WCHAR* ServerIP, int Port, WCHAR* LocalFilename, __int64 Filesize);
bool __declspec(dllimport) _SetInitServerPeerConnectCount(int Count);
int __declspec(dllimport) _GetMaxSpeed();
bool __declspec(dllimport) _SetCutSpeed(int CutSpeed);
//WCHAR* __declspec(dllimport) _GetKGridHashW(WCHAR* Filename, __int64 Filesize);
int __declspec(dllimport) _StartDownloadW_KHash(WCHAR* SiteName, WCHAR* AHash, WCHAR* Filename, __int64 Filesize );


// MultiDownload Support Function
int     __declspec(dllimport) _New_KGRID();
bool    __declspec(dllimport) _Delete_KGRID(int Handle);
bool    __declspec(dllimport) _Init_KGRID(int Handle);
bool    __declspec(dllimport) _UnInit_KGRID(int Handle);
int     __declspec(dllimport) _StartDownload_KGRID(int Handle, char* ServerIP, char* ServerFilename, char* SiteName, __int64 SiteFileID, char* Filename, __int64 Filesize, int WantPeerCount );
void    __declspec(dllimport) _StopDownload_KGRID(int Handle);
int     __declspec(dllimport) _Status_KGRID(int Handle);
double  __declspec(dllimport) _Speed_KGRID(int Handle, int* RemainTime);
__int64 __declspec(dllimport) _ReceiveSize_KGRID(int Handle);
__int64 __declspec(dllimport) _GetCurrentFileSize_KGRID(int Handle);
void    __declspec(dllimport) _GetTraffic_KGRID(int Handle,__int64* ServerTraffic, __int64* PeerTraffic);
bool    __declspec(dllimport) _SetDownloadSpeed_KGRID(int Handle, int MaxSpeed); // default=0, 40=40K, 50=50KByte/sec, 100=100KByte/sec

}