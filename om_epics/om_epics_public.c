#include "om_epics_public.h"



//------------------------------------------------------------------------------------------------------------------
int om_epics_context_create()
{
	int rt;
	char msg[STR_LEN];

	rt = ca_context_create(ca_enable_preemptive_callback);
	if (rt != ECA_NORMAL) {
		sprintf(msg, "om_epics_context_create: ca_context_create failed: %d", rt);
		error_msg(msg);
		return -1;
	}

	return 0;
}
//------------------------------------------------------------------------------------------------------------------
int om_epics_caput(char *_name, char *_pv_name, char *_value)
{
	int rt;
	int t_cnt = 0;
	struct epics_pv pv = { 0 };
	//struct ca_client_context *ctx;
	char msg[STR_LEN];
	char PvFullName[STR_LEN];

	dbr_string_t	tmp_string = "\0";
	dbr_char_t	    tmp_char = 0;
	dbr_short_t	    tmp_short = 0;
	dbr_int_t	    tmp_int = 0;
	dbr_enum_t	    tmp_enum = 0;
	dbr_long_t	    tmp_long = 0;
	dbr_double_t	tmp_double = 0;

	strcpy(pv.Name, _name);
	strcpy(pv.PvName, _pv_name);
	sprintf(PvFullName, "%s%s", pv.Name, pv.PvName);


	// start up channel access
	rt = ca_context_create(ca_disable_preemptive_callback);
	if (rt != ECA_NORMAL)
	{
	    sprintf(msg, "om_epics_caget: ca_context_create failed: %s", PvFullName);
	    error_msg(msg);
	    return -1;
	}

		
	pv.type = -1;		// reset PV type
	pv.event_cb = NULL;	// no subscription
	pv.user_cb = NULL;	// no callback

	// issue channel connection
	rt = ca_create_channel(	PvFullName,		// R	channel name string
							NULL,			// R	address of connection state change callback function
							NULL,			// R	placed in the channel's user private field
										    //		- can be fetched later by ca_puser(CHID)
											//		- passed as void * arg to *pConnectCallback above
							50,				// R	priority leve in the server 0 - 100
							&pv.channel_id);	// RW	channel id written here

	if (rt != ECA_NORMAL) {
		sprintf(msg, "om_epics_caput: ca_create_channel failed: %s, %s", PvFullName, _value);
		error_msg(msg);		
		return -1;
	}

	rt = ca_pend_io(IO_TIMEOUT);
	if (rt == ECA_TIMEOUT)
	{
	    sprintf(msg, "om_epics_caput: ca_create_channel failed: IO_TIMEOUT: %s", PvFullName);
	    error_msg(msg);
	    ca_context_destroy();
	    return -1;
	}

	pv.type = ca_field_type(pv.channel_id);

	switch (pv.type)
	{
		case DBR_STRING:
			sprintf(msg, "om_epics_caput: strings have not been implemented yet: %s, %s", PvFullName, _value);
			error_msg(msg);
			rt = -1;
			break;

		case DBR_SHORT:
			tmp_short = atoi(_value);
			rt = ca_put(pv.type, pv.channel_id, (void*)&tmp_short);
			break;

		case DBR_FLOAT:
			tmp_double = atof(_value);
			rt = ca_put(pv.type, pv.channel_id, (void*)&tmp_double);
			break;

		case DBR_ENUM:
			tmp_enum = atoi(_value);
			rt = ca_put(pv.type, pv.channel_id, (void*)&tmp_enum);
			break;

		case DBR_CHAR:
			tmp_char = _value[0];
			rt = ca_put(pv.type, pv.channel_id, (void*)&tmp_char);
			break;

		case DBR_LONG:
			tmp_long = atoi(_value);
			rt = ca_put(pv.type, pv.channel_id, (void*)&tmp_long);
			break;

		case DBR_DOUBLE:
			tmp_double = atof(_value);
			rt = ca_put(pv.type, pv.channel_id, (void*)&tmp_double);
			break;

		default:
			sprintf(msg, "om_epics_caput: pv.type: unknown type %d, %s, %s", pv.type, PvFullName, _value);
			error_msg(msg);
			rt = -1;		
	}

	if (rt != ECA_NORMAL)
	{
		sprintf(msg, "om_epics_caput: failed: %s %s", PvFullName, _value);
		error_msg(msg);		
		ca_context_destroy();
		return -1;
	}

	rt = ca_pend_io(IO_TIMEOUT);
	if (rt == ECA_TIMEOUT)
	{
	    sprintf(msg, "om_epics_caput: ca_put failed: IO_TIMEOUT: %s", PvFullName);
	    error_msg(msg);
	    ca_context_destroy();
	    return -1;
	}

	ca_context_destroy();

	return 0;
}
int om_epics_caget(char *_name, char *_pv_name, char **_value)
{
	int rt;
	int t_cnt = 0;
	struct epics_pv pv = { 0 };
	char msg[STR_LEN];
	char value_str[STR_LEN];
	char PvFullName[STR_LEN];

	dbr_string_t	tmp_string = "\0";
	dbr_char_t		tmp_char = 0;
	dbr_short_t		tmp_short = 0;
	dbr_int_t		tmp_int = 0;
	dbr_enum_t		tmp_enum = 0;
	dbr_long_t		tmp_long = 0;
	dbr_double_t	tmp_double = 0;

	strcpy(pv.Name, _name);
	strcpy(pv.PvName, _pv_name);
	sprintf(PvFullName, "%s%s", pv.Name, pv.PvName);


	// start up channel access
	rt = ca_context_create(ca_disable_preemptive_callback);
	if (rt != ECA_NORMAL)
	{
	    sprintf(msg, "om_epics_caget: ca_context_create failed: %s", PvFullName);
	    error_msg(msg);
	    return -1;
	}

	// issue channel connection
	rt = ca_create_channel(	PvFullName,		// R	channel name string
							NULL,			// R	address of connection state change callback function
							NULL,			// R	placed in the channel's user private field
											//		- can be fetched later by ca_puser(CHID)
											//		- passed as void * arg to *pConnectCallback above
							10,							// R	priority leve in the server 0 - 100
							&pv.channel_id);	// RW	channel id written here	
	if (rt != ECA_NORMAL) {
		sprintf(msg, "om_epics_caget: ca_create_channel failed: %s", PvFullName);
		error_msg(msg);
		ca_context_destroy();
		return -1;
	}
	
	// wait for channel to connect
	rt = ca_pend_io(IO_TIMEOUT);
	if (rt == ECA_TIMEOUT)
	{
	    sprintf(msg, "om_epics_caget: ca_create_channel failed: IO_TIMEOUT: %s", PvFullName);
	    error_msg(msg);
	    ca_context_destroy();
	    return -1;
	}
	
	pv.type = ca_field_type(pv.channel_id);	
	switch (pv.type)
	{
		case DBR_STRING:		
			rt = ca_get(pv.type, pv.channel_id, &tmp_string);
			break;

		case DBR_SHORT:
			rt = ca_get(pv.type, pv.channel_id, &tmp_short);			
			break;

		case DBR_FLOAT:
			rt = ca_get(pv.type, pv.channel_id, &tmp_double);			
			break;

		case DBR_ENUM:
			rt = ca_get(pv.type, pv.channel_id, &tmp_enum);			
			break;

		case DBR_CHAR:
			rt = ca_get(pv.type, pv.channel_id, &tmp_char);			
			break;

		case DBR_LONG:
			rt = ca_get(pv.type, pv.channel_id, &tmp_long);			
			break;

		case DBR_DOUBLE:
			rt = ca_get(pv.type, pv.channel_id, &tmp_double);			
			break;

		default:
			sprintf(msg, "om_epics_caget: unknown channel type: %d, %s", pv.type, PvFullName);
			error_msg(msg);
			rt = -1;
	}

	if (rt != ECA_NORMAL)
	{
		sprintf(msg, "om_epics_caget: ca_get failed: %s", PvFullName);
		error_msg(msg);
		ca_context_destroy();
		return -1;
	}
	
	// wait for completion
	rt = ca_pend_io(IO_TIMEOUT);
	if (rt == ECA_TIMEOUT)
	{
	    sprintf(msg, "om_epics_caget: ca_get failed: IO_TIMEOUT: %s", PvFullName);
	    error_msg(msg);
	    ca_context_destroy();
	    return -1;
	}
	
	// convert the data
	switch (pv.type)
	{
		case DBR_STRING:
			sprintf(value_str, "%s", tmp_string);
			break;

		case DBR_SHORT:
			sprintf(value_str, "%d", tmp_short);
			break;

		case DBR_FLOAT:
			sprintf(value_str, "%f", tmp_double);
			break;

		case DBR_ENUM:
			sprintf(value_str, "%d", tmp_enum);
			break;

		case DBR_CHAR:
			sprintf(value_str, "%c", tmp_char);
			debug(value_str);
			break;

		case DBR_LONG:
			sprintf(value_str, "%d", tmp_long);
			break;

		case DBR_DOUBLE:
			sprintf(value_str, "%f", tmp_double);
			break;

		default:
			sprintf(msg, "om_epics_caget: unknown channel type: %d, %s", pv.type, PvFullName);
			error_msg(msg);			
	}
	
	*_value = (char*)calloc(strlen(value_str), sizeof(char));
	if (*_value == NULL) {
		sprintf(msg, "om_epics_caget: calloc failed: %s", PvFullName);
		error_msg(msg);
		ca_context_destroy();
		return -1;
	}
	
	strcpy(*_value, value_str);

	ca_context_destroy();

	return 0;
}
//------------------------------------------------------------------------------------------------------------------
int om_epics_caget_arr(char* _name, char* _pv_name, int* _count, void** _buf)
{
	int rt;
	int t_cnt = 0;
	struct epics_pv pv = { 0 };
	char msg[STR_LEN];
	char PvFullName[STR_LEN];

	int dbrLen = 1;

	strcpy(pv.Name, _name);
	strcpy(pv.PvName, _pv_name);
	sprintf(PvFullName, "%s%s", pv.Name, pv.PvName);


	// start up channel access
	rt = ca_context_create(ca_disable_preemptive_callback);
	if (rt != ECA_NORMAL)
	{
		sprintf(msg, "om_epics_caget_arr: ca_context_create failed: %s", PvFullName);
		error_msg(msg);
		return -1;
	}

	// issue channel connection
	rt = ca_create_channel(	PvFullName,			// R	channel name string
							NULL,				// R	address of connection state change callback function
							NULL,				// R	placed in the channel's user private field
												//		- can be fetched later by ca_puser(CHID)
												//		- passed as void * arg to *pConnectCallback above
							10,					// R	priority leve in the server 0 - 100
							&pv.channel_id);	// RW	channel id written here	
	if (rt != ECA_NORMAL) {
		sprintf(msg, "om_epics_caget_arr: ca_create_channel failed: %s", PvFullName);
		error_msg(msg);
		ca_context_destroy();
		return -1;
	}

	// wait for channel to connect
	rt = ca_pend_io(IO_TIMEOUT);
	if (rt == ECA_TIMEOUT)
	{
		sprintf(msg, "om_epics_caget_arr: ca_create_channel failed: IO_TIMEOUT: %s", PvFullName);
		error_msg(msg);
		ca_context_destroy();
		return -1;
	}

	pv.type = ca_field_type(pv.channel_id);
	pv.count = ca_element_count(pv.channel_id);
	if (pv.type == 6) // EPICS_DOUBLE = 6, EPICS_STRING not implemented
		dbrLen = 2;

	*_count = pv.count * dbrLen;
	*_buf = (void*)calloc(pv.count * dbrLen, sizeof(unsigned int));
	if (*_buf == NULL)
	{
		sprintf(msg, "om_epics_caget_arr: calloc failed: %s", PvFullName);
		error_msg(msg);
		ca_context_destroy();
		return -1;
	}

	rt = ca_array_get(pv.type, pv.count, pv.channel_id, (void*)*_buf);
	if (rt != ECA_NORMAL)
	{
		sprintf(msg, "om_epics_caget_arr: ca_Array_get failed: %s", PvFullName);
		error_msg(msg);
		ca_context_destroy();
		return -1;
	}

	// wait for completion
	rt = ca_pend_io(IO_TIMEOUT);
	if (rt == ECA_TIMEOUT)
	{
		sprintf(msg, "om_epics_caget_array: ca_array_get failed: IO_TIMEOUT: %s", PvFullName);
		error_msg(msg);
		ca_context_destroy();
		return -1;
	}

	ca_context_destroy();

	return 0;
}
//------------------------------------------------------------------------------------------------------------------


	

