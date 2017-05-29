/*
** $Id:$
** 
** commdlg.c: Common Dialog.
** 
** Copyright (C) 2004 ~ 2008 Feynman Software.
**
** Current maintainer: wangjian
**
** Create date: 2008/04/07
*/

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include "mgutils.h"

BOOL ShowCommonDialog (PDLGTEMPLATE dlg_template, HWND hwnd, 
        WNDPROC proc, void* private_data)
{
    if (IDCANCEL == DialogBoxIndirectParam (dlg_template, 
            hwnd, proc, (LPARAM)(private_data)))
        return FALSE;
    return TRUE;
}
