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

#ifndef DEFS_VCONGUI_H
#define DEFS_VCONGUI_H

#define _USE_ASM    0

/* memory handling routines */
#if _USE_ASM
static inline void bzero2(void *head, int n)
{
    __asm__ ("cld\n\t"
         "rep\n\t"
         "stosb"
         ::"a" (0),
         "c" (n),
         "D" ((long)head)
         :"cx","di");
}
#else
static inline void bzero2 (void *head, int n)
{
    int i;
    BYTE* b = (BYTE*) head;

    for (i = 0; i < n; i ++)
        b [i] = 0;
}
#endif

#if _USE_ASM
static inline void wzero(void *head, int n)
{
    __asm__ ("cld\n\t"
         "rep\n\t"
         "stosw"
         ::"a" (0),
         "c" (n>>1),
         "D" ((long)head)
         :"cx","di");
}
#else
static inline void wzero (void *head, int n)
{
    int i;
    WORD* w = (WORD*) head;

    for (i = 0; i < (n>>1); i ++)
        w [i] = 0;
}
#endif

#if _USE_ASM
static inline void lzero(void *head, int n)
{
    __asm__ ("cld\n\t"
	    "rep\n\t"
	    "stosl"
	    ::"a" (0),
	    "c" (n>>2),
	    "D" ((long)head)
	    :"cx","di");
}
#else
static inline void lzero (void *head, int n)
{
    int i;
    LONG* l = (LONG*) head;

    for (i = 0; i < (n>>2); i ++)
        l [i] = 0;
}
#endif

#if _USE_ASM
static inline void bmove(void *dst, void *src, int n)
{
    __asm__ ("cld\n\t"
	    "rep\n\t"
	    "movsb\n\t"
	    ::"c" (n),
	    "D" ((long)dst),
	    "S" ((long)src)
	    :"cx","di","si");
}
#else
// not used by vcongui.
#endif

#if _USE_ASM
static inline void brmove(void *dst, void *src, int n)
{
    __asm__ ("std\n\t"
	    "rep\n\t"
	    "movsb\n\t"
	    ::"c" (n),
	    "D" ((long)dst),
	    "S" ((long)src)
	    :"cx","di","si");
}
#else
static inline void brmove(void *dst, void *src, int n)
{
    int i;
    char* mydst = (char*) dst;
    char* mysrc = (char*) src;

    for (i = n - 1; i >= 0; i--)
        mydst [i] = mysrc [i];
}
#endif

#if _USE_ASM
static inline void wmove(void *dst, void *src, int n)
{
    __asm__ ("cld\n\t"
        "rep\n\t"
        "movsw\n\t"
        ::"c" (n>>1),
        "D" ((long)dst),
        "S" ((long)src)
        :"cx","di","si");
}
#else
// not used by vcongui
#endif

#if _USE_ASM
static inline void lmove(void *dst, void *src, int n)
{
    __asm__ ("cld\n\t"
        "rep\n\t"
        "movsl\n\t"
        ::"c" (n>>2),
        "D" ((long)dst),
        "S" ((long)src)
        :"cx","di","si");
}
#else
// not used by vcongui
#endif

static inline void SafeFree (void **p)
{
    if (*p) {
        free (*p);
        *p = NULL;
    }
}

#endif /* DEFS_VCONGUI_H */

