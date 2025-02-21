// Cache_Object.cpp,v 1.2 2003/11/01 11:15:22 dhinton Exp

#include "JAWS/Cache_Object.h"
#include "ace/OS_NS_time.h"

JAWS_Cache_Object::JAWS_Cache_Object (const void *data, size_t size)
  : internal_ (0),
    data_ (data),
    size_ (size)
{
  this->first_access_ = ACE_OS::time ((time_t *)0);
  this->new_last_access_ = this->last_access_ = this->first_access_;
}

JAWS_Cache_Object::~JAWS_Cache_Object ()
{
  this->data_ = 0;
  this->size_ = 0;
}

void *
JAWS_Cache_Object::internal (void) const
{
  return this->internal_;
}

void
JAWS_Cache_Object::internal (void *item)
{
  this->internal_ = item;
}

const void *
JAWS_Cache_Object::data (void) const
{
  return this->data_;
}

size_t
JAWS_Cache_Object::size (void) const
{
  return this->size_;
}

unsigned int
JAWS_Cache_Object::count (void) const
{
  return this->count_i ();
}

int
JAWS_Cache_Object::acquire (void)
{
  this->new_last_access_ = ACE_OS::time ((time_t *)0);
  return this->acquire_i ();
}

int
JAWS_Cache_Object::release (void)
{
  this->last_access_ = this->new_last_access_;
  return this->release_i ();
}

time_t
JAWS_Cache_Object::last_access (void) const
{
  return this->last_access_;
}

time_t
JAWS_Cache_Object::first_access (void) const
{
  return this->first_access_;
}

unsigned int
JAWS_Cache_Object::priority (void) const
{
  return this->priority_i ();
}

void *
JAWS_Cache_Object::heap_item (void) const
{
  return this->heap_item_;
}

void
JAWS_Cache_Object::heap_item (void *item)
{
  this->heap_item_ = item;
}


JAWS_Referenced_Cache_Object::
JAWS_Referenced_Cache_Object (const void *data, size_t size)
  : JAWS_Cache_Object (data, size),
    lock_adapter_ (count_)
{
}

JAWS_Referenced_Cache_Object::~JAWS_Referenced_Cache_Object (void)
{
}

ACE_Lock &
JAWS_Referenced_Cache_Object::lock (void)
{
  return this->lock_adapter_;
}

unsigned int
JAWS_Referenced_Cache_Object::count_i (void) const
{
  JAWS_Referenced_Cache_Object *mutable_this
    = (JAWS_Referenced_Cache_Object *) this;

  if (mutable_this->count_.tryacquire_write () == 0)
    return 0;

  return 1;
}

int
JAWS_Referenced_Cache_Object::acquire_i (void)
{
  return this->count_.acquire_read ();
}

int
JAWS_Referenced_Cache_Object::release_i (void)
{
  return this->count_.release ();
}

unsigned int
JAWS_Referenced_Cache_Object::priority_i (void) const
{
  unsigned int priority = ~(0U);
  double delta
    = ACE_OS::difftime (this->last_access (), this->first_access ());

  if (delta >= 0.0 && delta < ~(0U))
    priority = (unsigned) delta;

  return priority;
}



JAWS_Counted_Cache_Object::
JAWS_Counted_Cache_Object (const void *data, size_t size)
  : JAWS_Cache_Object (data, size),
    count_ (0),
    new_count_ (0),
    lock_adapter_ (lock_)
{
}

JAWS_Counted_Cache_Object::~JAWS_Counted_Cache_Object (void)
{
}

ACE_Lock &
JAWS_Counted_Cache_Object::lock (void)
{
  return this->lock_adapter_;
}

unsigned int
JAWS_Counted_Cache_Object::count_i (void) const
{
  JAWS_Counted_Cache_Object *mutable_this = (JAWS_Counted_Cache_Object *) this;

  {
    ACE_Guard<ACE_SYNCH_MUTEX> g (mutable_this->lock_);

    return this->count_;
  }
}

