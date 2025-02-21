// Filecache.cpp,v 1.2 2003/11/01 11:15:22 dhinton Exp

#include "ace/FILE_Connector.h"

#include "JAWS/Filecache.h"
#include "JAWS/Cache_List_T.h"

void
JAWS_Referenced_Filecache_Factory::destroy (JAWS_Cache_Object *object)
{
  JAWS_FILE *file = (JAWS_FILE *) object->data ();
  file->close ();
  if (file->map ())
    file->map ()->close ();
  delete file;
  this->JAWS_Referenced_Cache_Object_Factory::destroy (object);
}

void
JAWS_Counted_Filecache_Factory::destroy (JAWS_Cache_Object *object)
{
  JAWS_FILE *file = (JAWS_FILE *) object->data ();
  file->close ();
  if (file->map ())
    file->map ()->close ();
  delete file;
  this->JAWS_Counted_Cache_Object_Factory::destroy (object);
}

JAWS_Cached_FILE::JAWS_Cached_FILE (const char *const &filename,
                                    JAWS_Filecache_Proxy::Cache_Manager *cm)
  : JAWS_Filecache_Proxy (filename, cm)
{
  ACE_HANDLE handle = ACE_INVALID_HANDLE;

  if (this->data () != 0)
    {
      handle = ACE_OS::dup (this->data ()->get_handle ());
    }
  else
    {
      JAWS_FILE *file = new JAWS_FILE;
      ACE_FILE_Connector file_connector;

      int result = file_connector.connect (*file, ACE_FILE_Addr (filename));
      if (result == -1 || file->get_handle () == ACE_INVALID_HANDLE)
        {
          // TODO: do something here!
        }

      ACE_FILE_Info info;
      file->get_info (info);

      handle = ACE_OS::dup (file->get_handle ());

      {
        JAWS_Cached_FILE cf (filename, file, info.size_, cm);
        if (cf.data () != 0)
          {
            new (this) JAWS_Cached_FILE (filename, cm);
            return;
          }
      }
    }

  this->file_.set_handle (handle);
}

JAWS_Cached_FILE::JAWS_Cached_FILE (const char *const &filename,
                                    JAWS_FILE *&file,
                                    size_t size,
                                    JAWS_Filecache_Proxy::Cache_Manager *cm)
  : JAWS_Filecache_Proxy (filename, file, size, cm)
{
}

JAWS_Cached_FILE::~JAWS_Cached_FILE (void)
{
  this->file_.close ();
}

ACE_FILE_IO *
JAWS_Cached_FILE::file (void)
{
  return &(this->file_);
}

ACE_Mem_Map *
JAWS_Cached_FILE::mmap (void)
{
  return (this->data () == 0 ? 0 : this->data ()->mem_map ());
}


#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)

template class JAWS_Cache_List_Item<JAWS_Strdup_String,
                                    JAWS_Referenced_Filecache_Factory,
                                    JAWS_String_Hash_Functor,
                                    JAWS_String_Equal_Functor>;
template class JAWS_Cache_List_Item<JAWS_Strdup_String,
                                    JAWS_Counted_Filecache_Factory,
                                    JAWS_String_Hash_Functor,
                                    JAWS_String_Equal_Functor>;
template class JAWS_Cache_List<JAWS_Strdup_String,
                               JAWS_Referenced_Filecache_Factory,
                               JAWS_String_Hash_Functor,
                               JAWS_String_Equal_Functor>;
template class JAWS_Cache_List<JAWS_Strdup_String,
                               JAWS_Counted_Filecache_Factory,
                               JAWS_String_Hash_Functor,
                               JAWS_String_Equal_Functor>;
template class JAWS_Cache_Manager<JAWS_Strdup_String,
                                  JAWS_Referenced_Filecache_Factory,
                                  JAWS_String_Hash_Functor,
                                  JAWS_String_Equal_Functor>;
template class JAWS_Cache_Manager<JAWS_Strdup_String,
                                  JAWS_Counted_Filecache_Factory,
                                  JAWS_String_Hash_Functor,
                                  JAWS_String_Equal_Functor>;
template class JAWS_Cache_Proxy<char const *,
                                JAWS_FILE,
                                JAWS_Referenced_Filecache_Manager>;
template class JAWS_Cache_Proxy<char const *,
                                JAWS_FILE,
                                JAWS_Counted_Filecache_Manager>;
template class ACE_Singleton<JAWS_Referenced_Filecache_Manager,
                             ACE_Thread_Mutex>;
template class ACE_Singleton<JAWS_Counted_Filecache_Manager,
                             ACE_Thread_Mutex>;
template class ACE_Singleton<JAWS_Referenced_Filecache_Factory,
                             ACE_Thread_Mutex>;
template class ACE_Singleton<JAWS_Counted_Filecache_Factory, ACE_Thread_Mutex>;

#endif
