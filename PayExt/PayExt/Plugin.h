#ifndef __PLUGIN_H__
#define __PLUGIN_H__

#include <Windows.h>

typedef struct _stack_t
{
	struct _stack_t *next;
	TCHAR text[1];			// this should be the length of string_size
} stack_t;

enum
{
INST_0,         // $0
INST_1,         // $1
INST_2,         // $2
INST_3,         // $3
INST_4,         // $4
INST_5,         // $5
INST_6,         // $6
INST_7,         // $7
INST_8,         // $8
INST_9,         // $9
INST_R0,        // $R0
INST_R1,        // $R1
INST_R2,        // $R2
INST_R3,        // $R3
INST_R4,        // $R4
INST_R5,        // $R5
INST_R6,        // $R6
INST_R7,        // $R7
INST_R8,        // $R8
INST_R9,        // $R9
INST_CMDLINE,   // $CMDLINE
INST_INSTDIR,   // $INSTDIR
INST_OUTDIR,    // $OUTDIR
INST_EXEDIR,    // $EXEDIR
__INST_LAST
};

extern unsigned int g_StringSize;
extern TCHAR *g_Variables;
extern stack_t **g_StackTop;

int PopString( TCHAR *str );		// 0 on success, 1 on empty stack
void PushString( TCHAR *str );
TCHAR *GetUserVariable( int VarNum );
#endif
