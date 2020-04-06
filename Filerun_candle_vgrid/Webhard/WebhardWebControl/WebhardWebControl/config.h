#ifndef _MMSV_CONFIG_H
#define _MMSV_CONFIG_H

#pragma once


/*
	***** �߿� ���� ******
*/
#define OPTION_USE_BOARD			1		// �Խ����� �ַ�� �������
#define OPTION_USE_LAUNCHER			1		// ��ó ��������
#define OPTION_USE_DAEMON			1		// ���� �������� : 1 �϶� ���� �ɼǿ� ������ ���� OPTION_USE_FTPDIVISION, OPTION_USE_FTPINFO_UPDATE, OPTION_USE_FILEREMOVE
#define OPTION_USE_SPEED			1		// ���ǵ弭�� ��������

#define OPTION_USE_FILEUNITY		0		// ���� ���ո�� �������
#define OPTION_MIN_FILEUNITY		1048576	// ���� ���ո�� ���� �ּ� �������� ������(���ϴ� �ߺ����� ���ε�)

#define OPTION_USE_FTPDIVISION		0		// ���Ϻ��� ���� �������
#define OPTION_USE_FTPINFO_UPDATE	1		// ���ϼ��� ���� ������Ʈ �������
#define OPTION_USE_FILEREMOVE		1		// ���ϻ��� ó�� �������(���ո��ÿ��� �ӽ����ϸ� ����ó��)

#define OPTION_FREE_MODE			0		// ������ ��� �ɼ�

#define OPTION_MYSQL_41_OVER		1		// MySQL4.1 �̻� �ɼ�

#define OPTION_FTP_CONNECT_TIME		3		// ���ϼ��� ���� �ð�(�ʴ���)
#define OPTION_FTP_QUERY_TIME		3		// ���ϼ��� ���� �ð�(�ʴ���)

#define OPTION_MAX_DWSPEED_FIX		1		// ������ �ٿ�ε� �ӵ�����

#define OPTION_CHECKTIEM_DAY		3		// �������� ���� : 0(�Ͽ���) ~ 
#define OPTION_CHECKTIEM_TIME_S		9		// �������� ���� �ð�(24�� ����)
#define OPTION_CHECKTIEM_TIME_E		10		// �������� �� �ð�(24�� ����)

#define OPTION_FILE_DEF_LIMIT		30		// ���� ��Ͻ� ��ȿ�Ⱓ
#define OPTION_FIND_PAGE_SIZE		50		// �˻� ������ ������

#define OPTION_DOWN_LIST_MAX		300		// �ѹ��� ť�� ����� �� �ִ� ������

/*
	***** �ɼ� ���� ******
*/

//#define SERVER_IP_EXPLORER			"222.96.155.83"
#define SERVER_IP_UP				"112.175.148.141"//"121.144.119.163"//
#define SERVER_IP_DOWN1				"112.175.148.141"
#define SERVER_IP_DOWN2				"112.175.148.141"
//#define SERVER_IP_DOWN3				"222.96.155.83"
//#define SERVER_IP_DOWN4				"222.96.155.83"
#define SERVER_IP_SPEED				"112.175.148.141"
//20080520 �����ڷ� ��&�ٿ� ����
//#define SERVER_IP_OPEN				"113.30.102.145"

#define TESTSERVER_IP				"127.0.0.1"//"118.218.79.151"//		//�׽�Ʈ ����

#define SERVER_PORT_EXPLORER		3611
#define SERVER_PORT_DOWN			3001
#define SERVER_PORT_UP				3002
#define SERVER_PORT_SPEED			3003
#define SERVER_PORT_OPEN			3004		//20080509 ���� �ڷ� ���� ��Ʈ, jyh
#define SERVER_PORT_STORAGE			3100


#define WINDOWS_SERVICENAME			"_FILERUN_"
#define DAEMON_FILE_NAME			"Filerun_Daemon.exe"
#define LAUNCHER_FILE_NAME			"Filerun_Launcher.exe"
#define SPEED_FILE_NAME				"Filerun_Speed.exe"
#define DOWN_FILE_NAME				"Filerun_Down.exe"
#define UP_FILE_NAME				"Filerun_Up.exe"
//20080121 ���� ������ ���� ���� ����, jyh
#define SENDSIZE_FILENAME			"Filerun_SendSize.bak"
//20080509 ���� �ڷ� ���� �߰�, jyh
//#define OPEN_FILE_NAME				"Nfile_Open.exe"



