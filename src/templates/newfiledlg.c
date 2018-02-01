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
#include <ctype.h>

#include <minigui/common.h>

#if !defined(__NOUNIX__) && defined(_MGCTRL_LISTVIEW)
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <pwd.h>
#include <errno.h>

#include "mgutils.h"

#ifdef _MGUTILS_DLGOPENFILE

static HICON icon_ft_dir, icon_ft_file;

static BOOL InitNewOpenFileBox (void)
{
    icon_ft_dir = LoadSystemIcon (SYSICON_FT_DIR, 0);
    if (icon_ft_dir == 0)
        return FALSE;

    icon_ft_file = LoadSystemIcon (SYSICON_FT_FILE, 0);
    if (icon_ft_file == 0) {
        DestroyIcon (icon_ft_dir);
        icon_ft_dir = 0;
        return FALSE;
    }

    return TRUE;
}

static void NewOpenFileBoxCleanup (void)
{
    if (icon_ft_dir) {
        DestroyIcon (icon_ft_dir);
        icon_ft_dir = 0;
    }

    if (icon_ft_file) {
        DestroyIcon (icon_ft_file);
        icon_ft_file = 0;
    }
}

#define IDC_UP               510
#define IDC_STATIC1          520
#define IDC_STATIC2          521
#define IDC_STATIC3          522
#define IDC_FILECHOISE       530
#define IDC_FILETYPE         540
#define IDC_FILENAME         550
#define IDC_PATH             560
#define IDC_OKAY             570
#define IDC_CANCEL           580
#define IDC_ISHIDE           590

#define SPACE_WIDTH     7 
#define SPACE_HEIGHT    7
#define MIN_WIDTH       490
#define OTHERS_WIDTH    330

#define COL_COUNT       4

typedef struct _FILEINFO
{
    char filename[MY_NAMEMAX+1];
    int  filesize;
    int  accessmode;
    time_t modifytime;
    BOOL IsDir;

} FILEINFO; 

typedef FILEINFO* PFILEINFO;

static int ColWidth [COL_COUNT] =
{
    160, 100, 70, 160
};

static BOOL IsInFilter (char *filtstr, char *filename)
{
    char chFilter[MAX_FILTER_LEN+1];
    char chTemp[255];
    char chFileExt[255];
    char *p1;
    char *p2;

    if (filename == NULL)
        return FALSE;
    if (filtstr == NULL)
        return TRUE;
   
    //strtrimall (filename);
    //strtrimall (filtstr);
    if (filename [0] == '\0')
        return FALSE;
    if (strlen (filtstr) == 0)
        return TRUE;
    
    p1 = strchr (filename, '.');
    p2 = NULL;
    while (p1 != NULL) {
        p2 = p1;
        p1 = strchr (p2 + 1, '.');
    }

    if (p2 != NULL) {
        strcpy (chFileExt, "*");
        strcpy (chFileExt + 1, p2);
    }
    else 
        strcpy (chFileExt, "*.*");
    
    p1 = strchr (filtstr, '(');
    p2 = strchr (filtstr, ')');

    if (p1 == NULL || p2 == NULL)
        return FALSE;

    memset (chFilter, 0, sizeof (chFilter)); 
    strncpy (chFilter, p1 + 1, p2 - p1 - 1); 

    p1 = strchr (chFilter, ';');
    p2 = chFilter;
    while ( p1 != NULL ) {
        strncpy (chTemp, p2, p1 - p2);
        //strtrimall (chTemp);
        if ( strcmp (chTemp, "*.*") == 0 || strcmp (chTemp, chFileExt) == 0 ) 
            return TRUE;
        p2 = p1 + 1;
        p1 = strchr (p2, ';');
    }

    strcpy (chTemp, p2);
    //strtrimall (chTemp);
    if ( strcmp (chTemp, "*.*") == 0 || strcmp (chTemp, chFileExt) == 0)
        return TRUE;

    return FALSE;

}

static char* GetParentDir (char *dir)
{
    int i, nParent = 0;
    
    for (i = 0; i < strlen (dir) -1; i++) 
       if (dir [i] == '/') 
            nParent = i;

    if (nParent == 0)
        dir [nParent + 1] = 0;
    else 
        dir [nParent] = 0;
       
    return dir;
}

static int GetAccessMode (HWND hWnd, char * dir, BOOL IsSave, BOOL IsDisplay)
{
    char msg[255];
    int  nResult=0;

    memset (msg, 0, sizeof (msg));

    if (access (dir, F_OK) == -1){
        sprintf (msg, GetSysText(IDS_MGST_SHOWHIDEFILE), dir);
        nResult = -1;
    } 
    else {
        if ( access (dir, R_OK) == -1) {
            sprintf (msg, GetSysText(IDS_MGST_NR), dir);
            nResult = -2;
        } 
        else  if ( IsSave == TRUE && access (dir, W_OK) == -1) {
            sprintf (msg, GetSysText(IDS_MGST_NW), dir);
            nResult = -3;
        } 
    }
   
     
    if (IsDisplay && nResult != 0)
       MessageBox (hWnd, msg, GetSysText(IDS_MGST_INFO), 
           MB_OK | MB_ICONSTOP| MB_BASEDONPARENT);
    
    return nResult;
}

/*
 * If file has been exist or no write access, return non-zero. 
 * Otherwise return zero.
 * */
static int FileExistDlg (HWND hWnd, char *dir, char *filename)
{
    char msg[255];
    memset (msg, 0, sizeof (msg));

    if (access (dir, F_OK) == 0){
        sprintf (msg, GetSysText(IDS_MGST_FILEEXIST), filename);
        if (MessageBox (hWnd, msg, GetSysText(IDS_MGST_INFO), 
                MB_OKCANCEL | MB_ICONSTOP| MB_BASEDONPARENT) == IDOK)
        {
            if (access (dir, W_OK) == -1) { /*no write access*/
                sprintf (msg, GetSysText(IDS_MGST_NR), filename);

                MessageBox (hWnd, msg, GetSysText(IDS_MGST_INFO), 
                    MB_OK | MB_ICONSTOP| MB_BASEDONPARENT);
                return -1; 
            }
            else { /*want to replace*/
                return 0; 
            }
        }
        else /*no replace*/
            return -2; 
    }
    /*file doesn't exist*/
    return 0; 
}

static void InsertToListView( HWND hWnd, PFILEINFO pfi)
{
    HWND      hListView;
    int       nItemCount;
    LVSUBITEM subdata;
    LVITEM    item;
    char      chTemp[255];
    struct tm *ptm;
    
    hListView = GetDlgItem (hWnd, IDC_FILECHOISE);
    nItemCount = SendMessage (hListView, LVM_GETITEMCOUNT, 0, 0);
    
    item.nItem = nItemCount;
    item.itemData = (DWORD)pfi->IsDir;
    item.nItemHeight = 24;
    SendMessage (hListView, LVM_ADDITEM, 0, (LPARAM)&item);

    subdata.nItem = nItemCount;
    subdata.nTextColor = PIXEL_black;
    subdata.flags = LVFLAG_ICON;
    
    if (pfi->IsDir)
        subdata.image = (DWORD)icon_ft_dir;
    else
        subdata.image = (DWORD)icon_ft_file;

    subdata.subItem = 0;
    subdata.pszText = (char *)malloc (MY_NAMEMAX+1);
    if ( subdata.pszText == NULL) 
        return ;

    strcpy (subdata.pszText, pfi->filename);
    SendMessage (hListView, LVM_SETSUBITEM, 0, (LPARAM)&subdata);

    subdata.flags = 0;
    subdata.image = 0;
    subdata.subItem = 1;
    sprintf(chTemp, " %d", pfi->filesize);
    strcpy(subdata.pszText, chTemp);
    SendMessage (hListView, LVM_SETSUBITEM, 0, (LPARAM)&subdata);

    subdata.subItem = 2;
    strcpy (subdata.pszText, "");
    switch (pfi->accessmode) {
    case 1:
        strcpy (subdata.pszText, GetSysText (IDS_MGST_R));
        break;
    case 2:
        strcpy (subdata.pszText, GetSysText (IDS_MGST_W));
        break;
    case 3:
        strcpy (subdata.pszText, GetSysText (IDS_MGST_WR));
        break;
    }
    SendMessage (hListView, LVM_SETSUBITEM, 0, (LPARAM)&subdata);

    subdata.subItem = 3;
    ptm = (struct tm *)localtime (&(pfi->modifytime));

    sprintf (subdata.pszText, "%d-%.2d-%.2d",
        ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday);

    sprintf (subdata.pszText + 10, " %.2d:%.2d:%.2d",
        ptm->tm_hour, ptm->tm_min, ptm->tm_sec);

    SendMessage (hListView, LVM_SETSUBITEM, 0, (LPARAM)&subdata);

    if (subdata.pszText != NULL) free (subdata.pszText);
}

static int 
ListViewSortBySize( HLVITEM nItem1, HLVITEM nItem2, PLVSORTDATA sortdata)
{
    HWND       hCtrlWnd;
    LVSUBITEM  subItem1, subItem2;
    int        nIsDir1, nIsDir2;
    int        nSize1, nSize2;

    hCtrlWnd = sortdata->hLV;

    nIsDir1 = SendMessage (hCtrlWnd, LVM_GETITEMADDDATA, 0, (LPARAM)nItem1);
    nIsDir2 = SendMessage (hCtrlWnd, LVM_GETITEMADDDATA, 0, (LPARAM)nItem2);

    if ( nIsDir1 >nIsDir2 )
        return 1;

    if (nIsDir1 < nIsDir2)
        return -1;

    //subItem1.nItem = nItem1;
    subItem1.subItem = 1;
    subItem1.pszText = (char *)malloc (MY_NAMEMAX+1);
    if ( subItem1.pszText == NULL) 
        return 0;
    SendMessage (hCtrlWnd, LVM_GETSUBITEMTEXT, 
            (WPARAM)nItem1, (LPARAM)&subItem1);

    //subItem2.nItem = nItem2;
    subItem2.subItem = 1;
    subItem2.pszText = (char *)malloc (MY_NAMEMAX+1);
    if (subItem2.pszText == NULL) {
        if (subItem1.pszText != NULL) free (subItem1.pszText);
        return 0;
    }

    SendMessage (hCtrlWnd, LVM_GETSUBITEMTEXT, 
            (WPARAM)nItem2, (LPARAM)&subItem2);

    nSize1 = atoi (subItem1.pszText);
    nSize2 = atoi (subItem2.pszText);

    if (subItem1.pszText) free (subItem1.pszText);
    if (subItem2.pszText) free (subItem2.pszText);
    
    if ( nSize1 >nSize2 )
        return 1;
    else if (nSize1 <nSize2)
        return -1;

    return 0;
}

static int 
ListViewSortByName( HLVITEM nItem1, HLVITEM nItem2, PLVSORTDATA sortdata)
{
    HWND       hCtrlWnd;
    LVSUBITEM  subItem1, subItem2;
    int        nIsDir1, nIsDir2;
    int        nResult;
    
    hCtrlWnd = sortdata->hLV;
    
    nIsDir1 = SendMessage (hCtrlWnd, LVM_GETITEMADDDATA, 0, (LPARAM)nItem1);
    nIsDir2 = SendMessage (hCtrlWnd, LVM_GETITEMADDDATA, 0, (LPARAM)nItem2);
   
    if ( nIsDir1 >nIsDir2 ) 
        return 1;
    
    if (nIsDir1 < nIsDir2)
        return -1;
   
    //subItem1.nItem = nItem1;
    subItem1.subItem = 0;
    subItem1.pszText = (char *)malloc (MY_NAMEMAX+1);
    if (subItem1.pszText == NULL) 
        return 0;

    SendMessage (hCtrlWnd, LVM_GETSUBITEMTEXT, 
            (WPARAM)nItem1, (LPARAM)&subItem1);

    //subItem2.nItem = nItem2;
    subItem2.subItem = 0;
    subItem2.pszText = (char *)malloc (MY_NAMEMAX+1);
    if (subItem2.pszText == NULL) {
        if (subItem1.pszText != NULL) free (subItem1.pszText);
        return 0;
    }

    SendMessage (hCtrlWnd, LVM_GETSUBITEMTEXT, 
            (WPARAM)nItem2, (LPARAM)&subItem2);

    nResult =  strcmp (subItem1.pszText, subItem2.pszText);

    if (subItem1.pszText != NULL) free (subItem1.pszText);
    if (subItem2.pszText != NULL) free (subItem2.pszText);

    return nResult;
}

static int 
ListViewSortByDate( HLVITEM nItem1, HLVITEM nItem2, PLVSORTDATA sortdata)
{
    HWND       hCtrlWnd;
    LVSUBITEM  subItem1, subItem2;
    int        nIsDir1, nIsDir2;
    int        nResult;
    
    hCtrlWnd = sortdata->hLV;

    nIsDir1 = SendMessage (hCtrlWnd, LVM_GETITEMADDDATA, 0, (LPARAM)nItem1);
    nIsDir2 = SendMessage (hCtrlWnd, LVM_GETITEMADDDATA, 0, (LPARAM)nItem2);
    
    if ( nIsDir1 >nIsDir2 )
        return 1;
        
    if (nIsDir1 < nIsDir2)
        return -1;

    //subItem1.nItem = nItem1;
    subItem1.subItem = 3;
    subItem1.pszText = (char *)malloc (MY_NAMEMAX+1);
    if (subItem1.pszText == NULL)
        return 0;
    SendMessage (hCtrlWnd, LVM_GETSUBITEMTEXT, 
            (WPARAM)nItem1, (LPARAM)&subItem1);

    //subItem2.nItem = nItem2;
    subItem2.subItem = 3;
    subItem2.pszText = (char *)malloc (MY_NAMEMAX+1);
    if (subItem2.pszText == NULL) {
        if (subItem1.pszText != NULL) free (subItem1.pszText);
        return 0;
    }
    SendMessage (hCtrlWnd, LVM_GETSUBITEMTEXT, 
            (WPARAM)nItem2, (LPARAM)&subItem2);

    nResult =  strcmp (subItem1.pszText, subItem2.pszText);

    if (subItem1.pszText != NULL) free (subItem1.pszText);
    if (subItem2.pszText != NULL) free (subItem2.pszText);

    return nResult;
}

