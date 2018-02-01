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
 *   <http://www.minigui.com/en/about/licensing-policy/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include "mgutils.h"

static int control_init (skin_head_t* skin, skin_item_t* item)
{
    return 0;
}

static int control_on_create (skin_item_t* item)
{
    PCTRLDATA    pCtrlData = item->type_data;

    item->hwnd = 
    CreateWindowEx (pCtrlData->class_name,
                    pCtrlData->caption,
                    pCtrlData->dwStyle | WS_CHILD,
                    pCtrlData->dwExStyle,
                    pCtrlData->id,
                    pCtrlData->x,
                    pCtrlData->y,
                    pCtrlData->w,
                    pCtrlData->h,
                    item->hostskin->hwnd,
                    pCtrlData->dwAddData);
    return 1;                
}

static int control_on_destroy (skin_item_t* item)
{
    DestroyWindow ((HWND)item->hwnd);
    return 1;                
}

static DWORD control_get_hwnd (skin_item_t *item)
{
    return (DWORD)item->hwnd;
}

static skin_item_ops_t control_ops = {
    control_init,
    NULL,
    control_on_create,
    control_on_destroy,
    NULL,
    NULL,
    control_get_hwnd,
    NULL,
    NULL
};

skin_item_ops_t *CONTROL_OPS = &control_ops;