/* ���̺�� */

// Core DB Table
#define TABLE_COMMON_FILES 			"_webhard_file"			// ���� ���� ���̺�
#define TABLE_COMMON_FILE_DIVISION	"_webhard_file_division"// ���� ���� ���̺�
#define TABLE_COMMON_FTP_SERVER  	"_webhard_ftp_server"	// ���� ���� ���̺�
#define TABLE_COMMON_FTP_VOLUME		"_webhard_ftp_volume"	// ���� ���� ���̺�
#define TABLE_COMMON_FTP_ACCOUNT	"_webhard_ftp_account"	// ���� ���� ���̺�

// CP Common
#define TABLE_MEMBER_BASE			"_member"				// ȸ�� �⺻ ���̺��
#define TABLE_MEMBER_PAY			"_webhard_pay"			// ȸ�� ���� ���̺��
#define	FIELD_MEMBER_ID				"userid"				// �ʵ��
#define	FIELD_MEMBER_PW				"userpw"				// �ʵ��

// CP BBS Type
#define TABLE_BBS					"_webhard_bbs" 			// �Խù� �⺻ ���̺��
#define TABLE_BLOG					"_webhard_blog" 		// �Խù� �⺻ ���̺��
#define TABLE_FILE_BBS				"_webhard_file_bbs" 	// ���� �⺻ ���̺��
#define TABLE_FILE_BLOG				"_webhard_file_blog" 	// ���� �⺻ ���̺��
#define TABLE_FILE_BBS_PATH			"_webhard_file_bbs_path"// ���ε� ť ���̺��
#define TABLE_FILE_BLOG_PATH		"_webhard_file_blog_path"// ���ε� ť ���̺��
#define TABLE_LOG_DOWNLOAD			"_webhard_file_downlog"	// ���� �ٿ�ε� �α� ���̺�
#define TABLE_FILE_REMOVE			"_webhard_file_remove"	// ���� ���� ���̺�
#define TABLE_FILE_UPLOAD_COMPLETE	"_webhard_file_complete"// ���ε� �Ϸ� ó�� ���̺� 
#define TABLE_PURCHASE				"_webhard_purchase" 	// ������ ������ �ڷ� ���̺��
#define TABLE_HASH_COPYRIGHT		"hash_copyright"		//20071211 ���۱� �ؽð� ��� ���̺�, jyh
#define TABLE_HASH_BBS				"hash_bbs"				//20071218 ���۱� ��� ���̺�, jyh
#define TABLE_CONTENTS_UNIQUE		"_webhard_contents_unique"	//20080214 ������ ���ε� �Ϸ�� ��� ���̺�, jyh
#define TABLE_BBS_REQ				"_webhard_bbs_req"		//20081028 ��û �Խ��� ���̺�, jyh
//20080507 ���� �ڷ� ���̺� �߰�, jyh
#define TABLE_OPEN_DATA				"_opendata_file"		//���� �ڷ� ���� ���� ���̺�
#define TABLE_OPEN_SERVER			"_opendata_ftp_server"	//���� �ڷ� ���� ���� ���̺�
#define TABLE_OPEN_NECESSARY		"bbs_necessary_b"		//�ʼ� �ڷ� ���̺�
#define TABLE_OPEN_CAPTION			"bbs_caption_b"			//�ڸ� �ڷ� ���̺�
#define TABLE_OPEN_AS				"bbs_contents_as_b"		//A/S �ڷ� ���̺�


/*
	***** ����/Ŭ���̾�Ʈ ���� �⺻ ���� *****
*/

// Ÿ��Ʋ, Ŭ���̾�Ʈ ���ϸ�, ��� ���
#define CLIENT_REG_PATH				"Software\\Filerun"
#define CLIENT_SERVICE_NAME			"���Ϸ�"
#define CLIENT_UPDATE_TITLE			"���Ϸ� ���α׷� ��ġ"
#define CLIENT_DOWN_TITLE			"���Ϸ�-���� �ٿ�ε�"
#define CLIENT_UP_TITLE				"���Ϸ�-���� ���ε�"
#define CLIENT_HOMEPAGE				"���Ϸ� Ȩ������"
#define CLIENT_APP_PATH				"Filerun"
#define CLIENT_DOWN_NAME			"FilerunDown.exe"
#define CLIENT_UP_NAME				"FilerunUp.exe"
//20080514 ActiveX ��Ʈ�Ѹ�, jyh
#define CLIENT_WEBCTRL_NAME			"FilerunWebControl.dll"