static void GetFileAndDirList( HWND hWnd, char* path, char* filtstr)
{
    HWND     hCtrlWnd;
    struct   dirent* pDirEnt;
    DIR*     dir;
    struct   stat ftype;
    char     fullpath [MY_PATHMAX + 1];
    char     filefullname[MY_PATHMAX+MY_NAMEMAX+1];
    FILEINFO fileinfo;
    int      nRet;
    int      nCheckState;

    hCtrlWnd = GetDlgItem (hWnd, IDC_ISHIDE);
    nCheckState = SendMessage (hCtrlWnd, BM_GETCHECK, 0, 0);
    
    hCtrlWnd = GetDlgItem (hWnd, IDC_FILECHOISE);
    SendMessage (hCtrlWnd, LVM_DELALLITEM, 0, 0);
   
    if (path == NULL) return;
    //strtrimall (path);
    if (strlen (path) == 0) return;

    if (strcmp (path, "/") != 0 && path [strlen(path)-1] == '/')
        path [strlen(path)-1]=0;

    SendMessage (hCtrlWnd, MSG_FREEZECTRL, TRUE, 0);
 
    dir = opendir (path);
    while (dir && (pDirEnt = readdir ( dir )) != NULL ) {

        memset (&fileinfo, 0, sizeof (fileinfo));    
        
        strncpy (fullpath, path, MY_PATHMAX);
        strcat (fullpath, "/");
        strcat (fullpath, pDirEnt->d_name);
         
        if ( strcmp (pDirEnt->d_name, ".") == 0 
            || strcmp (pDirEnt->d_name, "..") == 0) 
            continue;

        if ( nCheckState != BST_CHECKED) {
            if (pDirEnt->d_name [0] == '.') 
                continue;
        }

        if (stat (fullpath, &ftype) < 0 ){
           continue;
        }
        
        if (S_ISDIR (ftype.st_mode)){
            fileinfo.IsDir = TRUE;
            fileinfo.filesize = ftype.st_size;
        }
        else if (S_ISREG (ftype.st_mode)) {
            if ( !IsInFilter (filtstr, pDirEnt->d_name) )
                continue;
            
            fileinfo.IsDir = FALSE;
            fileinfo.filesize = ftype.st_size;
        }
        
        strcpy (fileinfo.filename, pDirEnt->d_name);
        
        sprintf (filefullname, "%s/%s", 
            strcmp (path, "/") == 0 ? "" : path, pDirEnt->d_name);  

        fileinfo.accessmode = 0;
        nRet = GetAccessMode (hWnd, filefullname, FALSE, FALSE);
        if (nRet != -1 && nRet != -2)
            fileinfo.accessmode = 1;


        nRet = GetAccessMode (hWnd, filefullname, TRUE, FALSE);
        if (nRet != -1 && nRet != -3)
            fileinfo.accessmode = fileinfo.accessmode + 2;

        fileinfo.modifytime = ftype.st_mtime;

        InsertToListView (hWnd, &fileinfo);
    }

    closedir (dir);

    SendMessage (hCtrlWnd, LVM_COLSORT, (WPARAM)0, 0);
    SendMessage (hCtrlWnd, MSG_FREEZECTRL, FALSE, 0);
}

static void InitListView( HWND hWnd)
{
    HWND      hListView;
    int       i;
    LVCOLUMN  lvcol;
    int       nWidth, nColWidth;
    RECT      rcLv;
 
    hListView = GetDlgItem (hWnd, IDC_FILECHOISE);
    GetWindowRect (hListView, &rcLv);
    nWidth = rcLv.right - rcLv.left;
    nColWidth = MIN_WIDTH - OTHERS_WIDTH;

    if (nWidth > MIN_WIDTH)
        nColWidth = nWidth - OTHERS_WIDTH;

    for (i = 0; i < COL_COUNT; i++) {
        lvcol.nCols = i;
        lvcol.pszHeadText = (char *)GetSysText(IDS_MGST_LOCATION + i + 2);
        lvcol.width = ColWidth[i];
        /*lvcol.pfnCompare = ListViewSort;*/
        lvcol.pfnCompare = NULL;
        if (i == 0){ 
            lvcol.width = nColWidth - 28;
            lvcol.pfnCompare = ListViewSortByName;
        }
        else if (i == 1)
            lvcol.pfnCompare = ListViewSortBySize;
        else if (i == 3)
            lvcol.pfnCompare = ListViewSortByDate;

        lvcol.colFlags = 0;
        SendMessage (hListView, LVM_ADDCOLUMN, 0, (LPARAM) &lvcol);
    }
}

static void InitPathCombo(HWND hWnd, char *path)
{
    HWND hCtrlWnd;
    char chSubPath[MY_PATHMAX];
    char chPath[MY_PATHMAX];
    char *pStr;

    if (path == NULL) return;

    strcpy (chPath, path);
    //strtrimall (chPath);
    if ( strlen (chPath) == 0 ) return;

    if (strcmp (chPath, "/") != 0 &&  chPath[strlen(chPath)-1] == '/') {
        chPath [strlen (chPath) - 1] = 0;
    }

    hCtrlWnd = GetDlgItem (hWnd, IDC_PATH);
    SendMessage (hCtrlWnd, CB_RESETCONTENT, 0, 0);
    SendMessage (hCtrlWnd, CB_SETITEMHEIGHT, 0, (LPARAM)GetSysCharHeight()+2);
    
    strcpy(chSubPath, "/");
    SendMessage (hCtrlWnd, CB_ADDSTRING, 0,(LPARAM)chSubPath);
    
    pStr = strchr(chPath + 1, '/');
    while (pStr != NULL){
        memset (chSubPath, 0, sizeof (chSubPath));
        strncpy (chSubPath, chPath, pStr -chPath);
        SendMessage (hCtrlWnd, CB_INSERTSTRING, 0,(LPARAM)chSubPath);
        pStr = strchr (chPath + (pStr -chPath +1), '/');
    }
    
    if (strcmp (chPath, "/") != 0 ){
        SendMessage (hCtrlWnd, CB_INSERTSTRING, 0,(LPARAM)chPath);
    }

    SetWindowText (hCtrlWnd, chPath);            
}

