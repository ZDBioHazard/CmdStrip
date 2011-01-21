/**********************************************************************
* CmdStrip! - So they locked you out of the "run box" eh?
* Copyright (C) 2005 BioHazard / Vectec Software
***********************************************************************
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 or any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
***********************************************************************
* cmdstrip.c : The whole bananna!
**********************************************************************/
#include "cmdstrip.h"

#define BAR_HEIGHT 20
#define PAD_EDIT 6

typedef struct{
	unsigned char idx;
	char **hist;
	HWND wnd,edt;
	WNDPROC prc;
}VARS;VARS vr={0};

typedef struct{
	unsigned char maxhist;
	signed short x,y,w;
}CONFIG;CONFIG cf={8,32,32,128};

LRESULT CALLBACK AboutProc(HWND dlg,UINT msg,WPARAM wp,LPARAM lp){if(msg==WM_CLOSE){EndDialog(dlg,0);}return 0;}

LRESULT CALLBACK ConfigProc(HWND dlg,UINT msg,WPARAM wp,LPARAM lp){
	switch(msg){
		case WM_INITDIALOG:
		break;
		case WM_COMMAND:switch(HIWORD(wp)){
			case BN_CLICKED:switch(LOWORD(wp)){
				case BTN_OK:
				case BTN_CANCEL:EndDialog(dlg,0);break;
			}break;
		}break;
	}return 0;
}

LRESULT CALLBACK EdtProc(HWND dlg,UINT msg,WPARAM wp,LPARAM lp){
	char *tmp;
	unsigned char i;
	switch(msg){
		case WM_KEYDOWN:switch(LOWORD(wp)){
			case VK_RETURN:if(SendMessage(vr.edt,WM_GETTEXTLENGTH,0,0)){
				tmp=calloc(SendMessage(vr.edt,WM_GETTEXTLENGTH,0,0),sizeof(char));
				SendMessage(vr.edt,WM_GETTEXT,0xFFFFFFFF,(WPARAM)tmp);
			// Update history
				if(cf.maxhist){
					vr.idx=0;
					if(vr.hist[cf.maxhist]){free(vr.hist[cf.maxhist]);} // Free the oldest entry
					for(i=cf.maxhist;i>0;i--){vr.hist[i]=vr.hist[i-1];} // Shift history
					vr.hist[0]=tmp;
					SetFocus(dlg);
				}else{free(tmp);} // Free the command if there is no history
			}break;
			case VK_UP:if(cf.maxhist&&vr.idx<cf.maxhist&&vr.hist[vr.idx+1]){
				vr.idx++;
				SendMessage(dlg,WM_SETTEXT,0,(LPARAM)vr.hist[vr.idx]);
			}return 0;
			case VK_DOWN:if(cf.maxhist&&vr.idx){
				vr.idx--;
				SendMessage(dlg,WM_SETTEXT,0,(LPARAM)vr.hist[vr.idx]);
			}return 0;
		}break;
	}return CallWindowProc(vr.prc,dlg,msg,wp,lp);
}

LRESULT CALLBACK MainProc(HWND dlg,UINT msg,WPARAM wp,LPARAM lp){
	POINT pt;
	switch(msg){
		case WM_CREATE:
			vr.edt=CreateWindow("EDIT","",WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL,PAD_EDIT,0,cf.w-(PAD_EDIT*2),BAR_HEIGHT,dlg,0,GetModuleHandle(0),0);
			vr.prc=(WNDPROC)GetWindowLong(vr.edt,GWL_WNDPROC);
			SetWindowLong(vr.edt,GWL_WNDPROC,(long)&EdtProc);
			SetFocus(vr.edt);
		break;
	// Resize
		case WM_SIZE:
			cf.w=LOWORD(lp);
			SetWindowPos(vr.edt,0,PAD_EDIT,0,cf.w-(PAD_EDIT*2),BAR_HEIGHT,SWP_NOZORDER);
		break;
		case WM_GETMINMAXINFO:
			pt.x=128;
			((MINMAXINFO*)lp)->ptMinTrackSize=pt;
		break;
	// Clicking
		case WM_LBUTTONDOWN:SendMessage(dlg,WM_NCLBUTTONDOWN,(LOWORD(lp)>cf.w-PAD_EDIT)?HTRIGHT:HTCAPTION,0);break;
		case WM_RBUTTONUP:
			GetCursorPos(&pt);
			TrackPopupMenu(GetSubMenu(LoadMenu(GetModuleHandle(0),MAKEINTRESOURCE(MNU_MENU)),0),TPM_LEFTALIGN,pt.x,pt.y,0,dlg,0);
		break;
		case WM_COMMAND:switch(HIWORD(wp)){
			case BN_CLICKED:switch(LOWORD(wp)){
				case MNU_CONFIG:DialogBox(GetModuleHandle(0),MAKEINTRESOURCE(DLG_CONFIG),dlg,&ConfigProc);break;
				case MNU_ABOUT:DialogBox(GetModuleHandle(0),MAKEINTRESOURCE(DLG_ABOUT),dlg,&AboutProc);break;
				case MNU_EXIT:SendMessage(dlg,WM_CLOSE,0,0);break;
			}break;
		}break;
	// Exiting
		case WM_CLOSE:PostQuitMessage(0);break;
	}return DefWindowProc(dlg,msg,wp,lp);
}

int WINAPI WinMain(HINSTANCE inst,HINSTANCE pinst,LPSTR cline,int cshow){
	MSG msg;
	WNDCLASS wcls={0,MainProc,0,0,inst,LoadIcon(0,MAKEINTRESOURCE(ICO_ICON)),LoadCursor(0,IDC_ARROW),(HBRUSH)(COLOR_ACTIVECAPTION+1),0,"cmdwnd"};
	RegisterClass(&wcls);
	InitCommonControls();
	// Allocate space for history
	if(cf.maxhist){vr.hist=calloc(cf.maxhist,sizeof(char*));}
	// GooooOOOOooooOOOOoo!
	vr.wnd=CreateWindowEx(WS_EX_TOOLWINDOW,"cmdwnd","CmdStrip",WS_VISIBLE|WS_POPUP,cf.x,cf.y,cf.w,BAR_HEIGHT,HWND_DESKTOP,0,inst,0);
	while(GetMessage(&msg,0,0,0)){TranslateMessage(&msg);DispatchMessage(&msg);}
	// Free settings
	free(vr.hist);
	return msg.wParam;
}
