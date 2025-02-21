/* -*- C++ -*- */

//=============================================================================
/**
 *  @file    MFC_Log.h
 *
 *  MFC_Log.h,v 1.1 2002/08/02 22:15:49 crodrigu Exp
 *
 *  @author Craig Rodrigues <crodrigu@bbn.com>
 *  @author Chris Hafey <chafey@stentor.com>
 *  @author Don Hinton <dhinton@ieee.org>
 */
//=============================================================================

#ifndef MFC_LOG_H
#define MFC_LOG_H

#include "ace/Log_Msg_Callback.h"

class ACE_Log_Record;

/**
 * @class MFC_Log
 *
 * @brief A callback class for ACE_Log_Msg which sends output to Microsoft debugger
 *
 * Register this class as a MSG_CALLBACK with the ACE_Log_Msg singleton and
 * send debug output to the Microsoft debugger console window with ACE_DEBUG macros.
 */
class MFC_Log  :  public virtual ACE_Log_Msg_Callback
{
public:
    /// Constructor
	MFC_Log();

    /// method called by ACE_Log_Msg to log an event
    void log(ACE_Log_Record& log_record);

    /// destructor
	virtual ~MFC_Log();

};

#endif /* MFC_LOG_H */

