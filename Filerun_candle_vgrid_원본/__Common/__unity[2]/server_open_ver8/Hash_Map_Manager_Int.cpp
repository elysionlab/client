#include "Hash_Map_Manager_Int.h"


int Hash_Map_Manager_Int::InitMap(int nMapSize)
{
	m_nMax_Hash = nMapSize;
	ACE_NEW_RETURN(m_pAllocator_Fix, ACE_New_Allocator, -1);
	m_pHashMap.open(m_nMax_Hash, m_pAllocator_Fix);
	return 0;
}

void Hash_Map_Manager_Int::CloseMap()
{
	m_pHashMap.unbind_all();
}

mmsv_User* Hash_Map_Manager_Int::UserMapCheck(const ACE_UINT64 nMemberIdx)
{
	MMSV_HASH_MAP_ENTRY* entry;
	mmsv_User* pUserPtr = 0;

	try
	{
		if(m_pHashMap.find(nMemberIdx, entry) != -1)
		{
			pUserPtr = (mmsv_User*)entry->int_id_;
				
			if(!strcmp(pUserPtr->m_pUserPoint, "mmsv"))
				return pUserPtr;
			else
			{
				m_pHashMap.unbind(nMemberIdx);
				return 0;
			}
		}	
		return 0;
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋Hash_Map_Manager_Int::UserMapCheck()━catch(...)\n")));
	}
	return 0;
}

void Hash_Map_Manager_Int::UserMapInsert(const mmsv_User* pUserPtr)
{
	MMSV_HASH_MAP_ENTRY* entry;
	mmsv_User* pUserPtrPre = 0;

	try
	{
		if(m_pHashMap.find(pUserPtr->m_nMemberIdx, entry) != -1)
		{
			pUserPtrPre = (mmsv_User*)entry->int_id_;

			if(pUserPtr == pUserPtrPre)
				return;
			else
			{
				// 강제 연결 종료 알림
				if(!strcmp(pUserPtrPre->m_pUserPoint, "mmsv"))
					pUserPtrPre->force_close(pUserPtr);
			}

			if(m_pHashMap.unbind(pUserPtr->m_nMemberIdx) == -1)
			{
				ACE_DEBUG((LM_WARNING, ACE_TEXT("╋Hash_Map_Manager_Int::UserMapInsert()━unbind Error\n")));
				return;
			}
		}	
		
		m_pHashMap.bind(pUserPtr->m_nMemberIdx, pUserPtr);
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋Hash_Map_Manager_Int::UserMapInsert()━catch(...)\n")));
	}
}

void  Hash_Map_Manager_Int::UserMapRemove(const mmsv_User* pUserPtr)
{
	MMSV_HASH_MAP_ENTRY* entry;

	try
	{
		if(strcmp(pUserPtr->m_pUserPoint, "mmsv"))
		{
			ACE_DEBUG((LM_WARNING, ACE_TEXT("╋Hash_Map_Manager_Int::UserMapRemove()━m_pUserPoint != 100\n")));
			return;
		}

		if(m_pHashMap.find(pUserPtr->m_nMemberIdx, entry) != -1)
		{
			if(pUserPtr == (mmsv_User*)entry->int_id_)
			{
				if(m_pHashMap.unbind(pUserPtr->m_nMemberIdx) == -1)
				{
					ACE_DEBUG((LM_WARNING, ACE_TEXT("╋Hash_Map_Manager_Int::UserMapRemove()━Remove Failed : Fix-> UserID(%s)\n"), pUserPtr->m_pUserid));
					return;
				}
			}
		}
	}
	catch(...)
	{
		ACE_DEBUG((LM_WARNING, ACE_TEXT("╋Hash_Map_Manager_Int::UserMapRemove()━catch(...)\n")));
		return;
	}
}



size_t Hash_Map_Manager_Int::UserMapCount()
{
	return m_pHashMap.current_size();
}

size_t Hash_Map_Manager_Int::UserMapSize()
{
	return m_pHashMap.total_size();
}


