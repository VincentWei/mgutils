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

#ifndef PAINT_VCONGUI_H
#define PAINT_VCONGUI_H

void WindowScrollUp (PCONINFO con, int scroll, int color);
void WindowScrollDown (PCONINFO con, int scroll, int color);
void WindowClearAll (PCONINFO con, int color);
void WindowStringPut (PCONINFO con, const char* text, 
                            int fc, int bc, int x, int y);
void WindowWput (PCONINFO con, WORD word, int fc, int bc, int pos);
void WindowSput (PCONINFO con, u_char ch, int fc, int bc, int pos);
void WindowSetCursor (PCONINFO con, int x, int y, bool IsKanji);

#define WindowHideCaret HideCaret
#define WindowShowCaret ShowCaret

#endif /* PAINT_VCONGUI_H */

