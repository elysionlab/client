#ifndef _MMSV_CONFIG_H
#define _MMSV_CONFIG_H

#pragma once


/*
	***** 중요 설정 ******
*/
#define OPTION_USE_BOARD			1		// 게시판형 솔루션 사용유무
#define OPTION_USE_LAUNCHER			1		// 런처 실행유무
#define OPTION_USE_DAEMON			1		// 데몬 실행유무 : 1 일때 다음 옵션에 영향을 받음 OPTION_USE_FTPDIVISION, OPTION_USE_FTPINFO_UPDATE, OPTION_USE_FILEREMOVE
#define OPTION_USE_SPEED			1		// 스피드서버 실행유무

#define OPTION_USE_FILEUNITY		0		// 파일 통합모드 사용유무
#define OPTION_MIN_FILEUNITY		1048576	// 파일 통합모드 사용시 최소 통합파일 사이즈(이하는 중복으로 업로드)

#define OPTION_USE_FTPDIVISION		0		// 파일분할 관리 사용유무
#define OPTION_USE_FTPINFO_UPDATE	1		// 파일서버 정보 업데이트 사용유무
#define OPTION_USE_FILEREMOVE		1		// 파일삭제 처리 사용유무(통합모드시에는 임시파일만 삭제처리)

#define OPTION_FREE_MODE			0		// 무료모드 사용 옵션

#define OPTION_MYSQL_41_OVER		1		// MySQL4.1 이상 옵션

#define OPTION_FTP_CONNECT_TIME		3		// 파일서버 연결 시간(초단위)
#define OPTION_FTP_QUERY_TIME		3		// 파일서버 쿼리 시간(초단위)

#define OPTION_MAX_DWSPEED_FIX		1		// 정액제 다운로드 속도제한

#define OPTION_CHECKTIEM_DAY		3		// 정기점검 요일 : 0(일요일) ~ 
#define OPTION_CHECKTIEM_TIME_S		9		// 정기점검 시작 시간(24시 기준)
#define OPTION_CHECKTIEM_TIME_E		10		// 정기점검 끝 시간(24시 기준)

#define OPTION_FILE_DEF_LIMIT		30		// 파일 등록시 유효기간
#define OPTION_FIND_PAGE_SIZE		50		// 검색 페이지 사이즈

#define OPTION_DOWN_LIST_MAX		300		// 한번에 큐에 등록할 수 있는 사이즈

/*
	***** 옵션 설정 ******
*/

//#define SERVER_IP_EXPLORER			"222.96.155.83"
#define SERVER_IP_UP				"112.175.148.141"//"121.144.119.163"//
#define SERVER_IP_DOWN1				"112.175.148.141"
#define SERVER_IP_DOWN2				"112.175.148.141"
//#define SERVER_IP_DOWN3				"222.96.155.83"
//#define SERVER_IP_DOWN4				"222.96.155.83"
#define SERVER_IP_SPEED				"112.175.148.141"
//20080520 오픈자료 업&다운 서버
//#define SERVER_IP_OPEN				"113.30.102.145"

#define TESTSERVER_IP				"127.0.0.1"//"118.218.79.151"//		//테스트 서버

#define SERVER_PORT_EXPLORER		3611
#define SERVER_PORT_DOWN			3001
#define SERVER_PORT_UP				3002
#define SERVER_PORT_SPEED			3003
#define SERVER_PORT_OPEN			3004		//20080509 오픈 자료 서버 포트, jyh
#define SERVER_PORT_STORAGE			3100


#define WINDOWS_SERVICENAME			"_FILERUN_"
#define DAEMON_FILE_NAME			"Filerun_Daemon.exe"
#define LAUNCHER_FILE_NAME			"Filerun_Launcher.exe"
#define SPEED_FILE_NAME				"Filerun_Speed.exe"
#define DOWN_FILE_NAME				"Filerun_Down.exe"
#define UP_FILE_NAME				"Filerun_Up.exe"
//20080121 전송 사이즈 로컬 저장 파일, jyh
#define SENDSIZE_FILENAME			"Filerun_SendSize.bak"
//20080509 오픈 자료 서버 추가, jyh
//#define OPEN_FILE_NAME				"Nfile_Open.exe"



/* 테이블명 */

