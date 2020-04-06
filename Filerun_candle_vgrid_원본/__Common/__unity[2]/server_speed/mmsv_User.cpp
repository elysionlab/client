
#include "ace/Proactor.h"
#include "ace/Malloc_T.h"
#include "ace/Message_Queue.h"
#include "mmsv_User.h"
#include "mmsv_Aio_Acceptor.h"


extern SERVER_RUNTIME_INFO RunServerInfo;

mmsv_User::mmsv_User(mmsv_Acceptor* acc/* = 0*/)
: m_pAcceptor(acc),    
  m_bFlag_Read(0), 
  m_bTimerCallMode(0),
  m_bFlag_Close(0)
{
}

mmsv_User::~mmsv_User(void)
{	
	if(m_bTimerCallMode != 0)
		m_pAcceptor->m_pProactor->cancel_timer(*this);

	m_pAcceptor->closeuser();
	m_pReader.cancel();
}


void mmsv_User::open(ACE_HANDLE new_handle, ACE_Message_Block &message_block)
{	
	this->handle(new_handle);

	if(m_pReader.open(*this, new_handle, 0, m_pAcceptor->m_pProactor) != 0)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋mmsv_User::open()━%D━m_pReader.open() or m_pWriter.open() Faild\n"))); 

		delete this;
		return;
	}

	/* 접속제한 */
	ACE_Time_Value tCheck(MMSV_CHECK_CLOSE_SEC);
	m_pAcceptor->m_pProactor->schedule_timer(*this, 0, tCheck, tCheck);
	m_bTimerCallMode = 1;

	/* 헤더사이즈 만큼 읽기 요청 */	
	ACE_NEW_NORETURN(m_pMB_Read, ACE_Message_Block((size_t)RunServerInfo.nMaxSockBuffer, ACE_Message_Block::MB_DATA, 0, 0, m_pAcceptor->m_pAllocator));
	if(recv_packet(m_pMB_Read, (size_t)RunServerInfo.nMaxSockBuffer) == -1)
	{
		closeclient(2);
		return;
	}
}

int	mmsv_User::closeclient(int bCloseMode)
{
	int close_now = 0;

	try
	{			
		// Recv Result == 0
		if(bCloseMode == 1)
		{
			if(m_bFlag_Read == 0)
				close_now = 1;
			else 
				m_bFlag_Close = 1;
		}
		// session close
		else if(bCloseMode == 2)
		{
			// 연결된 소켓을 종료하도록 설정
			if(handle() != ACE_INVALID_HANDLE)
			{
				ACE_OS::closesocket(handle());
				handle(ACE_INVALID_HANDLE);
			}

			if(m_bFlag_Read == 0 && m_bFlag_Close == 1)
				close_now = 1;
			else
				return 0;
		}

		if(close_now == 1)
		{		
			if(handle() != ACE_INVALID_HANDLE)
			{
				ACE_OS::closesocket(handle());
				handle(ACE_INVALID_HANDLE);
			}

			delete this;
			return -1;
		}
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋closeclient()━%D━Mode(%d)\n"), bCloseMode)); 
	}

	return 0;
}

void mmsv_User::handle_time_out(const ACE_Time_Value &tv, const void *arg)
{
	try
	{
		closeclient(2);
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋mmsv_User::handle_time_out()━%D━m_bTimerCallMode(%d)\n"), m_bTimerCallMode)); 
	}
}

void mmsv_User::handle_read_stream(const ACE_Asynch_Read_Stream::Result &result)
{
	try
	{
		m_bFlag_Read = 0;
		m_pMB_Read->release();

		/* 읽기를 실패했거나 */
		if(!result.success() || result.bytes_transferred() == 0)
		{
			closeclient(1);
			return;
		}
		else
		{
			ACE_NEW_NORETURN(m_pMB_Read, ACE_Message_Block((size_t)RunServerInfo.nMaxSockBuffer, ACE_Message_Block::MB_DATA, 0, 0, m_pAcceptor->m_pAllocator));
			if(recv_packet(m_pMB_Read, (size_t)RunServerInfo.nMaxSockBuffer) == -1)
			{
				closeclient(2);
				return;
			}
		}
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋handle_read_stream()━%D━m_bFlag_Read(%d)\n"), m_bFlag_Read)); 
	}
}


int mmsv_User::recv_packet(ACE_Message_Block* pMB_Block, size_t nSize)
{
	try
	{
		if(m_pReader.read(*pMB_Block, nSize) == -1)
		{
			/* 요청한 메시지블럭을 해제하지 않고 최상위 블럭을 해제해서 전체에 적용 */
			m_pMB_Read->release();			
			closeclient(2);
			return -1;
		}
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋recv_packet()━%D━m_bFlag_Read(%d)\n"), m_bFlag_Read)); 
	}

	m_bFlag_Read = 1;
	return 0;
}
