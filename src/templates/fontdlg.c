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
#include <stdint.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include "mgutils.h"

#ifdef _MGUTILS_DLGFONTSEL

#define ARRAY_LEN(array) (sizeof(array)/sizeof(array[0]))

static const char* style_str [] =
{
    "Regular",
    "Bold",
    "Book",
    "Demibold",
    "Light",
    "Subpixel",
};

static const char style_arg [] = 
{
    FONT_WEIGHT_REGULAR,
    FONT_WEIGHT_BOLD,
    FONT_WEIGHT_BOOK,
    FONT_WEIGHT_DEMIBOLD,
    FONT_WEIGHT_LIGHT,
    FONT_WEIGHT_SUBPIXEL
};

static const char* flip_str [] =
{
    "None    ",
    "Horz    ",
    "Vert    ",
    "Horzvert",
};

static const char flip_arg [] = 
{
    FONT_FLIP_NIL,
    FONT_FLIP_HORZ,
    FONT_FLIP_VERT,
    FONT_FLIP_HORZVERT,
};

static const char *color_str[] =
{
    "Black      ",  
    "White      ",  
    "Gray       ",  
    "Blue       ",  
    "Green      ",  
    "Cyan       ",  
    "Red        ",  
    "Magenta    ",  
    "Yellow     ",  
    "DarkBlue   ",  
    "DarkGreen  ",  
    "DarkCyan   ",  
    "DarkRed    ",  
    "DarkMagenta",  
    "DarkYellow ",  
    "DarkGray   ",  
};

static const RGB FontColor [] = 
{
    {0x00, 0x00, 0x00},     /* black         */
    {0xFF, 0xFF, 0xFF},     /* white         */
    {0xC0, 0xC0, 0xC0},     /* gray          */
    {0x00, 0x00, 0xFF},     /* blue          */
    {0x00, 0xFF, 0x00},     /* green         */
    {0x00, 0xFF, 0xFF},     /* cyan          */
    {0xFF, 0x00, 0x00},     /* red           */
    {0xFF, 0x00, 0xFF},     /* magenta       */
    {0xFF, 0xFF, 0x00},     /* yellow        */
    {0x00, 0x00, 0x80},     /* dark blue     */
    {0x00, 0x80, 0x00},     /* dark green    */
    {0x00, 0x80, 0x80},     /* dark cyan     */
    {0x80, 0x00, 0x00},     /* dark red      */
    {0x80, 0x00, 0x80},     /* dark magenta  */
    {0x80, 0x80, 0x00},     /* dark yellow   */
    {0x80, 0x80, 0x80},     /* dark gray     */
};

/* Data Structure*/

typedef struct _MGU_CHARSET
{
    char                    name [LEN_FONT_NAME + 1];
    struct _MGU_CHARSET     *next;
} MGU_CHARSET, *MGU_PCHARSET;

typedef struct _MGU_FONT
{
    char                name [LEN_FONT_NAME + 1];
    MGU_CHARSET         _mgu_chset;
	int   is_ttf;//is truetype font? 
	int   sizeCount;
	int * supportSize;
    struct _MGU_FONT    *next;
} MGU_FONT, *MGU_PFONT;

typedef struct _MGU_FONTDIA
{
    MGU_PFONT       _mgu_font;
    PFONTDLGDATA    pfdd;
} MGU_FONTDIA, *MGU_PFONTDIA;

