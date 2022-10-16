#include "om_epics_support.h"



//------------------------------------------------------------------------------------------------------------------
void debug(char* _msg)
{
	if (M_callback_debug_msg != NULL) {
		M_callback_debug_msg(_msg);
	}
}
void error(char* _msg)
{
	if (M_callback_error_msg != NULL) {
		M_callback_error_msg(_msg);
	}
}
void event(char* _msg)
{
	if (M_callback_event_msg != NULL) {
		M_callback_event_msg(_msg);
	}
}
//------------------------------------------------------------------------------------------------------------------
void debug_msg(char *_msg)
{
	char msg[MSG_LEN];

	sprintf(msg, "DEBUG: %s", _msg);
	debug(msg);
}
void event_msg(char *_msg)
{
	char msg[MSG_LEN];

	sprintf(msg, "EVENT: %s", _msg);
	event(msg);
}
void error_msg(char *_msg)
{
	char msg[MSG_LEN];

	sprintf(msg, "ERROR: %s", _msg);
	error(msg);
}
//------------------------------------------------------------------------------------------------------------------




//------------------------------------------------------------------------------------------------------------------
// EXTERNAL FUNCTIONS
//------------------------------------------------------------------------------------------------------------------
__declspec(dllexport) int __stdcall ext_debug_msg(void(__stdcall *callback_string_function)(char*))
{
	M_callback_debug_msg = callback_string_function;
	debug("DEBUG callback activated");

	return 0;
}
__declspec(dllexport) int __stdcall ext_event_msg(void(__stdcall *callback_string_function)(char*))
{
	M_callback_event_msg = callback_string_function;
	debug("EVENT callback activated");
	return 0;
}
__declspec(dllexport) int __stdcall ext_error_msg(void(__stdcall *callback_string_function)(char*))
{
	M_callback_error_msg = callback_string_function;
	debug("ERROR callback activated");
	return 0;
}
//------------------------------------------------------------------------------------------------------------------