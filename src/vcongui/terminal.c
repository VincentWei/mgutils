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
#define __USE_GNU
#define __USE_XOPEN
#define __USE_XOPEN_EXTENDED
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
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
#include "terminal.h"
#include "vt.h"

static BOOL OpenTerminal (PCONINFO con, PCHILDINFO pChildInfo);
static void CloseTerminal (PCONINFO con);

static int TermFork (PCONINFO con, PCHILDINFO pChildInfo);

#ifdef _MGRM_THREADS

#include <pthread.h>
#include <semaphore.h>
static pthread_key_t con_key;

/* Once-only initialisation of the key */
static pthread_once_t con_key_once = PTHREAD_ONCE_INIT;
static pthread_once_t sig_handler_once = PTHREAD_ONCE_INIT;

/* Allocate the key */
static void con_key_alloc(void)
{
    pthread_key_create (&con_key, NULL);
}

/* Set console info */
void set_coninfo (void* data)
{
    pthread_once (&con_key_once, con_key_alloc);
    pthread_setspecific (con_key, data);
}

void SigChildHandler (int signo)
{
    int status;
    PCONINFO con;

    con = (PCONINFO) pthread_getspecific (con_key);
    if (!con) return;

    if (waitpid (con->childPid, &status, WNOHANG))
       con->terminate = 1;
}

static void InstallSignalHandlers (void)
{
    struct sigaction siga;

    siga.sa_handler = SigChildHandler;
    siga.sa_flags = 0;
    memset (&siga.sa_mask, 0, sizeof (sigset_t));

    sigaction (SIGCHLD, &siga, NULL);
}

static void SetSignalHandlers (void)
{
    pthread_once (&sig_handler_once, InstallSignalHandlers);
}

BOOL TerminalStart (PCONINFO con, PCHILDINFO pChildInfo)
{
    sigset_t sa_mask;

    // We must handle SIGCHLD.
    // We handle coninfo as a thread-specific data
    set_coninfo (con);

    SetSignalHandlers ();

    sigemptyset (&sa_mask);
    sigaddset (&sa_mask, SIGCHLD);
    pthread_sigmask (SIG_UNBLOCK, &sa_mask, NULL);

    return OpenTerminal (con, pChildInfo);
}

#else
static PCONINFO sg_coninfo = NULL;

void SigChildHandler (int signo)
{
    int status;

    if (!sg_coninfo) return;

    if (waitpid (sg_coninfo->childPid, &status, WNOHANG))
       sg_coninfo->terminate = 1;
}

static void InstallSignalHandlers (void)
{
    struct sigaction siga;

    siga.sa_handler = SigChildHandler;
    siga.sa_flags = 0;
    memset (&siga.sa_mask, 0, sizeof (sigset_t));

    sigaction (SIGCHLD, &siga, NULL);
}

BOOL TerminalStart (PCONINFO con, PCHILDINFO pChildInfo)
{
    // We must handle SIGCHLD.
    sg_coninfo = con;

    InstallSignalHandlers ();

    return OpenTerminal (con, pChildInfo);
}
#endif

void TerminalCleanup (PCONINFO con)
{
    CloseTerminal (con);

    if (con->log_font)
        DestroyLogFont (con->log_font);
}

#ifndef HAVE_GETPT