static void InitFilterCombo( HWND hWnd, char * filtstr, int _index)
{
    HWND hCtrlWnd;
    char chFilter[MAX_FILTER_LEN+1];
    char chTemp[MAX_FILTER_LEN+1];
    char *p1=NULL;
    char *p2=NULL;
    int  nCount;

    if ( filtstr ==NULL ) return;
    
    strcpy (chFilter, filtstr);    
    //strtrimall (chFilter);
    if ( strlen(chFilter) == 0 ) return;
     
    
    hCtrlWnd = GetDlgItem (hWnd, IDC_FILETYPE);
    SendMessage (hCtrlWnd, CB_RESETCONTENT, 0, 0);
    SendMessage (hCtrlWnd, CB_SETITEMHEIGHT, 0, (LPARAM)GetSysCharHeight()+2);

    p1 = strchr (chFilter, '|');
    p2 = chFilter;
    while ( p1 != NULL ) {
        memset (chTemp, 0, sizeof (chTemp));
        strncpy (chTemp, p2, p1 - p2);
        SendMessage (hCtrlWnd, CB_ADDSTRING, 0,(LPARAM)chTemp);
        p2 = p1 + 1;
        p1 = strchr (p2, '|');
    }
   
    memset (chTemp, 0, sizeof(chTemp));
    strcpy (chTemp, p2);
    SendMessage (hCtrlWnd, CB_ADDSTRING, 0,(LPARAM)chTemp);
    nCount = SendMessage (hCtrlWnd, CB_GETCOUNT, 0, 0);
    
    if (_index >=nCount)
        SendMessage (hCtrlWnd, CB_SETCURSEL, nCount - 1, 0);
    else
        SendMessage (hCtrlWnd, CB_SETCURSEL, _index, 0);

}

static void InitOpenDialog( HWND hWnd, PNEWFILEDLGDATA pnfdd)
{
    HWND hCtrlWnd;
    char chFilter[MAX_FILTER_LEN+1];

    InitPathCombo (hWnd, pnfdd->filepath);
    InitFilterCombo (hWnd, pnfdd->filter, pnfdd->filterindex); 
    InitListView (hWnd);
    
    hCtrlWnd = GetDlgItem (hWnd, IDC_FILETYPE);
    memset (chFilter, 0, sizeof(chFilter));
    GetWindowText (hCtrlWnd, chFilter, MAX_FILTER_LEN);    
    //if ( GetAccessMode (hWnd, pnfdd->filepath, pnfdd->IsSave, TRUE)==0)
    GetFileAndDirList (hWnd, pnfdd->filepath, chFilter);

    hCtrlWnd = GetDlgItem (hWnd, IDC_FILENAME);
    SetWindowText (hCtrlWnd, pnfdd->filename);
    //hCtrlWnd = GetDlgItem (hWnd, IDC_FILECHOISE);
    //SendMessage (hCtrlWnd, LVM_COLSORT, (WPARAM)1, 0);
}