/* default templates */
CTRLDATA DefFontCtrl [] =
{ 
    { CTRL_STATIC, 
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        7, 5, 40, 20,
        IDC_FSD_FONT_NOTE, 
        "Font",  
        0, WS_EX_TRANSPARENT | WS_EX_USEPARENTRDR
    },

    { CTRL_COMBOBOX, 
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST 
        | CBS_NOTIFY | CBS_READONLY | WS_BORDER | CBS_EDITNOBORDER,
        7, 27, 140, 20, 
        IDC_FSD_FONT, 
        "", 
        0,  WS_EX_USEPARENTRDR 
    },

    { CTRL_STATIC, 
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        153, 5, 40, 20,
        IDC_FSD_STYLE_NOTE, 
        "Style",  
        0,  WS_EX_USEPARENTRDR
    },

    { CTRL_COMBOBOX, 
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_NOTIFY 
        | CBS_READONLY | WS_BORDER | CBS_EDITNOBORDER,
        153, 27, 90, 20, 
        IDC_FSD_STYLE, 
        "",
        0,  WS_EX_USEPARENTRDR
    },

    { CTRL_STATIC, 
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        250, 5, 40, 20,
        IDC_FSD_SIZE_NOTE, 
        "Size",  
        0, WS_EX_TRANSPARENT | WS_EX_USEPARENTRDR
    },

    { CTRL_COMBOBOX, 
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_NOTIFY 
        | CBS_READONLY | WS_BORDER | CBS_EDITNOBORDER,
        250, 27, 60, 20, 
        IDC_FSD_SIZE, 
        "", 
        0,  WS_EX_USEPARENTRDR
    },

    { CTRL_STATIC, 
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        20, 51, 40, 20,
        IDC_FSD_EFFECTS_NOTE, 
        "Effects",  
        0, WS_EX_USEPARENTRDR
    },

    { CTRL_STATIC, 
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        15, 75, 40, 20,
        IDC_FSD_FLIP_NOTE, 
        "Flip",  
        0, WS_EX_TRANSPARENT | WS_EX_USEPARENTRDR
    },

    { CTRL_COMBOBOX, 
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_NOTIFY 
        | CBS_READONLY | WS_BORDER | CBS_EDITNOBORDER,
        45, 75, 80, 20, 
        IDC_FSD_FLIP, 
        "", 
        0,0
    },

    { CTRL_STATIC, 
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        165, 75, 40, 20,
        IDC_FSD_COLOR_NOTE, 
        "Color",  
        0, WS_EX_TRANSPARENT | WS_EX_USEPARENTRDR
    },

    { CTRL_COMBOBOX, 
        WS_CHILD | WS_VISIBLE | WS_TABSTOP |CBS_DROPDOWNLIST | CBS_NOTIFY 
        | CBS_READONLY | WS_BORDER | CBS_EDITNOBORDER,
        205, 75, 80, 20, 
        IDC_FSD_COLOR, 
        "", 
        0, WS_EX_USEPARENTRDR 
    },

    { CTRL_BUTTON, 
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX,
        15 , 100, 70, 20, 
        IDC_FSD_ITALIC, 
        "Italic", 
        0, WS_EX_USEPARENTRDR 
    },

    { CTRL_BUTTON, 
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX,
        120 , 100, 70, 20, 
        IDC_FSD_STRIKEOUT, 
        "Strikeout", 
        0, WS_EX_USEPARENTRDR
    },

    { CTRL_BUTTON, 
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX,
        210 , 100, 70, 20, 
        IDC_FSD_UNDERLINE, 
        "Underline", 
        0, WS_EX_USEPARENTRDR
    },

    { CTRL_STATIC, 
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        7, 130, 40, 20,
        IDC_FSD_SAMPLE_NOTE, 
        "Sample",  
        0, WS_EX_TRANSPARENT  | WS_EX_USEPARENTRDR
    },

    { CTRL_STATIC, 
        WS_CHILD | WS_VISIBLE | WS_BORDER | SS_LEFT,
        7, 150, 140, 30,
        IDC_FSD_SAMPLE, 
        "AaBbYyZz",  
        0, WS_EX_TRANSPARENT | WS_EX_USEPARENTRDR
    },

    { CTRL_STATIC, 
        WS_CHILD | WS_VISIBLE |SS_LEFT,
        160, 130, 40, 20,
        IDC_FSD_CHARSET_NOTE, 
        "Charset",  
        0, WS_EX_TRANSPARENT | WS_EX_USEPARENTRDR
    },

    { CTRL_COMBOBOX, 
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_NOTIFY 
        | CBS_READONLY | WS_BORDER | CBS_EDITNOBORDER,
        160, 150, 150, 20, 
        IDC_FSD_CHARSET, 
        "", 
        0, WS_EX_USEPARENTRDR 
    },

    { CTRL_BUTTON, 
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
        7 , 185, 140, 25, 
        IDC_FSD_OK, 
        "Ok", 
        0, WS_EX_USEPARENTRDR
    },

    { CTRL_BUTTON, 
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
        160 , 185, 150, 25, 
        IDC_FSD_CANCEL, 
        "Cancel", 
        0, WS_EX_USEPARENTRDR
    }

};

DLGTEMPLATE DefFontDlg =
{
    WS_DLGFRAME | WS_BORDER | WS_CAPTION, 
    WS_EX_USEPARENTRDR,
    0, 0, 320, 240, 
    "Font", 0, 0, 
    TABLESIZE(DefFontCtrl), 
    DefFontCtrl 
};


/////////////////////////////////////////
// color static
static WNDPROC _oldColorStaticProc = NULL;
//Set Color
//wParam = 0
//lPram = (DWORD)color
#define CS_SETTEXTCOLOR MSG_USER + 100

static LRESULT _colorStaticProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message){
	case CS_SETTEXTCOLOR:
		SetWindowAdditionalData(hwnd, lParam);
		InvalidateRect(hwnd, NULL, TRUE);
		return 0;
	case MSG_PAINT:
		{
			char szText[100];
			HDC hdc;
			GetWindowText(hwnd, szText,sizeof(szText)-1);
			hdc = BeginPaint(hwnd);
			SetBkMode(hdc, BM_TRANSPARENT);
			SetTextColor(hdc, GetWindowAdditionalData(hwnd));
			TextOut(hdc,0,0, szText);
			EndPaint(hwnd,hdc);
		}
		return 0;
	}
	return (*_oldColorStaticProc)(hwnd, message, wParam, lParam);
}


static MGU_PFONT InsertFont (MGU_PFONT head, const char* family)
{
    MGU_PFONT cur = NULL;
    MGU_PFONT pre = NULL;

    pre = head;
    cur = head->next;
    while (cur)
    {
        if (strcasecmp (cur->name, family) == 0)
            return cur;
        pre = cur;
        cur = cur->next;
    }

    cur = (MGU_PFONT)calloc(1,sizeof(*cur));
    strncpy (cur->name, family, LEN_FONT_NAME);
    cur->name [LEN_FONT_NAME] = '\0';
    cur->_mgu_chset.next = NULL;
    cur->next = NULL;

    pre->next = cur;
    return cur;
}

static MGU_PFONT FindFont (MGU_PFONT head, const char* family)
{
    MGU_PFONT cur = head->next;
    
    while (cur)
    {
        if (strcasecmp (cur->name, family) == 0)
            return cur;
        cur = cur->next;
    }
    return NULL;
}

static void InsertCharset (MGU_PFONT font, const char* chset_name)
{
    MGU_PCHARSET cur = font->_mgu_chset.next;
    MGU_PCHARSET pre = &(font->_mgu_chset);

    while (cur)
    {
        if (strcasecmp (cur->name, chset_name) == 0)
            return;
        pre = cur;
        cur = cur->next;
    }

    cur = (MGU_PCHARSET)malloc(sizeof(MGU_CHARSET));
    strncpy (cur->name, chset_name, LEN_FONT_NAME);
    cur->name [LEN_FONT_NAME] = '\0';
    cur->next = NULL;

    pre->next = cur;
}

static int isTrueTypeFont(const char* type)
{
	static const char* ttf_types[] = {
		"ttf", "type1"
	};

	int i;

	for(i=0;i<sizeof(ttf_types)/sizeof(char*); i++)
		if(strcmp(type, ttf_types[i]) == 0)
			return 1;
	
	return 0;
}

static int * insertIntArray(int * tarr, int tn, int nin, int * newlen, int *insertAt)
{
	int i;
	if(tarr == NULL)
	{
		tarr = (int*)malloc(sizeof(int)*1);
		tarr[0] = nin;
		if(newlen) 
			*newlen = 1;
		if(insertAt) 
			*insertAt = 0;
		return tarr;
	}
	
	if(newlen) *newlen = tn;
	for(i=(insertAt?*insertAt:0); i<tn; i++)
	{
		if(tarr[i] == nin)
			return tarr;
		else if(tarr[i] > nin)
			break;
	}

	tarr = (int*)realloc(tarr, sizeof(int)*(tn+1));

	if(i<tn)
	{
		for(;tn>i;tn--)
			tarr[tn] = tarr[tn-1];
	}
	tarr[i] = nin;
	if(newlen) 
		(*newlen) ++;
	if(insertAt)
		(*insertAt) = i;
	return tarr;
}