// Core DB Table
#define TABLE_COMMON_FILES 			"_webhard_file"			// 공용 파일 테이블
#define TABLE_COMMON_FILE_DIVISION	"_webhard_file_division"// 공용 파일 테이블
#define TABLE_COMMON_FTP_SERVER  	"_webhard_ftp_server"	// 공용 서버 테이블
#define TABLE_COMMON_FTP_VOLUME		"_webhard_ftp_volume"	// 공용 서버 테이블
#define TABLE_COMMON_FTP_ACCOUNT	"_webhard_ftp_account"	// 공용 서버 테이블

// CP Common
#define TABLE_MEMBER_BASE			"_member"				// 회원 기본 테이블명
#define TABLE_MEMBER_PAY			"_webhard_pay"			// 회원 결제 테이블명
#define	FIELD_MEMBER_ID				"userid"				// 필드명
#define	FIELD_MEMBER_PW				"userpw"				// 필드명

// CP BBS Type
#define TABLE_BBS					"_webhard_bbs" 			// 게시물 기본 테이블명
#define TABLE_BLOG					"_webhard_blog" 		// 게시물 기본 테이블명
#define TABLE_FILE_BBS				"_webhard_file_bbs" 	// 파일 기본 테이블명
#define TABLE_FILE_BLOG				"_webhard_file_blog" 	// 파일 기본 테이블명
#define TABLE_FILE_BBS_PATH			"_webhard_file_bbs_path"// 업로드 큐 테이블명
#define TABLE_FILE_BLOG_PATH		"_webhard_file_blog_path"// 업로드 큐 테이블명
#define TABLE_LOG_DOWNLOAD			"_webhard_file_downlog"	// 파일 다운로드 로그 테이블
#define TABLE_FILE_REMOVE			"_webhard_file_remove"	// 공용 파일 테이블
#define TABLE_FILE_UPLOAD_COMPLETE	"_webhard_file_complete"// 업로드 완료 처리 테이블 
#define TABLE_PURCHASE				"_webhard_purchase" 	// 본인이 구매한 자료 테이블명
#define TABLE_HASH_COPYRIGHT		"hash_copyright"		//20071211 저작권 해시값 등록 테이블, jyh
#define TABLE_HASH_BBS				"hash_bbs"				//20071218 저작권 등록 테이블, jyh
#define TABLE_CONTENTS_UNIQUE		"_webhard_contents_unique"	//20080214 컨텐츠 업로드 완료시 등록 테이블, jyh
#define TABLE_BBS_REQ				"_webhard_bbs_req"		//20081028 요청 게시판 테이블, jyh
//20080507 오픈 자료 테이블 추가, jyh
#define TABLE_OPEN_DATA				"_opendata_file"		//오픈 자료 파일 정보 테이블
#define TABLE_OPEN_SERVER			"_opendata_ftp_server"	//오픈 자료 파일 서버 테이블
#define TABLE_OPEN_NECESSARY		"bbs_necessary_b"		//필수 자료 테이블
#define TABLE_OPEN_CAPTION			"bbs_caption_b"			//자막 자료 테이블
#define TABLE_OPEN_AS				"bbs_contents_as_b"		//A/S 자료 테이블


/*
	***** 서버/클라이언트 공용 기본 설정 *****
*/

// 타이틀, 클라이언트 파일명, 경로 등등
#define CLIENT_REG_PATH				"Software\\Filerun"
#define CLIENT_SERVICE_NAME			"파일런"
#define CLIENT_UPDATE_TITLE			"파일런 프로그램 설치"
#define CLIENT_DOWN_TITLE			"파일런-파일 다운로드"
#define CLIENT_UP_TITLE				"파일런-파일 업로드"
#define CLIENT_HOMEPAGE				"파일런 홈페이지"
#define CLIENT_APP_PATH				"Filerun"
#define CLIENT_DOWN_NAME			"FilerunDown.exe"
#define CLIENT_UP_NAME				"FilerunUp.exe"
//20080514 ActiveX 컨트롤명, jyh
#define CLIENT_WEBCTRL_NAME			"FilerunWebControl.dll"

//20071016 오픈자료실 업,다운로더 jyh

