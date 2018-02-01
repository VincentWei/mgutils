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
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include "defs.h"
#include "mgutils.h"
#include "vc.h"
#include "child.h"
#include "terminal.h"
#include "error.h"

static void VCOnGUIPrintf (const char *head, const char *format, va_list args)
{
#if 0
    FILE *fp = fopen ("/tmp/VCOnGUI-log","a");
    fprintf (fp, "%s", head);
    vfprintf (fp, format, args);
    fclose (fp);
#endif

    fprintf (stderr, "%s", head);
    vfprintf (stderr, format, args);
}

void MyFatal (const char *format, ...)
{
    va_list args;

    va_start (args, format);
    fprintf (stderr, "VCOnGUI> Fatal error: ");
    vfprintf (stderr, format, args);
    va_end (args);
    exit (EXIT_FAILURE);
}

void myWarn (const char *format, ...)
{
    va_list args;

    va_start(args, format);
    VCOnGUIPrintf ("VCOnGUI> Warning: ", format, args);
    va_end(args);
}

void myError (const char *format, ...)
{
    va_list args;

    va_start(args, format);
    VCOnGUIPrintf ("VCOnGUI> Error: ", format, args);
    va_end (args);
}

void myMessage (const char *format, ...)
{
    va_list args;

    va_start (args, format);
    VCOnGUIPrintf ("VCOnGUI> ", format, args);
    va_end (args);
}

void Perror (const char *msg)
{
    myMessage ("system error - %s: %s\r\n", msg, strerror(errno));
}

void PerrorExit (const char *message)
{
    fprintf (stderr, "%s: %s\r\n", message, strerror(errno));
    exit (0);
}