int om_epics_camonitor(struct epics_pv* _pv, void(__stdcall* _cb)(int*, struct epics_pv*))
{
	int rt;
	char msg[STR_LEN];
	char PvFullName[STR_LEN];	
	
	sprintf(PvFullName, "%s%s", _pv->Name, _pv->PvName);
	_pv->user_cb = _cb;
	_pv->event_cb = event_handler;

	// start up channel access
	rt = ca_context_create(ca_disable_preemptive_callback);
	if (rt != ECA_NORMAL)
	{
		sprintf(msg, "om_epics_camonitor: ca_context_create failed: %s%s", _pv->Name, _pv->PvName);
		error_msg(msg);
		return -1;
	}

	// issue channel connection	
	rt = ca_create_channel(	PvFullName,			// R	channel name string
							connection_handler,	// R	address of connection state change callback function
							_pv,				// R	placed in the channel's user private field
												//		- can be fetched later by ca_puser(CHID)
												//		- passed as void * arg to *pConnectCallback above
							1,				// R	priority leve in the server 0 - 100
							&(_pv->channel_id));	// RW	channel id written here

	if (rt != ECA_NORMAL) {
		sprintf(msg, "om_epics_camonitor: ca_create_channel failed: %s", PvFullName);
		error_msg(msg);
		ca_context_destroy();
		return -1;
	}

	// wait for channel to connect
	ca_pend_event(EVENT_TIMEOUT);	

	// check if channel connected
	if (!_pv->onceConnnected)
	{
		sprintf(msg, "om_epics_camonitor: channel did not connect: %s", PvFullName);
		error_msg(msg);
		ca_context_destroy();
		return -1;
	}

	// keep the monitor alive
	debug_msg("om_epics_camonitor: LOOP reached!");
	//while (_pv->type >= 0)
	while(_pv->onceConnnected)
	{
		ca_pend_event(0.5);
	}
	debug_msg("om_epics_camonitor: LOOP exited!");

	ca_context_destroy();

	return 0;
}

