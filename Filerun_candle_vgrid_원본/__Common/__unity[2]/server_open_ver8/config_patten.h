

#ifndef _MMSV_CONFIG_PATTEN_H
#define _MMSV_CONFIG_PATTEN_H


#pragma once
#include "ace/Log_Msg.h"
#include "config.h"
#include "../config_protocol.h"
#include "../config_struct.h"


/* 기본설정 */
#define	MMSV_CHECK_USER_AUTH			60					// 인증제한시간 체크
#define MMSV_DEFAULT_DAEMON				1					// 기본실행될 데몬



/* 일반 설정 */

#define MMSV_SERVER_MODE_MAIN			0
#define MMSV_SERVER_MODE_DOWN			1
#define MMSV_SERVER_MODE_UP				2
#define MMSV_SERVER_MODE_SPEED			3
#define MMSV_SERVER_MODE_OPEN			4


#endif /* _MMSV_CONFIG_PATTEN_H */