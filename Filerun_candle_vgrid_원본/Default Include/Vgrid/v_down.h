#pragma once


enum DOWNLOAD_STATUS
{
	DOWN_READY =0,
	DOWN_ING ,
	DOWN_FINISH,
	DOWN_PAUSE,
	DOWN_ERROR    //cdn, member ���� ����(���� ó���� �ٿ� �Ϸ��� ��õ� �ʿ�)
};

enum DOWNLOAD_ERR
{
	ERR_DISK_SPACE		= -1,  //��ũ ��������
	ERR_FILE_NOT_EXIST  = -2,  //���� ����
	ERR_VDOWN_FAILE		= -3,  //�ٿ�ε� ����(�ٿ����, ���� ��� ������ ����)
	ERR_START_POSITION	= -4	,	//�߸��� ������ġ
	ERR_FILE_CREATE		= -5	,	//���� ���� ����
	ERR_START_FAIL		= -6	,//�̹� �ٿ�ε� ���� �Ǿ���  
	ERR_ETC_FAIL		= -7
};



//download
__declspec(dllexport)  void InitNodeLib(char* site_code);

/*
type : 1  => vgrid ���� ���(ds)(������)
type : 2  => vgrid ������ ftp ������ �̿�(��������)
type : 3  => vgrid ���� ���
type : 5  => vgrid ������� ���  
*/
 __declspec(dllexport) void	SetDownMainSvrInfo(char* ip, int port, int type=3);
__declspec(dllexport) void	SetSearchSvrDNS(char* search_dns);
//__declspec(dllexport) int	SetDownMainSvrInfoEx(char* ip, int port,char* id, char*pwd, int type=2);

/*
_hash : hash_size �� ��� hash�� ���( ���ϵ� hash, kth hash ����)

flag : 10 => �ַ� ���ϵ忡�� ����ϴ� MD5 �ؽ�
flag : 30 => kth �ؽ�
flag : 40 => �ش���� ����

*/
__declspec(dllexport) void	SetDownloadFileExW(wchar_t* down_path, wchar_t* storage_full_name, wchar_t* real_name, __int64 size, wchar_t* _hash, int flag=10);
__declspec(dllexport) void	SetDownloadFileEx(wchar_t* down_path, wchar_t* storage_full_name, wchar_t* real_name, __int64 size, wchar_t* _hash, int flag=10);
 __declspec(dllexport) void	SetDownloadFileEx(char* down_path, char* storage_full_name, char* real_name, __int64 size, char* _hash, int flag=10);


/*
return :
   0: �α��� ����, 1: ����
*/
__declspec(dllexport) int	SetFtpAccount(char* id, char*pwd);



__declspec(dllexport) void		OnStop_Download() ;
__declspec(dllexport) int		OnStart_Download(int opt); //return 1: ���� //opt 0: ó������ 1: �̾�ޱ�  
					


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

