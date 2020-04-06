/*

// Ÿ�Ӿƿ�, ��������� ���� ū���̾�Ʈ�� ������ ���� �� Ŭ������ �Ҹ� ��ų�� ������ �߻�
ACE_OS::closesocket(handle());
closeclient(1);

// �Ҹ�ɶ� send, recv ��ϵ� ��� ������ �� �̺�Ʈ ���� ó�� �ʿ���
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


#define FLAG_EMPTY_		0			// ���ᰡ������ üũ
#define FLAG_WRITE_		1			// ���� ���� ����
#define FLAG_READ_		2			// �б� ���� ����
#define FLAG_QUEUE_		3			// �б� ���� �۾� ����
#define FLAG_WRITE_C_	4			// ���� ���� ��û
#define FLAG_QUEUE_C_	5			// ť�۾� ����� ��û


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

	ACE_UINT16			m_bTimerCallMode;	// Ÿ�̸� ���� ����	
	
	
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