static int TermFork (PCONINFO con, PCHILDINFO pChildInfo)
{
    char ls, ln;

    // 1. Looking for unused PTY/TTY Master/Slave

    /* Open PTY(master) from [pqrs][5-F], in fact p-z is ok? */
    /* MasterPty:  pty[p-z][5-f] pty[a-e][5-f]  16*16=256
       SlaveTty:   tty[p-z][5-f] tty[a-e][5-f]  16*16=256 */

    for (ls = 'p'; ls <= 's'; ls ++) {
        for (ln = 5; ln <= 0xF; ln ++) {
            sprintf(con->ptyName, "/dev/ptm%1c%1x", ls, ln);
            if ((con->masterPty = open (con->ptyName, O_RDWR)) >= 0)
                break;
        }

        if (con->masterPty >= 0)
            break;
    }

    if (con->masterPty < 0) {
        myMessage ("can not get master pty!\n");
        Perror (con->ptyName);
        return -1;
    }

    con->ptyName[7] = 's';   /* slave tty */

    if ((con->childPid = fork()) < 0) {
        Perror ("fork");
        return -1;
    }
    else if (con->childPid == 0) {
        // in child process
        int   errfd, slavePty;
        FILE *errfp;
        struct winsize twinsz;

        errfd = dup (2);
        errfp = fdopen (errfd, "w");

        /* I'm child, make me process leader */
        setsid ();

        // close any no used fd here!!
        close (con->masterPty);

        /* Open TTY(slave) */
        if ((slavePty = open (con->ptyName, O_RDWR)) < 0)
            PerrorExit (con->ptyName);

        /* Set new TTY's termio with parent's termio */
        tcsetattr (slavePty, TCSANOW, (struct termios*)GetOriginalTermIO ());

        /* Set new terminal window size */
        twinsz.ws_row = con->rows;
        twinsz.ws_col = con->cols;
        ioctl (slavePty, TIOCSWINSZ, &twinsz);

        /* Set std??? to pty, dup2 (oldfd, newfd) */
        dup2 (slavePty, 0);
        dup2 (slavePty, 1);
        dup2 (slavePty, 2);
        close (slavePty);

        // execute the shell
        ChildStart (con, errfp, pChildInfo->startupMessage,
                           pChildInfo->startupStr,
                           pChildInfo->execProg,
                           pChildInfo->execArgs);
    }

    return 1; // parent process
}

#else

static int TermFork (PCONINFO con, PCHILDINFO pChildInfo)
{
    const char* pts_name;

    con->masterPty = getpt ();

    if (con->masterPty < 0) {
        Perror ("getpt");
        return -1;
    }
    if (grantpt (con->masterPty)) {
        Perror ("grantpt");
        return -1;
    }
    if (unlockpt (con->masterPty)) {
        Perror ("unlockpt");
        return -1;
    }

    pts_name = ptsname (con->masterPty);

    if ((con->childPid = fork()) < 0) {
        Perror ("fork");
        return -1;
    }
    else if (con->childPid == 0) {
        // in child process
        int   errfd, slavePty;
        FILE *errfp;
        struct winsize twinsz;

        errfd = dup (2);
        errfp = fdopen (errfd, "w");

        /* I'm child, make me process leader */
        setsid ();

        // close any no used fd here!!
        close (con->masterPty);

        /* Open TTY(slave) */
        if ((slavePty = open (pts_name, O_RDWR)) < 0)
            PerrorExit (pts_name);

        /* Set new TTY's termio with parent's termio */
        tcsetattr (slavePty, TCSANOW, (struct termios*)GetOriginalTermIO ());

        /* Set new terminal window size */
        twinsz.ws_row = con->rows;
        twinsz.ws_col = con->cols;
        ioctl (slavePty, TIOCSWINSZ, &twinsz);

        /* Set std??? to pty, dup2 (oldfd, newfd) */
        dup2 (slavePty, 0);
        dup2 (slavePty, 1);
        dup2 (slavePty, 2);
        close (slavePty);

        // execute the shell
        ChildStart (con, errfp, pChildInfo->startupMessage,
                           pChildInfo->startupStr,
                           pChildInfo->execProg,
                           pChildInfo->execArgs);
    }
    return 1; // parent process
}

#endif /* !HAVE_GETPT */

static BOOL OpenTerminal (PCONINFO con, PCHILDINFO pChildInfo)
{
    int iRet;

    if ((iRet = TermFork (con, pChildInfo)) < 0)
        return FALSE;

    if (iRet) {
        // In parent process.
        VtStart (con);
    }

    return TRUE;
}

static void CloseTerminal (PCONINFO con)
{
    close (con->masterPty);
    VtCleanup (con);
}

void ReadMasterPty (PCONINFO con)
{
    char buff [BUFSIZ + 1];
    int nRead;
#ifdef _MGRM_THREADS
    struct pollfd fd = {con->masterPty, POLLIN, 0};
    int ret;

    ret = poll (&fd, 1, 10);   // about 0.01s

    if (ret == 0) {
        return;
    }
    else if (ret > 0) {
        if ((nRead = read (con->masterPty, buff, BUFSIZ)) > 0) {
            VtWrite (con, buff, nRead);
            TextRefresh (con, true);
        }
    }
#else
    if ((nRead = read (con->masterPty, buff, BUFSIZ)) > 0) {
        VtWrite (con, buff, nRead);
        TextRefresh (con, true);
    }
#endif
}