//20071016 �����ڷ�� ��,�ٿ�δ� jyh

#define CLIENT_LAUNCHER_MUTEX		"Filerun_mutex_launcher"
#define CLIENT_DAEMON_MUTEX			"Filerun_mutex_daemon"
#define CLIENT_DOWN_MUTEX			"Filerun_mutex_down"
#define CLIENT_UP_MUTEX				"Filerun_mutex_upload"
#define CLIENT_DOWN_CLASS			"Filerun_class_down"
#define CLIENT_UP_CLASS				"Filerun_class_upload"


// Ŭ���̾�Ʈ ������Ʈ ����
#define UPDATE_SETUP				"Filerun.exe"
#ifdef _TEST
#define UPDATE_SERVER_VER			"http://192.168.0.10/ver.ini"
#define UPDATE_SERVER_SETUP			"http://192.168.0.10/Filerun.exe"
#else
#define UPDATE_SERVER_VER			"http://app.filerun.co.kr/app/ver.ini"
#define UPDATE_SERVER_SETUP			"http://app.filerun.co.kr/app/Filerun.exe"
#endif

// Ŭ���̾�Ʈ�� ���α��� ����
#define	URL_HOMEPAGE				"http://filerun.co.kr"
#define	URL_BANNER					"http://app.filerun.co.kr/down_app.html?pq_mode="
#define	URL_BANNER_UP				"http://app.filerun.co.kr/up_app.html?pq_mode="
#define URL_HELP					"http://app.filerun.co.kr/app_help.htm?id="
//20090304 ������ ���ε�Ϸ� �뺸 ������, 
#define URL_SENDCONTENTS			"http://app.filerun.co.kr/contents/cp_check.php?bbs_no="
//������
#define ICONNAME					"Filerun.ico"
#define ICONURL						"http://app.filerun.co.kr/app/Filerun.ico"

//��Ÿ
#define UPDATEWINDOWURL			"http://app.filerun.co.kr/app/UpdateWindow.exe"
#define UPDATEWINDOWFILENAME	"UpdateWindow.exe"
#define CREATESHORTCUTFILENAME	"CreateShortCut.exe"
#define CREATESHORTCUTURL		"http://app.filerun.co.kr/app/CreateShortCut.exe"

//���� �ڷ��
#define UPLOADER_EXE			"FilerunOpenUploader.exe"
#define DOWNLOADER_EXE			"FilerunOpenDownloader.exe"
#define CHECKFILE				"FilerunOpenVer.ini"
#define UPLOADER_APPNAME		"FilerunOpenUploader"
#define DOWNLOADER_APPNAME		"FilerunOpenDownloader"
#define OPENUPLOADERURL			"http://app.filerun.co.kr/app/FilerunOpenUploader.exe"
#define OPENDOWNLOADERURL		"http://app.filerun.co.kr/app/FilerunOpenDownloader.exe"
#define CHECKURL				"http://app.filerun.co.kr/app/FilerunOpenVer.ini"


/*
	***** ����/Ŭ���̾�Ʈ ���� *****
*/

#define	QUERY_COMPLETE				1		// ��� ���� ��� : ����ó��
#define QUERY_DISCNNT				2		// ��� ���� ��� : �������
#define QUERY_BADREG				3		// ��� ���� ��� : �߸��� ������


#define	MMSV_PACKET_HEAD_SIZE		12		// ��Ŷ �Ӹ� ������
#define	MMSV_PACKET_FOOT_SIZE		4		// ��Ŷ ���� ������
#define	MMSV_PACKET_BASE_SIZE		16		// ��Ŷ �⺻ ������


#define	ITEMLISTADD_OPTION			0		// ���� ��ġ�� ������ �߰��� �ɼǻ���

#ifdef _TEST
#define CONNECT_CHECK_TIME			3000		// ���� ��ȿ�� üũ Ÿ�̸�(�ʴ���)
#else
#define CONNECT_CHECK_TIME			10//3000//		// ���� ��ȿ�� üũ Ÿ�̸�(�ʴ���)
#endif
#define CONNECT_UPDATE_TIME			10		// ���� ��ȿ�� ������Ʈ Ÿ�̸�(�ʴ���)-Ŭ���̾�Ʈ
#define CONNECT_LIMIT_TIME			20		// ���� ��ȿ�� ���� �ð�(�ʴ���)-����

#define SEP_ITEM					"#>"	// ���۵� �� �������� ������
#define	SEP_FIELD					"#<"	// ���۵� �� �������� �ʵ� ������
#define SEP							"\\"	// ������ ������

// �α��� ���
#define LOGIN_RESULT_SERVERSTOP		0		// ����������			
#define LOGIN_RESULT_AUTH			1		// ��������
#define LOGIN_RESULT_FAIL			2		// ��������	
#define LOGIN_RESULT_FAIL_STOP		3		// ��������[��������]
#define LOGIN_RESULT_FAIL_ID		4		// ��������[���̵� �������� ����]
#define LOGIN_RESULT_FAIL_PW		5		// ��������[��й�ȣ Ʋ��]
#define LOGIN_RESULT_FAIL_DUPC		6		// ��������[�ߺ�����]


// �ٿ�ε� ��û ���
#define DOWN_RESULT_ERROR			0		// �˼����� ����
#define DOWN_RESULT_NOTFOUND		1		// ������ �������� ���� ����
#define DOWN_RESULT_UPLOAD			2		// ���ε尡 �Ϸ���� ���� ����
#define DOWN_RESULT_SIZEOVER		3		// ���ε�� ������� ū������ �䱸��
#define DOWN_RESULT_OK				4		// �ٿ�ε� ����
#define DOWN_RESULT_REMOVE_QUEUE	5		// ������� ����
#define DOWN_RESULT_SERVERSTOP		7		// ���ϼ��� ������� ����
#define DOWN_RESULT_INFOFAIL		10		// ȸ������ ���� üũ ����					
#define DOWN_RESULT_TIMEOVER		11		// �����ڷ� �Ⱓ ���
#define DOWN_RESULT_FIX_TIMEOVER	12		// ������ ���� �ð� �ƴ�
#define DOWN_RESULT_FIX_DATEOVER	13		// ������ �Ⱓ ����
#define DOWN_RESULT_OTHER_USER		14		// �����ڷ�ǿ��� ������ �ڷᰡ �ƴҶ�
#define DOWN_RESULT_PAY_CANCEL		15		// ������ ȯ�ҽ��ε� ���
#define DOWN_RESULT_FREE_WEEKEND	16		// 20080311 ���� �ٿ�ε� �ָ� ��� ����, jyh
#define DOWN_RESULT_FREE_TIMEOVER	17		// 20080311 ���� �ٿ�ε� Ư�� �ð��� ��� ����, jyh
#define DOWN_RESULT_FREE_LIMIT_COUNT 18		// 20080311 ���� �ٿ�ε� �ο� �������� ��� �Ұ�, jyh	
#define DOWN_RESULT_SIGN_____		20		// ##### <--- ���⼭ ���ʹ� ������
#define DOWN_RESULT_SERVERBUSY		21		// �������ϻ���
#define DOWN_RESULT_SERVICE_STOP	22		// ���񽺰� ������ ����
#define DOWN_RESULT_ERROR_DELAY		23		//20081027 ftp ���� ���� ����, jyh
#define DOWN_RESULT_FTP_CONFAILED	24		//20081027 ���ϼ��� ���� ����, jyh


//20080506 ���� �ڷ� ���ε� ��û ���
#define OPENUP_RESULT_ERROR			0		//�˼����� ����
#define OPENUP_RESULT_NOTFOUND		1		//������ �������� ����
#define OPENUP_RESULT_ERROR_DB		2		//�����������
#define OPENUP_RESULT_SERVICE_STOP	3		//���񽺰� ������ ����
#define OPENUP_RESULT_DISKFULL		4		//��ũ �뷮����
#define OPENUP_RESULT_OK			5		//���ε� ����
#define	OPENUP_RESULT_ERROR_DELAY	6		//���� ���� ���� ����

//20080506 ���� �ڷ� �ٿ�ε� ��û ���
#define OPENDOWN_RESULT_ERROR			0	//�˼����� ����
#define OPENDOWN_RESULT_NOTFOUND		1	//������ �������� ����
#define OPENDOWN_RESULT_ERROR_DB		2	//�����������
#define OPENDOWN_RESULT_SERVICE_STOP	3	//���񽺰� ������ ����
#define OPENDOWN_RESULT_OK				4	//�ٿ�ε� ����
#define	OPENDOWN_RESULT_ERROR_DELAY		5	//���� ���� ���� ����

