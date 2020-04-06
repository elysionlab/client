/*
	������ �������� �ʴ� �����ڴ� Hash Map�� �߰����� ����
	�������� ���� �����ڴ� ������ƾ�� ���� ��������� �ڵ�����ó��

	1. ������ �Ϸ�Ǹ� ù��° ��Ŷ�� üũ�ϱ� ���� Ÿ�̸� ����� Ÿ�Ӿƿ� �߻��� ��������
	2. ù ��Ŷ�� ���������� �������� ���� �����̸� �������� Ÿ�̸� ����� Ÿ�Ӿƿ� �߻��� ��������
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

	// ��û�� ���ӿ�û�� ��� Ŭ���̾�Ʈ�� ����
	void close(void);


	// Ŭ���̾�Ʈ ���� ����
	bool connect_manager(int nCallMode, mmsv_User* pUser);

	// �������� ���� ���� üũ
	mmsv_User* user_check(ACE_UINT64 nMemberIdx);
	// �������� ���
	void user_insert(mmsv_User* pUser);
	// �������� ����
	void user_remove(mmsv_User* pUser);

	// �ʱ�ȭ
	int mmsv_init(ACE_Proactor*	pProactor, char* pIP, char* pName, char* pID, char* pPW);
	//int mmsv_init(ACE_Proactor*	pProactor, SERVER_RUNTIME_INFO* pServerInfo);	//20080218 jyh

	// ������ ���� ���
	void mmsv_server_state();

	// �޸� Ǯ ���� üũ
	size_t check_allocator_pool_depth();

	

public:

	UNBOUNDED_SET			m_pClients;

	mmsv_Aio_Daemon*		m_pDaemon;
	ACE_Allocator*			m_pAllocator;	
	ACE_Proactor*			m_pProactor;	
	mmsv_Queue*				m_pQueue;
	Hash_Map_Manager_Int*	m_pUserHashMap;
	ACE_Thread_Mutex		m_pLock_C_Manager_;

	int						m_bTimerCall;		// Ÿ�̸� ���� ����
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