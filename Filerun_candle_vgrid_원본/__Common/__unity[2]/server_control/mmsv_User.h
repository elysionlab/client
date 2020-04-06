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

#define FLAG_EMPTY_		0			// ���ᰡ������ üũ
#define FLAG_WRITE_		1			// ���� ���� ����
#define FLAG_READ_		2			// �б� ���� ����
#define FLAG_QUEUE_		3			// �б� ���� �۾� ����
#define FLAG_WRITE_C_	4			// ���� ���� ��û
#define FLAG_QUEUE_C_	5			// ť�۾� ���� ��û

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
	
	ACE_TCHAR			m_pUserid[21];		// ���̵�
	ACE_TCHAR			m_pUserInfo[255];	// ������ ������
	ACE_UINT64			m_nMemberIdx;		// ȸ���� member idx
	ACE_UINT16			m_bSession;			// �α��� ����
	ACE_UINT16			m_bTimerCallMode;	// Ÿ�̸� ���� ����
	ACE_UINT16			m_bCleanProcess;	// �����۾� ����
	ACE_UINT16			m_bDelayQuery;		// ��� �۾��� �����ɸ��� ���� ó���ÿ��� �÷��׸� �����ϸ� ������� ����
		
	/* �ٿ�ε� ������ ���� */
	DWORD				m_nLoginMode;		// �α��θ��			
		
	ACE_UINT64			m_nFileNo;			// �ٿ�ε� ���� Index
	ACE_UINT32			m_nFilePayMethod;	// �ٿ�ε� ���� ���Ź��
	ACE_UINT64			m_nFileSizeBegin;	// �ٿ�ε� ���۽��� ũ��
	char				m_pUseridFile[21];	// �ٿ�ε� ���ϼ�����

	ACE_UINT64			m_nLastVolumeUpload;// ���������� ���ε� ���� ����

	char				m_pUserPoint[5];	// ���� ��ȿ��
	
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
	
	// �������� ���� ���� üũ
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

	ACE_INT32				m_nPacketLast;		// ������ ��Ŷ�ڵ�
	__int64					m_nPacketCount;		// ��ü ���� ��Ŷ ��

	ACE_Time_Value			m_tConnectTime;	
	ACE_Time_Value			m_tAccessTime;	
	
	
	virtual void handle_time_out(const ACE_Time_Value &tv, const void *arg);
	virtual void handle_read_stream(const ACE_Asynch_Read_Stream::Result &result);
	virtual void handle_write_stream(const ACE_Asynch_Write_Stream::Result & result);	
};


#endif /* _MMSV_USER_H */