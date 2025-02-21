// Cache_Manager.cpp,v 1.2 2003/11/01 11:15:22 dhinton Exp

#include "ace/ACE.h"

#include "JAWS/Cache_Manager.h"
#include "JAWS/Cache_List_T.h"

JAWS_String_Hash_Functor::JAWS_String_Hash_Functor (const char *s)
  : i_ (0)
{
  this->i_ = ACE::hash_pjw (s);
}

JAWS_String_Hash_Functor::operator unsigned long (void) const
{
  return this->i_;
}

JAWS_String_Equal_Functor::JAWS_String_Equal_Functor (const char *s1,
                                                    const char *s2)
  : i_ (0)
{
  this->i_ = ACE_OS::strcmp (s1, s2);
}

JAWS_String_Equal_Functor::operator int (void) const
{
  return this->i_ == 0;
}

JAWS_Strdup_String::JAWS_Strdup_String (void)
  : c_ (0),
    s_ (0)
{
}

JAWS_Strdup_String::JAWS_Strdup_String (const char *s)
  : c_ (0),
    s_ (0)
{
  this->c_ = new int (1);
  this->s_ = ACE_OS::strdup (s);
}

JAWS_Strdup_String::JAWS_Strdup_String (const JAWS_Strdup_String &s)
  : c_ (s.c_),
    s_ (s.s_)
{
  ++*(this->c_);
}

JAWS_Strdup_String::~JAWS_Strdup_String (void)
{
  if (this->c_ && --*(this->c_) == 0)
    {
      if (this->s_)
        ACE_OS::free (this->s_);
      delete this->c_;
    }
  this->s_ = 0;
  this->c_ = 0;
}

JAWS_Strdup_String::operator const char * (void) const
{
  return this->s_;
}

void
JAWS_Strdup_String::operator = (const char *s)
{
  if (this->c_ && --*(this->c_) == 0)
    {
      if (this->s_)
        ACE_OS::free (this->s_);
      delete this->c_;
    }
  this->c_ = new int (1);
  this->s_ = ACE_OS::strdup (s);
}

void
JAWS_Strdup_String::operator = (const JAWS_Strdup_String &s)
{
  if (this->c_ && --*(this->c_) == 0)
    {
      if (this->s_)
        ACE_OS::free (this->s_);
      delete this->c_;
    }
  this->c_ = s.c_;
  this->s_ = s.s_;
  ++*(this->c_);
}

#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)

template class JAWS_Hash_Bucket_Item<JAWS_Strdup_String, JAWS_Cache_Object *>;
template class JAWS_Hash_Bucket_DLCStack<JAWS_Strdup_String, JAWS_Cache_Object *>;
template class JAWS_Hash_Bucket_DLCStack_Iterator<JAWS_Strdup_String,
                                                 JAWS_Cache_Object *>;
template class JAWS_Hash_Bucket_Manager<JAWS_Strdup_String,
                                       JAWS_Cache_Object *,
                                       JAWS_String_Equal_Functor>;
template class JAWS_Cache_Hash<JAWS_Strdup_String,
                              JAWS_String_Hash_Functor,
                              JAWS_String_Equal_Functor>;
template class JAWS_Cache_List_Item<JAWS_Strdup_String,
                                   JAWS_Referenced_Cache_Object_Factory,
                                   JAWS_String_Hash_Functor,
                                   JAWS_String_Equal_Functor>;
template class JAWS_Cache_List<JAWS_Strdup_String,
                              JAWS_Referenced_Cache_Object_Factory,
                              JAWS_String_Hash_Functor,
                              JAWS_String_Equal_Functor>;
template class JAWS_Cache_Manager<JAWS_Strdup_String,
                                 JAWS_Referenced_Cache_Object_Factory,
                                 JAWS_String_Hash_Functor,
                                 JAWS_String_Equal_Functor>;
template class JAWS_Cache_List_Item<JAWS_Strdup_String,
                                   JAWS_Counted_Cache_Object_Factory,
                                   JAWS_String_Hash_Functor,
                                   JAWS_String_Equal_Functor>;
template class JAWS_Cache_List<JAWS_Strdup_String,
                              JAWS_Counted_Cache_Object_Factory,
                              JAWS_String_Hash_Functor,
                              JAWS_String_Equal_Functor>;
template class JAWS_Cache_Manager<JAWS_Strdup_String,
                                 JAWS_Counted_Cache_Object_Factory,
                                 JAWS_String_Hash_Functor,
                                 JAWS_String_Equal_Functor>;
template class ACE_Singleton<JAWS_Referenced_Cache_Object_Factory,
                             ACE_SYNCH_MUTEX>;
template class ACE_Singleton<JAWS_Counted_Cache_Object_Factory,
                             ACE_SYNCH_MUTEX>;
template class ACE_Singleton<JAWS_String_Referenced_Cache_Manager,
                             ACE_SYNCH_MUTEX>;
template class ACE_Singleton<JAWS_String_Counted_Cache_Manager,
                             ACE_SYNCH_MUTEX>;

#elif defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)

#pragma instantiate JAWS_Hash_Bucket_Item<JAWS_Strdup_String, JAWS_Cache_Object *>
#pragma instantiate JAWS_Hash_Bucket_DLCStack<JAWS_Strdup_String, \
                                             JAWS_Cache_Object *>
#pragma instantiate JAWS_Hash_Bucket_DLCStack_Iterator<JAWS_Strdup_String, \
                                                      JAWS_Cache_Object *>
#pragma instantiate JAWS_Hash_Bucket_Manager<JAWS_Strdup_String, \
                                            JAWS_Cache_Object *, \
                                            JAWS_String_Equal_Functor>
#pragma instantiate JAWS_Cache_Hash<JAWS_Strdup_String, \
                                   JAWS_String_Hash_Functor, \
                                   JAWS_String_Equal_Functor>
#pragma instantiate JAWS_Cache_Heap_Item<JAWS_Strdup_String, \
                                        ACE_Referenced_Cache_Object_Factory, \
                                        JAWS_String_Hash_Functor, \
                                        JAWS_String_Equal_Functor>
#pragma instantiate JAWS_Cache_Heap<JAWS_Strdup_String, \
                                   JAWS_Referenced_Cache_Object_Factory, \
                                   JAWS_String_Hash_Functor, \
                                   JAWS_String_Equal_Functor>
#pragma instantiate JAWS_Cache_Manager<JAWS_Strdup_String, \
                                      JAWS_Referenced_Cache_Object_Factory, \
                                      JAWS_String_Hash_Functor, \
                                      JAWS_String_Equal_Functor>
#pragma instantiate JAWS_Cache_Heap_Item<JAWS_Strdup_String, \
                                        JAWS_Counted_Cache_Object_Factory, \
                                        JAWS_String_Hash_Functor, \
                                        JAWS_String_Equal_Functor>
#pragma instantiate JAWS_Cache_Heap<JAWS_Strdup_String, \
                                   JAWS_Counted_Cache_Object_Factory, \
                                   JAWS_String_Hash_Functor, \
                                   JAWS_String_Equal_Functor>
#pragma instantiate JAWS_Cache_Manager<JAWS_Strdup_String, \
                                      JAWS_Counted_Cache_Object_Factory, \
                                      JAWS_String_Hash_Functor, \
                                      JAWS_String_Equal_Functor>
#pragma instantiate ACE_Singleton<JAWS_Referenced_Cache_Object_Factory, \
                                  ACE_SYNCH_MUTEX>
#pragma instantiate ACE_Singleton<JAWS_Counted_Cache_Object_Factory, \
                                  ACE_SYNCH_MUTEX>
#pragma instantiate ACE_Singleton<JAWS_String_Referenced_Cache_Manager, \
                                  ACE_SYNCH_MUTEX>
#pragma instantiate ACE_Singleton<JAWS_String_Counted_Cache_Manager, \
                                  ACE_SYNCH_MUTEX>

#endif /* ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION */
