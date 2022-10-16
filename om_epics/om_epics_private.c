#include "om_epics_private.h"


//------------------------------------------------------------------------------------------------------------------
//int add_pv_to_list(struct epics_pv *_pv, struct epics_pv **_pv_list)
//{
//	// Adds a PV (struct epics_pv) to the first, NULL, position of an epics_pv array.
//	// If all positions of that array already reference a given PV,
//	// the function returns -1 and the array remains unchanged.
//	// On success the function returns 0.
//
//	int i;
//	char msg[STR_LEN];
//
//	for (i = 0; i < EPICS_PV_LIST_MAX; i++)
//	{
//		if (_pv_list[i] == NULL)
//		{
//			_pv_list[i] = _pv;			
//			return 0;
//		}
//	}
//	return -1;
//}
//int remove_pv_from_list(struct epics_pv *_pv, struct epics_pv **_pv_list)
//{
//	// removes a PV (struct epics_pv) form an epics_pv array.
//	// Each position of the array is checked if it references the given PV.
//	// If so the current index is set NULL and the function returns 0;
//	// If no position of the array references the given PV
//	// the function returns -1 and the array remains unchanged.	
//
//	int i;
//
//	for (i = 0; i < EPICS_PV_LIST_MAX; i++)
//	{
//		if (_pv_list[i] == _pv)
//		{
//			_pv_list[i] = NULL;
//			return 0;
//		}
//	}
//	return -1;
//}
//int get_epics_pv(chid _chid, struct epics_pv **_pv_list)
//{
//	// returns the position of a reference to a PV in an epics_pv array.
//	// each PV in the array is checked for a channel id (chid) match
//	// if no match is found the function returns -1
//
//	int i;	
//
//	for (i = 0; i < EPICS_PV_LIST_MAX; i++)
//	{
//		if (_pv_list[i] != NULL && _pv_list[i]->channel_id == _chid)
//		{			
//			return i;
//		}		
//	}
//
//	return -1;
//}
//------------------------------------------------------------------------------------------------------------------

int epics_create_channel(struct epics_pv *_pv, char *_name, char *_pv_name, void(*msg_out)(char*))
{
	int rt;
	int t_cnt = 0;
	char msg[STR_LEN];
	char PvFullName[STR_LEN];	

	strcpy(_pv->Name, _name);
	strcpy(_pv->PvName, _pv_name);
	sprintf(PvFullName, "%s%s", _pv->Name, _pv->PvName);

	if (msg_out != NULL) {
		sprintf(msg, "epics_create_channel: %s", PvFullName);
		msg_out(msg);
	}

	// reset PV type
	_pv->type = -1;

	// create the channel	
	rt = ca_create_channel(	PvFullName,
				connection_handler,
				_pv,
				20, 
				&_pv->channel_id);
	if (rt != ECA_NORMAL) {		
		error_msg("epics_create_channel: ca_create_channel failed");
		return -1;
	}

	ca_pend_io(IO_TIMEOUT);
	ca_pend_event(EVENT_TIMEOUT);
	
	while (_pv->type < 0) {
		ca_pend_event(EVENT_TIMEOUT);

		t_cnt++;
		if (t_cnt > MAX_TIMEOUT) {
			error_msg("epics_create_channel: timeout error");
			return -1;
		}
			
	}

	return 0;
}
void connection_handler(struct connection_handler_args con_arg)
{	
	struct epics_pv *pv = (struct epics_pv*)ca_puser(con_arg.chid);	
	char msg[STR_LEN];	

	sprintf(msg, "connection_handler: %s%s", pv->Name, pv->PvName);	
	debug_msg(msg);

	if (con_arg.op == CA_OP_CONN_UP)
	{
		debug_msg("con_cb: arg.op is CA_OP_CONN_UP");


		if (!pv->onceConnnected)		
		{
			
			pv->onceConnnected = 1;			
			pv->channel_id = con_arg.chid;
			pv->type = ca_field_type(con_arg.chid);
			pv->count = ca_element_count(con_arg.chid);

			if(pv->event_cb == NULL)
				return;
			
			ca_create_subscription(	pv->type,		// R	type		data type from db_access.h
									pv->count,		// R	count		array element count
									pv->channel_id,	// R	chan		channel identifier
									DBE_VALUE,		// R	mask		event mask - one of (DBE_VALUE, DBE_ALARM, DBE_LOG)					
									pv->event_cb,	// R	pFunc		pointer to call-back function									
									(void*)pv,		// R	pArg		copy of this pointer passed to pFunc
									&pv->event_id);	// W	pEventID	event id written at specified address
			
			debug_msg("subscription created");
		}
	}
	else if(con_arg.op == CA_OP_CONN_DOWN)
	{
		debug_msg("con_cb: arg.op is *not* CA_OP_CONN_UP");
		pv->onceConnnected = 0;
	}

	return;
}
void event_handler(struct event_handler_args event_arg)
{
	struct epics_pv* pv = (struct epics_pv*)ca_puser(event_arg.chid);
	//struct epics_pv* pv = (struct epics_pv*)event_arg.usr;

	pv->channel_id = event_arg.chid;
	pv->count = ca_element_count(event_arg.chid);	
	pv->user_cb((void*)event_arg.dbr, (void*)pv);
	
	return;
}
//------------------------------------------------------------------------------------------------------------------



int om_epics_caget_ptr(char *_name, char *_pv_name, unsigned int *_ptr, int *_type)
{
	int rt;
	struct epics_pv pv = { 0 };

	//char msg[STR_LEN];
	char PvFullName[STR_LEN];

	strcpy(pv.Name, _name);
	strcpy(pv.PvName, _pv_name);

	sprintf(PvFullName, "%s%s", pv.Name, pv.PvName);

	rt = ca_create_channel(PvFullName, NULL, NULL, 0, &(pv.channel_id));
	if (rt != ECA_NORMAL)
		return -1;

	rt = ca_pend_io(IO_TIMEOUT);
	if (rt != ECA_NORMAL)
		return -1;

	*_type = pv.type;
	rt = ca_get(pv.type, pv.channel_id, _ptr);
	if (rt != ECA_NORMAL)
		return -1;

	rt = ca_pend_io(IO_TIMEOUT);
	if (rt != ECA_NORMAL)
		return -1;

	rt = ca_clear_channel(pv.channel_id);
	if (rt != ECA_NORMAL)
		return -1;

	return 0;
}
