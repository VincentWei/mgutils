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

#define DECLARE(type, var, ret) \
    type *var = (type *) item->type_data; \
    if (!var) return ret;
    
static DWORD slider_get_pos (skin_item_t *item) 
{
    DECLARE (si_nrmslider_t, slider, 0);

    slider = (si_nrmslider_t*) item->type_data;
    return slider->slider_info.cur_pos;
}

static DWORD slider_set_pos (skin_item_t *item, DWORD pos) 
{
    DECLARE (si_nrmslider_t, slider, 0);

    slider = (si_nrmslider_t*) item->type_data;

    if ( pos < slider->slider_info.min_pos ) pos = slider->slider_info.min_pos;
    if ( pos > slider->slider_info.max_pos ) pos = slider->slider_info.max_pos;
    slider->slider_info.cur_pos = pos;
    return 1;
}

static void slider_draw_bg (HDC hdc, skin_item_t *item)
{
    FillBoxWithBitmap (hdc, item->x, item->y, 0, 0, &ITEMBMP(item));
}

static void slider_draw_attached (HDC hdc, skin_item_t* item)
{
    int pos_min, pos_max, x, y, w, h;
    si_nrmslider_t* slider = (si_nrmslider_t*) item->type_data;
    double percent;
    
    if ( slider->slider_info.cur_pos < slider->slider_info.min_pos ) 
        slider->slider_info.cur_pos = slider->slider_info.min_pos;
    if ( slider->slider_info.cur_pos > slider->slider_info.max_pos )
        slider->slider_info.cur_pos = slider->slider_info.max_pos;

    percent = (slider->slider_info.cur_pos - slider->slider_info.min_pos) * 1. /
                     (slider->slider_info.max_pos - slider->slider_info.min_pos);
                     
    if (item->style & SI_NRMSLIDER_HORZ) {    /* - */
        pos_min = item->rc_hittest.left;
        pos_max = item->rc_hittest.right - BMP(item, slider->thumb_bmp_index).bmWidth;
        x = pos_min + (pos_max - pos_min) * percent; 
        y = item->rc_hittest.top;
        // pbar
        w = RECTW(item->rc_hittest) * percent;
        h = RECTH(item->rc_hittest);

        h = h < BMP(item, slider->thumb_bmp_index).bmHeight ? 
            h : BMP(item, slider->thumb_bmp_index).bmHeight; 
    }
    else{    /* | */
        pos_min = item->rc_hittest.top;
        pos_max = item->rc_hittest.bottom - BMP(item, slider->thumb_bmp_index).bmHeight;
        y = pos_min + (pos_max - pos_min) * percent;
        x = item->rc_hittest.left;
        // pbar
        h = RECTH(item->rc_hittest) * percent;
        w = RECTW(item->rc_hittest);

        w = w < BMP(item, slider->thumb_bmp_index).bmWidth? 
            w : BMP(item, slider->thumb_bmp_index).bmWidth; 
    }
    if ( item->style & SI_NRMSLIDER_STATIC )
        FillBoxWithBitmap ( hdc, item->rc_hittest.left, item->rc_hittest.top, 
                            w, h, &BMP(item, slider->thumb_bmp_index) );
    else
        FillBoxWithBitmapPart ( hdc, x, y, 
                                BMP(item, slider->thumb_bmp_index).bmWidth,
                                BMP(item, slider->thumb_bmp_index).bmHeight,
                                0, 0, 
                                &BMP(item, slider->thumb_bmp_index),
                                0, 0);
}

static int get_changed_pos (skin_item_t* item, int x, int y)
{
    sie_slider_t sie;
    int cur_pos = 0;
    si_nrmslider_t* nrmslider = (si_nrmslider_t*) item->type_data;
    
    sie.min_pos = 0;

    if (item->style & SI_NRMSLIDER_HORZ) {    /* - */
        sie.max_pos = RECTW (item->rc_hittest)  - BMP(item,nrmslider->thumb_bmp_index).bmWidth;
        sie.cur_pos = x - item->rc_hittest.left - BMP(item,nrmslider->thumb_bmp_index).bmWidth/2;

        ROUND (sie.cur_pos, sie.min_pos, sie.max_pos);

        cur_pos = .5 + nrmslider->slider_info.min_pos + sie.cur_pos *
            (0. + nrmslider->slider_info.max_pos - nrmslider->slider_info.min_pos) / sie.max_pos;
    }
    else {    /* | */
        sie.max_pos = RECTH (item->rc_hittest)  - BMP(item,nrmslider->thumb_bmp_index).bmHeight;
        sie.cur_pos = y - item->rc_hittest.top  - BMP(item,nrmslider->thumb_bmp_index).bmHeight/2;

        ROUND (sie.cur_pos, sie.min_pos, sie.max_pos);

        cur_pos = .5 + nrmslider->slider_info.min_pos + sie.cur_pos *
            (0. + nrmslider->slider_info.max_pos - nrmslider->slider_info.min_pos) / sie.max_pos;
    }
    return cur_pos;
}
static int slider_msg_proc (skin_item_t* item, int message, WPARAM wparam, LPARAM lparam)
{
    int pos = 0;
    
    if ( item->style & SI_NRMSLIDER_STATIC )
        return 0;
    
    switch (message) {
//    case SKIN_MSG_CLICK:     /* SLIDER_CHANGED event */
    case SKIN_MSG_LBUTTONDOWN:
    case SKIN_MSG_MOUSEDRAG:     /* SLIDER_CHANGED event */
        pos = get_changed_pos (item, (int)wparam, (int)lparam);
        /* default operation */
        if (skin_get_thumb_pos (item->hostskin, item->id) != pos) {
            skin_set_thumb_pos ( item->hostskin, item->id, pos);
            RAISE_EVENT ( SIE_SLIDER_CHANGED, (void *)(DWORD)pos );
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

static skin_item_ops_t slider_ops = {
    NULL,
    NULL,
    NULL,
    NULL,
    slider_draw_bg,
    slider_draw_attached,
    slider_get_pos,
    slider_set_pos,
    slider_msg_proc
};

skin_item_ops_t *SLIDER_OPS = &slider_ops;

