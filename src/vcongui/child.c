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
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include "mgutils.h"
#include "defs.h"
#include "error.h"
#include "vc.h"
#include "child.h"

static void RunStartupCmd (const char* startupStr)
{
    char *p;
    char *str;

    if (!startupStr)
        return;

    str = strdup (startupStr);

    p = strtok (str, "\n");
    while (p) {
        int ret = system(p);
        printf ("return value of system (%s) is: %d\n", p, ret);
        p = strtok (NULL, "\n");
    }

    free (str);
}

void ChildStart (PCONINFO con, FILE *errfp, bool startupMessage,
                    const char* startupStr, const char* execProg,
                    const char* execArgs)
{
    char *tail;
    char buff [80];

    if (setgid (getgid ())) {
        printf ("Error on calling setgid.\n");
        return;
    }

    if (setuid (getuid ())) {
        printf ("Error on calling setuid.\n");
        return;
    }

    RunStartupCmd (startupStr);

//    sprintf (buff, "TERMCAP=:co#%d:li#%d:", con->cols, con->rows);
//    tcap = strdup (buff);
//    putenv (tcap);

    if (startupMessage)
    {
        printf("\rVCOnGUI - Virtual Console On MiniGUI " VCONGUI_VERSION
           " running on VC No. %c\r\n"
           "%s\r\n"
           "%s\r\n"
           "%s\r\n"
           "%s\r\n\r\n",
           *((char *)ttyname(fileno (errfp)) + 8),
"Copyright (C) 2003-2018 Beijing FMSoft Technologies Co., Ltd.",
"Copyright (C) 1999-2002 WEI Yongming (vincent@minigui.org).",
"Some idea comes from CCE by He Rui and others.",
"    CCE: Copyright (C) 1998-1999 He Rui and others.");
   }

    fflush(stdout);

    if (execProg)
        execlp (execProg, execProg, execArgs, NULL);
    else
        {
        if ((execProg = getenv("SHELL")) == NULL)
            execProg = "/bin/sh";
        if ((tail = strrchr(execProg, '/')) == NULL)
            tail = " sh";
        sprintf (buff, "-%s", tail + 1);
        execl (execProg, buff, NULL);
    }

    fprintf (errfp, "VCOnGUI> couldn't exec shell\r\n");
    fprintf (errfp, "%s: %s\r\n", execProg, strerror(errno));
    exit (EXIT_FAILURE);
}

