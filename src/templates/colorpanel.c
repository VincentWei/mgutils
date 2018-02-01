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

#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include "mgutils.h"

#ifdef _MGUTILS_DLGCOLORSEL

#include "colorpanel.h"

#define CELL_INTERVAL  5

typedef struct _CELL_POS{
	int row:16;
	int col:16;
}CELL_POS;

typedef struct _COLORPANEL{
	HWND hwnd;

	BOOL is_active;

	int rows;
	int cols;

	CELL_POS focusCell;
	CELL_POS selCell;

	RGB * rgbs;

	int cell_width;
	int cell_height;

}COLORPANEL,*PCOLORPANEL;

#define THIS(hwnd)   ((PCOLORPANEL)GetWindowAdditionalData2(hwnd))
#define CELLIDX(pcp,col,row)  ((pcp)->cols*(row)+(col))
#define VALIDSEL(pcp, row, col) ((row)>=0 && (row)<(pcp)->rows && (col)>=0 && (col)<(pcp)->cols)

static inline void expandRect(RECT *rt, int exp)
{
	if(exp == 0 || rt == NULL)
		return;
	
	rt->left -= exp;
	rt->top -= exp;
	rt->right += exp;
	rt->bottom += exp;
}

static void cpGetCellRect(PCOLORPANEL pcp, int row, int col, RECT* rt)
{
	rt->left = (pcp->cell_width + CELL_INTERVAL) * col + CELL_INTERVAL;
	rt->right = rt->left + pcp->cell_width;
	rt->top = (pcp->cell_height + CELL_INTERVAL) * row + CELL_INTERVAL;
	rt->bottom = rt->top + pcp->cell_height;
}

static void cpGetSelCellRect(PCOLORPANEL pcp, int row, int col, RECT *rt)
{
	cpGetCellRect(pcp, row, col, rt);
	expandRect(rt, 2);
}

static void cpGetFocusCellRect(PCOLORPANEL pcp, int row, int col, RECT *rt)
{
	cpGetCellRect(pcp, row, col, rt);
	expandRect(rt,3);
}

static void cpFrameRect(HDC hdc, RECT *rt)
{
	MoveTo(hdc, rt->left, rt->top);
	LineTo(hdc, rt->right, rt->top);
	LineTo(hdc, rt->right, rt->bottom);
	LineTo(hdc, rt->left, rt->bottom);
	LineTo(hdc, rt->left, rt->top);

//	Rectangle(hdc, rt->left, rt->top, rt->right, rt->bottom);
}

static void cpDrawSelRect(HDC hdc, RECT *rt)
{
	SetPenType(hdc, PT_SOLID);
	SetPenColor(hdc, RGB2Pixel(hdc, 0,0,0));
	cpFrameRect(hdc, rt);
}

static void cpDrawFocusRect(HWND hwnd,HDC hdc, RECT *rt)
{
	WINDOW_ELEMENT_RENDERER* win_rdr ;
	win_rdr = GetWindowInfo(hwnd)->we_rdr;
	if(win_rdr)
	{
		win_rdr->draw_focus_frame(hdc, rt, 
			GetWindowElementAttr(hwnd, WE_FGC_THREED_BODY));	
	}
}

static void cpChangeFocus(PCOLORPANEL pcp, int new_row, int new_col)
{
	RECT rt;
	int old_row = 0, old_col = 0;

	if(new_row < 0)
		new_row = 0;
	else if(new_row >= pcp->rows)
		new_row = pcp->rows - 1;
	if(new_col < 0)
		new_col = 0;
	else if(new_col >= pcp->cols)
		new_col = pcp->cols - 1;

	if(new_col == pcp->focusCell.col && new_row == pcp->focusCell.row)
		return ;

	if(pcp->is_active){
		old_row = pcp->focusCell.row;
		old_col = pcp->focusCell.col;
	}
	
	pcp->focusCell.row = new_row;
	pcp->focusCell.col = new_col;

	if(pcp->is_active)
	{
		if(old_row >= 0 && old_row < pcp->rows 
			&& old_col >= 0 && old_col < pcp->cols)
		{
			cpGetFocusCellRect(pcp, old_row, old_col, &rt);
			expandRect(&rt,1);
			InvalidateRect(pcp->hwnd, &rt, TRUE);
		}

		cpGetFocusCellRect(pcp, new_row, new_col, &rt);
		expandRect(&rt,1);
		InvalidateRect(pcp->hwnd, &rt, TRUE);
		NotifyParent(pcp->hwnd,GetWindowInfo(pcp->hwnd)->id,CPN_CELLFOCUSCHANGE);
	}
}