#define CLIENT_LAUNCHER_MUTEX		"Filerun_mutex_launcher"
#define CLIENT_DAEMON_MUTEX			"Filerun_mutex_daemon"
#define CLIENT_DOWN_MUTEX			"Filerun_mutex_down"
#define CLIENT_UP_MUTEX				"Filerun_mutex_upload"
#define CLIENT_DOWN_CLASS			"Filerun_class_down"
#define CLIENT_UP_CLASS				"Filerun_class_upload"


// 클라이언트 업데이트 정보
#define UPDATE_SETUP				"Filerun.exe"
#ifdef _TEST
#define UPDATE_SERVER_VER			"http://192.168.0.10/ver.ini"
#define UPDATE_SERVER_SETUP			"http://192.168.0.10/Filerun.exe"
#else
#define UPDATE_SERVER_VER			"http://app.filerun.co.kr/app/ver.ini"
#define UPDATE_SERVER_SETUP			"http://app.filerun.co.kr/app/Filerun.exe"
#endif

// 클라이언트의 웹로그인 정보
#define	URL_HOMEPAGE				"http://filerun.co.kr"
#define	URL_BANNER					"http://app.filerun.co.kr/down_app.html?pq_mode="
#define	URL_BANNER_UP				"http://app.filerun.co.kr/up_app.html?pq_mode="
#define URL_HELP					"http://app.filerun.co.kr/app_help.htm?id="
//20090304 컨텐츠 업로드완료 통보 페이지, 
#define URL_SENDCONTENTS			"http://app.filerun.co.kr/contents/cp_check.php?bbs_no="
//아이콘
#define ICONNAME					"Filerun.ico"
#define ICONURL						"http://app.filerun.co.kr/app/Filerun.ico"

//비스타
#define UPDATEWINDOWURL			"http://app.filerun.co.kr/app/UpdateWindow.exe"
#define UPDATEWINDOWFILENAME	"UpdateWindow.exe"
#define CREATESHORTCUTFILENAME	"CreateShortCut.exe"
#define CREATESHORTCUTURL		"http://app.filerun.co.kr/app/CreateShortCut.exe"

//오픈 자료실
#define UPLOADER_EXE			"FilerunOpenUploader.exe"
#define DOWNLOADER_EXE			"FilerunOpenDownloader.exe"
#define CHECKFILE				"FilerunOpenVer.ini"
#define UPLOADER_APPNAME		"FilerunOpenUploader"
#define DOWNLOADER_APPNAME		"FilerunOpenDownloader"
#define OPENUPLOADERURL			"http://app.filerun.co.kr/app/FilerunOpenUploader.exe"
#define OPENDOWNLOADERURL		"http://app.filerun.co.kr/app/FilerunOpenDownloader.exe"
#define CHECKURL				"http://app.filerun.co.kr/app/FilerunOpenVer.ini"


/*
	***** 서버/클라이언트 공용 *****
*/

#define	QUERY_COMPLETE				1		// 디비 쿼리 결과 : 정상처리
#define QUERY_DISCNNT				2		// 디비 쿼리 결과 : 연결실패
#define QUERY_BADREG				3		// 디비 쿼리 결과 : 잘못된 쿼리문


#define	MMSV_PACKET_HEAD_SIZE		12		// 패킷 머리 사이즈
#define	MMSV_PACKET_FOOT_SIZE		4		// 패킷 꼬리 사이즈
#define	MMSV_PACKET_BASE_SIZE		16		// 패킷 기본 사이즈


#define	ITEMLISTADD_OPTION			0		// 최초 설치시 아이템 추가시 옵션사항

#ifdef _TEST
#define CONNECT_CHECK_TIME			3000		// 연결 유효성 체크 타이머(초단위)
#else
#define CONNECT_CHECK_TIME			10//3000//		// 연결 유효성 체크 타이머(초단위)
#endif
#define CONNECT_UPDATE_TIME			10		// 연결 유효성 업데이트 타이머(초단위)-클라이언트
#define CONNECT_LIMIT_TIME			20		// 연결 유효성 제한 시간(초단위)-서버

#define SEP_ITEM					"#>"	// 전송될 각 아이템의 구분자
#define	SEP_FIELD					"#<"	// 전송될 각 아이템의 필드 구분자
#define SEP							"\\"	// 폴더의 구분자

