/*

// 타임아웃, 서버종료로 인해 큰라이언트로 연결을 끊고 본 클래스를 소멸 시킬때 문제점 발생
ACE_OS::closesocket(handle());
closeclient(1);

// 소멸될때 send, recv 등록된 모든 데이터 및 이벤트 관련 처리 필요함
*/


#ifndef _MMSV_USER_H
#define _MMSV_USER_H

#pragma once

#include "config_patten.h"
#include "ace/Synch.h"
#include "ace/Guard_T.h"
#include "ace/Message_Queue.h"
#include "ace/Asynch_IO.h"
#include "ace/INET_Addr.h"


#define FLAG_EMPTY_		0			// 종료가능한지 체크
#define FLAG_WRITE_		1			// 쓰기 상태 변경
#define FLAG_READ_		2			// 읽기 상태 변경
#define FLAG_QUEUE_		3			// 읽기 관련 작업 여부
#define FLAG_WRITE_C_	4			// 쓰기 상태 요청
#define FLAG_QUEUE_C_	5			// 큐작업 사아태 요청


class mmsv_User : public ACE_Service_Handler
{

public:

	friend class mmsv_Acceptor;

	mmsv_User(mmsv_Acceptor* acc = 0);		
	~mmsv_User(void);

	virtual void open(ACE_HANDLE new_handle, ACE_Message_Block &message_block);

public:

	mmsv_Acceptor*		m_pAcceptor;

	ACE_INET_Addr		m_pAddr_Local;
	ACE_INET_Addr		m_pAddr_Remote;

	ACE_UINT16			m_bTimerCallMode;	// 타이머 설정 여부	
	
	
public:
	
	
	int	 closeclient(int bCloseMode);	

private:

	int recv_packet(ACE_Message_Block* pMB_Block, size_t nSize);

protected:
	
	ACE_Message_Block*		m_pMB_Read;
	ACE_Asynch_Read_Stream	m_pReader;

	int						m_bFlag_Read;	
	int						m_bFlag_Close;	

	virtual void handle_time_out(const ACE_Time_Value &tv, const void *arg);
	virtual void handle_read_stream(const ACE_Asynch_Read_Stream::Result &result);
};


#endif /* _MMSV_USER_H */