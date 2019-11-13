/*
 *   This file is part of mGUtils, a component for MiniGUI.
 * 
 *   Copyright (C) 2003~2018, Beijing FMSoft Technologies Co., Ltd.
 *   Copyright (C) 1998~2002, WEI Yongming
 * 
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 * 
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 * 
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *   Or,
 * 
 *   As this program is a library, any link to this program must follow
 *   GNU General Public License version 3 (GPLv3). If you cannot accept
 *   GPLv3, you need to be licensed from FMSoft.
 * 
 *   If you have got a commercial license of this program, please use it
 *   under the terms and conditions of the commercial license.
 * 
 *   For more information about the commercial license, please refer to
 *   <http://www.minigui.com/blog/minigui-licensing-policy/>.
 */

#if defined(WIN32)
#include <stdio.h>
#include <windows.h>

#include "windows-funcs.h"


BOOL test_dir(const char* path)
{
	DWORD dwRet = GetFileAttributes((LPCTSTR)path);
	if(dwRet == INVALID_FILE_ATTRIBUTES)
		return FALSE;
	return dwRet & FILE_ATTRIBUTE_DIRECTORY;
}

unsigned int getdrivers()
{
	return (unsigned int)GetLogicalDrives();
}


static void  FileTimeToTime_t(  FILETIME  ft,  time_t  *t  )  
{  
	LONGLONG  ll;  

	ULARGE_INTEGER            ui;  
	ui.LowPart            =  ft.dwLowDateTime;  
	ui.HighPart            =  ft.dwHighDateTime;  

	ll            =  ft.dwHighDateTime  <<  32  +  ft.dwLowDateTime;  

	*t            =  ((LONGLONG)(ui.QuadPart  -  116444736000000000)  /  10000000);  
} 


static int init_find_file_info(win_find_file_info* win_info)
{
	LPWIN32_FIND_DATAA pfd = (LPWIN32_FIND_DATAA)win_info->private_data;

	win_info->file_size = pfd->nFileSizeLow;

	strcpy(win_info->file_name, pfd->cFileName);

	win_info->file_attrs = 0;
	if(pfd->dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
		win_info->file_attrs |= WIN_FFI_HIDE;
	if(pfd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		win_info->file_attrs |= WIN_FFI_DIR;
	if(pfd->dwFileAttributes & (FILE_ATTRIBUTE_NORMAL|FILE_ATTRIBUTE_ARCHIVE))
		win_info->file_attrs |= WIN_FFI_NORMAL;

	if(pfd->dwFileAttributes &  FILE_ATTRIBUTE_READONLY)
		win_info->access_mode = 1;
	else
		win_info->access_mode = 3;


	FileTimeToTime_t(pfd->ftLastWriteTime, &win_info->last_access_time);
	//win_info->last_access_time = 0.0;
    
    return 0;
}

int win_find_first_file(const char* path, win_find_file_info* win_info)
{
	HANDLE hfile;
	char szFile[1024*4];
	if(!path || !win_info)
		return 0;

	memset(win_info, 0, sizeof(win_find_file_info));

	sprintf(szFile, "%s\\*.*", path);
	win_info->private_data = (void*)malloc(sizeof(WIN32_FIND_DATAA));

	hfile = FindFirstFileA(szFile, (LPWIN32_FIND_DATAA)(win_info->private_data));

	if(hfile == INVALID_HANDLE_VALUE)
	{
		free(win_info->private_data);
		return 0;
	}
	win_info->handle = (unsigned int)hfile;

	init_find_file_info(win_info);

	return 1;
}

int win_find_next_file(win_find_file_info* win_info)
{
	BOOL bret;
	if(!win_info)
		return 0;

	bret = FindNextFileA((HANDLE)(win_info->handle), (LPWIN32_FIND_DATAA)(win_info->private_data));
	if(!bret)
		return 0;

	init_find_file_info(win_info);
	return 1;

}
void win_end_find_file(win_find_file_info* win_info)
{
	if(!win_info)
		return ;
	
	if(win_info->private_data)
		free(win_info->private_data);

	FindClose((HANDLE)(win_info->handle));
}


#endif