#define GetFontSize(font_ops)   ((intptr_t(*)(PLOGFONT,const DEVFONT*,int))(((intptr_t*)(font_ops))[4]))
static void EnumSupportSize(MGU_PFONT pfont, const DEVFONT* dev_font)
{
	LOGFONT font;
	const char* str, *str2;
	int i;
	int oldsize = 0;
	int insertAt = 0;

	memset(&font, 0, sizeof(font));
	str = strchr(dev_font->name,'-');
	//type
	strncpy(font.type, dev_font->name, str - dev_font->name);
	if((pfont->is_ttf = isTrueTypeFont(font.type)))
		return;
	//family
	str ++;
	str2 = strchr(str, '-');
	strncpy(font.family, str, str2-str);
	
	for(i=FONT_MIN_SIZE; i<=FONT_MAX_SIZE; i++)
	{
		int size = GetFontSize(dev_font->font_ops)(&font, dev_font, i);
		if( size != oldsize)
		{
			pfont->supportSize = insertIntArray(pfont->supportSize, pfont->sizeCount, size, &pfont->sizeCount, &insertAt);
			oldsize = size;
		}
	}
}

static MGU_PFONT CreateFontAttrList (void)
{
    int  i;
    char *sn_start, *sn_sep;
    char *fn_start, *fn_end;
    char font_name[LEN_UNIDEVFONT_NAME + 2];
    MGU_PFONT font_head, cur_font;
    const DEVFONT* dev_font = NULL; 

    font_head = (MGU_PFONT) malloc (sizeof (MGU_FONT));
    font_head->next = NULL;
    font_head->_mgu_chset.next = NULL;

    while ((dev_font = GetNextDevFont (dev_font)))
    {
        strncpy (font_name, dev_font->name, LEN_DEVFONT_NAME);
        strcat (font_name, ",");
        
        fn_start = strchr (font_name, '-') + 1;
        fn_end = strchr (fn_start, '-');
        *fn_end = '\0';

        cur_font = InsertFont (font_head, fn_start);
        
        sn_start = fn_end + 1;
        for (i = 0; i < 3; i++)
            sn_start = strchr (sn_start, '-') + 1;

        while (*sn_start) 
        {
            sn_sep = strchr (sn_start, ',');
            *sn_sep = '\0';
            InsertCharset (cur_font, sn_start);

            sn_start = sn_sep + 1;
        };

		EnumSupportSize(cur_font, dev_font);
    }

    return font_head;
}

static void FreeFontAttrList (MGU_PFONT font_head)
{
    MGU_PFONT    font_tmp, font_cur;
    MGU_PCHARSET chset_cur, chset_tmp;

    font_cur = font_head->next;
    while (font_cur)
    {
        chset_cur = font_cur->_mgu_chset.next;
        while (chset_cur)
        {
            chset_tmp = chset_cur->next;
            free (chset_cur);
            chset_cur = chset_tmp;
        }

		if(font_cur->supportSize)
			free(font_cur->supportSize);
        
        font_tmp = font_cur->next;
        free (font_cur);
        font_cur = font_tmp;
    }

    free (font_head);
}


static void InsertItem (HWND hWnd, const char** strs, int len)
{
    int i;

    for (i = 0; i < len; i++)
    {
        SendMessage (hWnd, CB_INSERTSTRING, i, (LPARAM)strs[i]);
    }
    
    SendMessage (hWnd, CB_SETCURSEL, 0, 0);

    return;
}

static inline MGU_PFONT GetSelectFont(HWND hWnd)
{
    char         buff [LEN_FONT_NAME + 1];
	int cur_sel = SendDlgItemMessage (hWnd, IDC_FSD_FONT, CB_GETCURSEL, 0, 0);
    MGU_PFONTDIA pfdia = (MGU_PFONTDIA)GetWindowAdditionalData (hWnd);
    SendDlgItemMessage (hWnd, IDC_FSD_FONT,  CB_GETLBTEXT, cur_sel, (LPARAM)buff);
    return FindFont (pfdia->_mgu_font, buff);
}