//20071210 ���ε��� �ؽð� �� ��û ���, jyh
#define UPLOAD_RESULT_HASH_EXIST			0	//�ؽð� ����
#define UPLOAD_RESULT_HASH_NOTEXIST			1	//�ؽð� ����
#define UPLOAD_RESULT_HASH_REG_EXIST		2	//���۱� ��� ������ ���
#define UPLOAD_RESULT_HASH_REG_OK			3	//�ؽð� ��� ����
#define UPLOAD_RESULT_HASH_REG_FAIL			4	//�ؽð� ��� ����
#define UPLOAD_RESULT_HASH_ERROR			5	//����
#define UPLOAD_RESULT_HASH_NOTMD5SUM		6	//�ؽð� ����� �ȵǾ� ����

// ���ε� ��û ���
#define UPLOAD_RESULT_ERROR			0		// �˼����� ����
#define UPLOAD_RESULT_SIZE			1		// ��Ͻÿ� ���ϻ����� �ٸ�
#define UPLOAD_RESULT_PATH			2		// ���� ��� �������� ����
#define UPLOAD_RESULT_END			3		// ���ε�Ϸ�
#define UPLOAD_RESULT_NEW			4		// ���ʾ��ε�
#define UPLOAD_RESULT_TRICK			5		// Ʈ�����ε�
#define UPLOAD_RESULT_NOTFOUND		6		// ������ �����Ǿ��ų� ���������� ����
#define UPLOAD_RESULT_ERROR_DB		7		// �����������
#define UPLOAD_RESULT_ERROR_STOP	8		// �ش缭�� �������� ����
#define UPLOAD_RESULT_ERROR_DELAY	9		// ftp ���� ���� ����
#define UPLOAD_RESULT_ERROR_ALLO	10		// ���ϼ��� �Ҵ� ����
#define UPLOAD_RESULT_SERVICE_STOP	11		// ���񽺰� ������ ����
#define UPLOAD_RESULT_DISKFULL		12		// ��ũ �뷮����
#define UPLOAD_RESULT_SERVERBUSY	13		// �������ϻ���
#define UPLOAD_RESULT_FTP_CONFAILED 14		//20081027 ���ϼ��� ���� ����, jyh

// ��������� ó������
#define FILE_OVERWRITE_CHECK		0		// �׻󹰾��
#define FILE_OVERWRITE_CONTINUE		1		// �̾�ó��
#define FILE_OVERWRITE_NEW			2		// �����
#define FILE_OVERWRITE_CANCEL		3		// ���

// �ٿ�ε� ������ �߰��� 
#define DOWN_ITEM_ADD_CHECK			0		// �׻󹰾��
#define DOWN_ITEM_ADD_LAST			1		// �ֱ������� ���
#define DOWN_ITEM_ADD_BAGIC			2		// �⺻������ ���

// ����Ʈ ������ �̵�
#define MOVE_MF						1		// ��ó������
#define MOVE_MU						2		// ��ĭ����
#define MOVE_ML						3		// ���ϸ���������
#define MOVE_MD						4		// ��ĭ�Ʒ���

// ����Ʈ �������� ����
#define ITEM_STATE_WAIT				0		// �����
#define ITEM_STATE_TRANS			1		// ������
#define ITEM_STATE_STOP				2		// ����
#define ITEM_STATE_CANCEL			3		// ���
#define ITEM_STATE_ERROR			4		// ����
#define ITEM_STATE_COMPLETE			5		// �Ϸ�
#define ITEM_STATE_CONNECT			6		// ������
#define ITEM_STATE_DEFER			7		// ����� �ʰ�
#define ITEM_STATE_NOFREEDOWN		8		// ����ٿ�Ұ�
#define ITEM_STATE_CONFAILED		9		//20081027 �������ӽ���, jyh
#define ITEM_STATE_DELAY			10		//20081027 ���۽���, jyh


//20071119 ��ư ����
#define BTN_STATE_RETRY				0		//��õ�
#define BTN_STATE_JUMP				1		//�ǳʶٱ�
#define BTN_STATE_YES				2		//��
#define BTN_STATE_NO				3		//�ƴϿ�
#define BTN_STATE_CANCEL			4		//���

// ���� �Ӽ�
#define FOLDER_SHARE_CLOSE			0		// �Ϲ�����
#define FOLDER_SHARE_OPEN_ALL		1		// ��ü����
#define FOLDER_SHARE_OPEN_19		2		// 19������



#define SAFE_DELETE(p) {if(p) {delete (p); (p)=NULL;}}


#endif /* _MMSV_CONFIG_H */