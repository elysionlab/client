/*
	접속후 인증되지 않는 접속자는 Hash Map에 추가하지 않음
	인증되지 않은 접속자는 다음루틴에 따라 인증대기후 자동종료처리

	1. 접속이 완료되면 첫번째 패킷을 체크하기 위한 타이머 등록후 타임아웃 발생시 연결종료
	2. 첫 패킷이 도착했을때 인증되지 않은 상태이면 인증제한 타이머 등록후 타임아웃 발생시 연결종료
*/


#ifndef _MMSV_AIO_ACCEPTOR_H
#define _MMSV_AIO_ACCEPTOR_H

#pragma once

#include "config_patten.h"
#include "ace/Asynch_Acceptor.h"
#include "mmsv_User.h"


class mmsv_Aio_Daemon;
class mmsv_Acceptor : public ACE_Asynch_Acceptor<mmsv_User>
{

public:
	
	mmsv_Acceptor(mmsv_Aio_Daemon*	pDaemon);
	~mmsv_Acceptor(void){}

	// 초기화
	int mmsv_init(ACE_Proactor*	pProactor);
	void close(void);

public:

	mmsv_Aio_Daemon*	m_pDaemon;
	ACE_Allocator*		m_pAllocator;	
	ACE_Proactor*		m_pProactor;

	int					m_nCloseComplete;
	int					m_bTimerCall;		// 타이머 설정 여부
	size_t				m_nNowConnects;


	void closeuser();

protected:
	
	virtual mmsv_User* make_handler(void);
	virtual void handle_time_out (const ACE_Time_Value &tv, const void *arg);
	virtual int validate_connection(const ACE_Asynch_Accept::Result& result, const ACE_INET_Addr &remote, const ACE_INET_Addr &local);
};


#endif /* _MMSV_AIO_ACCEPTOR_H */