static void RefreshSize(HWND hWnd)
{
	int i;
	HWND hctrl = GetDlgItem(hWnd, IDC_FSD_SIZE);
	MGU_PFONT cur_font = GetSelectFont(hWnd);
	if(cur_font == NULL)
		return ;
	
    SendMessage (hctrl, CB_RESETCONTENT, 0, 0);

	if(cur_font->is_ttf){
		for(i=FONT_MIN_SIZE; i<= FONT_MAX_SIZE; i++)
		{
			char szbuf[10];
			sprintf(szbuf, "%d", i);
			SendMessage(hctrl, CB_ADDSTRING, 0, (LPARAM) szbuf);
		}
	}
	else {
		for(i=0;i<cur_font->sizeCount; i++)
		{
			char szbuf[10];
			sprintf(szbuf, "%d", cur_font->supportSize[i]);
			SendMessage(hctrl, CB_ADDSTRING,0,(LPARAM) szbuf);
		}
	}
	SendMessage(hctrl, CB_SETCURSEL, 0, 0);
}

static int RefreshCharset (HWND hWnd)
{
    HWND         combo;
    MGU_PFONT    cur_font;
    MGU_PCHARSET chset_cur;
    
    cur_font = GetSelectFont(hWnd);
    if (!cur_font)
        return -1;
    
    combo = GetDlgItem (hWnd, IDC_FSD_CHARSET);    
    SendMessage (combo, CB_RESETCONTENT, 0, 0);
    chset_cur = cur_font->_mgu_chset.next;
    while (chset_cur)
    {
        SendMessage (combo, CB_ADDSTRING, 0, (LPARAM)chset_cur->name);
        chset_cur = chset_cur->next;
    }
    SendMessage (combo, CB_SETCURSEL, 0, 0);
   
    return 0;
}

static int InitDialog (HWND hWnd)
{
    HWND         hctrl;
    MGU_PFONT    cur_font ;
    MGU_PFONTDIA pfdia ;

    pfdia = (MGU_PFONTDIA)GetWindowAdditionalData(hWnd);
    cur_font = pfdia->_mgu_font->next;
    
    hctrl = GetDlgItem (hWnd, IDC_FSD_FONT);
    while (cur_font)
    {
        SendMessage (hctrl, CB_ADDSTRING, 0, (LPARAM)cur_font->name);
        cur_font = cur_font->next;
    }

    SendMessage (hctrl, CB_SETCURSEL, 0, 0);

    hctrl = GetDlgItem (hWnd, IDC_FSD_STYLE);
    InsertItem (hctrl, style_str, ARRAY_LEN(style_str));

    hctrl = GetDlgItem (hWnd, IDC_FSD_FLIP);
    InsertItem (hctrl, flip_str, ARRAY_LEN(flip_str));

    hctrl = GetDlgItem (hWnd, IDC_FSD_COLOR);
    InsertItem (hctrl, color_str, ARRAY_LEN(color_str));

    RefreshCharset (hWnd);
    RefreshSize (hWnd);

	return 0;
}

static LOGFONT* CreateFont(HWND hWnd)
{
    HWND hctrl;
    int  sel, checked, size;
    char chset [LEN_FONT_NAME +1];
    char family [LEN_FONT_NAME +1];
	char szSize[10];
    char flip, slant, weight, strikeout, underline;
    
    hctrl = GetDlgItem (hWnd, IDC_FSD_FONT);
    sel = SendMessage (hctrl, CB_GETCURSEL, 0, 0);
    SendMessage (hctrl, CB_GETLBTEXT, sel, (LPARAM)family);

    hctrl = GetDlgItem (hWnd, IDC_FSD_CHARSET);
    sel = SendMessage (hctrl, CB_GETCURSEL, 0, 0);
    SendMessage (hctrl, CB_GETLBTEXT, sel, (LPARAM)chset);
    
    hctrl = GetDlgItem (hWnd, IDC_FSD_STYLE);
    sel = SendMessage (hctrl, CB_GETCURSEL, 0, 0);
    weight = style_arg [sel];

    hctrl = GetDlgItem (hWnd, IDC_FSD_ITALIC);
    checked = SendMessage (hctrl, BM_GETCHECK, 0, 0);
    if (BST_CHECKED == checked)
        slant = FONT_SLANT_ITALIC;
    else
        slant = FONT_SLANT_ROMAN;

    hctrl = GetDlgItem (hWnd, IDC_FSD_STRIKEOUT);
    checked = SendMessage (hctrl, BM_GETCHECK, 0, 0);
    if (BST_CHECKED == checked)
        strikeout = FONT_STRUCKOUT_LINE;
    else
        strikeout = FONT_STRUCKOUT_NONE;

    hctrl = GetDlgItem (hWnd, IDC_FSD_UNDERLINE);
    checked = SendMessage (hctrl, BM_GETCHECK, 0, 0);
    if (BST_CHECKED == checked)
        underline = FONT_UNDERLINE_LINE;
    else
        underline = FONT_UNDERLINE_NONE;

    sel = SendMessage (GetDlgItem (hWnd, IDC_FSD_FLIP), CB_GETCURSEL, 0, 0);
    flip = flip_arg [sel];
	
	hctrl = GetDlgItem(hWnd, IDC_FSD_SIZE);
    sel = SendMessage (hctrl, CB_GETCURSEL, 0, 0);
	SendMessage(hctrl, CB_GETLBTEXT, sel, (LPARAM)szSize);
	size = atoi(szSize);

    return CreateLogFont (FONT_TYPE_NAME_ALL, family, 
                chset, weight, slant, flip, 
                FONT_OTHER_AUTOSCALE, underline, 
                strikeout, size, 0);

}

static int GetSelectColor (HWND hDlg)
{
    int  sel;
    HWND h_color;

    h_color = GetDlgItem (hDlg, IDC_FSD_COLOR);
    sel = SendMessage (h_color, CB_GETCURSEL, 0, 0);

    return sel;
}

static LOGFONT* UpdateFont(HWND hDlg, PLOGFONT* old_font)
{
	LOGFONT* font = CreateFont(hDlg);
	if (font)
	{
		SetWindowFont(GetDlgItem(hDlg, IDC_FSD_SAMPLE), font);
		return font;
	}
	return *old_font;
}


static int SaveFontData (HWND hDlg, PLOGFONT font, int color)
{
    MGU_PFONTDIA pfdia = NULL;

    pfdia = (MGU_PFONTDIA)GetWindowAdditionalData (hDlg);
    
    pfdia->pfdd->logfont = font;
    
    pfdia->pfdd->color.r = FontColor[color].r;
    pfdia->pfdd->color.g = FontColor[color].g;
    pfdia->pfdd->color.b = FontColor[color].b;

    return 0;
}