static void cpChangeSel(PCOLORPANEL pcp, int new_row, int new_col)
{
	RECT rt;
	int old_row, old_col;

	if(new_row < 0)
		new_row = 0;
	else if(new_row >= pcp->rows)
		new_row = pcp->rows - 1;
	if(new_col < 0)
		new_col = 0;
	else if(new_col >= pcp->cols)
		new_col = pcp->cols - 1;

	if(new_col == pcp->selCell.col && new_row == pcp->selCell.row)
		return ;

	old_row = pcp->selCell.row;
	old_col = pcp->selCell.col;

	pcp->selCell.row = new_row;
	pcp->selCell.col = new_col;

	if(VALIDSEL(pcp, old_row, old_col))
	{
		cpGetSelCellRect(pcp, old_row, old_col, &rt);
		expandRect(&rt, 1);
		InvalidateRect(pcp->hwnd, &rt, TRUE);
	}

	cpGetSelCellRect(pcp, new_row, new_col, &rt);
	expandRect(&rt, 1);
	InvalidateRect(pcp->hwnd, &rt, TRUE);
	NotifyParent(pcp->hwnd,GetWindowInfo(pcp->hwnd)->id,CPN_CELLSELCHANGE);
}

static void cpDrawFocus(PCOLORPANEL pcp, HDC hdc)
{
	HDC hdct;
	RECT rt;
	if(pcp == NULL)
		return ;

	if(pcp->focusCell.row < 0 || pcp->focusCell.row >= pcp->rows
		|| pcp->focusCell.col < 0 || pcp->focusCell.col >= pcp->cols)
		return ;
	
	cpGetFocusCellRect(pcp, pcp->focusCell.row, pcp->focusCell.col, &rt);

	hdct = hdc;
	if(hdct == HDC_INVALID) 
		hdct = GetClientDC(pcp->hwnd);
	cpDrawFocusRect(pcp->hwnd, hdct, &rt);
	if(hdc == HDC_INVALID)
		ReleaseDC(hdct);

}

static void cpDrawCell(HDC hdc, int x, int y, int w, int h, RGB rgb)
{
	SetPenColor(hdc, RGB2Pixel(hdc, 60,60,60));
	MoveTo(hdc, x, y);
	LineTo(hdc, x+w-1, y);
	MoveTo(hdc, x, y);
	LineTo(hdc, x, y+h);

	SetPenColor(hdc, RGB2Pixel(hdc, 150,150,150));
	MoveTo(hdc, x+1, y+1);
	LineTo(hdc, x+w-2, y+1);
	MoveTo(hdc, x+1, y+1);
	LineTo(hdc, x+1, y+h-1);

	SetPenColor(hdc, RGB2Pixel(hdc, 255,255,255));
	MoveTo(hdc, x+w, y);
	LineTo(hdc, x+w, y+h);
	LineTo(hdc, x, y+h);
	MoveTo(hdc, x+w-1, y+1);
	LineTo(hdc, x+w-1, y+h-1);
	LineTo(hdc, x+1, y+h-1);

	SetBrushColor(hdc, RGB2Pixel(hdc, rgb.r, rgb.g, rgb.b));
	FillBox(hdc, x+2, y+2, w-3, h-3);
}

static void cpResize(PCOLORPANEL pcp, int width, int height)
{
	if(pcp->rows <= 0 || pcp->cols <= 0)
		return ;

	pcp->cell_width = (width - (pcp->cols+1)*CELL_INTERVAL)/pcp->cols;
	pcp->cell_height = (height - (pcp->rows+1)*CELL_INTERVAL)/pcp->rows;
}


static int cpSetRowCol(PCOLORPANEL pcp, int rows, int cols);
static int cpSetColor(PCOLORPANEL pcp, int rows, int cols, RGB rgb);
static int cpAppendColor(PCOLORPANEL pcp, RGB rgb);
static int cpInsertColor(PCOLORPANEL pcp, int row, int col, RGB rgb);
static int cpSetSel(PCOLORPANEL pcp, int row, int col);
static int cpSetFocus(PCOLORPANEL pcp, int row, int col);
static int cpSetCells(PCOLORPANEL pcp, PRGBCELLINFO rgbCellInfo);
static int cpGetCellByPos(PCOLORPANEL pcp, int x, int y, int* prow, int *pcol);

static LRESULT ColorPanelProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


BOOL RegisterColorPanel(void)
{
    WNDCLASS WndClass;
	WndClass.spClassName = CTRL_COLORPANEL;
	WndClass.dwStyle     = WS_NONE;
	WndClass.dwExStyle   = WS_EX_CLIPCHILDREN;
	WndClass.hCursor     = GetSystemCursor (IDC_ARROW);
	WndClass.iBkColor    = GetWindowElementPixel (HWND_NULL, WE_MAINC_THREED_BODY);
	WndClass.WinProc     = ColorPanelProc; 
	return RegisterWindowClass (&WndClass);
}

static LRESULT ColorPanelProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case MSG_CREATE:
	{
		PCOLORPANEL pcp = (PCOLORPANEL)calloc(1,sizeof(COLORPANEL));
		pcp->selCell.row = pcp->selCell.col = -1;

		pcp->hwnd = hWnd;
		SetWindowAdditionalData2(hWnd, (DWORD)pcp);
		return 0;
	}
	case MSG_SETFOCUS:
		{
			PCOLORPANEL pcp = THIS(hWnd);
			if(!pcp->is_active){
				pcp->is_active = TRUE;
				cpDrawFocus(pcp,HDC_INVALID);
			}
		}
		break;
	case MSG_KILLFOCUS:
		{
			PCOLORPANEL pcp = THIS(hWnd);
			if(pcp->is_active){
				RECT rt;
				pcp->is_active = FALSE;
				cpGetFocusCellRect(pcp,pcp->focusCell.row, pcp->focusCell.col, &rt);
				expandRect(&rt, 1);
				InvalidateRect(hWnd, &rt, FALSE);
			}
		}
		break;
	case CP_SETROWCOL:
		return cpSetRowCol(THIS(hWnd), wParam,lParam);
	case CP_SETCOLOR:
		return cpSetColor(THIS(hWnd), LOWORD(wParam), HIWORD(wParam), *(RGB*)(&lParam));
	case CP_APPENDCOLOR:
		return cpAppendColor(THIS(hWnd), *(RGB*)(&lParam));
	case CP_INSERTCOLOR:
		return cpInsertColor(THIS(hWnd), LOWORD(wParam), HIWORD(wParam), *(RGB*)(&lParam));
	case CP_SETSELCELL:
		return cpSetSel(THIS(hWnd), wParam, lParam);
	case CP_GETSELCELL:
		return MAKELONG (THIS(hWnd)->selCell.row, THIS(hWnd)->selCell.col);
	case CP_SETFOCUSCELL:
		return cpSetFocus(THIS(hWnd), wParam, lParam);
	case CP_GETFOCUSCELL:
		return MAKELONG (THIS(hWnd)->focusCell.row, THIS(hWnd)->focusCell.col);
	case CP_GETCELLCOLOR:
		{
			int row = (int)wParam;
			int col = (int)lParam;
			PCOLORPANEL pcp = THIS(hWnd);
			if(row < 0) 
				row = 0;
			else if(row >= pcp->rows)
				row = pcp->rows - 1;
			if(col < 0)
				col = 0;
			else if(col >= pcp->cols)
				col = pcp->cols - 1;
			return (LRESULT)&pcp->rgbs[CELLIDX(pcp, col, row)];
		}
	case CP_GETSELCOLOR:
		{
			PCOLORPANEL pcp = THIS(hWnd);
			return SendMessage(hWnd, CP_GETCELLCOLOR, pcp->selCell.row, pcp->selCell.col);
		}
	case CP_SETCELLS:
		return cpSetCells(THIS(hWnd), (PRGBCELLINFO)lParam);
	
	case MSG_NCPAINT:
		return 0;
	case MSG_PAINT:
		{
			HDC hdc = BeginPaint(hWnd);
			PCOLORPANEL pcp = THIS(hWnd);
			RECT rt;
			int i;
			int j;
			int idx=0;
			int x;
			int y=CELL_INTERVAL;
			for(i=0;i<pcp->rows;i++)
			{
				x = CELL_INTERVAL;
				for(j=0;j<pcp->cols;j++)
				{
					cpDrawCell(hdc, x, y, pcp->cell_width, pcp->cell_height, pcp->rgbs[idx]);
                    _MG_PRINTF ("Color of cell (%d, %d): %x, %x, %x\n", i, j, 
                            pcp->rgbs[idx].r, pcp->rgbs[idx].g, pcp->rgbs[idx].b);
					idx ++;
					x += (pcp->cell_width + CELL_INTERVAL);
				}
				y += (pcp->cell_height + CELL_INTERVAL);
			}
			
			if(VALIDSEL(pcp, pcp->selCell.row, pcp->selCell.col)){
				cpGetSelCellRect(pcp, pcp->selCell.row, pcp->selCell.col, &rt);
				cpDrawSelRect(hdc, &rt);
			}

			if(pcp->is_active)
			{
				cpGetFocusCellRect(pcp, pcp->focusCell.row, pcp->focusCell.col, &rt);
				cpDrawFocusRect(hWnd, hdc, &rt);
			}

			EndPaint (hWnd, hdc);
			return 0;
		}
		case MSG_SIZECHANGED:
		{
			RECT *rtClient = (RECT*)lParam;
			
			cpResize(THIS(hWnd),rtClient->right - rtClient->left, rtClient->bottom - rtClient->top);

			break;
		}
		case MSG_DESTROY:		
		{
			PCOLORPANEL pcp = THIS(hWnd);
			if(pcp->rgbs)
				free(pcp->rgbs);
			free(pcp);
			break;
		}
		case MSG_KEYDOWN:
		{
			PCOLORPANEL pcp = THIS(hWnd);
			switch(wParam)
			{
			case SCANCODE_CURSORBLOCKUP:
				cpChangeFocus(pcp, pcp->focusCell.row-1, pcp->focusCell.col);
				break;
			case SCANCODE_CURSORBLOCKDOWN:
				cpChangeFocus(pcp, pcp->focusCell.row+1, pcp->focusCell.col);
				break;
			case SCANCODE_CURSORBLOCKLEFT:
				cpChangeFocus( pcp, pcp->focusCell.row, pcp->focusCell.col-1);
				break;
			case SCANCODE_CURSORBLOCKRIGHT:
				cpChangeFocus(pcp, pcp->focusCell.row, pcp->focusCell.col+1);
				break;
			case SCANCODE_SPACE:
				cpChangeSel(pcp, pcp->focusCell.row, pcp->focusCell.col);
				break;
			}
			break;
		}
	case MSG_GETDLGCODE:
		return DLGC_WANTARROWS;
	case MSG_LBUTTONDOWN:
		{
			int row, col;
			PCOLORPANEL pcp = THIS(hWnd);
			if(cpGetCellByPos(pcp, LOWORD(lParam), HIWORD(lParam), &row, &col))
			{
				pcp->focusCell.row = pcp->selCell.row = row;
				pcp->focusCell.col = pcp->selCell.col = col;
				InvalidateRect(hWnd, NULL, TRUE);
				NotifyParent(hWnd,GetWindowInfo(hWnd)->id,CPN_CELLSELCHANGE);
				NotifyParent(hWnd,GetWindowInfo(hWnd)->id,CPN_CELLFOCUSCHANGE);
			}
		}
		return 0;
	}

	return DefaultControlProc(hWnd, message, wParam, lParam); 
}

static int cpSetRowCol(PCOLORPANEL pcp, int rows, int cols)
{
	RECT rt;
	if(pcp == NULL)
		return 0;

	if(pcp->rows == rows && pcp->cols == cols)
		return 0;

	if(pcp->rgbs)
		free(pcp->rgbs);
	
	pcp->rows = rows;
	pcp->cols = cols;

	pcp->rgbs = (RGB*)calloc(rows * cols, sizeof(RGB));
	
	pcp->selCell.row = -1;
	pcp->selCell.col = -1;
	pcp->focusCell.row = 0;
	pcp->focusCell.col = 0;

	GetClientRect(pcp->hwnd, &rt);

	cpResize(pcp, rt.right-rt.left, rt.bottom-rt.top);

	return 0;
}

static int cpSetColor(PCOLORPANEL pcp, int row, int col, RGB rgb)
{
	RECT rt;
	if(pcp == NULL)
		return 0;
	
	if(row<0 || row >= pcp->rows
		|| col<0 || col>= pcp->cols)
		return 0;
	
	pcp->rgbs[CELLIDX(pcp,row,col)] = rgb;

	InvalidateRect(pcp->hwnd, &rt, TRUE);
	NotifyParent(pcp->hwnd, GetWindowInfo(pcp->hwnd)->id,CPN_CELLCOLORCHANGE);
	return 0;
}

static int cpAppendColor(PCOLORPANEL pcp, RGB rgb)
{
	return 0;
}

static int cpInsertColor(PCOLORPANEL pcp, int row, int col, RGB rgb)
{
	int idx;
	int i;
	if(pcp == NULL)
		return 0;
	
	if(row<0 || row>= pcp->rows
		|| col<0 || col>= pcp->cols)
		return 0;

	idx = CELLIDX(pcp, row, col);
	for(i=pcp->rows*pcp->cols-1; i>idx;i--)
		pcp->rgbs[i] = pcp->rgbs[i-1];

	pcp->rgbs[i] = rgb;

	InvalidateRect(pcp->hwnd, NULL, TRUE);

	return 0;
}

static int cpSetSel(PCOLORPANEL pcp, int row, int col)
{
	RECT rtOld, rtNew;
	BOOL bcancel = FALSE;
	int old_row, old_col;
	
	if(row<0 || row>= pcp->rows
		|| col<0 || col>= pcp->cols)
		bcancel = TRUE;

	if(bcancel && !VALIDSEL(pcp, pcp->selCell.row, pcp->selCell.col))
		return 0;

	if(pcp->selCell.row == row && pcp->selCell.col == col)
		return 0;

	old_row = pcp->selCell.row;
	old_col = pcp->selCell.col;
	
	pcp->selCell.row = row;
	pcp->selCell.col = col;

	
	if(VALIDSEL(pcp, old_row, old_col ))
	{
		cpGetSelCellRect(pcp, old_row, old_col, &rtOld);
		expandRect(&rtOld,1);
		InvalidateRect(pcp->hwnd, &rtOld, TRUE);
	}
	
	if(!bcancel)
	{
		cpGetSelCellRect(pcp, row, col, &rtNew);
		expandRect(&rtNew,1);
		InvalidateRect(pcp->hwnd, &rtNew, TRUE);
	}

	return 0;
}

static int cpSetFocus(PCOLORPANEL pcp, int row, int col)
{
	RECT rtOld, rtNew;

	if(row<0 || row>= pcp->rows
		|| col<0 || col>= pcp->cols)
		return 0;

	if(pcp->focusCell.row == row && pcp->focusCell.col == col)
		return 0;
	
	cpGetFocusCellRect(pcp, pcp->focusCell.row, pcp->focusCell.col, &rtOld);

	pcp->focusCell.row = row;
	pcp->focusCell.col = col;

	cpGetFocusCellRect(pcp, row, col, &rtNew);

	InvalidateRect(pcp->hwnd, &rtOld, FALSE);
	InvalidateRect(pcp->hwnd, &rtNew, TRUE);

	return 0;

}

static int cpSetCells(PCOLORPANEL pcp, PRGBCELLINFO rgbCellInfo)
{
	int idx;
	int count;
	int i;

	if(pcp == NULL || rgbCellInfo == NULL)
		return 0;
	
	if(rgbCellInfo->begin_row < 0 || rgbCellInfo->begin_row >= pcp->rows
		|| rgbCellInfo->begin_col < 0 || rgbCellInfo->begin_col >= pcp->cols)
		return 0;

	idx = CELLIDX(pcp, rgbCellInfo->begin_col, rgbCellInfo->begin_row);
	count = pcp->rows * pcp->cols;
	
	for(i=0;i<rgbCellInfo->cell_count && idx<count;idx++,i++)
	{
		pcp->rgbs[idx] = rgbCellInfo->rgbInfo[i];
	}

	if(i>0)
	{
		InvalidateRect(pcp->hwnd, NULL, TRUE);
		NotifyParent(pcp->hwnd, GetWindowInfo(pcp->hwnd)->id,CPN_CELLCOLORCHANGE);
	}
	return i;
}

static int cpGetCellByPos(PCOLORPANEL pcp, int x, int y, int* prow, int *pcol)
{
	int row;
	int col;
	if(pcp == NULL || pcp->cell_height <= 0 || pcp->cell_width <= 0)
		return 0;

	row = (y - CELL_INTERVAL) / (pcp->cell_height + CELL_INTERVAL);
	if(((y - CELL_INTERVAL) % (pcp->cell_height + CELL_INTERVAL)) > pcp->cell_height){
		return 0;
	}

	col = ( x - CELL_INTERVAL) / (pcp->cell_width+CELL_INTERVAL);
	if(((x - CELL_INTERVAL) % (pcp->cell_width + CELL_INTERVAL)) > pcp->cell_width)
	{
		return 0;
	}

	if(prow) 
		*prow = row;
	if(pcol)
		*pcol = col;
	
	return 1;
}

#endif /* _MGUTILS_DLGCOLORSEL */

