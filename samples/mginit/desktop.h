///////////////////////////////////////////////////////////////////////////////
//
//                          IMPORTANT NOTICE
//
// The following open source license statement does not apply to any
// entity in the Exception List published by FMSoft.
//
// For more information, please visit:
//
// https://www.fmsoft.cn/exception-list
//
//////////////////////////////////////////////////////////////////////////////
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
/* 
 * $Id: desktop.h 1 2008-01-30 07:41:08Z wangjian $
 * 
 * The head file of desktop for mginit.
 */

#define ID_UP 0x101
#define ID_CB 0x102
#define ID_AB 0x103

#define ID_OP 0x111
#define ID_RN 0x112

#define APP_INFO_FILE   "mginit.rc"

typedef struct _Context
{
    PBITMAP bg;
    HMENU hmenu;
    HMENU icon_menu;
}Context;

typedef struct tagDspItem 
{
    BOOL    cdpath;
	RECT    hot_spot_rc;
	RECT    text_rc;
    char    path [PATH_MAX + 1];
    char    name [NAME_MAX + 1];
    char    layer [LEN_LAYER_NAME + 1];
    char    bmp_path [PATH_MAX + NAME_MAX + 1];
    BITMAP  bmp;
} DSPITEM;

typedef struct _ICON_INFO
{
    int      nr_apps;
    int      focus;
    char     bg_path [PATH_MAX + 1];
    DSPITEM* app_items;
}ICON_INFO;

#define START_X 20
#define START_Y 10
#define SPACE   30
#define DEF_WIDTH 64
#define DEF_HEIGHT 64

extern DESKTOPOPS mg_dsk_ops;