LRESULT DefFontDialogProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    static PLOGFONT font = NULL;
    MGU_PFONTDIA    pfdia = NULL;
    static PLOGFONT old_font = NULL;

    if (message != MSG_INITDIALOG)
        pfdia = (MGU_PFONTDIA)GetWindowAdditionalData(hDlg);

    switch (message)
    {
        case MSG_INITDIALOG:
            {
                pfdia = (MGU_PFONTDIA)malloc(sizeof(MGU_FONTDIA));
                if (pfdia == NULL)
                {
                    fprintf(stderr, "DefFontDialogProc: malloc error.\n");
                    return -1;
                }
                pfdia->_mgu_font = CreateFontAttrList ();
                pfdia->pfdd = (PFONTDLGDATA)lParam;
                SetWindowAdditionalData(hDlg, (DWORD)pfdia);

				_oldColorStaticProc = SetWindowCallbackProc(GetDlgItem(hDlg, IDC_FSD_SAMPLE), _colorStaticProc);
              
                InitDialog (hDlg);
                
                if (NULL != (font = CreateFont (hDlg)))
                    old_font = font;

                if (pfdia->pfdd->hook)
                {
                    return pfdia->pfdd->hook (hDlg, message, wParam, lParam);
                }
                break;
            }
        case MSG_CLOSE:
            {
                if (old_font)
                    DestroyLogFont (old_font);

                EndDialog (hDlg, IDCANCEL);
                break;
            }
        case MSG_DESTROY:
            {
                FreeFontAttrList (pfdia->_mgu_font);
                free (pfdia);
                break;
            }
        case MSG_PAINT:
            {
                HDC hdc = BeginPaint (hDlg);

                /* 
                 * paint a group box, 
                 * if we used CTRL_STATIC with style SS_GOUPBOX
                 * the combobox show error.
                 */
                SetPenColor (hdc, PIXEL_lightwhite);
                Rectangle (hdc, 8, 61, 304, 126);
                SetPenColor (hdc, PIXEL_darkgray);
                Rectangle (hdc, 7, 60, 303, 125);
                /*
                 * end of paint the group box.
                 */
                EndPaint (hDlg, hdc);
                return 0;
            }
        case MSG_COMMAND:
            {
                int id = LOWORD (wParam);
                int nc = HIWORD (wParam);
                switch (id)
                {
                    case IDC_FSD_ITALIC:
                    case IDC_FSD_STRIKEOUT:
                    case IDC_FSD_UNDERLINE:
                        if (nc == BN_CLICKED)
                        {
							font = UpdateFont(hDlg, &old_font);                           
                        }
                        break;
                    case IDC_FSD_FONT:
                        if (nc == CBN_SELCHANGE)
                        { 
                            RefreshCharset (hDlg);
							RefreshSize(hDlg);
							font = UpdateFont(hDlg, &old_font);
                        }
						break;
                    case IDC_FSD_COLOR:
						if(nc == CBN_SELCHANGE)
						{
							DWORD color;
							int sel = GetSelectColor(hDlg);
							color = RGB2Pixel(HDC_SCREEN,FontColor[sel].r, FontColor[sel].g, FontColor[sel].b);
							SendDlgItemMessage(hDlg, IDC_FSD_SAMPLE, CS_SETTEXTCOLOR, 0, (LPARAM)color);
						}
						break;
                   case IDC_FSD_CHARSET:
                    case IDC_FSD_STYLE:
                    case IDC_FSD_FLIP:
                    case IDC_FSD_SIZE:
                        if (nc == CBN_SELCHANGE)
                        { 
							font = UpdateFont(hDlg,&old_font);
                        }
                        break;
                    case IDC_FSD_OK:
                        if (old_font)
                            SaveFontData (hDlg, old_font, GetSelectColor(hDlg));
                        
                        if (!pfdia->pfdd->hook || (pfdia->pfdd->hook 
                                    && !pfdia->pfdd->hook (hDlg, MSG_FONTSELOK, 
                                        0, (LPARAM)pfdia->pfdd)))
                        {
                            EndDialog (hDlg, IDOK);
                            break;
                        }
                        else
                            return -1;
                    case IDC_FSD_CANCEL:
                        if (old_font)
                            DestroyLogFont (old_font);

                        if (!pfdia->pfdd->hook || (pfdia->pfdd->hook 
                                    && !pfdia->pfdd->hook (hDlg, MSG_FONTSELCANCEL, 
                                        0, (LPARAM)pfdia->pfdd)))
                        {
                            EndDialog (hDlg, IDCANCEL);
                            break;
                        }
                        else
                            return -1;
                }
                break;
            }
    }
    
    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

/**
 * 
 */
BOOL FontSelectDialog  (PDLGTEMPLATE dlg_template, 
        HWND hwnd, WNDPROC proc, PFONTDLGDATA pfdd)
{
    PDLGTEMPLATE font_dlg;
    WNDPROC font_proc;

    if (dlg_template) {
        font_dlg = dlg_template;
    }
    else {
        font_dlg = &DefFontDlg;
    }

    if (pfdd->is_trans == TRUE)
    {
        int i;
        for (i = 0; i < font_dlg->controlnr; i++){
            printf ("i:%d, %s.\n", i, font_dlg->controls[i].class_name);
            font_dlg->controls[i].dwExStyle |= WS_EX_TRANSPARENT;
        }
    }

    if (proc) {
        font_proc = proc;
    }
    else {
        font_proc = DefFontDialogProc;
    }
        
    return ShowCommonDialog (font_dlg, hwnd, font_proc, pfdd);
}

#endif /* _MGUTILS_DLGFONTSEL */