static LRESULT WinFileProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    HWND hCtrlWnd;
    int  nId, nNc;
    char chPath[MY_PATHMAX+1];
    char chFilter[MAX_FILTER_LEN+1];
    char chFileName[MY_NAMEMAX+1];
    char chFullName[MY_PATHMAX+MY_NAMEMAX+1];
    HLVITEM hSelItem;
    int  nIsDir;
    LVSUBITEM subItem;

    switch (message) {
    case MSG_INITDIALOG:
    {
        PNEWFILEDLGDATA pnfdd = (PNEWFILEDLGDATA)lParam;
        /* get current directory name */
        if (strcmp (pnfdd->filepath, ".") == 0 ||
                strcmp (pnfdd->filepath, "./") == 0) {
            if (getcwd (pnfdd->filepath, MY_PATHMAX) == NULL) {
                return 0;
            }
        }

        SetWindowAdditionalData (hDlg, (DWORD)lParam);
        
        InitOpenDialog (hDlg, pnfdd);        
        
        if (pnfdd->IsSave) {
            hCtrlWnd = GetDlgItem (hDlg, IDC_OKAY); 
            SetWindowText (hCtrlWnd, GetSysText (IDS_MGST_SAVE));
        }
 
        return 1;
    }
    break;
    case MSG_CLOSE:
        EndDialog (hDlg, IDCANCEL);
        break;
    case MSG_SHOWWINDOW:
        SetFocus (GetDlgItem (hDlg,IDC_FILECHOISE));
        break;
    case MSG_KEYDOWN:
        if (wParam == SCANCODE_KEYPADENTER)
            SendNotifyMessage (hDlg, MSG_COMMAND, IDC_OKAY, 0);
        else if (wParam == SCANCODE_ESCAPE)
            SendNotifyMessage (hDlg, MSG_COMMAND, IDC_CANCEL, 0);
        break;
    case MSG_COMMAND:
    {  
        PNEWFILEDLGDATA pnfdd = (PNEWFILEDLGDATA) GetWindowAdditionalData (hDlg);
        
        hCtrlWnd = GetDlgItem (hDlg, IDC_FILENAME);
        memset (chFileName, 0, sizeof(chFileName));
        GetWindowText (hCtrlWnd, chFileName, MY_NAMEMAX);
        hCtrlWnd = GetDlgItem (hDlg, IDC_PATH);
        memset (chPath, 0, sizeof(chPath));
        GetWindowText (hCtrlWnd, chPath, MY_PATHMAX);
        hCtrlWnd = GetDlgItem (hDlg, IDC_FILETYPE);
        memset (chFilter, 0, sizeof(chFilter));
        GetWindowText (hCtrlWnd, chFilter, MAX_FILTER_LEN);
 
        nId = LOWORD(wParam);
        nNc = HIWORD(wParam);

        hCtrlWnd = GetDlgItem (hDlg, nId);
 
        switch (nId) {
        case IDC_PATH:
        case IDC_FILETYPE:
        case IDC_ISHIDE:
            if (nNc == CBN_SELCHANGE || nNc == BN_CLICKED){
                GetFileAndDirList (hDlg, chPath, chFilter);
            }
            break;
        case IDC_UP:
            if (nNc == BN_CLICKED) {
                GetParentDir (chPath);
                hCtrlWnd = GetDlgItem (hDlg, IDC_PATH);
                SetWindowText (hCtrlWnd, chPath); 
                GetFileAndDirList (hDlg, chPath, chFilter);
            } 
            break;
        case IDC_OKAY:
            {
                hCtrlWnd = GetDlgItem (hDlg, IDC_FILECHOISE);
                hSelItem = (HLVITEM)SendMessage (hCtrlWnd, LVM_GETSELECTEDITEM, 0, 0);
                if (hSelItem) {
                    nIsDir = SendMessage (hCtrlWnd, LVM_GETITEMADDDATA, 0, (LPARAM)hSelItem);
                    if (nIsDir == 1 ) {
                        memset (&subItem, 0, sizeof (subItem));
                        subItem.subItem = 0;
                        subItem.pszText = (char *)malloc (MY_NAMEMAX+1);
                        if (subItem.pszText == NULL) 
                            break;
                        SendMessage (hCtrlWnd, LVM_GETSUBITEMTEXT, 
                                (WPARAM)hSelItem, (LPARAM)&subItem);
                        sprintf (chPath, "%s/%s", 
                            strcmp (chPath, "/") == 0 ? "" : chPath, subItem.pszText);
                        hCtrlWnd = GetDlgItem (hDlg, IDC_PATH);
                        if (GetAccessMode (hDlg, chPath, 0, TRUE) == 0){
                            GetFileAndDirList (hDlg, chPath, chFilter);
                            SetWindowText (hCtrlWnd, chPath);
                            if (CB_ERR == SendMessage (hCtrlWnd, CB_FINDSTRINGEXACT, 0,(LPARAM)chPath))
                                SendMessage (hCtrlWnd, 
                                    CB_INSERTSTRING, 0,(LPARAM)chPath);
                        }

                        if (subItem.pszText != NULL ) free (subItem.pszText);
                        break;
                    }
                } 
                if (strlen (chFileName) != 0) {
                    memset (chFullName, 0, sizeof(chFullName));
                    sprintf (chFullName, "%s/%s", 
                            strcmp (chPath, "/") == 0 ? "" : chPath, chFileName);
                    if (!pnfdd->IsSave 
                            || (pnfdd->IsSave 
                                && FileExistDlg (hDlg, chFullName, chFileName) == 0))
                    {
                        strcpy (pnfdd->filefullname, chFullName);
                        strcpy (pnfdd->filename, chFileName);
                        strcpy (pnfdd->filepath, chPath);
                        EndDialog (hDlg, IDOK);
                    }
                }
            } 
            break;
        case IDC_CANCEL:
            strcpy (pnfdd->filefullname, "");
            strcpy (pnfdd->filename, "");
            EndDialog (hDlg, IDCANCEL);

            break;
        case IDC_FILECHOISE:
            if (nNc == LVN_SELCHANGE || nNc == LVN_ITEMDBCLK || nNc == LVN_ITEMCLK || nNc == LVN_CLICKED) {
                hSelItem = (HLVITEM)SendMessage (hCtrlWnd, LVM_GETSELECTEDITEM, 0, 0);
                if (hSelItem) {
                    nIsDir = SendMessage (hCtrlWnd, LVM_GETITEMADDDATA, 0, (LPARAM)hSelItem);    
                    memset (&subItem, 0, sizeof (subItem));
                    subItem.subItem = 0;
                    subItem.pszText = (char *)calloc (MY_NAMEMAX + 1, 1);
                    if (subItem.pszText == NULL) 
                        break;
                    SendMessage (hCtrlWnd, LVM_GETSUBITEMTEXT,  (WPARAM)hSelItem, (LPARAM)&subItem);

                    hCtrlWnd = GetDlgItem (hDlg, IDC_FILENAME);
                    if (nIsDir == 0){
                        SetWindowText (hCtrlWnd, subItem.pszText);
                    }

                    if ((nIsDir == 0 && nNc == LVN_ITEMDBCLK) || (nIsDir == 1 && nNc != LVN_SELCHANGE)) 
                        SendNotifyMessage (hDlg, MSG_COMMAND, IDC_OKAY, 0);

                    if (subItem.pszText != NULL ) free (subItem.pszText); 
                }          
            }
 
            break;
        } /*switch (nId) end */

        break;
    }

    } /* switch (message) */
    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

