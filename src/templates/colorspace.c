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
#include <stdarg.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgutils.h"

#ifdef _MGUTILS_DLGCOLORSEL

#define LIMIT(a, b, c ) do {a=((a<b)?b:a);a=(a>c)?c:a;} while (0)

#if 0
// Needed by rgb2hsv()
static float maxrgb (float r, float g, float b)
{
    float max;
  
    if (r > g)
        max = r;
    else
        max = g;
    if (b > max)
        max = b;

    return max;
}


// Needed by rgb2hsv()
static float minrgb (float r, float g, float b)
{
    float min;
  
    if (r < g)
        min = r;
    else
        min = g;
    if (b < min)
        min = b;

    return min;
}

/* 
   Assumes (r,g,b) range from 0.0 to 1.0
   Sets h in degrees: 0.0 to 360. s,v in [0.,1.]
*/
static void rgb2hsva (float r, float g, float b, float *hout, float *sout, float *vout)
{
    float h=0, s=1.0, v=1.0;
    float max_v, min_v, diff, r_dist, g_dist, b_dist;
    float undefined = 0.0;

    max_v = maxrgb (r,g,b);
    min_v = minrgb (r,g,b);
    diff = max_v - min_v;
    v = max_v;

    if (max_v != 0)
        s = diff/max_v;
    else
        s = 0.0;
    if (s == 0)
        h = undefined;
    else {
        r_dist = (max_v - r)/diff;
        g_dist = (max_v - g)/diff;
        b_dist = (max_v - b)/diff;
        if (r == max_v) 
            h = b_dist - g_dist;
        else
        if (g == max_v)
            h = 2 + r_dist - b_dist;
        else {
            if (b == max_v)
                h = 4 + g_dist - r_dist;
            else
                printf("rgb2hsv::How did I get here?\n");
        }

        h *= 60;
        if (h < 0)
            h += 360.0;
    }

    *hout = h;
    *sout = s;
    *vout = v;
}

/* 
   Assumes (r,g,b) range from 0 to 255
   Sets h in degrees: 0 to 360. s,v in [0,255]
*/
void RGB2HSV (Uint8 r, Uint8 g, Uint8 b, Uint16 *hout, Uint8 *sout, Uint8 *vout)
{
    float h, s, v;

    rgb2hsva (r/255., g/255., b/255., &h, &s, &v);

    *hout = h;
    *sout = 256 * s;
    *vout = 256 * v;
}

void HSV2RGB (Uint16 hin, Uint8 sin, Uint8 vin, Uint8 *rout, Uint8 *gout, Uint8 *bout)
{
    float h, s, v;
    float r = 0, g = 0, b = 0;
    float f, p, q, t;
    int i;

    h = hin;
    s = sin/255.;
    v = vin/255.;

    if (sin == 0 ) {
        r = v;
        g = v;
        b = v;
    }
    else {
        if (hin == 360)
            h = 0.0;
        h /= 60.;
        i = (int) h;
        f = h - i;
        p = v*(1-s);
        q = v*(1-(s*f));
        t = v*(1-s*(1-f));

        switch (i) {
        case 0:
            r = v;
            g = t;
            b = p;
            break;
        case 1:
            r = q;
            g = v;
            b = p;
            break;
        case 2:
            r = p;
            g = v;
            b = t;
            break;
        case 3:
            r = p;
            g = q;
            b = v;
            break;
        case 4:
            r = t;
            g = p;
            b = v;
            break;
        case 5:
            r = v;
            g = p;
            b = q;
            break;
        default:
            r = 1.0;
            b = 1.0;
            b = 1.0;
            break;
        }
    }

    *rout = 256*r;
    *gout = 256*g;
    *bout = 256*b;
}
#endif

#define MAX3(a,b,c)  ((a)>(b)?((a)>(c)?(a):(c)):((b)>(c)?(b):(c)))
#define MIN3(a,b,c)  ((a)<(b)?((a)<(c)?(a):(c)):((b)<(c)?(b):(c)))