// 로그인 결과
#define LOGIN_RESULT_SERVERSTOP		0		// 서버점검중			
#define LOGIN_RESULT_AUTH			1		// 인증성공
#define LOGIN_RESULT_FAIL			2		// 인증실패	
#define LOGIN_RESULT_FAIL_STOP		3		// 인증실패[정지유저]
#define LOGIN_RESULT_FAIL_ID		4		// 인증실패[아이디 존재하지 않음]
#define LOGIN_RESULT_FAIL_PW		5		// 인증실패[비밀번호 틀림]
#define LOGIN_RESULT_FAIL_DUPC		6		// 인증실패[중복접속]


// 다운로드 요청 결과
#define DOWN_RESULT_ERROR			0		// 알수없는 에러
#define DOWN_RESULT_NOTFOUND		1		// 파일이 존재하지 않은 상태
#define DOWN_RESULT_UPLOAD			2		// 업로드가 완료되지 않은 상태
#define DOWN_RESULT_SIZEOVER		3		// 업로드된 사이즈보다 큰사이즈 요구시
#define DOWN_RESULT_OK				4		// 다운로드 가능
#define DOWN_RESULT_REMOVE_QUEUE	5		// 삭제대상 파일
#define DOWN_RESULT_SERVERSTOP		7		// 파일서버 운영중지된 상태
#define DOWN_RESULT_INFOFAIL		10		// 회원결제 정보 체크 에러					
#define DOWN_RESULT_TIMEOVER		11		// 구매자료 기간 경과
#define DOWN_RESULT_FIX_TIMEOVER	12		// 정액제 가능 시간 아님
#define DOWN_RESULT_FIX_DATEOVER	13		// 정액제 기간 만료
#define DOWN_RESULT_OTHER_USER		14		// 개인자료실에서 본인의 자료가 아닐때
#define DOWN_RESULT_PAY_CANCEL		15		// 구매후 환불승인된 경우
#define DOWN_RESULT_FREE_WEEKEND	16		// 20080311 무료 다운로드 주말 사용 금지, jyh
#define DOWN_RESULT_FREE_TIMEOVER	17		// 20080311 무료 다운로드 특정 시간대 사용 금지, jyh
#define DOWN_RESULT_FREE_LIMIT_COUNT 18		// 20080311 무료 다운로드 인원 제한으로 사용 불가, jyh	
#define DOWN_RESULT_SIGN_____		20		// ##### <--- 여기서 부터는 중지됨
#define DOWN_RESULT_SERVERBUSY		21		// 서버부하상태
#define DOWN_RESULT_SERVICE_STOP	22		// 서비스가 중지된 상태
#define DOWN_RESULT_ERROR_DELAY		23		//20081027 ftp 파일 전송 실패, jyh
#define DOWN_RESULT_FTP_CONFAILED	24		//20081027 파일서버 접속 실패, jyh


//20080506 오픈 자료 업로드 요청 결과
#define OPENUP_RESULT_ERROR			0		//알수없는 에러
#define OPENUP_RESULT_NOTFOUND		1		//파일이 존재하지 않음
#define OPENUP_RESULT_ERROR_DB		2		//디비쿼리실패
#define OPENUP_RESULT_SERVICE_STOP	3		//서비스가 중지된 상태
#define OPENUP_RESULT_DISKFULL		4		//디스크 용량부족
#define OPENUP_RESULT_OK			5		//업로드 가능
#define	OPENUP_RESULT_ERROR_DELAY	6		//파일 서버 접속 에러

//20080506 오픈 자료 다운로드 요청 결과
#define OPENDOWN_RESULT_ERROR			0	//알수없는 에러
#define OPENDOWN_RESULT_NOTFOUND		1	//파일이 존재하지 않음
#define OPENDOWN_RESULT_ERROR_DB		2	//디비쿼리실패
#define OPENDOWN_RESULT_SERVICE_STOP	3	//서비스가 중지된 상태
#define OPENDOWN_RESULT_OK				4	//다운로드 가능
#define	OPENDOWN_RESULT_ERROR_DELAY		5	//파일 서버 접속 에러

