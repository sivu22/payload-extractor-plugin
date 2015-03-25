/*
payload-extractor NSIS plugin
Copyright (c) 2012 Cristian Sava <cristianzsava@gmail.com> 

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <stdio.h>
#include "Plugin.h"
#include "Unzip.h"

#define INITVARS() { g_StringSize = string_size; g_StackTop = stacktop; g_Variables = variables; }

HINSTANCE g_hInstance;

DWORD GetFileSize( TCHAR *szName )
{
	BOOL bOK;
    WIN32_FILE_ATTRIBUTE_DATA fileInfo;

    if( szName == NULL ) return -1;

    bOK = GetFileAttributesEx( szName, GetFileExInfoStandard, (void*)&fileInfo );
    if( !bOK ) return -1;
    
	return fileInfo.nFileSizeLow;
}

BOOL UnzipFromMemory( BYTE *pConfigData, int nConfigLen, TCHAR *szConfigFolder )
{
	HZIP hZIP = OpenZip( pConfigData, nConfigLen, NULL );
	if( hZIP == NULL ) return FALSE;
	
	ZIPENTRY ZE;
	memset( &ZE, 0, sizeof( ZIPENTRY ) );
	GetZipItem( hZIP, -1, &ZE );
	int nNumItems = ZE.index;
	if( nNumItems < 1 ) return FALSE;

	if( SetUnzipBaseDir( hZIP, szConfigFolder ) )
	{
		CloseZip( hZIP );
		return FALSE;
	}

	for( int i = 0; i < nNumItems; i++ )
	{ 
		if( GetZipItem( hZIP, i, &ZE ) )
		{
			CloseZip( hZIP );
			return FALSE;
		}
		if( UnzipItem( hZIP, i, ZE.name ) )
		{
			CloseZip( hZIP );
			return FALSE;
		}
	}
	CloseZip( hZIP );

	return TRUE;
}

extern "C" void __declspec(dllexport) ExtractPayload( HWND hwndParent, int string_size, TCHAR *variables, stack_t **stacktop )
{
	INITVARS();

	// do your stuff here
	TCHAR *szDestFolder = new TCHAR[255];
	memset( szDestFolder, 0, 255 * sizeof( TCHAR ) );
	
	PopString( szDestFolder );
	
	if( *szDestFolder == '\0' )
	{
		PushString( TEXT( "Failed: no destination folder specified" ) );
		delete szDestFolder;
		szDestFolder = NULL;
		return;
	}
	
	BOOL bGo = TRUE;

	TCHAR szInstallerPath[MAX_PATH];			// Path to the running executable
	DWORD res = GetModuleFileName( NULL, szInstallerPath, _countof( szInstallerPath ) );
	if( res == 0 || res == _countof( szInstallerPath ) ) 
	{
		PushString( TEXT( "Failed: could not find the path" ) );
		delete szDestFolder;
		szDestFolder = NULL;
		return;
	}
	
	FILE *fIn = NULL;
	fopen_s( &fIn, szInstallerPath, "rb" );
	
	if( fIn == NULL ) 
	{
		PushString( TEXT( "Failed: cannot open file" ) );
		delete szDestFolder;
		szDestFolder = NULL;
		return;
	}

	if( fseek( fIn, -4, SEEK_END ) ) 
	{
		PushString( TEXT( "Failed: seek counter" ) );
		bGo = FALSE;
	}
	
	BYTE nPayloadB[4];
	int nPayloadLen = 0;
	if( bGo && fread( nPayloadB, sizeof( nPayloadB ), 1, fIn ) != 1 ) 
	{
		PushString( TEXT( "Failed: read counter" ) );
		bGo = FALSE;
	}
	if( bGo )
		for( int i = 0; i < 4; i++ )
		{
			nPayloadLen |= nPayloadB[i];
			if( i < 3 ) nPayloadLen <<= 8;
		}
	
	if( bGo && ( nPayloadLen < 1 || nPayloadLen > 1048576 ) ) 
	{
		PushString( TEXT( "Failed: bad counter" ) );
		bGo = FALSE;
	}
	
	if( bGo && fseek( fIn, -4 - nPayloadLen, SEEK_END ) ) 
	{
		PushString( TEXT( "Failed: seek archive" ) );
		bGo = FALSE;
	}

	BYTE *pPayloadData = NULL;
	if( bGo )
	{
		pPayloadData = new BYTE[nPayloadLen];
		if( fread( pPayloadData, nPayloadLen, 1, fIn ) != 1 ) 
		{
			PushString( TEXT( "Failed: cannot read archive" ) );
			bGo = FALSE;
		}
	}

	if( bGo && !UnzipFromMemory( pPayloadData, nPayloadLen, szDestFolder ) ) 
	{
		PushString( TEXT( "Failed: unzip error" ) );
		bGo = FALSE;
	}

	if( pPayloadData != NULL ) 
	{
		delete pPayloadData;
		pPayloadData = NULL;
	}
	
	delete szDestFolder;
	szDestFolder = NULL;
	
	fclose( fIn );

	if( bGo ) PushString( TEXT( "Done" ) );
}

BOOL WINAPI DllMain( HANDLE hInst, ULONG ul_reason_for_call, LPVOID lpReserved )
{
	g_hInstance = (HINSTANCE)hInst;
	
	return TRUE;
}
