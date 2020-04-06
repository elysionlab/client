
#ifndef _MMSV_HASH_MAP_MANAGER_H
#define _MMSV_HASH_MAP_MANAGER_H

#pragma once

#include "config_patten.h"
#include "ace/Hash_Map_Manager.h"
#include "ace/Malloc_T.h"
#include "ace/Thread_Mutex.h"
#include "mmsv_User.h"

typedef ACE_Hash_Map_Entry<ACE_UINT64, const mmsv_User*> MMSV_HASH_MAP_ENTRY;
typedef ACE_Hash_Map_Manager_Ex<ACE_UINT64, const mmsv_User*, ACE_Hash<ACE_UINT64>, ACE_Equal_To<ACE_UINT64>, ACE_Thread_Mutex> MMSV_HASH_INT_MAP;
typedef ACE_Hash_Map_Iterator_Ex<ACE_UINT64, const mmsv_User*, ACE_Hash<ACE_UINT64>, ACE_Equal_To<ACE_UINT64>, ACE_Thread_Mutex> MMSV_HASH_INT_ITER;


class Hash_Map_Manager_Int
{


public:

	Hash_Map_Manager_Int(void)
	:	m_bInit(0)
	{
	}
	~Hash_Map_Manager_Int(void)
	{
		CloseMap();
	}

	int  InitMap(int nMapSize);
	void CloseMap();

	mmsv_User* UserMapCheck(const ACE_UINT64 nMemberIdx);
	void UserMapInsert(const mmsv_User* pUserPtr);
	void UserMapRemove(const mmsv_User* pUserPtr);	

	size_t UserMapCount();
	size_t UserMapSize();


protected:

	MMSV_HASH_INT_MAP		m_pHashMap;
	ACE_Allocator*			m_pAllocator_Fix;
	
	int						m_bInit;
	int						m_nMax_Hash;
};



#endif /* _MMSV_HASH_MAP_MANAGER_H */


#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)

template class ACE_Hash_Map_Entry<ACE_UINT64, const mmsv_User*>;
template class ACE_Hash_Map_Manager_Ex<ACE_UINT64, const mmsv_User*, ACE_Hash<ACE_UINT64>, ACE_Equal_To<ACE_UINT64>, ACE_Thread_Mutex>;
template class ACE_Hash_Map_Iterator_Ex<ACE_UINT64, const mmsv_User*, ACE_Hash<ACE_UINT64>, ACE_Equal_To<ACE_UINT64>, ACE_Thread_Mutex>;

#elif defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)

#pragma instantiate ACE_Hash_Map_Entry<ACE_UINT64, const mmsv_User*>
#pragma instantiate ACE_Hash_Map_Manager_Ex<ACE_UINT64, const mmsv_User*, ACE_Hash<ACE_UINT64>, ACE_Equal_To<ACE_UINT64>, ACE_Thread_Mutex>
#pragma instantiate ACE_Hash_Map_Iterator_Ex<ACE_UINT64, const mmsv_User*, ACE_Hash<ACE_UINT64>, ACE_Equal_To<ACE_UINT64>, ACE_Thread_Mutex>

#endif /* ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION */