#pragma once


enum DOWNLOAD_STATUS
{
	DOWN_READY =0,
	DOWN_ING ,
	DOWN_FINISH,
	DOWN_PAUSE,
	DOWN_ERROR    //cdn, member 파일 없음(보류 처리로 다운 완료후 재시도 필요)
};

enum DOWNLOAD_ERR
{
	ERR_DISK_SPACE		= -1,  //디스크 공간부족
	ERR_FILE_NOT_EXIST  = -2,  //파일 없음
	ERR_VDOWN_FAILE		= -3,  //다운로드 실패(다운로중, 파일 블록 나누기 실패)
	ERR_START_POSITION	= -4	,	//잘못된 시작위치
	ERR_FILE_CREATE		= -5	,	//파일 생성 실패
	ERR_START_FAIL		= -6	,//이미 다운로드 시작 되었음  
	ERR_ETC_FAIL		= -7
};



//download
__declspec(dllexport)  void InitNodeLib(char* site_code);

/*
type : 1  => vgrid 데몬 사용(ds)(미지원)
type : 2  => vgrid 데몬대신 ftp 서버를 이용(지원예정)
type : 3  => vgrid 데몬 사용
type : 5  => vgrid 데몬없이 사용  
*/
 __declspec(dllexport) void	SetDownMainSvrInfo(char* ip, int port, int type=3);
__declspec(dllexport) void	SetSearchSvrDNS(char* search_dns);
//__declspec(dllexport) int	SetDownMainSvrInfoEx(char* ip, int port,char* id, char*pwd, int type=2);

/*
_hash : hash_size 인 경우 hash만 사용( 웹하드 hash, kth hash 공통)

flag : 10 => 주로 웹하드에서 사용하는 MD5 해쉬
flag : 30 => kth 해쉬
flag : 40 => 해당사항 없음

*/
__declspec(dllexport) void	SetDownloadFileExW(wchar_t* down_path, wchar_t* storage_full_name, wchar_t* real_name, __int64 size, wchar_t* _hash, int flag=10);
__declspec(dllexport) void	SetDownloadFileEx(wchar_t* down_path, wchar_t* storage_full_name, wchar_t* real_name, __int64 size, wchar_t* _hash, int flag=10);
 __declspec(dllexport) void	SetDownloadFileEx(char* down_path, char* storage_full_name, char* real_name, __int64 size, char* _hash, int flag=10);


/*
return :
   0: 로그인 실패, 1: 성공
*/
__declspec(dllexport) int	SetFtpAccount(char* id, char*pwd);



__declspec(dllexport) void		OnStop_Download() ;
__declspec(dllexport) int		OnStart_Download(int opt); //return 1: 성공 //opt 0: 처음부터 1: 이어받기  
					


__declspec(dllexport) BOOL		CheckDownFinish();

__declspec(dllexport) __int64		GetFileSize();
__declspec(dllexport) __int64		GetStartPoint();
__declspec(dllexport) int		GetStatus();
__declspec(dllexport) __int64		GetValidDownSize();
__declspec(dllexport) __int64		GetDownSize();
_declspec(dllexport) int		GetSpeed(); //byte

__declspec(dllexport) int		_SetSpeedLimit(int speed); //byte
__declspec(dllexport) int		_SetMinPeerCount(int speed); 

__declspec(dllexport) int		GetCount(int& cdn, int& peer); //byte
__declspec(dllexport) int		GetSize(__int64& cdn, __int64& peer); //byte