int om_epics_clear_camonitor(struct epics_pv *_pv)
{
	//int rt;
	//char msg[MSG_LEN];
	
	//rt = ca_clear_subscription(_pv->event_id);
	//if (rt != ECA_NORMAL) {
	//	sprintf(msg, "om_epics_clear_camonitor: ca_clear_subscription failed");
	//	error_msg(msg);
	//	return -1;
	//}
	//ca_pend_event(EVENT_TIMEOUT);
	//ca_pend_io(IO_TIMEOUT);
	//
	//rt = ca_clear_channel(_pv->channel_id);
	//if (rt != ECA_NORMAL) {
	//	sprintf(msg, "om_epics_clear_camonitor: ca_clear_channel failed");
	//	error_msg(msg);
	//	return -2;
	//}		
	//ca_pend_event(EVENT_TIMEOUT);
	//ca_pend_io(IO_TIMEOUT);

	//_pv->type = -1;		// this will break the while loop in om_epics_camonitor
	_pv->onceConnnected = 0;

	return 0;
}
//------------------------------------------------------------------------------------------------------------------







//------------------------------------------------------------------------------------------------------------------
// EXTERNAL FUNCTIONS
//------------------------------------------------------------------------------------------------------------------
__declspec(dllexport) int __stdcall ext_om_epics_context_create()
{
	return om_epics_context_create();
}
//------------------------------------------------------------------------------------------------------------------
__declspec(dllexport) int __stdcall ext_om_epics_caput(char *_name, char *_pv_name, char *_value)
{    
    return om_epics_caput(_name, _pv_name, _value);
}
__declspec(dllexport) int __stdcall ext_om_epics_caget(char *_name, char *_pv_name, char **_value)
{
	return om_epics_caget(_name, _pv_name, _value);	
}
//------------------------------------------------------------------------------------------------------------------
__declspec(dllexport) int __stdcall ext_om_epics_caget_arr(char* _name, char* _pv_name, int* _count, void** _arr)
{
	return om_epics_caget_arr(_name, _pv_name, _count, _arr);
}
__declspec(dllexport) int __stdcall ext_om_epics_free_arr(void* _arr)
{
	free(_arr);
	return 0;
}
//------------------------------------------------------------------------------------------------------------------
__declspec(dllexport) int __stdcall ext_om_epics_camonitor(void* _pv, void(__stdcall* _cb)(void*, void*))
{	
	return om_epics_camonitor((struct epics_pv*)_pv, _cb);
}
__declspec(dllexport) int __stdcall ext_om_epics_clear_camonitor(void *_pv)
{
	return om_epics_clear_camonitor((struct epics_pv*)_pv);	
}
//------------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------------
__declspec(dllexport) int __stdcall ext_om_epics_create_pv(char* _name, char* _pv_name, void** _pv)
{
	*_pv = (struct epics_pv*)calloc(1, sizeof(struct epics_pv));

	if (*_pv == NULL)
		return -1;

	strcpy(((struct epics_pv*)*_pv)->Name, _name);
	strcpy(((struct epics_pv*)*_pv)->PvName, _pv_name);

	return 0;
}
__declspec(dllexport) int __stdcall ext_om_epics_free_pv(void* _pv)
{
	free(_pv);

	return 0;
}
//------------------------------------------------------------------------------------------------------------------