static void _RGB2HSV(int r, int g, int b, int *hout, int *sout, int *vout)
{
	int max;
	int min;
	int delta;
	int h = 0, s, v;

	max = MAX3(r, g, b);
	min = MIN3(r, g, b);
	delta = max - min;
	
    v = (max<<16)/255 * 100 >> 16;
	//v = ((max*100)<<16)/255 >> 16;
		
	if(delta == 0){
		h = 0;
		s = 0;
	}else {
		int del_r, del_g, del_b, d;
		s = (((delta<<16) / max) * 100 ) >> 16 ;

		max <<= 16;
		r <<= 16;
		g <<= 16;
		b <<= 16;
		d = delta << 16;
		
		del_r = ((max - r) / 6 + d / 2) / delta;
		del_g = ((max - g) / 6 + d / 2) / delta;
		del_b = ((max - b) / 6 + d / 2) / delta;

		if( r == max)
			h = ((del_b - del_g) ) * 360 >> 16;
		else if( g== max)
			h = ((del_r - del_b + (1<<16) / 3 ) ) * 360 >> 16;
		else if( b == max)
			h = ((del_g - del_r + (2<<16) / 3 ) ) * 360 >> 16;

		if(h < 0) 
			h += 360;
		else if(h > 360)
			h -= 360;
	}
	
	*hout = h;
	*sout = s;
	*vout = v;

}

void RGB2HSV (Uint8 r, Uint8 g, Uint8 b, 
                Uint16 *hout, Uint8 *sout, Uint8 *vout)
{
    int h, s, v;

	_RGB2HSV(r, g, b, &h, &s, &v);

    *hout = (Uint16)h;
    *sout = (Uint8)s;
    *vout = (Uint8)v;
}

#define SetRGB(r, g, b) do { (*rout)=((r)*255>>16); (*gout)=((g)*255)>>16; (*bout)=((b)*255)>>16; }while(0)
//#define SetRGB(r, g, b) do { (*rout)=((r)*256>>16); (*gout)=((g)*256)>>16; (*bout)=((b)*256)>>16; }while(0)
static void _HSV2RGB(int h, int s, int v, int *rout, int *gout, int *bout)
{
	if( s == 0){
		*rout = (((v*255)<<16 ) / 100 ) >> 16;
		//*rout = (((v*256)<<16 ) / 100 ) >> 16;
		*gout = *bout = *rout;
	}else {
		int i;
		int m,n;
		int hr;

		i = h / 60;
		hr = h % 60;

		if(!(i&1)) 
			hr = 60 - hr;

		m = ((v*(100-s))<<16) / 10000;
		n = ((v*(6000-s*hr)/60)<<16) / 10000;
		v = (v<<16)/100;

		switch(i){
			case 6:
			case 0:
				SetRGB(v, n, m); break;
			case 1: SetRGB(n, v, m); break;
			case 2: SetRGB(m, v, n); break;
			case 3: SetRGB(m, n, v); break;
			case 4: SetRGB(n, m, v); break;
			case 5: SetRGB(v, m, n); break;
		}
	}

}

void HSV2RGB (Uint16 hin, Uint8 sin, Uint8 vin, 
                Uint8 *rout, Uint8 *gout, Uint8 *bout)
{
	int r = 0, g = 0, b = 0;
	_HSV2RGB(hin, sin, vin, &r, &g, &b);
	*rout = (Uint8)r;
	*gout = (Uint8)g;
	*bout = (Uint8)b;
}

void YUV2RGB (int y, int u, int v, Uint8 *r, Uint8 *g, Uint8 *b)
{
    double rr, gg ,bb;
    
    y = y - 16;
    u = u - 128;
    v = v - 128;
    rr = 0.00456621 * y + 0.00625893 * v;
    gg = 0.00456621 * y - 0.00153632 * u - 0.00318811 * v;
    bb = 0.00456621 * y + .00791071 * u;
    
    *r = 255 * rr;
    *g = 255 * gg;
    *b = 255 * bb;
}

void RGB2YUV (Uint8 r, Uint8 g, Uint8 b, int *y, int *u, int *v)
{
#if 1 /* these are the two formulas that I found on the FourCC site... */
    *y = 0.299*r + 0.587*g + 0.114*b;
    *u = (b-*y)*0.565 + 128;
    *v = (r-*y)*0.713 + 128;
#else
    *y = (0.257 * r) + (0.504 * g) + (0.098 * b) + 16;
    *u = 128 - (0.148 * r) - (0.291 * g) + (0.439 * b);
    *v = 128 + (0.439 * r) - (0.368 * g) - (0.071 * b);
#endif
}

#endif /* _MGUTILS_DLGCOLORSEL */