int
JAWS_Counted_Cache_Object::acquire_i (void)
{
  ACE_Guard<ACE_SYNCH_MUTEX> g (this->lock_);

  this->new_count_++;
  return 0;
}

int
JAWS_Counted_Cache_Object::release_i (void)
{
  ACE_Guard<ACE_SYNCH_MUTEX> g (this->lock_);

  this->new_count_--;
  this->count_ = this->new_count_;
  return 0;
}

unsigned int
JAWS_Counted_Cache_Object::priority_i (void) const
{
  return this->count_i ();
}

JAWS_Cache_Object_Factory::JAWS_Cache_Object_Factory (ACE_Allocator *alloc)
  : allocator_ (alloc)
{
  if (this->allocator_ == 0)
    this->allocator_ = ACE_Allocator::instance ();
}

JAWS_Cache_Object_Factory::~JAWS_Cache_Object_Factory (void)
{
}

int
JAWS_Cache_Object_Factory::open (ACE_Allocator *alloc)
{
  this->allocator_ = alloc;

  if (this->allocator_ == 0)
    this->allocator_ = ACE_Allocator::instance ();

  return 0;
}

JAWS_Referenced_Cache_Object_Factory
::JAWS_Referenced_Cache_Object_Factory (ACE_Allocator *alloc)
  : JAWS_Cache_Object_Factory (alloc)
{
}

JAWS_Referenced_Cache_Object_Factory
::~JAWS_Referenced_Cache_Object_Factory (void)
{
}

JAWS_Cache_Object *
JAWS_Referenced_Cache_Object_Factory::create (const void *data, size_t size)
{
  JAWS_Referenced_Cache_Object *obj;

  size_t obj_size = sizeof (JAWS_Referenced_Cache_Object);
  ACE_NEW_MALLOC_RETURN (obj,
                         (JAWS_Referenced_Cache_Object *)
                         this->allocator_->malloc (obj_size),
                         JAWS_Referenced_Cache_Object (data, size), 0);

  return obj;
}

void
JAWS_Referenced_Cache_Object_Factory::destroy (JAWS_Cache_Object *obj)
{
  JAWS_Referenced_Cache_Object *rco = (JAWS_Referenced_Cache_Object *) obj;
  ACE_DES_FREE (rco, this->allocator_->free, JAWS_Referenced_Cache_Object);
}

JAWS_Counted_Cache_Object_Factory
::JAWS_Counted_Cache_Object_Factory (ACE_Allocator *alloc)
  : JAWS_Cache_Object_Factory (alloc)
{
}

JAWS_Counted_Cache_Object_Factory
::~JAWS_Counted_Cache_Object_Factory (void)
{
}

JAWS_Cache_Object *
JAWS_Counted_Cache_Object_Factory::create (const void *data, size_t size)
{
  JAWS_Counted_Cache_Object *obj;

  size_t obj_size = sizeof (JAWS_Counted_Cache_Object);
  ACE_NEW_MALLOC_RETURN (obj,
                         (JAWS_Counted_Cache_Object *)
                         this->allocator_->malloc (obj_size),
                         JAWS_Counted_Cache_Object (data, size), 0);

  return obj;
}

void
JAWS_Counted_Cache_Object_Factory::destroy (JAWS_Cache_Object *obj)
{
  JAWS_Counted_Cache_Object *cco = (JAWS_Counted_Cache_Object *) obj;
  ACE_DES_FREE (cco, this->allocator_->free, JAWS_Counted_Cache_Object);
}

#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)
// These are only specialized with ACE_HAS_THREADS.
template class ACE_Lock_Adapter<ACE_SYNCH_RW_MUTEX>;
template class ACE_Lock_Adapter<ACE_SYNCH_MUTEX>;
#elif defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)
// These are only specialized with ACE_HAS_THREADS.
#pragma instantiate ACE_Lock_Adapter<ACE_SYNCH_RW_MUTEX>
#pragma instantiate ACE_Lock_Adapter<ACE_SYNCH_MUTEX>
#endif /* ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION */

