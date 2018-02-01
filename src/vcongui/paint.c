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
#include <unistd.h>
#include <sys/time.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include "mgutils.h"
#include "defs.h"
#include "vc.h"
#include "vt.h"
#include "paint.h"

#if 0
static inline int color_map (int color)
{
    switch (color) {
        case 1: return 4;
        case 3: return 5;
        case 4: return 1;
        case 5: return 6;
        case 6: return 3;
        case 9: return 12;
        case 11: return 13;
        case 12: return 9;
        case 13: return 14;
        case 14: return 11;
    }

    return color;
}
#else
#define color_map(c) c
#endif

void WindowClearAll (PCONINFO con, int color)
{
    HDC hdc;
    RECT rcClient;
    
    GetClientRect (con->hWnd, &rcClient);

    hdc = GetClientDC (con->hWnd);
    SetBrushColor (hdc, SysColorIndex [color_map (color)]);
    FillBox (hdc, rcClient.left, rcClient.top,
                  RECTW (rcClient), RECTH (rcClient));
    ReleaseDC (hdc);
}

void WindowScrollUp (PCONINFO con, int scroll, int color)
{
    RECT rcScroll;

    if ((con->ymin != 0)
            || (con->dispxmax != con->xmax)
            || (con->dispymax != con->ymax)) {
        rcScroll.left = 0;
        rcScroll.top  = 0;
        rcScroll.right = con->xmax * GetCharWidth ();
        rcScroll.bottom = con->ymax * GetCharHeight ();
        ScrollWindow (con->hWnd, 0, -scroll * GetCharHeight (), 
            &rcScroll, NULL);
    }
    else
        ScrollWindow (con->hWnd, 0, -scroll * GetCharHeight (), NULL, NULL);
        
}

void WindowScrollDown (PCONINFO con, int scroll, int color)
{
    RECT rcScroll;

    if ((con->ymin != 0)
            || (con->dispxmax != con->xmax)
            || (con->dispymax != con->ymax)) {
        rcScroll.left = 0;
        rcScroll.top  = 0;
        rcScroll.right = con->xmax * GetCharWidth ();
        rcScroll.bottom = con->ymax * GetCharHeight ();
        ScrollWindow (con->hWnd, 0, scroll * GetCharHeight (), 
            &rcScroll, NULL);
    }
    else
        ScrollWindow (con->hWnd, 0, scroll * GetCharHeight (), NULL, NULL);
}

void WindowStringPut (PCONINFO con, const char* text, 
                            int fc, int bc, int x, int y)
{
    HDC hdc;
    
#if 0
    {
    FILE *fff;

    fff = fopen("/tmp/ccegb-output.log", "a");
    fprintf (fff,"x=%d, y=%d, fc=%d, bc=%d, text=%s\n", 
                x, y, fc, bc, text);
	fclose (fff);
    }
#endif

    x *= GetCharWidth ();
    y *= GetCharHeight ();

    hdc = GetClientDC (con->hWnd);
    SetTextColor (hdc, SysColorIndex [color_map (fc) & 0x0F]);
    SetBkColor (hdc, SysColorIndex [color_map (bc) & 0x0000000F]);
    TextOut (hdc, x, y, text);
    ReleaseDC (hdc);
}

void WindowWput (PCONINFO con, WORD word, int fc, int bc, int pos)
{
    char szCharBuff [3];
    HDC hdc;
    int col, row;

    szCharBuff [0] = HIBYTE (word);
    szCharBuff [1] = LOBYTE (word);
    szCharBuff [2] = '\0';
   
    col = pos % con->cols;
    row = pos / con->cols;
   
    hdc = GetClientDC (con->hWnd);
    SetTextColor (hdc, SysColorIndex [color_map (fc) & 0x0F]);
    SetBkColor (hdc, SysColorIndex [color_map (bc) & 0x0F]);
    TextOut (hdc, col * GetCharWidth (), row * GetCharHeight (), szCharBuff);
    ReleaseDC (hdc);
}

void WindowSput (PCONINFO con, u_char ch, int fc, int bc, int pos)
{
    char szCharBuff [2];
    HDC hdc;
    int col, row;

    szCharBuff [0] = ch;
    szCharBuff [1] = '\0';
   
    col = pos % con->cols;
    row = pos / con->cols;
   
    hdc = GetClientDC (con->hWnd);
    SetTextColor (hdc, SysColorIndex [color_map (fc) & 0x0F]);
    SetBkColor (hdc, SysColorIndex [color_map (bc) & 0x0F]);
    TextOut (hdc, col * GetCharWidth (), row * GetCharHeight (), szCharBuff);
    ReleaseDC (hdc);
}

void WindowSetCursor (PCONINFO con, int x, int y, bool IsKanji)
{
    if (IsKanji)
        ChangeCaretSize (con->hWnd, GetCCharWidth (), GetCharHeight ());
    else
        ChangeCaretSize (con->hWnd, GetCharWidth (), GetCharHeight ());
    SetCaretPos (con->hWnd, x * GetCharWidth (), y * GetCharHeight ());
}


