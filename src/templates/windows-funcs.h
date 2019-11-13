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

#ifndef WINDOWS_FUNCS_H
#define WINDOWS_FUNCS_H
#ifdef WIN32

#define WIN_FFI_HIDE 0x1
#define WIN_FFI_DIR  0x2
#define WIN_FFI_NORMAL 0x4

typedef struct _win_find_file_info_t {
	unsigned int handle;
	unsigned int file_attrs;
	time_t       last_access_time;
	unsigned int file_size;
	int          access_mode;
	char         file_name[1024];
	void*        private_data;
}win_find_file_info;

int win_find_first_file(const char* path, win_find_file_info* win_info);
int win_find_next_file(win_find_file_info* win_info);
void win_end_find_file(win_find_file_info* win_info);



#endif
#endif
