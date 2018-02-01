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

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include "mgutils.h"

#ifdef _MGUTILS_DLGSHOWINFO

#define LEFT_MARGIN         12
#define BOTTOM_MARGIN       6
#define SPINBOX_RECTWH      27

CTRLDATA DefInfoCtrl [] =
{
    {
        CTRL_SPINBOX,
        SPS_AUTOSCROLL | WS_BORDER | WS_CHILD | WS_VISIBLE,
        320-SPINBOX_RECTWH, 220-SPINBOX_RECTWH, 20, 20,
        IDC_IFD_SPIN,
        "",
        0, WS_EX_USEPARENTRDR
    }
};

DLGTEMPLATE DefInfoDlg =
{
    WS_DLGFRAME | WS_BORDER | WS_CAPTION, 
    WS_EX_USEPARENTRDR,
    0, 0, 320, 240, 
    "Info", 0, 0, 
    TABLESIZE (DefInfoCtrl), 
    DefInfoCtrl 
};

LRESULT DefInfoDialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    PINFODLGDATA info;
    SPININFO spinfo;

    info = (PINFODLGDATA) GetWindowAdditionalData (hDlg);

    switch (message) {
        case MSG_INITDIALOG:
            {
                info = (PINFODLGDATA) lParam;
                spinfo.min = 0;
                spinfo.max = info->nr_lines - info->vis_lines;
                if (spinfo.max < 0) spinfo.max = 0;
                spinfo.cur = 0;

                SendMessage (GetDlgItem (hDlg, IDC_IFD_SPIN), SPM_SETTARGET, 0, (LPARAM) hDlg);

                SendMessage (GetDlgItem ( hDlg, IDC_IFD_SPIN), SPM_SETINFO, 0, (LPARAM) &spinfo);

                SetWindowAdditionalData (hDlg, (DWORD) lParam);

                if (info->hook)
                    return info->hook (hDlg, message, wParam, lParam);

                return 1;
            }
        case MSG_PAINT:
            {
                HDC hdc = BeginPaint (hDlg);
                RECT rc = info->rc;
                RECT tmprc = info->rc;
                DTFIRSTLINE firstline={0};

                SelectClipRect (hdc, &info->rc);
#if 0
                rc.top -= info->start_line * GetSysCharHeight ();
#else
                DrawTextEx2(hdc, info->msg, -1, 
                                &tmprc, 0, DT_CALCRECT, &firstline);
                rc.top -= info->start_line * firstline.height;
                rc.bottom -= RECTH(rc)%firstline.height;
#endif
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
                    SendDlgItemMessage ( hDlg, IDC_IFD_SPIN, SPM_SETCUR, 
                            info->start_line, 0);
                    InvalidateRect (hDlg, &info->rc, TRUE);
                }
                return 0;
            } else if ( wParam == SCANCODE_CURSORBLOCKDOWN ) {
                if (info->start_line + info->vis_lines < info->nr_lines) {
                    info->start_line++;
                    SendDlgItemMessage ( hDlg, IDC_IFD_SPIN, SPM_SETCUR, 
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

/**
* If parameter dlg_template is null, it will use default dialog template.
* If parameter proc is null, it will use default window procedure.
*/
BOOL InfoShowDialog  (PDLGTEMPLATE dlg_template, 
        HWND hwnd, WNDPROC proc, PINFODLGDATA pidd)
{
    PDLGTEMPLATE info_dlg;
    WNDPROC info_proc;
    RECT temp_rc;

    if (dlg_template) {
        info_dlg = dlg_template;
    }
    else {
        info_dlg = &DefInfoDlg;
    }

    temp_rc.top = 0;
    temp_rc.left = LEFT_MARGIN;
    temp_rc.right = info_dlg->w 
        - GetWindowElementAttr (HWND_NULL, WE_METRICS_WND_BORDER) * 2 
        - LEFT_MARGIN;
    temp_rc.bottom = info_dlg->h;

    SelectFont (HDC_SCREEN, GetSystemFont (SYSLOGFONT_WCHAR_DEF));
    DrawText (HDC_SCREEN, pidd->msg, -1, &temp_rc,
            DT_LEFT | DT_TOP | DT_WORDBREAK | DT_EXPANDTABS | DT_CALCRECT);
    
    pidd->rc.left = LEFT_MARGIN;
    pidd->rc.right = info_dlg->w - GetWindowElementAttr (HWND_NULL, 
                        WE_METRICS_WND_BORDER) * 2 - LEFT_MARGIN;
    pidd->rc.top = BOTTOM_MARGIN;
    pidd->rc.bottom = 240 - SPINBOX_RECTWH - BOTTOM_MARGIN;
    
    pidd->nr_lines = RECTH (temp_rc) / GetSysCharHeight (); 
    if (pidd->nr_lines <= 0) return -1;

    pidd->vis_lines = RECTH (pidd->rc) / GetSysCharHeight ();

    pidd->rc.bottom = pidd->rc.top + pidd->vis_lines * GetSysCharHeight ();
    pidd->start_line = 0;

    if (proc) {
        info_proc = proc;
    }
    else {
        info_proc = DefInfoDialogProc;
    }

    return ShowCommonDialog (info_dlg, hwnd, info_proc, pidd);
}

#endif /* _MGUTILS_DLGSHOWINFO */

