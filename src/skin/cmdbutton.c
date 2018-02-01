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
#include "item_comm.h"

static void button_draw_bg (HDC hdc, skin_item_t *item)
{
    const BITMAP* bmp = &item->hostskin->bmps[item->bmp_index];
    int w = bmp->bmWidth / (item->style & SI_CMDBUTTON_2STATE?2:4);
    int h = bmp->bmHeight;
    int style_no = 0;

    if (item->style & SI_STATUS_HILIGHTED) {
        if (item->style & SI_CMDBUTTON_2STATE) 
            style_no = 0;
        else
            style_no = 1;
    }

    if (item->style & SI_BTNSTATUS_CLICKED)
        style_no = 2;

    if (item->style & SI_STATUS_DISABLED) {
        if (item->style & SI_CMDBUTTON_2STATE) 
            style_no = 0;
        else
            style_no = 3;
    }

    FillBoxWithBitmapPart (hdc,    item->x, item->y, w, h, 0, 0, bmp, style_no*w, 0);
}

static int button_msg_proc (skin_item_t* item, int message, WPARAM wparam, LPARAM lparam)
{
    /* assert (item != NULL) */
    switch (message) {
    case SKIN_MSG_LBUTTONDOWN:    /* click-down event */
        //RAISE_EVENT ( SIE_LBUTTONDOWN, NULL ); /*item msg not defined*/
        /* default operation */
        skin_set_check_status ( item->hostskin, item->id, TRUE );    /*button down*/
        break;
    case SKIN_MSG_LBUTTONUP:    /* click-up event */
        //RAISE_EVENT ( SIE_LBUTTONUP, NULL ); /*item msg not defined*/
        /* default operation */
        skin_set_check_status ( item->hostskin, item->id, FALSE );    /*button up*/
        break;
    case SKIN_MSG_CLICK:        /* CLICK event */
        RAISE_EVENT ( SIE_BUTTON_CLICKED, NULL );
        /* default operation */
        skin_set_check_status ( item->hostskin, item->id, FALSE );    /* button up */
        break;
    case SKIN_MSG_MOUSEDRAG:
        //RAISE_EVENT ( SIE_MOUSEDRAG, NULL );
        /* default operation */
        if ( PtInRegion (&item->region, (int)wparam, (int)lparam) ){
            /* if mouse moves in, click-down item */
            if (!skin_get_check_status(item->hostskin, item->id))
                skin_set_check_status ( item->hostskin, item->id, TRUE);    /*button down*/
        }
        else{
            /* if mouse moves out, click-up item */
            if (skin_get_check_status(item->hostskin, item->id))
                skin_set_check_status ( item->hostskin, item->id, FALSE);    /*button up*/
        }
        break;
    case SKIN_MSG_SETFOCUS:
        RAISE_EVENT ( SIE_GAIN_FOCUS, NULL );
        break;
    case SKIN_MSG_KILLFOCUS:
        RAISE_EVENT ( SIE_LOST_FOCUS, NULL );
        break;
    }
    return 1;
}

static skin_item_ops_t button_ops = {
    NULL,
    NULL,
    NULL,
    NULL,
    button_draw_bg,
    NULL,
    NULL,
    NULL,
    button_msg_proc
};

skin_item_ops_t *BUTTON_OPS = &button_ops;

