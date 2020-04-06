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
#include "mmsv_User.h"


class mmsv_Aio_Daemon;
class mmsv_Acceptor : public ACE_Asynch_Acceptor<mmsv_User>
{

public:
	
	mmsv_Acceptor(mmsv_Aio_Daemon*	pDaemon);
	~mmsv_Acceptor(void){}

	// �ʱ�ȭ
	int mmsv_init(ACE_Proactor*	pProactor);
	void close(void);

public:

	mmsv_Aio_Daemon*	m_pDaemon;
	ACE_Allocator*		m_pAllocator;	
	ACE_Proactor*		m_pProactor;

	int					m_nCloseComplete;
	int					m_bTimerCall;		// Ÿ�̸� ���� ����
	size_t				m_nNowConnects;


	void closeuser();

protected:
	
	virtual mmsv_User* make_handler(void);
	virtual void handle_time_out (const ACE_Time_Value &tv, const void *arg);
	virtual int validate_connection(const ACE_Asynch_Accept::Result& result, const ACE_INET_Addr &remote, const ACE_INET_Addr &local);
};


#endif /* _MMSV_AIO_ACCEPTOR_H */