//20071210 업로드전 해시값 비교 요청 결과, jyh
#define UPLOAD_RESULT_HASH_EXIST			0	//해시값 존재
#define UPLOAD_RESULT_HASH_NOTEXIST			1	//해시값 없음
#define UPLOAD_RESULT_HASH_REG_EXIST		2	//저작권 등록 페이지 결과
#define UPLOAD_RESULT_HASH_REG_OK			3	//해시값 등록 성공
#define UPLOAD_RESULT_HASH_REG_FAIL			4	//해시값 등록 실패
#define UPLOAD_RESULT_HASH_ERROR			5	//에러
#define UPLOAD_RESULT_HASH_NOTMD5SUM		6	//해시값 등록이 안되어 있음

// 업로드 요청 결과
#define UPLOAD_RESULT_ERROR			0		// 알수없는 에러
#define UPLOAD_RESULT_SIZE			1		// 등록시와 파일사이즈 다름
#define UPLOAD_RESULT_PATH			2		// 파일 경로 존재하지 않음
#define UPLOAD_RESULT_END			3		// 업로드완료
#define UPLOAD_RESULT_NEW			4		// 최초업로드
#define UPLOAD_RESULT_TRICK			5		// 트릭업로드
#define UPLOAD_RESULT_NOTFOUND		6		// 파일이 삭제되었거나 비정상적인 상태
#define UPLOAD_RESULT_ERROR_DB		7		// 디비쿼리실패
#define UPLOAD_RESULT_ERROR_STOP	8		// 해당서버 점검중인 상태
#define UPLOAD_RESULT_ERROR_DELAY	9		// ftp 파일 전송 실패
#define UPLOAD_RESULT_ERROR_ALLO	10		// 파일서버 할당 실패
#define UPLOAD_RESULT_SERVICE_STOP	11		// 서비스가 중지된 상태
#define UPLOAD_RESULT_DISKFULL		12		// 디스크 용량부족
#define UPLOAD_RESULT_SERVERBUSY	13		// 서버부하상태
#define UPLOAD_RESULT_FTP_CONFAILED 14		//20081027 파일서버 접속 실패, jyh

// 파일존재시 처리사항
#define FILE_OVERWRITE_CHECK		0		// 항상물어보기
#define FILE_OVERWRITE_CONTINUE		1		// 이어처리
#define FILE_OVERWRITE_NEW			2		// 덮어쓰기
#define FILE_OVERWRITE_CANCEL		3		// 취소

// 다운로드 아이템 추가시 
#define DOWN_ITEM_ADD_CHECK			0		// 항상물어보기
#define DOWN_ITEM_ADD_LAST			1		// 최근저장경로 사용
#define DOWN_ITEM_ADD_BAGIC			2		// 기본저장경로 사용

// 리스트 아이템 이동
#define MOVE_MF						1		// 맨처음으로
#define MOVE_MU						2		// 한칸위로
#define MOVE_ML						3		// 제일마지막으로
#define MOVE_MD						4		// 한칸아래로

// 리스트 아이템의 상태
#define ITEM_STATE_WAIT				0		// 대기중
#define ITEM_STATE_TRANS			1		// 전송중
#define ITEM_STATE_STOP				2		// 중지
#define ITEM_STATE_CANCEL			3		// 취소
#define ITEM_STATE_ERROR			4		// 에러
#define ITEM_STATE_COMPLETE			5		// 완료
#define ITEM_STATE_CONNECT			6		// 연결중
#define ITEM_STATE_DEFER			7		// 사용자 초과
#define ITEM_STATE_NOFREEDOWN		8		// 무료다운불가
#define ITEM_STATE_CONFAILED		9		//20081027 서버접속실패, jyh
#define ITEM_STATE_DELAY			10		//20081027 전송실패, jyh


//20071119 버튼 상태
#define BTN_STATE_RETRY				0		//재시도
#define BTN_STATE_JUMP				1		//건너뛰기
#define BTN_STATE_YES				2		//예
#define BTN_STATE_NO				3		//아니오
#define BTN_STATE_CANCEL			4		//취소

// 폴더 속성
#define FOLDER_SHARE_CLOSE			0		// 일반폴더
#define FOLDER_SHARE_OPEN_ALL		1		// 전체공유
#define FOLDER_SHARE_OPEN_19		2		// 19세공유



#define SAFE_DELETE(p) {if(p) {delete (p); (p)=NULL;}}


#endif /* _MMSV_CONFIG_H */