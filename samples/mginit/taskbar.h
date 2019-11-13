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
 * $Id: taskbar.h 1 2008-01-30 07:41:08Z wangjian $
 * 
 * The head file of task bar.
 */

#ifndef _TASK_BAR
#define _TASK_BAR

#define HEIGHT_TASKBAR  28
#define HEIGHT_IMEWIN   24

#define _ID_START_BUTTON    100
#define _ID_TIME_STATIC     110
#define _ID_APPS_COOLBAR    120
#define _ID_LAYER_BOX       200

#define _MARGIN             2
#define _HEIGHT_CTRL        24
#define _WIDTH_START        40
#define _WIDTH_TIME         52
#define _WIDTH_APPS         250
#define _ID_TIMER           100
#define _MAX_WIDTH_LAYER_BOX    80
#define _MIN_WIDTH_LAYER_BOX    20

#define _LEFT_BOXES     (_MARGIN + _WIDTH_START + _MARGIN + _WIDTH_APPS + _MARGIN)
#define _WIDTH_BOXES    (g_rcScr.right - _WIDTH_TIME - _MARGIN - _MARGIN - _LEFT_BOXES)

#define APP_INFO_FILE   "mginit.rc"

#define TIP_MAX         255

typedef struct tagAppItem {
    BOOL cdpath;
    char path [PATH_MAX + 1];
    char name [NAME_MAX + 1];
    char layer [LEN_LAYER_NAME + 1];
    char tip [TIP_MAX + 1];
    char bmp_path [PATH_MAX + NAME_MAX + 1];
    BITMAP bmp;
} APPITEM;

typedef struct tagAppInfo {
    int nr_apps;
    int autostart;
    char logo_path [PATH_MAX + NAME_MAX + 1];
    APPITEM* app_items;
} APPINFO;

extern APPINFO app_info;

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

HWND create_task_bar (void);
pid_t exec_app (int app);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _TASK_BAR */
