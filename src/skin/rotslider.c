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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <minigui/fixedmath.h>

#ifdef _MGHAVE_FIXED_MATH

#include "mgutils.h"
#include "item_comm.h"

#ifndef M_PI
#define M_PI    3.14159265358979323846
#endif

#define DECLARE_VOID(type, var) \
    type *var = (type *) item->type_data; \
    if (!var) return;
#define DECLARE(type, var, ret) \
    type *var = (type *) item->type_data; \
    if (!var) return ret;

/* make degree belong [0, 360) */
static void adjust_degree ( int *degree )
{
    while ( *degree < 0 ){
        *degree += 360;
    };
    *degree %= 360;
}

/* wether degree between start_deg and end_deg (both belong [0, 360)) */
/* according to direction (clock or anitclock) */
static BOOL degree_between ( int degree, int start_deg, int end_deg, BOOL clock )
{
    adjust_degree ( & degree    );
    adjust_degree ( & start_deg    );
    adjust_degree ( & end_deg    );

    if ( clock ){    /* clock direction */
        if ( end_deg > start_deg )    end_deg -= 360;
        if ( degree  > start_deg )    degree  -= 360;
        return ( degree >= end_deg );
    }
    else{            /* anticlock direc */
        if ( end_deg < start_deg )    end_deg += 360;
        if ( degree  < start_deg )    degree  += 360;
        return ( degree <= end_deg );
    }
}


static DWORD rotslider_get_pos (skin_item_t *item)
{
    DECLARE (si_rotslider_t, slider, -1);

    return slider->cur_pos;
}

static DWORD rotslider_set_pos (skin_item_t *item, DWORD pos)
{
    DWORD old_pos;
    DECLARE (si_rotslider_t, slider, -1);

    old_pos = slider->cur_pos;
    slider->cur_pos = pos;
    if ( !degree_between(pos, slider->start_deg, slider->end_deg, item->style&SI_ROTSLIDER_CW) )
        slider->cur_pos = slider->start_deg;
    return old_pos;
}

static void rotslider_draw_bg (HDC hdc, skin_item_t *item)
{
    FillBoxWithBitmap (hdc, item->x, item->y, 0, 0, &ITEMBMP(item));
}

#define CAL_GOU(r, d)       \
        fixtoi (fixmul (itofix(r), fixcos (ftofix(d))))

#define CAL_GU(r, d)       \
        fixtoi (fixmul (itofix(r), fixsin (ftofix(d))))

static void rotslider_draw_attached (HDC hdc, skin_item_t* item)
{
    int xo = 0, degree;
    const BITMAP *bmp;
    DECLARE_VOID (si_rotslider_t, slider);

    rotslider_set_pos (item, (DWORD)slider->cur_pos);

    if ( item->style & SI_STATUS_CLICKED )
        xo = 1;
    else if ( item->style & SI_STATUS_HILIGHTED )
        xo = 2;

    bmp = &BMP(item, slider->thumb_bmp_index );
    degree = slider->cur_pos;
    adjust_degree ( &degree );
    FillBoxWithBitmapPart (hdc,
        (item->rc_hittest.left + item->rc_hittest.right)/2 +
            CAL_GOU (slider->radius, (degree*M_PI/180)) - bmp->bmWidth/3/2,
        (item->rc_hittest.top+ item->rc_hittest.bottom)/2 -
            CAL_GU (slider->radius, (degree*M_PI/180)) -bmp->bmHeight/2,
        bmp->bmWidth/3, bmp->bmHeight, 0, 0,
        bmp, xo * bmp->bmWidth / 3, 0);
}

static int get_changed_pos (skin_item_t* item, int x, int y)
{
    int cx, cy, degree;
    fixed radius;
    fixed radian_acos, radian;
    DECLARE (si_rotslider_t, slider, -1);

    cx = (item->shape.left + item->shape.right) / 2;
    cy = (item->shape.top + item->shape.bottom) / 2;
    radius = fixhypot ( itofix (cx - x), itofix (cy - y));
    radian_acos = fixacos (fixdiv (itofix (x - cx), radius));
    radian = cy < y ? (fixsub (ftofix (2 * M_PI), radian_acos)) : radian_acos;
    degree = ((int)(fixtof (fixmul (radian, itofix (180))) / M_PI + 0.5) + 360) % 360;

    /* we've got current degree which belong [0, 360) */
    if ( !degree_between(degree, slider->start_deg, slider->end_deg, item->style&SI_ROTSLIDER_CW) )
        degree = slider->cur_pos;

    return degree;
}

static int rotslider_msg_proc (skin_item_t* item, int message, WPARAM wparam, LPARAM lparam)
{
    int pos = 0;

    if ( item->style & SI_ROTSLIDER_STATIC )
        return 0;

    switch (message) {
    case SKIN_MSG_LBUTTONDOWN:
        skin_set_check_status ( item->hostskin, item->id, TRUE );
    case SKIN_MSG_MOUSEDRAG:     /* ROTSLIDER_CHANGED event */
        /* calculate new degree [start_deg, end_deg] belong to [0,2*Pi) */
        pos = get_changed_pos (item, wparam, lparam);
        RAISE_EVENT ( SIE_SLIDER_CHANGED, (void *)(DWORD)pos );
        /* default operation */
        skin_set_thumb_pos (item->hostskin, item->id, pos);
        break;
    case SKIN_MSG_LBUTTONUP:
        skin_set_check_status ( item->hostskin, item->id, FALSE );
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

static skin_item_ops_t rotslider_ops = {
    NULL,
    NULL,
    NULL,
    NULL,
    rotslider_draw_bg,
    rotslider_draw_attached,
    rotslider_get_pos,
    rotslider_set_pos,
    rotslider_msg_proc
};

skin_item_ops_t *ROTSLIDER_OPS = &rotslider_ops;

#endif /* _MGHAVE_FIXED_MATH */
