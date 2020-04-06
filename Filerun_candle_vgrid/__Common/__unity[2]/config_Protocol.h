#pragma once

#ifndef __CONFIG_PROTOCOL__
#define __CONFIG_PROTOCOL__


// 패킷 코드 
enum Codes { 

	/*
		서버 기본 프로토콜
	*/
	USER_PTR_UPDATE=500,// 클라이언트 접속유지 요청	
	CLOSE_CLIENT,		// 클라이언트의 연결 종료
	


	/*
		공용 프로토콜
	*/
	CHK_AUTH=600,		// 회원인증
	CHK_PACKET_HAVE,	// 잔여 패킷 조회
	CHK_DUPLI_CLOSE,	// 다른 컴퓨터 로그인
	UPLOAD_SIZE,		//20080122 비정상 종료로 인해 로컬에 저장돼 있던 사이즈 업데이트, jyh

	/*
		다운로드 전용 프로토콜
	*/		

	LOAD_DOWNLIST=800,	// 다운로드 목록 추가 요청	
	LOAD_DOWNPRE,		// 다운로드 전에 처리과정
	LOAD_DOWNLOAD,		// 실제 다운로드
	LOAD_DOWNEND,		// 다운로드 중지/완료 통보	
	LOAD_DOWNSTATE,		// 다운로드 진행상황	
	DOWN_SAVE_MUREKA_DATA,


	/*
		업로드 컨트롤 서버 전용 프로토콜
	*/
	LOAD_UPLIST=850,	// 업로드 목록 추가 요청
	LOAD_UPPRE,			// 업로드 전에 처리과정
	LOAD_UPLOAD,		// 실제 업로드 
	LOAD_UPEND,			// 업로드 중지/완료 통보
	LOAD_UPSTATE,		// 업로드된 사이즈(진행중)
	LOAD_UPHASH,		//20071210 업로드전 해시값 비교요청, jyh
	LOAD_UPGURAEND,		//20071211 구라 전송 완료, jyh
	LOAD_UPCONTENTSCOMPLETE,	//20090303 컨텐츠 업로드 완료, jyh
	SAVE_MUREKA_DATA,			//20100914 뮤레카

	//20080506 오픈 자료 서버 전용 프로토콜
	OPEN_UPLIST=900,	//업로드 목록 추가 요청
	OPEN_UPLOAD,		//실제 업로드
	OPEN_UPEND,			//업로드 중지/완료
	OPEN_DOWNLIST,		//다운로드 목록 추가 요청
	OPEN_DOWNLOAD,		//실제 다운로드
	OPEN_DOWNEND		//다운로드 중지/완료
};

#endif