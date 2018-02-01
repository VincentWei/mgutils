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

#if defined (_MGCTRL_BUTTON) && defined (_MGCTRL_SPINBOX)
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include "mgutils.h"

#define IDC_SPIN    100

static CTRLDATA _help_win_ctrls [] =
{ 
    {
        CTRL_BUTTON,
        WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON, 
        0, 0, 0, 0, IDOK, "OK", 0, WS_EX_USEPARENTRDR
    },
    {
        CTRL_SPINBOX,
        WS_CHILD | SPS_AUTOSCROLL | WS_VISIBLE,
        0, 0, 0, 0, IDC_SPIN, "", 0, WS_EX_USEPARENTRDR
    }
};

static DLGTEMPLATE _help_win =
{
    WS_BORDER | WS_CAPTION | WS_VISIBLE,
    WS_EX_USEPARENTRDR,
    0, 0, 0, 0,
    NULL,
    0, 0, 2
};

typedef struct tagHELPMSGINFO {
    const char* msg;
    int nr_lines;

    int vis_lines;
    int start_line;

    RECT rc;
} HELPMSGINFO;

static LRESULT _help_win_proc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    HELPMSGINFO* info;
    SPININFO spinfo;

    info = (HELPMSGINFO*) GetWindowAdditionalData (hDlg);

    switch (message) {
    case MSG_INITDIALOG:
        info = (HELPMSGINFO*) lParam;
        spinfo.min = 0;
        spinfo.max = info->nr_lines - info->vis_lines;
        if (spinfo.max < 0) spinfo.max = 0;
        spinfo.cur = 0;

        SendMessage ( GetDlgItem ( hDlg, IDC_SPIN), 
                    SPM_SETTARGET, 0, (LPARAM) hDlg);

        SendMessage ( GetDlgItem ( hDlg, IDC_SPIN), 
                    SPM_SETINFO, 0, (LPARAM) &spinfo);

        SetWindowAdditionalData (hDlg, (DWORD) lParam);
        return 1;
        
    case MSG_COMMAND:
        if (wParam == IDOK)
            EndDialog (hDlg, IDOK);
        break;

    case MSG_PAINT:
    {
        HDC hdc = BeginPaint (hDlg);
        RECT rc = info->rc;

        rc.top -= info->start_line * GetSysCharHeight ();
        SetBkMode (hdc, BM_TRANSPARENT);
        DrawText (hdc, info->msg, -1, &rc,
            DT_LEFT | DT_TOP | DT_WORDBREAK | DT_EXPANDTABS);

        EndPaint (hDlg, hdc);
        return 0;
    }

    case MSG_KEYDOWN:
        if (wParam == SCANCODE_CURSORBLOCKUP) {
            if (info->start_line > 0) {
                info->start_line--;
                if (info->start_line == 0 && !(lParam & KS_SPINPOST))
                    SendDlgItemMessage ( hDlg, IDC_SPIN, SPM_SETCUR, 
                            info->start_line, 0);
                InvalidateRect (hDlg, &info->rc, TRUE);
            }
            return 0;
        } else if ( wParam == SCANCODE_CURSORBLOCKDOWN ) {
            if (info->start_line + info->vis_lines < info->nr_lines) {
                info->start_line++;
                if (info->start_line + info->vis_lines == info->nr_lines
                                && !(lParam & KS_SPINPOST))
                    SendDlgItemMessage ( hDlg, IDC_SPIN, SPM_SETCUR, 
                            info->start_line, 0);
                InvalidateRect (hDlg, &info->rc, TRUE);
            }
            return 0;
        } 
        break;

    case MSG_CLOSE:
        EndDialog (hDlg, IDOK);
        return 0;        
    }
    
    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

#define LEFT_MARGIN     12
#define BOTTOM_MARGIN   6

int myWinHelpMessage (HWND hwnd, int width, int height, 
                const char* help_title, const char* help_msg)
{
    HELPMSGINFO info;
    RECT rc;
    SIZE text_size;

    rc.top = 0;
    rc.left = LEFT_MARGIN;
    rc.right = width 
        - GetWindowElementAttr (HWND_NULL, WE_METRICS_WND_BORDER) * 2 
        - LEFT_MARGIN;
    rc.bottom = height;

    _help_win_ctrls[0].caption = GetSysText (IDS_MGST_OK);
    GetTabbedTextExtent (HDC_SCREEN, _help_win_ctrls[0].caption, -1, &text_size);

    _help_win_ctrls[0].x = LEFT_MARGIN;
    _help_win_ctrls[0].w = text_size.cx + 8*BTN_WIDTH_BORDER;
    _help_win_ctrls[0].h = text_size.cy + 2*BTN_WIDTH_BORDER + 2;
    _help_win_ctrls[0].y = height 
            - GetWindowElementAttr (HWND_NULL, WE_METRICS_WND_BORDER) * 2 
            - GetWindowElementAttr (HWND_NULL, WE_METRICS_CAPTION) - 
            _help_win_ctrls[0].h - 2*BTN_WIDTH_BORDER - 2 - BOTTOM_MARGIN;

    _help_win_ctrls[1].h = _help_win_ctrls[1].w = 
                2*GetSysFontMaxWidth(SYSLOGFONT_WCHAR_DEF);
    _help_win_ctrls[1].x = width 
            - GetWindowElementAttr (HWND_NULL, WE_METRICS_WND_BORDER) * 2
            - _help_win_ctrls[1].w - LEFT_MARGIN;
    _help_win_ctrls[1].y = _help_win_ctrls[0].y + BTN_WIDTH_BORDER + 1; 

    _help_win.w = width;
    _help_win.h = height;
    _help_win.caption = help_title;
    _help_win.controls = _help_win_ctrls;

    info.msg = help_msg;

    SelectFont (HDC_SCREEN, GetSystemFont (SYSLOGFONT_WCHAR_DEF));
    DrawText (HDC_SCREEN, info.msg, -1, &rc,
        DT_LEFT | DT_TOP | DT_WORDBREAK | DT_EXPANDTABS | DT_CALCRECT);

    info.nr_lines = RECTH (rc) / GetSysCharHeight (); 

    if (info.nr_lines <= 0) return -1;

    info.rc.top = BOTTOM_MARGIN;
    info.rc.left = LEFT_MARGIN;
    info.rc.right = width 
        - GetWindowElementAttr (HWND_NULL, WE_METRICS_WND_BORDER) * 2
        - LEFT_MARGIN;
    info.rc.bottom = _help_win_ctrls[0].y - BOTTOM_MARGIN;

    info.vis_lines = RECTH (info.rc) / GetSysCharHeight ();

    info.rc.bottom = info.rc.top + info.vis_lines * GetSysCharHeight ();

    info.start_line = 0;


    DialogBoxIndirectParam (&_help_win, hwnd, _help_win_proc, (LPARAM)&info);

    return 0;
}

#endif /* defined (_MGCTRL_BUTTON) && defined (_MGCTRL_SPINBOX) */

