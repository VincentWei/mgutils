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

#ifndef COLOR_PANEL_H
#define COLOR_PANEL_H

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#define CTRL_COLORPANEL   ("colorpanel")

BOOL RegisterColorPanel(void);

#define CP_MESSAGE  MSG_USER + 67

//set the color row and column
// int row = (int)wParam
// int col = (int)lParam
#define CP_SETROWCOL CP_MESSAGE + 1

//set the color of cell
//int cell_row = LOWORD(wParam);
//int cell_col = HIWORD(wParam);
//RGB rgb = (RGB)lParam
#define CP_SETCOLOR  CP_MESSAGE + 2

//append a color of cell
//RGB rgb = (RGB)lParam
#define CP_APPENDCOLOR CP_MESSAGE + 3

//insert a color of cell
//RGB rgb = (RGB)lParam;
#define CP_INSERTCOLOR CP_MESSAGE + 4

//set new select cell
//int row = (int)wParam
//int col = (int)lParam
#define CP_SETSELCELL  CP_MESSAGE + 5

//get select cell
// return MAKELPARAM(row_cell, col_cell)
#define CP_GETSELCELL CP_MESSAGE + 6

#define CPGetSelCell(hwnd, prow, pcol) do{ \
	LRESULT sel = SendMessage(hwnd, CP_GETSELCELL, 0, 0); \
	*(prow) = (int)(LOWORD(sel)); \
	*(pcol) = (int)(HIWORD(sel)); \
}while(0)


//set focus cell
//int row = (int)wParam
//int col = (int)lParam
#define CP_SETFOCUSCELL CP_MESSAGE + 7

//get focus cell
//return MAKELPARAM(row_cell, col_cell)
#define CP_GETFOCUSCELL CP_MESSAGE + 8

typedef struct _RGBCELLINFO{
	int begin_row;
	int begin_col;
	int cell_count;
	RGB * rgbInfo;
}RGBCELLINFO, *PRGBCELLINFO;
//set the cells colors
//PRGBCELLINFO rgbCellInfo = (PRGBCELLINFO)lParam;
#define CP_SETCELLS CP_MESSAGE + 9

#define CP_GETCELLCOLOR CP_MESSAGE + 10

#define CP_GETSELCOLOR CP_MESSAGE + 11

////////////////////////////////
#define CPN_NOTIFY  CP_MESSAGE + 100
//Notify message

//sel change
#define CPN_CELLSELCHANGE CPN_NOTIFY + 1

//focus change
#define CPN_CELLFOCUSCHANGE CPN_NOTIFY + 2

//color change
#define CPN_CELLCOLORCHANGE CPN_NOTIFY + 3

#define CPGetSelColor(hwnd, rgb) do { \
	LRESULT clr = SendMessage(hwnd, CP_GETSELCOLOR, 0, 0); \
    (rgb) = *(RGB*)clr; \
}while(0)

#define CPGetCellColor(hwnd, row, col,rgb) do { \
	LRESULT clr = SendMessage(hwnd, CP_GETCELLCOLOR, (WPARAM)(row),(LPARAM)(col)); \
    (rgb) = *(RGB*)clr; \
}while(0)

#endif
