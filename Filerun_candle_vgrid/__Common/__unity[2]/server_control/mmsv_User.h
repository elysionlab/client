#ifndef _MMSV_USER_H
#define _MMSV_USER_H

#pragma once

#include "config_patten.h"
#include "ace/Synch.h"
#include "ace/Guard_T.h"
#include "ace/Message_Queue.h"
#include "ace/Asynch_IO.h"
#include "ace/INET_Addr.h"
#include "../mmsv_FtpClient.h"


typedef ACE_Message_Queue<ACE_NULL_SYNCH> QUEUE;

#define FLAG_EMPTY_		0			// 종료가능한지 체크
#define FLAG_WRITE_		1			// 쓰기 상태 변경
#define FLAG_READ_		2			// 읽기 상태 변경
#define FLAG_QUEUE_		3			// 읽기 관련 작업 여부
#define FLAG_WRITE_C_	4			// 쓰기 상태 요청
#define FLAG_QUEUE_C_	5			// 큐작업 상태 요청

class mmsv_User : public ACE_Service_Handler
{

public:

	friend class mmsv_Acceptor;

	mmsv_User(mmsv_Acceptor* acc = 0);		
	~mmsv_User(void);

	virtual void open(ACE_HANDLE new_handle, ACE_Message_Block &message_block);

public:

	mmsv_Acceptor*		m_pAcceptor;
	QUEUE*				m_pQueue_Writer;
	QUEUE*				m_pQueue_Reader;
	mmsv_FtpClient*		m_pFtp;

	ACE_INET_Addr		m_pAddr_Local;
	ACE_INET_Addr		m_pAddr_Remote;
	
	ACE_TCHAR			m_pUserid[21];		// 아이디
	ACE_TCHAR			m_pUserInfo[255];	// 접속자 고유값
	ACE_UINT64			m_nMemberIdx;		// 회원의 member idx
	ACE_UINT16			m_bSession;			// 로그인 세션
	ACE_UINT16			m_bTimerCallMode;	// 타이머 설정 여부
	ACE_UINT16			m_bCleanProcess;	// 제거작업 여부
	ACE_UINT16			m_bDelayQuery;		// 디비 작업이 오래걸리는 쿼리 처리시에는 플래그를 설정하면 종료되지 않음
		
	/* 다운로드 정보를 위해 */
	DWORD				m_nLoginMode;		// 로그인모드			
		
	ACE_UINT64			m_nFileNo;			// 다운로드 파일 Index
	ACE_UINT32			m_nFilePayMethod;	// 다운로드 파일 구매방식
	ACE_UINT64			m_nFileSizeBegin;	// 다운로드 전송시작 크기
	char				m_pUseridFile[21];	// 다운로드 파일소유자

	ACE_UINT64			m_nLastVolumeUpload;// 마지막으로 업로드 중인 볼륨

	char				m_pUserPoint[5];	// 유저 유효성
	
public:
	
	int  LogIn(char* pUserid, __int64 nUserIdx);

	
	void update_packet(ACE_UINT32 nCode);
	int	 closeclient(int bCloseMode);	
	long connect_time();	
	int  connect_check(bool bCheckMode);

	void cleanprocess();
	void force_close(const mmsv_User* pUserPtr);

	char* getuserinfo();

	__int64 trans_result();
	
	// 접속유저 이전 접속 체크
	mmsv_User* user_check(ACE_UINT64 nMemberIdx);

	int  push_queue();

	int  flag_set(int nMode, int nValue);
	int  putqueue(int nMode, ACE_Message_Block* pNewMessage = 0);

	int  send_count();	
	int  send_packet_queue(int nCode, char* pData = 0, size_t nSize = 0);
		
	virtual void addresses(const ACE_INET_Addr &remote_address, const ACE_INET_Addr &local_address);

private:

	int recv_packet(ACE_Message_Block* pMB_Block, size_t nSize, int nMode);
	int send_packet(ACE_Message_Block* pWritePtr = 0);

protected:
	
	ACE_Message_Block*		m_pMB_Read;
	ACE_Asynch_Read_Stream	m_pReader;
	ACE_Asynch_Write_Stream	m_pWriter;

	ACE_Thread_Mutex		m_pLock_Flag_Set_;
	ACE_Thread_Mutex		m_pLock_PushQueue_;
	ACE_Thread_Mutex		m_pLock_Access_;
	ACE_Thread_Mutex		m_pLock_CloseCh_;
	ACE_Thread_Mutex		m_pLock_CleanPro_;

	int						m_bCloseSocket;
	int						m_bDeferred_Close;
	int						m_bFlag_Write;
	int						m_bFlag_Read;	
	int						m_bFlag_Queue;

	ACE_INT32				m_nPacketLast;		// 마지막 패킷코드
	__int64					m_nPacketCount;		// 전체 받은 패킷 수

	ACE_Time_Value			m_tConnectTime;	
	ACE_Time_Value			m_tAccessTime;	
	
	
	virtual void handle_time_out(const ACE_Time_Value &tv, const void *arg);
	virtual void handle_read_stream(const ACE_Asynch_Read_Stream::Result &result);
	virtual void handle_write_stream(const ACE_Asynch_Write_Stream::Result & result);	
};


#endif /* _MMSV_USER_H */