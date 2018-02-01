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

#define EMPTY    ""

/*type : si_nrmlabel_t or si_bmplabel_t */
#define DECL_LABEL(type,ret) \
    type *mylabel = (type *) item->type_data; \
    if (!mylabel) return ret;

static DWORD get_label (skin_item_t *item)
{
    DECL_LABEL ( si_nrmlabel_t, (DWORD)NULL );

    return (DWORD)mylabel->label;
}

#define NRM_MAX(a,b)   (a > b ? a : b)
static DWORD set_label (skin_item_t *item, DWORD label)
{
    HDC hdc;
    SIZE Size;
    RECT Rect;
    char new_label[1024] = {0};

    DECL_LABEL ( si_nrmlabel_t, (DWORD)FALSE );
    
    if ( (char *)label )    strcpy (new_label, (char *)label );

    /* new label's size */
    hdc = GetDC (item->hostskin->hwnd);
    SelectFont (hdc, (PLOGFONT)(&item->hostskin->fonts[mylabel->font_index]));
    GetTextExtent (hdc, new_label, strlen(new_label), &Size);
    ReleaseDC (hdc);

    /* Rect is the rect area that we should erase */
    memcpy (&Rect, &item->rc_hittest, sizeof(RECT) );
    Rect.right    = Rect.left + NRM_MAX( RECTW(Rect), Size.cx );
    Rect.bottom    = Rect.top  + NRM_MAX( RECTH(Rect), Size.cy );

    /* set new label */
    if (mylabel->label)    free (mylabel->label);
    mylabel->label = strdup (new_label);

    /* new label's hittest rect */
    item->rc_hittest.right    = item->rc_hittest.left + Size.cx;
    item->rc_hittest.bottom    = item->rc_hittest.top  + Size.cy;
    
    /* show new label */
    InvalidateRect ( item->hostskin->hwnd, &Rect, TRUE);

    /* we should refresh it's region also ... */

    return TRUE;
}

static int nrmlabel_on_create (skin_item_t* item)
{
    HDC hdc;
    SIZE Size;
    DECL_LABEL ( si_nrmlabel_t, 0 );

    /* init label's hittest rectangle  */
    hdc = GetDC (item->hostskin->hwnd);
    SelectFont (hdc, (PLOGFONT)(&item->hostskin->fonts[mylabel->font_index]));
    GetTextExtent (hdc, mylabel->label, strlen(mylabel->label), &Size);
    ReleaseDC (hdc);

    item->rc_hittest.right  = item->rc_hittest.left + Size.cx;
    item->rc_hittest.bottom = item->rc_hittest.top  + Size.cy;

    return 1;
}

static int nrmlabel_init (skin_head_t *skin, skin_item_t *item)
{
    DECL_LABEL ( si_nrmlabel_t, 0);

    mylabel->label = strdup ( mylabel->label ? mylabel->label : EMPTY );

    return 1;
}

static int nrmlabel_deinit (skin_item_t *item)
{
    DECL_LABEL ( si_nrmlabel_t, 0);

    if (mylabel->label) free (mylabel->label);

    return 1;
}

#define RGB_TO_PIXEL(hdc,color)    \
        RGB2Pixel (hdc,         \
            GetRValue (color),     \
            GetGValue (color),     \
            GetBValue (color))
static void nrmlabel_draw_attached (HDC hdc, skin_item_t* item)
{
    gal_pixel text_color;
    si_nrmlabel_t *mylabel = (si_nrmlabel_t *) item->type_data;
    if ( !mylabel )    return;
    if ( !(&item->hostskin->fonts[mylabel->font_index]) || !mylabel->label )
        return;

    if (item->style & SI_STATUS_CLICKED) 
        text_color = RGB_TO_PIXEL(hdc, mylabel->color_click);
    else if (item->style & SI_STATUS_HILIGHTED) 
        text_color = RGB_TO_PIXEL(hdc, mylabel->color_focus);
    else 
        text_color = RGB_TO_PIXEL(hdc, mylabel->color);

    SetTextColor (hdc, text_color);
    SetBkMode (hdc, BM_TRANSPARENT);
    SelectFont (hdc, (PLOGFONT)(&item->hostskin->fonts[mylabel->font_index]));
    DrawText (hdc, mylabel->label, -1, &(item->rc_hittest), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
}

static skin_item_ops_t nrmlabel_ops = {
    nrmlabel_init,
    nrmlabel_deinit,
    nrmlabel_on_create ,
    NULL,
    NULL,
    nrmlabel_draw_attached,
    get_label,
    set_label,
    NULL
};

skin_item_ops_t *NRMLABEL_OPS = &nrmlabel_ops;

