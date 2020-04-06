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
#include "ace/Containers.h"
#include "mmsv_User.h"
#include "mmsv_Queue.h"
#include "Hash_Map_Manager_Int.h"


typedef ACE_Unbounded_Set<mmsv_User*> UNBOUNDED_SET;
typedef ACE_Unbounded_Set_Iterator<mmsv_User*> UNBOUNDED_SET_ITERATOR;
typedef ACE_Unbounded_Set_Const_Iterator<mmsv_User*> UNBOUNDED_SET_CONST_ITERATOR;



class mmsv_Aio_Daemon;
class mmsv_Acceptor : public ACE_Asynch_Acceptor<mmsv_User>
{

public:
	
	mmsv_Acceptor(mmsv_Aio_Daemon*	pDaemon);
	~mmsv_Acceptor(void){}

	// 요청된 접속요청과 모든 클라이언트를 닫음
	void close(void);


	// 클라이언트 연결 관리
	bool connect_manager(int nCallMode, mmsv_User* pUser);

	// 접속유저 이전 접속 체크
	mmsv_User* user_check(ACE_UINT64 nMemberIdx);
	// 접속유저 등록
	void user_insert(mmsv_User* pUser);
	// 접속유저 제거
	void user_remove(mmsv_User* pUser);

	// 초기화
	int mmsv_init(ACE_Proactor*	pProactor, char* pIP, char* pName, char* pID, char* pPW);
	//int mmsv_init(ACE_Proactor*	pProactor, SERVER_RUNTIME_INFO* pServerInfo);	//20080218 jyh

	// 서버의 상태 출력
	void mmsv_server_state();

	// 메모리 풀 상태 체크
	size_t check_allocator_pool_depth();

	

public:

	UNBOUNDED_SET			m_pClients;

	mmsv_Aio_Daemon*		m_pDaemon;
	ACE_Allocator*			m_pAllocator;	
	ACE_Proactor*			m_pProactor;	
	mmsv_Queue*				m_pQueue;
	Hash_Map_Manager_Int*	m_pUserHashMap;
	ACE_Thread_Mutex		m_pLock_C_Manager_;

	int						m_bTimerCall;		// 타이머 설정 여부
	int						m_nCloseFlag;
	int						m_nCloseComplete;
	size_t					m_nHighConnects;
	size_t					m_nNowConnects;
	size_t					m_nMaxAllocator;

protected:
	
	
	virtual mmsv_User* make_handler(void);
	virtual void handle_time_out (const ACE_Time_Value &tv, const void *arg);
	virtual int validate_connection(const ACE_Asynch_Accept::Result& result, const ACE_INET_Addr &remote, const ACE_INET_Addr &local);
};


#endif /* _MMSV_AIO_ACCEPTOR_H */


#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)

template class ACE_Unbounded_Set<mmsv_User*>;
template class ACE_Unbounded_Set_Iterator<mmsv_User*>;
template class ACE_Unbounded_Set_Const_Iterator<mmsv_User*>;

#elif defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)

#pragma instantiate ACE_Unbounded_Set<mmsv_User*>
#pragma instantiate ACE_Unbounded_Set_Iterator<mmsv_User*>
#pragma instantiate ACE_Unbounded_Set_Const_Iterator<mmsv_User*>

#endif /* ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION */