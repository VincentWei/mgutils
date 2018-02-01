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

typedef struct hsv_struct 
{
    Uint16 clrh;
    Uint8  clrs;
    Uint8  clrv;
} HSV;

typedef struct indication 
{
    int px;
    int py;
    int pz;
} INDICAT;

typedef struct scd_struct 
{
    HDC             SpaceDC;
    PCOLORDLGDATA   pclr;
    INDICAT         indicat;
    RECT            rcSpace;
    RECT            rcYSpace;
    RECT            rcSelSpace;
    HSV             hsv;     
} SCOLORDIA, *PSCOLORDIA;

#define px          indicat.px
#define py          indicat.py
#define pz          indicat.pz

#define clrh        hsv.clrh
#define clrs        hsv.clrs
#define clrv        hsv.clrv

#define H           pclr->h
#define S           pclr->s
#define V           pclr->v
#define R           pclr->r
#define G           pclr->g
#define B           pclr->b
#define PIXEL       pclr->pixel

CTRLDATA DefColorCtrl [] =
{ 
    { CTRL_STATIC, 
        WS_VISIBLE | SS_LEFT,
        7, 5, 60, 20,
        IDC_CSD_BASIC_COLOR_NOTE, 
        "basic color",  
        0, WS_EX_TRANSPARENT 
    },

    {
        CTRL_COLORPANEL,
        WS_VISIBLE | WS_TABSTOP ,
        0,30,25*6+5,(16+5)*5+5,
        IDC_CSD_BASIC_COLOR,
        "",
        0, WS_EX_TRANSPARENT
    },

    { CTRL_STATIC, 
        WS_VISIBLE | SS_LEFT,
        7, 140, 60, 20,
        IDC_CSD_BASIC_COLOR_NOTE, 
        "custom color",  
        0, WS_EX_TRANSPARENT 
    },

    {
        CTRL_COLORPANEL,
        WS_VISIBLE | WS_TABSTOP ,
        0, 160, 25*6+5, 26,
        IDC_CSD_CUSTOM,
        "",
        0, WS_EX_TRANSPARENT
    },

    { CTRL_STATIC, 
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        210, 122, 15, 18,
        IDC_CSD_NOTE_H, 
        "H",  
        0, WS_EX_TRANSPARENT
    },

    { CTRL_STATIC, 
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        210, 142, 15, 18,
        IDC_CSD_NOTE_S, 
        "S",  
        0, WS_EX_TRANSPARENT
    },

    { CTRL_STATIC, 
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        210, 162, 15, 18,
        IDC_CSD_NOTE_V, 
        "V",  
        0, WS_EX_TRANSPARENT
    },

    { CTRL_STATIC, 
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        260, 122, 15, 18,
        IDC_CSD_NOTE_R, 
        "R",  
        0, WS_EX_TRANSPARENT
    },

    { CTRL_STATIC, 
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        260, 142, 15, 18,
        IDC_CSD_NOTE_G, 
        "G",  
        0, WS_EX_TRANSPARENT
    },

    { CTRL_STATIC, 
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        260, 162, 15, 18,
        IDC_CSD_NOTE_B, 
        "B",  
        0, WS_EX_TRANSPARENT
    },

    { CTRL_SLEDIT, 
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
        225, 120, 27, 18,
        IDC_CSD_VALUE_H, 
        "180",  
        0,0 
    },

    { CTRL_SLEDIT, 
        WS_CHILD | WS_VISIBLE | WS_BORDER |WS_TABSTOP,
        225, 140, 27, 18,
        IDC_CSD_VALUE_S, 
        "50",  
        0,0 
    },

    { CTRL_SLEDIT, 
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
        225, 160, 27, 18,
        IDC_CSD_VALUE_V, 
        "50",  
        0,0 
    },

    { CTRL_SLEDIT, 
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
        280, 120, 27, 18,
        IDC_CSD_VALUE_R, 
        "64",  
        0,0 
    },

    { CTRL_SLEDIT, 
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
        280, 140, 27, 18,
        IDC_CSD_VALUE_G, 
        "128",  
        0, 0
    },

    { CTRL_SLEDIT, 
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
        280, 160, 27, 18,
        IDC_CSD_VALUE_B, 
        "128",  
        0,0 
    },

    { CTRL_BUTTON, 
        WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
        160 , 185, 150, 20 , 
        IDC_CSD_ADD, 
        "Add to custom", 
        0 
    },


    { CTRL_BUTTON, 
        WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
        7, 185, 70, 20, 
        IDC_CSD_OK, 
        "Ok", 
        0 
    },

    { CTRL_BUTTON, 
        WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
        80, 185, 70, 20, 
        IDC_CSD_CANCEL, 
        "Cancel", 
        0 
    }

};

DLGTEMPLATE DefColorDlg =
{
    WS_DLGFRAME | WS_BORDER | WS_CAPTION, 
    WS_EX_CLIPCHILDREN,
    0, 0, 320, 240, 
    "Color", 0, 0, 
    TABLESIZE(DefColorCtrl), 
    DefColorCtrl 
};

CTRLDATA DefSimpleColorCtrl [] =
{
    {
        CTRL_COLORPANEL,
        WS_VISIBLE | WS_TABSTOP ,
        0,0,25*6+5,(16+5)*5+5,
        IDC_CSD_BASIC_COLOR,
        "",
        0, WS_EX_TRANSPARENT
    },

    { CTRL_BUTTON, 
        WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
        7, 115, 70, 20, 
        IDC_CSD_OK, 
        "Ok", 
        0 
    },

    { CTRL_BUTTON, 
        WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
        80, 115, 70, 20, 
        IDC_CSD_CANCEL, 
        "Cancel", 
        0 
    }

};

DLGTEMPLATE DefSimpleColorDlg =
{
    WS_DLGFRAME | WS_BORDER | WS_CAPTION, 
    WS_EX_NONE,
    0, 0, 163, 165, 
    "Color", 0, 0, 
    TABLESIZE(DefSimpleColorCtrl), 
    DefSimpleColorCtrl 
};

static inline void drawWindow3DFrame(HWND hwnd, HDC hdc, const RECT* rt)
{
    WINDOW_ELEMENT_RENDERER * wrdr = GetWindowInfo(hwnd)->we_rdr;
    if(wrdr){
		DWORD color;
        HDC hdct = hdc;
        if(hdct == HDC_INVALID)
            hdct = GetClientDC(hwnd);
        color = GetWindowElementPixel(hwnd, WE_FGC_THREED_BODY);
        wrdr->draw_3dbox(hdc, rt, color, LFRDR_BTN_STATUS_PRESSED);
    }
}

static BOOL getSelectColorPanel(HWND hDlg, RGB *rgb)
{
    int row, col;
    HWND hctrl;

    hctrl = GetDlgItem(hDlg, IDC_CSD_BASIC_COLOR);
    CPGetSelCell(hctrl, &row, &col);
    if(row < 0 || col < 0){
        hctrl = GetDlgItem(hDlg, IDC_CSD_CUSTOM);
        CPGetSelCell(hctrl, &row, &col);
        if(row < 0 || col < 0)
            return FALSE;
    }

    CPGetCellColor(hctrl, row, col, *rgb);
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////
//

//sub class of edit
static WNDPROC _oldSleditProc;
#define MSG_GETINTVALUE  MSG_USER+100
#define MSG_SETINTVALUE  MSG_USER+101
#define SetEditInt(hwnd,v)    SendMessage(hwnd, MSG_SETINTVALUE, (WPARAM)v, 0)
#define GetEditInt(hwnd)      SendMessage(hwnd, MSG_GETINTVALUE, 0, 0)
#define SetDlgEditInt(hdlg, id, v) SendDlgItemMessage(hdlg, id, MSG_SETINTVALUE, (WPARAM)v, 0)
#define GetDlgEditInt(hdlg, id)    SendDlgItemMessage(hdlg, id, MSG_GETINTVALUE, 0, 0)

static int getSlEditTextPrepareInsert(HWND hwnd, int ch, char* szText, int maxlen)
{
    int line_pos, char_pos;
    int len, insertbegin, insertend;
    int i;
    SendMessage(hwnd,EM_GETSEL,maxlen, (LPARAM)szText);
    if(GetWindowText(hwnd, szText, maxlen-1)<=0)
        return 0;
    
    insertbegin = SendMessage(hwnd, EM_GETSELPOS,(WPARAM)&line_pos, (LPARAM)&char_pos);
    insertend = SendMessage(hwnd, EM_GETCARETPOS,(WPARAM)&line_pos, (LPARAM)&char_pos);
    if(insertbegin > insertend){
        int tmp = insertbegin;
        insertbegin = insertend;
        insertend = tmp;
    }

    len = strlen(szText);

    if(insertbegin == insertend)
    {
        for(i=len+1; i>insertbegin; i--)
            szText[i] = szText[i-1];
    }
    else if(insertend-insertbegin>1){
        for(i=insertbegin+1; insertend<=len;insertend++)
            szText[i] = szText[insertend];
    }
    szText[insertbegin] = ch;

    return len+1;
}

static inline int sleditGetSelStart(HWND hwnd){
    int selstart, selend;
    int line_pos, char_pos;
    selstart = SendMessage(hwnd, EM_GETSELPOS,(WPARAM)&line_pos, (LPARAM)&char_pos);
    selend = SendMessage(hwnd, EM_GETCARETPOS,(WPARAM)&line_pos, (LPARAM)&char_pos);
    return selstart<selend?selstart:selend;
}

static LRESULT sleditNumberLimitProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    char szNumber[10];
    int value;
    switch(message)
    {
    case MSG_KEYDOWN:
        _oldSleditProc(hwnd, message, wParam, lParam);
        if(wParam == SCANCODE_REMOVE){
            if(GetWindowTextLength(hwnd) <= 0)
                SetEditInt(hwnd, 0);
        }
        return 0;

    case MSG_CHAR:
        {
            DWORD dw = GetWindowAdditionalData(hwnd);
            int min = dw & 0xFFFF;
            int max = dw >> 16;
            switch(wParam){
            case 127:
            case '\b': 
                _oldSleditProc(hwnd, message, wParam, lParam);
                if(GetWindowTextLength(hwnd) <= 0)
                    SetEditInt(hwnd, 0);
                return 0;
            case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':
                if(wParam == '0'){
                    if(GetWindowTextLength(hwnd) >= 1 && SendMessage(hwnd, EM_GETCARETPOS,0,0) <= 0)
                        return 0; //put '0' in the head of a value will not change number's value
                    if(GetEditInt(hwnd) == 0)
                        return 0;
                }
                else if(GetWindowTextLength(hwnd)==1){
                    GetWindowText(hwnd, szNumber, sizeof(szNumber));
                    if(szNumber[0] == '0' && SendMessage(hwnd, EM_GETCARETPOS,0,0)==1) //clear the text
                        SetWindowText(hwnd,"");
                }

                if(getSlEditTextPrepareInsert(hwnd,(int)wParam, szNumber,sizeof(szNumber))<=0)
                    break; //ok vertify 
                value = atoi(szNumber);
                if(value < min)
                {
                    SetEditInt(hwnd, min);
                    NotifyParent (hwnd, GetDlgCtrlID(hwnd), EN_CHANGE);
                    return 0;
                }
                else if(value > max)
                {
                    SetEditInt(hwnd, max);
                    NotifyParent (hwnd, GetDlgCtrlID(hwnd), EN_CHANGE);
                    return 0;
                }
                break;
            default:
                return 0;
            }
        }
        break;

    case MSG_GETINTVALUE:
        if(GetWindowText(hwnd, szNumber, sizeof(szNumber))<=0)
            return 0;
        return atoi(szNumber);

    case MSG_SETINTVALUE:
        {
            DWORD dw = GetWindowAdditionalData(hwnd);
            int min = dw & 0xFFFF;
            int max = dw >> 16;
            int value = (int)wParam;
            if(value < min) value = min;
            else if(value > max) value = max;
            sprintf(szNumber,"%d", value);
            SetWindowText(hwnd, szNumber);
        }
        break;
    }
    return _oldSleditProc(hwnd, message, wParam, lParam);
}

static int DrawIndication (HDC hdc, PSCOLORDIA scld)
{
    scld->px = scld->clrh * RECTW(scld->rcSpace)/360;
    scld->py = RECTH(scld->rcSpace) - scld->clrs * RECTH(scld->rcSpace)/100;
    SetBrushColor (hdc, RGB2Pixel (hdc, 0, 0, 0));
    Circle(hdc, scld->px, scld->py, 5);
    
    return 0;
}

static int DrawColorSpace (HDC hdc, int x, int y, int w, int h, PSCOLORDIA scld)
{
    HDC mdc;

    mdc = CreateCompatibleDCEx (hdc, w, h);
    BitBlt (scld->SpaceDC, 0, 0, w, h, mdc, 0 , 0, 0);
    DrawIndication (mdc, scld);
    BitBlt (mdc, 0, 0, w, h, hdc, x, y, 0);
    DeleteMemDC (mdc);
    
    return 0;
}

static RGB _rgbCustom[6] = {
	{ 0xFF,0xFF,0xFF,0xFF },
	{ 0xFF,0xFF,0xFF,0xFF },
	{ 0xFF,0xFF,0xFF,0xFF },
	{ 0xFF,0xFF,0xFF,0xFF },
	{ 0xFF,0xFF,0xFF,0xFF },
	{ 0xFF,0xFF,0xFF,0xFF }
};

static void InitColorPanels(HWND hDlg)
{
    HWND hctrl;
    static RGB rgbBasics[] = {
        { 0xFF, 0xC0, 0xC0, 0xFF}, /* Light Red */
        { 0xFF, 0x00, 0x00, 0xFF}, /* Red */
        { 0xC0, 0x00, 0x00, 0xFF}, /* Dark Red */
        { 0x80, 0x00, 0x00, 0xFF}, /* Very Drak Red */
        { 0xC0, 0xC0, 0xFF, 0xFF}, /* Light Green */
        { 0x00, 0x00, 0xFF, 0xFF}, /* Green */
        { 0x00, 0x00, 0xC0, 0xFF}, /* Dark Green */
        { 0x00, 0x00, 0x80, 0xFF}, /* Very Dark Green */
        { 0xC0, 0xFF, 0xC0, 0xFF}, /* Light Blue */
        { 0x00, 0xFF, 0x00, 0xFF}, /* Blue */
        { 0x00, 0xC0, 0x00, 0xFF}, /* Dark Blue */
        { 0x00, 0x80, 0x00, 0xFF}, /* Very Dark Blue */
        { 0xFF, 0xC0, 0xFF, 0xFF}, /* Light Yellow */
        { 0xFF, 0x00, 0xFF, 0xFF}, /* Yellow */
        { 0xC0, 0x00, 0xC0, 0xFF}, /* Drak Yellow */
        { 0x80, 0x00, 0x80, 0xFF}, /* Very Dark Yellow */
        { 0xC0, 0xFF, 0xFF, 0xFF}, /* Light Cyan */
        { 0x00, 0xFF, 0xFF, 0xFF}, /* Cyan */
        { 0x00, 0xC0, 0xC0, 0xFF}, /* Dark Cyan */ 
        { 0xFF, 0xFF, 0xC0, 0xFF}, /* Light Magenta */
        { 0xFF, 0xFF, 0x00, 0xFF}, /* Magenta */
        { 0xC0, 0xC0, 0x00, 0xFF}, /* Dark Magenta */
        { 0xFF, 0xA8, 0xDC, 0xFF}, /* Light Orange */
        { 0xFF, 0x58, 0xA8, 0xFF}, /* Orange */
        { 0xFF, 0x00, 0x80, 0xFF}, /* Dark Orange */
        { 0xF0, 0xF0, 0xF0, 0xFF}, /* White */
        { 0xC0, 0xC0, 0xC0, 0xFF}, /* Lieght Gray */
        { 0x80, 0x80, 0x80, 0xFF}, /* Gray */
        { 0x40, 0x40, 0x40, 0xFF}, /* Dark Gray */
        { 0x00, 0x00, 0x00, 0xFF}  /* Black */
    };

    RGBCELLINFO rgbcellInfoBasic = { 
        0, 0, 
        sizeof(rgbBasics)/sizeof(RGB),
        rgbBasics
    };
    
    hctrl = GetDlgItem(hDlg, IDC_CSD_BASIC_COLOR);
    SendMessage(hctrl, CP_SETROWCOL, 5, 6);
    SendMessage(hctrl, CP_SETCELLS, 0, (LPARAM)&rgbcellInfoBasic);

    {
        RGBCELLINFO rgbcellCustomInfo = {
            0, 0, 6,
            _rgbCustom
        };

        hctrl = GetDlgItem(hDlg, IDC_CSD_CUSTOM);
        SendMessage(hctrl, CP_SETROWCOL, 1, 6);
        SendMessage(hctrl, CP_SETCELLS, 0, (LPARAM)&rgbcellCustomInfo);
    }
}

static void save_custom_colors(HWND hClrPanel)
{
    int col;
    if (hClrPanel == 0)
        return;
    for(col=0; col < 6; col++)
    {
        CPGetCellColor(hClrPanel, 0, col, _rgbCustom[col]);
    }
}

static int DrawYSpace ( HDC hdc, int x, int y, int w, int h, PSCOLORDIA scld)
{
    int     i;
    HDC     mdc;
    Uint8   r, g, b;

    SelectClipRect(hdc, &scld->rcYSpace);
    
    mdc = CreateCompatibleDCEx (hdc, w, 101);
    for (i = 0; i <= 100; i ++) {
        HSV2RGB (scld->clrh, scld->clrs, i, &r, &g, &b );
        SetPenColor (mdc, RGB2Pixel(mdc, r, g, b));
        MoveTo (mdc, 0, i);
        LineTo (mdc, (w>>2)*3, i);
    }
    SetBrushColor (mdc, PIXEL_lightgray);
    FillBox (mdc, (w>>2)*3, 0, w, 101);
    StretchBlt (mdc, 0, 0, w, 101, hdc, x, y, w, h, 0);
    DeleteMemDC (mdc);
    
    scld->pz = scld->clrv * h / 100;
    SetPenColor (hdc, PIXEL_black);
    MoveTo (hdc, x+(w>>2)*3-1, y + scld->pz);
    LineTo (hdc, x+w-1, (scld->pz-2 < 0 ? (y) : (y+scld->pz-2)));
    MoveTo (hdc, x+(w>>2)*3-1, y + scld->pz);
    LineTo (hdc, x+w-1, (scld->pz+2 >= h ? (y+h) : (y+scld->pz+2)));

    SelectClipRect(hdc, NULL);
    
    return 0;
}

static int DrawSelSpace (HDC dc, int x, int y, int w, int h, PSCOLORDIA scld)
{
    Uint8 r, g, b;
    
    HSV2RGB (scld->clrh, scld->clrs, scld->clrv, &r, &g, &b);
    SetBrushColor (dc, RGB2Pixel(dc, r, g, b));
    FillBox (dc, x, y, w, h);
    
    return 0;
}

#define DrawRectFrame(hwnd, hdc, prt, expand) do{ \
    RECT rt = { (prt)->left - (expand), (prt)->top - (expand), (prt)->right + (expand), (prt)->bottom + (expand)}; \
    drawWindow3DFrame(hwnd, hdc, &rt); \
}while(0)
static int DrawAllSpace (HWND hDlg, HDC dc, PSCOLORDIA scld)
{
    DrawRectFrame(hDlg, dc, &scld->rcSpace,2);
    DrawColorSpace (dc, scld->rcSpace.left, scld->rcSpace.top, 
            RECTW(scld->rcSpace), RECTH(scld->rcSpace), scld);

    DrawRectFrame(hDlg, dc, &scld->rcYSpace,2);
    DrawYSpace (dc, scld->rcYSpace.left, scld->rcYSpace.top, 
            RECTW(scld->rcYSpace), RECTH(scld->rcYSpace), scld);

    DrawRectFrame(hDlg, dc, &scld->rcSelSpace,2);
    DrawSelSpace (dc, scld->rcSelSpace.left, scld->rcSelSpace.top, 
                RECTW(scld->rcSelSpace), RECTH(scld->rcSelSpace), scld);
    return 0;
}

#define SVF_RGB  0x01
#define SVF_HSV  0x02
#define SVF_ALL  SVF_RGB | SVF_HSV
static void SetValue (HWND hDlg, PSCOLORDIA scld, int flag)
{

    if(flag & SVF_HSV){
        SetDlgEditInt(hDlg, IDC_CSD_VALUE_H, scld->clrh);    
        SetDlgEditInt(hDlg, IDC_CSD_VALUE_S, scld->clrs);    
        SetDlgEditInt(hDlg, IDC_CSD_VALUE_V, scld->clrv);    
    }

    if(flag & SVF_RGB){
        Uint8 r, g, b;
        HSV2RGB (scld->clrh, scld->clrs, scld->clrv, &r, &g, &b);
        printf ("mGUtils>ColorDlgBox>SetValue: r %u, g %u, b %u\n", r, g, b);
        SetDlgEditInt(hDlg, IDC_CSD_VALUE_R, r);
        SetDlgEditInt(hDlg, IDC_CSD_VALUE_G, g);
        SetDlgEditInt(hDlg, IDC_CSD_VALUE_B, b);
    }
}

static void UpdateValue (HWND hDlg, int id, PSCOLORDIA scld)
{
    Uint8 r, g, b;
    
    HDC dc = GetClientDC (hDlg);
    int value = GetDlgEditInt(hDlg, id);
    int flag = 0;
       
    HSV2RGB(scld->clrh, scld->clrs, scld->clrv, &r, &g, &b);
    switch (id) {
    case IDC_CSD_VALUE_H:  
        scld->clrh = value;
        flag |= SVF_RGB;
        break;
    case IDC_CSD_VALUE_S:  
        scld->clrs = value;
        flag |= SVF_RGB;
        break;
    case IDC_CSD_VALUE_V:  
        scld->clrv = value;
        flag |= SVF_RGB;
        break;
    case IDC_CSD_VALUE_R:  
        RGB2HSV (value, g, b, &scld->clrh, &scld->clrs, &scld->clrv);
        flag |= SVF_HSV;
        break;
    case IDC_CSD_VALUE_G:  
        RGB2HSV (r,value, b, &scld->clrh, &scld->clrs, &scld->clrv);
        flag |= SVF_HSV;
        break;
    case IDC_CSD_VALUE_B:  
        RGB2HSV (r, g, value, &scld->clrh, &scld->clrs, &scld->clrv);
        flag |= SVF_HSV;
        break;
    }
    
    SetValue(hDlg, scld, flag);
    DrawAllSpace (hDlg, dc, scld);
    ReleaseDC (dc);
}

static int InitScolorDia (HWND hDlg, PSCOLORDIA scld, PCOLORDLGDATA pcdd)
{
    char str[8];
    
    SetRect(&scld->rcSpace, 162,12,162+118, 12+98);
    SetRect(&scld->rcYSpace, 289,12,289+18, 12+98);
    SetRect(&scld->rcSelSpace, 162, 122, 162+38, 122+38);

    if (0 != GetDlgItemText (hDlg, IDC_CSD_VALUE_H, str, 8))
    {
        scld->clrh = atoi (str);
        scld->clrh = MIN (359, scld->clrh);
    }
    if (0 != GetDlgItemText (hDlg, IDC_CSD_VALUE_S, str, 8))
    {
        scld->clrs = atoi (str);
    }
    if (0 != GetDlgItemText (hDlg, IDC_CSD_VALUE_V, str, 8))
    {
        scld->clrv = atoi (str);
    }

    scld->pclr = pcdd;

    return 0;
}

static int AddColor2Custom (HWND hDlg, Uint8 r, Uint8 g, Uint8 b)
{
    RGB rgb = {r, g, b, 0xFF};
      SendDlgItemMessage(hDlg, IDC_CSD_CUSTOM, CP_INSERTCOLOR, 0, *(LPARAM*)&rgb); 
    return 0;
}

static int ResetAllSpace (HWND hDlg, PSCOLORDIA scld)
{
    HDC     hdc;
    RGB color;

    if(!getSelectColorPanel(hDlg,&color))
        return -1;

    RGB2HSV (color.r, color.g, color.b, 
            &scld->clrh, &scld->clrs, &scld->clrv );
    
    hdc = GetClientDC (hDlg);
    DrawAllSpace (hDlg, hdc, scld);
    SetValue (hDlg, scld, SVF_ALL);
    ReleaseDC (hdc);
    
    return 0;
}

static int SaveColor (HWND hDlg, PSCOLORDIA scld)
{
    scld->H = scld->clrh;
    scld->S = scld->clrs;
    scld->V = scld->clrv;
    HSV2RGB (scld->clrh, scld->clrs, scld->clrv, 
             &scld->R, &scld->G, &scld->B);
    scld->PIXEL = RGB2Pixel (HDC_SCREEN, scld->R, scld->G, scld->B);

    return 0;

}

#define InitDlgLimitEdit(hDlg, id, min, max) do { \
    HWND hEdit = GetDlgItem(hDlg, id); \
    _oldSleditProc = SetWindowCallbackProc(hEdit, sleditNumberLimitProc); \
    SetWindowAdditionalData(hEdit, ((min)&0xFFFF)|((max)<<16)); \
}while(0)

#define ClearColorPanel(hDlg) do{ \
    SendDlgItemMessage(hDlg, IDC_CSD_BASIC_COLOR, CP_SETSELCELL, (WPARAM)-1, (LPARAM)-1); \
    SendDlgItemMessage(hDlg, IDC_CSD_CUSTOM, CP_SETSELCELL,(WPARAM)-1, (LPARAM)-1); \
}while(0)

LRESULT DefColorDialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC         hdc, mdc;
    Uint8       r, g, b;
    int         i, j, x, y;
    int         id, msg, scancode;
    PSCOLORDIA  scld;
    
    switch (message)
    {
        case MSG_INITDIALOG:
            {
                if ((scld = (PSCOLORDIA)calloc(1, sizeof(SCOLORDIA))) == NULL)
                //if ((scld = (PSCOLORDIA)malloc(sizeof(SCOLORDIA))) == NULL)
                {
                    fprintf(stderr, "DefColorDialogProc : memoy alloc error!");
                    return -1;
                }
                
                if (0 == InitScolorDia (hDlg, scld, (PCOLORDLGDATA)lParam))
                {
                    hdc = GetClientDC (hDlg);
                    scld->SpaceDC = CreateCompatibleDCEx(hdc, 
                            RECTW(scld->rcSpace), RECTH(scld->rcSpace));
                    mdc = CreateCompatibleDCEx (hdc, 360, 101);
                    for (i =  0; i <= 360; i ++) {
                        for (j = 0; j <= 100; j ++) {
                            HSV2RGB (i, j, 65, &r, &g, &b);
                            SetPixelRGB(mdc, i, 100-j, r, g, b);
                        }
                    }

                    StretchBlt(mdc, 0, 0, 360, 256, scld->SpaceDC, 
                            0, 0, RECTW(scld->rcSpace), RECTH(scld->rcSpace), 0);
                    DeleteMemDC(mdc);
                    ReleaseDC(hdc);
                }
                else
                {
                    scld->SpaceDC = HDC_INVALID; 
                }

                SetWindowAdditionalData (hDlg, (LPARAM)scld);

                InitDlgLimitEdit(hDlg, IDC_CSD_VALUE_R,0, 255);
                InitDlgLimitEdit(hDlg, IDC_CSD_VALUE_G,0, 255);
                InitDlgLimitEdit(hDlg, IDC_CSD_VALUE_B,0, 255);

                
                InitDlgLimitEdit(hDlg, IDC_CSD_VALUE_H,0, 360);
                InitDlgLimitEdit(hDlg, IDC_CSD_VALUE_S,0, 100);
                InitDlgLimitEdit(hDlg, IDC_CSD_VALUE_V,0, 100);

                InitColorPanels(hDlg);
 
                if (scld->pclr->hook) 
                {
                    return scld->pclr->hook (hDlg, message, wParam, lParam);
                }
                return TRUE;
                
               }        
            break;
        
        case MSG_CLOSE:
            EndDialog (hDlg, IDCANCEL);
            break;

        case MSG_DESTROY:
            scld = (PSCOLORDIA)GetWindowAdditionalData (hDlg);
			save_custom_colors(GetDlgItem(hDlg, IDC_CSD_CUSTOM));
            if (scld->SpaceDC != HDC_INVALID)
                DeleteMemDC (scld->SpaceDC);
            free(scld);
            break;
        
        case MSG_PAINT:
            {
                hdc  = BeginPaint (hDlg);
                scld = (PSCOLORDIA)GetWindowAdditionalData (hDlg);
                DrawAllSpace (hDlg, hdc, scld);
                EndPaint (hDlg, hdc);
                return 0;
            }

        case MSG_COMMAND:
            {
                msg  = HIWORD(wParam);
                id   = LOWORD(wParam);
                scld = (PSCOLORDIA)GetWindowAdditionalData (hDlg);

                switch (id) {
                    case IDC_CSD_VALUE_H :
                    case IDC_CSD_VALUE_S :
                    case IDC_CSD_VALUE_V :
                    case IDC_CSD_VALUE_R :
                    case IDC_CSD_VALUE_G :
                    case IDC_CSD_VALUE_B :
                        {
                            //if (msg == EN_CONTCHANGED) {
                            if (msg == EN_CHANGE) {
                                UpdateValue (hDlg, id, scld);
                            }
                            break;
                        }
                    case IDC_CSD_BASIC_COLOR:
                    case IDC_CSD_CUSTOM:
                        if (msg == CPN_CELLSELCHANGE) {
                            SendDlgItemMessage(hDlg,
                                id==IDC_CSD_CUSTOM?IDC_CSD_BASIC_COLOR:IDC_CSD_CUSTOM,
                                CP_SETSELCELL,
                                (WPARAM)-1,(LPARAM) -1); //cancel cell
                            ResetAllSpace(hDlg,scld);    
                        }
                        break;
                    case IDC_CSD_ADD:
                        {
                            hdc = GetClientDC (hDlg);
                            ReleaseDC (hdc);
                            HSV2RGB (scld->clrh, scld->clrs, scld->clrv, 
                                   // &scld->R, &scld->G, &scld->B);
                                    &r, &g, &b);
                            AddColor2Custom (hDlg, r, g, b);
                            break;
                        }
                    case IDC_CSD_CANCEL:
                        {
                            if (!scld->pclr->hook || (scld->pclr->hook 
                                && !scld->pclr->hook (hDlg, MSG_COLORSELCANCEL, 
                                    0, (LPARAM)scld->pclr)))
                            {
                                EndDialog (hDlg, IDCANCEL);
                                break;
                            }
                            else
                                return -1;
                        }
                    case IDC_CSD_OK:
                        {
                            SaveColor(hDlg, scld);
                            if (!scld->pclr->hook || (scld->pclr->hook 
                                && !scld->pclr->hook (hDlg, MSG_COLORSELOK, 
                                    0, (LPARAM)scld->pclr)))
                            {
                                EndDialog (hDlg, IDOK);
                                break;
                            }
                            else
                            return -1;
                        }
                    default :
                        break;
                }
                break;
            }
            
        case MSG_LBUTTONDOWN:
            {
                x    = LOSWORD (lParam);
                y    = HISWORD (lParam);
                scld = (PSCOLORDIA)GetWindowAdditionalData (hDlg);

                if (PtInRect (&scld->rcSpace, x, y)) {
                    hdc = GetClientDC (hDlg);
                    scld->clrh = (x-scld->rcSpace.left)*360/RECTW(scld->rcSpace);
                    scld->clrs = 100-(y-scld->rcSpace.top)*100/RECTH(scld->rcSpace);
                    DrawAllSpace (hDlg, hdc, scld);
                    SetValue (hDlg, scld, SVF_ALL);
                    ClearColorPanel(hDlg);
                    ReleaseDC (hdc);
                }

                if (PtInRect(&scld->rcYSpace, x,y)) {
                    hdc = GetClientDC (hDlg);
                    scld->clrv = (y-scld->rcYSpace.top)*100 / RECTH(scld->rcYSpace);
                    DrawAllSpace (hDlg, hdc, scld);
                    SetValue (hDlg, scld, SVF_ALL);
                    ClearColorPanel(hDlg);
                    ReleaseDC (hdc);
                }
                break;
            }
        case MSG_KEYDOWN:
            {
                HWND hFocus = GetFocus(hDlg);
                scancode = (int)wParam;
                if(hFocus){
                    int id = GetDlgCtrlID(hFocus);
//                    if(id < IDC_CSD_BASIC_COLOR_0 || id > IDC_CSD_CUSTOM_COLOR_5)
                    {
                        if( id >= IDC_CSD_VALUE_H && id <= IDC_CSD_VALUE_B) // H S V & R G B can switch focus by up and down key
                        {
                            if(scancode == SCANCODE_CURSORBLOCKUP)
                                id --;
                            else if (scancode == SCANCODE_CURSORBLOCKDOWN)
                                id ++;
                            else
                                break;
                            //out of the edits
                            if(id < IDC_CSD_VALUE_H || id > IDC_CSD_VALUE_B)
                                break;
                            SetFocus(GetDlgItem(hDlg,id));
                        }
                        break; // current key focuse is not on the basic colors
                    }
                }
                break;
           }
        default:
            break;
    }
    
    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

/**
 * 
 */
BOOL ColorSelectDialog (PDLGTEMPLATE dlg_template, 
        HWND hwnd, WNDPROC proc, PCOLORDLGDATA pcdd)
{
    PDLGTEMPLATE color_dlg;
    WNDPROC color_proc;
    static BOOL _init_color_controls = FALSE;

    if(!_init_color_controls)
    {
        RegisterColorPanel();
    }

    if (dlg_template) {
        color_dlg = dlg_template;
    }
    else {
        color_dlg = &DefColorDlg;
    }

    if (proc) {
        color_proc = proc;
    }
    else {
        color_proc = DefColorDialogProc;
    }
        
    return ShowCommonDialog (color_dlg, hwnd, color_proc, pcdd);
}

#endif /* _MGUTILS_DLGCOLORSEL */

