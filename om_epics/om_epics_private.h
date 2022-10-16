#ifndef OM_EPICS_PRIVATE_H
#define OM_EPICS_RIVATE_H



//------------------------------------------------------------------------------------------------------------------
// DEFINITIONS
//------------------------------------------------------------------------------------------------------------------
#define _CRT_SECURE_NO_WARNINGS
#define IO_TIMEOUT			0.5
#define EVENT_TIMEOUT		0.2
#define MAX_TIMEOUT			100
#define SLEEP				10
//------------------------------------------------------------------------------------------------------------------



//------------------------------------------------------------------------------------------------------------------
// INCLUDES
//------------------------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cadef.h>
//------------------------------------------------------------------------------------------------------------------
#include "om_epics_support.h"
#include "om_epics_var.h"
//------------------------------------------------------------------------------------------------------------------



//------------------------------------------------------------------------------------------------------------------
// Prototypes
//------------------------------------------------------------------------------------------------------------------

//int add_pv_to_list(struct epics_pv *_pv, struct epics_pv **_pv_list);
//int remove_pv_from_list(struct epics_pv *_pv, struct epics_pv **_pv_list);
//int get_epics_pv(chid _chid, struct epics_pv **_pv_list);

int epics_create_channel(struct epics_pv *_pv, char *_name, char *_pv_name, void(*msg_out)(char*));

void connection_handler(struct connection_handler_args con_arg);
void event_handler(struct event_handler_args event_arg);
//------------------------------------------------------------------------------------------------------------------

int om_epics_caget_ptr(char *_name, char *_pv_name, unsigned int *_ptr, int *_type);

#endif // !OM_EPICS_PRIVATE_H