void HandleInputChar (PCONINFO con, WPARAM wParam, LPARAM lParam)
{
    u_char ch [4];
    int chlen = 1;

    ch [0] = FIRSTBYTE (wParam);
    ch [1] = SECONDBYTE (wParam);
    ch [2] = THIRDBYTE (wParam);
    ch [3] = FOURTHBYTE (wParam);

    if (ch [3]) {
        chlen = 4;
    }
    else if (ch [2]) {
        chlen = 3;
    }
    else if (ch [1]) {
        chlen = 2;
    }

    my_write (con->masterPty, ch, chlen);
}

void HandleInputKeyDown (PCONINFO con, WPARAM wParam, LPARAM lParam)
{
    u_char buff [50];

    con->kinfo.state = lParam;
    con->kinfo.buff  = buff;
    con->kinfo.pos   = 0;

    handle_scancode_on_keydown (wParam, &con->kinfo);

    if (con->kinfo.pos != 0)
        my_write (con->masterPty, buff, con->kinfo.pos);
}

void HandleInputKeyUp (PCONINFO con, WPARAM wParam, LPARAM lParam)
{
    u_char buff [50];

    con->kinfo.state = lParam;
    con->kinfo.buff  = buff;
    con->kinfo.pos   = 0;

    handle_scancode_on_keyup (wParam, &con->kinfo);

    if (con->kinfo.pos != 0)
        my_write (con->masterPty, buff, con->kinfo.pos);

    con->kinfo.oldstate = con->kinfo.state;
}

static inline u_int TextAddress (CONINFO *con, u_int x, u_int y)
{
    return (con->textHead + x + y * con->dispxmax) % con->textSize;
}

static inline bool IsKanji (CONINFO *con, u_int x, u_int y)
{
    return(*(con->flagBuff + TextAddress (con, x, y)) & CODEIS_1);
}

static inline bool IsKanji2 (CONINFO *con, u_int x, u_int y)
{
    return(*(con->flagBuff + TextAddress (con, x, y)) & CODEIS_2);
}

static inline void KanjiAdjust (CONINFO *con, int *x, int *y)
{
    if (IsKanji2 (con, *x, *y))
        --*x;
}

void HandleMouseLeftDownWhenCaptured (PCONINFO con, int x, int y, WPARAM wParam)
{
    if (con->m_origx != -1) {
        TextRefresh (con, true);

        con->m_origx = con->m_origy = -1;
    }

    x = x / GetCharWidth ();
    y = y / GetCharHeight ();

    KanjiAdjust (con, &x, &y);
    con->m_origx = con->m_oldx = x;
    con->m_origy = con->m_oldy = y;
}

void HandleMouseMoveWhenCaptured (PCONINFO con, int x, int y, WPARAM wParam)
{
    x /= GetCharWidth ();
    y /= GetCharHeight ();

    if ( x < 0 || y < 0 || x >= con->dispxmax || y >= con->dispymax)
        return;

    if (x != con->m_oldx || y != con->m_oldy) {
        TextReverse (con, &con->m_origx, &con->m_origy, &x, &y);
        TextRefresh (con, true);
        TextReverse (con, &con->m_origx, &con->m_origy, &x, &y);

        con->m_oldx = x;
        con->m_oldy = y;
    }
}

void HandleMouseLeftUpWhenCaptured (PCONINFO con, int x, int y, WPARAM wParam)
{
    x /= GetCharWidth ();
    y /= GetCharHeight ();

    TextCopy (con, con->m_origx, con->m_origy, x, y);

    con->m_oldx = x;
    con->m_oldy = y;
}

void HandleMouseBothDown (PCONINFO con, int x, int y, WPARAM wParam)
{
    TextPaste (con);
}

void HandleMouseRightUp (PCONINFO con, int x, int y, WPARAM wParam)
{
}