int ShowOpenDialog (HWND hWnd, int lx, int ty, int w, int h, PNEWFILEDLGDATA pnfdd)
{
    int nMinW = w;
    int nMinH = h;
    int totalW = nMinW-4*SPACE_WIDTH;
    int nCharW = GetSysCCharWidth();
    int nCharH = GetSysCharHeight() +2;
    int nRet;

    if (!InitNewOpenFileBox ())
        return -1;

    CTRLDATA WinFileCtrl [] =
    { 
        { "static", WS_VISIBLE |SS_LEFT,
            SPACE_WIDTH, SPACE_HEIGHT, 4*nCharW, nCharH,
            IDC_STATIC1, GetSysText(IDS_MGST_LOCATION),  0, WS_EX_TRANSPARENT | WS_EX_USEPARENTRDR },

        { "combobox", WS_VISIBLE |WS_TABSTOP |CBS_DROPDOWNLIST |CBS_NOTIFY 
            |CBS_READONLY | WS_BORDER | CBS_EDITNOBORDER,
            2*SPACE_WIDTH+4*nCharW, SPACE_HEIGHT -5, totalW-7*nCharW-5, 
            nCharH+10, IDC_PATH, NULL, 120, WS_EX_USEPARENTRDR },
        
        { "button", WS_VISIBLE |WS_TABSTOP ,
            nMinW-SPACE_WIDTH-3*nCharW-5, SPACE_HEIGHT -5, 
            3*nCharW+2*BTN_WIDTH_BORDER,  nCharH+2*BTN_WIDTH_BORDER, 
            IDC_UP, GetSysText(IDS_MGST_UP), 0, WS_EX_USEPARENTRDR },

        { "listview", WS_VISIBLE |WS_CHILD |WS_TABSTOP |WS_BORDER |WS_VSCROLL 
            |WS_HSCROLL |LVS_SORT |LVS_NOTIFY,
            SPACE_WIDTH, 2*SPACE_HEIGHT+nCharH, nMinW-2*SPACE_WIDTH, 
            nMinH-8*SPACE_HEIGHT-4*nCharH-24,
            IDC_FILECHOISE, "file list", 0, WS_EX_USEPARENTRDR },

        { "static", WS_VISIBLE |SS_LEFT,
            SPACE_WIDTH, nMinH-5*SPACE_HEIGHT-4*nCharH, 4*nCharW, nCharH,
            IDC_STATIC2, GetSysText(IDS_MGST_FILENAME),  0, WS_EX_TRANSPARENT | WS_EX_USEPARENTRDR },

        { "sledit", WS_VISIBLE |WS_TABSTOP |WS_BORDER,
            2*SPACE_WIDTH+4*nCharW,  nMinH-5*SPACE_HEIGHT-4*nCharH -3, 
            totalW-4*nCharW+SPACE_WIDTH, nCharH+6,
            IDC_FILENAME, NULL, 0, WS_EX_USEPARENTRDR },
       

        { "static", WS_VISIBLE |SS_LEFT,
            SPACE_WIDTH, nMinH-4*SPACE_HEIGHT-3*nCharH, 4*nCharW, nCharH,
            IDC_STATIC3, GetSysText(IDS_MGST_FILETYPE),  0, WS_EX_TRANSPARENT | WS_EX_USEPARENTRDR },

        { "combobox", WS_VISIBLE |WS_TABSTOP |CBS_DROPDOWNLIST |CBS_NOTIFY 
            |CBS_READONLY | WS_BORDER | CBS_EDITNOBORDER,
            2*SPACE_WIDTH+4*nCharW,  nMinH-4*SPACE_HEIGHT-3*nCharH -3, 
            totalW-4*nCharW+SPACE_WIDTH, nCharH+6,
            IDC_FILETYPE, NULL, 120, WS_EX_USEPARENTRDR },

        { "button", WS_VISIBLE |WS_TABSTOP |BS_AUTOCHECKBOX,
            SPACE_WIDTH,   nMinH-3*SPACE_HEIGHT-2*nCharH, 8*nCharW,  nCharH+6,
            IDC_ISHIDE, GetSysText(IDS_MGST_SHOWHIDEFILE), 0, WS_EX_TRANSPARENT | WS_EX_USEPARENTRDR },
 
        { "button", WS_VISIBLE |WS_TABSTOP ,
            nMinW-2*SPACE_WIDTH-8*nCharW,  nMinH-3*SPACE_HEIGHT-2*nCharH, 4*nCharW,  
            nCharH+2*BTN_WIDTH_BORDER, IDC_OKAY, GetSysText(IDS_MGST_OPEN), 0, WS_EX_USEPARENTRDR },

        
        { "button", WS_VISIBLE |WS_TABSTOP ,
            nMinW-SPACE_WIDTH-4*nCharW,  nMinH-3*SPACE_HEIGHT-2*nCharH, 4*nCharW,  
            nCharH+2*BTN_WIDTH_BORDER, IDC_CANCEL, GetSysText(IDS_MGST_CANCEL), 0, WS_EX_USEPARENTRDR }

    };

    CTRLDATA WinFileCtrl_1 [] =
    {
        { "static", WS_VISIBLE |SS_LEFT,
            SPACE_WIDTH, SPACE_HEIGHT, 4*nCharW, nCharH,
            IDC_STATIC1, GetSysText(IDS_MGST_LOCATION),  0, WS_EX_TRANSPARENT | WS_EX_USEPARENTRDR },

        { "combobox", WS_VISIBLE |WS_TABSTOP |CBS_DROPDOWNLIST |CBS_NOTIFY 
            |CBS_READONLY | WS_BORDER | CBS_EDITNOBORDER,
            2*SPACE_WIDTH+4*nCharW, SPACE_HEIGHT -3, totalW-14*nCharW-5, nCharH+6,
            IDC_PATH, NULL, 120, WS_EX_USEPARENTRDR },

        { "button", WS_VISIBLE |WS_TABSTOP ,
            nMinW-2*SPACE_WIDTH-10*nCharW-5, SPACE_HEIGHT -3, 3*nCharW+5,  
            nCharH+2*BTN_WIDTH_BORDER, IDC_UP, GetSysText(IDS_MGST_UP), 0, WS_EX_USEPARENTRDR },
        
        { "button", WS_VISIBLE |WS_TABSTOP |BS_AUTOCHECKBOX,
            nMinW-SPACE_WIDTH-7*nCharW,   SPACE_HEIGHT, 8*nCharW,  nCharH+6,
            IDC_ISHIDE, GetSysText(IDS_MGST_SHOWHIDEFILE), 0, WS_EX_TRANSPARENT | WS_EX_USEPARENTRDR },


        { "listview", WS_VISIBLE |WS_CHILD |WS_TABSTOP |WS_BORDER |WS_VSCROLL 
            |WS_HSCROLL |LVS_SORT |LVS_NOTIFY,
            SPACE_WIDTH, 2*SPACE_HEIGHT+nCharH, nMinW-2*SPACE_WIDTH, 
            nMinH-6*SPACE_HEIGHT-3*nCharH-28, IDC_FILECHOISE, "file list", 0, WS_EX_USEPARENTRDR },

        { "static", WS_VISIBLE |SS_LEFT,
            SPACE_WIDTH, nMinH-4*SPACE_HEIGHT-3*nCharH, 4*nCharW, nCharH,
            IDC_STATIC2, GetSysText(IDS_MGST_FILENAME),  0, WS_EX_TRANSPARENT | WS_EX_USEPARENTRDR },

        { "sledit", WS_VISIBLE |WS_TABSTOP |WS_BORDER,
            2*SPACE_WIDTH+4*nCharW,  nMinH-4*SPACE_HEIGHT-3*nCharH -3, 
            totalW-9*nCharW+SPACE_WIDTH, nCharH+6, IDC_FILENAME, NULL, 0, WS_EX_USEPARENTRDR },

        { "static", WS_VISIBLE |SS_LEFT,
            SPACE_WIDTH, nMinH-3*SPACE_HEIGHT-2*nCharH, 4*nCharW, nCharH,
            IDC_STATIC3, GetSysText(IDS_MGST_FILETYPE),  0, WS_EX_TRANSPARENT | WS_EX_USEPARENTRDR },

        { "combobox", WS_VISIBLE |WS_TABSTOP |CBS_DROPDOWNLIST |CBS_NOTIFY 
            |CBS_READONLY | WS_BORDER | CBS_EDITNOBORDER, 
            2*SPACE_WIDTH+4*nCharW,  nMinH-3*SPACE_HEIGHT-2*nCharH -3, 
            totalW-9*nCharW+SPACE_WIDTH, nCharH+6, IDC_FILETYPE, NULL, 120, WS_EX_USEPARENTRDR },

        { "button", WS_VISIBLE |WS_TABSTOP ,
            nMinW-SPACE_WIDTH-5*nCharW,  nMinH-4*SPACE_HEIGHT-3*nCharH-9,  
            5*nCharW,  nCharH+2*BTN_WIDTH_BORDER, IDC_OKAY, GetSysText(IDS_MGST_OPEN), 0, WS_EX_USEPARENTRDR },

        { "button", WS_VISIBLE |WS_TABSTOP ,
            nMinW-SPACE_WIDTH-5*nCharW,  nMinH-3*SPACE_HEIGHT-2*nCharH-6, 
            5*nCharW,  nCharH+2*BTN_WIDTH_BORDER, IDC_CANCEL, GetSysText(IDS_MGST_CANCEL), 0, WS_EX_USEPARENTRDR }
    };

    DLGTEMPLATE WinFileDlg = {
        WS_BORDER | WS_CAPTION, WS_EX_USEPARENTRDR,
        lx, ty, nMinW, nMinH, NULL, 0, 0, 11, NULL };

    if (access (pnfdd->filepath, F_OK) == -1) 
        strcpy (pnfdd->filepath, "./");             

    WinFileDlg.caption = 
        (pnfdd->IsSave) ? GetSysText (IDS_MGST_SAVEFILE) : GetSysText (IDS_MGST_OPENFILE);

    if (nMinH <=220 )
        WinFileDlg.controls = WinFileCtrl_1;
    else
        WinFileDlg.controls = WinFileCtrl;
    
    if (pnfdd->IsTrans == TRUE)
    {
        int i;
        for (i = 0; i < WinFileDlg.controlnr; i++)
            WinFileDlg.controls[i].dwExStyle |= WS_EX_TRANSPARENT;
    }
    
    nRet = DialogBoxIndirectParam (&WinFileDlg, 
            hWnd, WinFileProc, (LPARAM)(pnfdd));

    NewOpenFileBoxCleanup ();
    return nRet;
}

#endif /* _MGUTILS_DLGOPENFILE */

#endif /* !defined(__NOUNIX__) && defined(_MGCTRL_LISTVIEW) */

