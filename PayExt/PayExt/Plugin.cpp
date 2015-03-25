#include "Plugin.h"

unsigned int g_StringSize;
TCHAR *g_Variables;
stack_t **g_StackTop;

int PopString( TCHAR *str )
{
	stack_t *th;
	
	if( !g_StackTop || !*g_StackTop ) return 1;
	
	th = *g_StackTop;
	lstrcpy( str, th->text );
	*g_StackTop = th->next;
	GlobalFree( (HGLOBAL)th );
	
	return 0;
}

void PushString( TCHAR *str )
{
	stack_t *th;
	
	if( !g_StackTop ) return;
	
	th = (stack_t *)GlobalAlloc( GPTR, sizeof( stack_t ) + g_StringSize );
	lstrcpyn( th->text, str, g_StringSize );
	th->next = *g_StackTop;
	
	*g_StackTop = th;
}

TCHAR *GetUserVariable( int VarNum )
{
	if( VarNum < 0 || VarNum >= __INST_LAST ) return NULL;
  
	return g_Variables + VarNum * g_StringSize;
}
