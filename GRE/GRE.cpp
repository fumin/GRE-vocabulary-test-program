#include "HelperFunctions.h"

EmbedBrowserObjectPtr		*lpEmbedBrowserObject;
UnEmbedBrowserObjectPtr		*lpUnEmbedBrowserObject;
DisplayHTMLPagePtr			*lpDisplayHTMLPage;
DisplayHTMLStrPtr			*lpDisplayHTMLStr;
ResizeBrowserPtr			*lpResizeBrowser;
GetWebPtrsPtr               *lpGetWebPtrs;

LRESULT CALLBACK WndProc     (HWND, UINT, WPARAM, LPARAM) ;
LRESULT CALLBACK MWndProc    (HWND, UINT, WPARAM, LPARAM) ;
BOOL    CALLBACK NewWordDlgProc(HWND, UINT, WPARAM, LPARAM) ;
BOOL    CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM) ;
BOOL    CALLBACK RecordDlgProc(HWND, UINT, WPARAM, LPARAM) ;
typedef LRESULT CALLBACK OriListBoxProcPtr(HWND, UINT, WPARAM, LPARAM);
OriListBoxProcPtr* OriListBoxProc;
TCHAR GoogleWord[20];
int* ptr_ThereIsCopyPopMenu;

HWND         Googlehwnd, Mhwnd, hDlgModeless, hDlgRecord, WBhwnd = 0, ListBoxhwnd;
HMENU hMenu;
bool         HollyFirst = 0;
std::vector<std::string> VocList, ListBoxVec;
IWebBrowser2 * p_IWebBrowser2;
HINSTANCE hInst;

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hInstNULL, LPSTR lpszCmdLine, int nCmdShow)
{
	HINSTANCE		Lilycwebdll;
	MSG				msg;
	WNDCLASSEX		wc, Mwc;

	static std::string str;
	std::ifstream file ("GRE.txt");
	if(file.is_open()){
		while (! file.eof() ){
			std::getline(file, str);
			VocList.push_back(str);
		}
		file.close();
	}
	else{
		MessageBox(0, TEXT("Can't open GRE.txt"), TEXT("Error"), 0);
		return -1;
	}

	ZeroMemory(&Mwc, sizeof(WNDCLASSEX));
	Mwc.cbSize = sizeof(WNDCLASSEX);
	Mwc.hInstance = hInstance;
	Mwc.lpfnWndProc = MWndProc;
	Mwc.lpszClassName = TEXT("Mother");
	Mwc.hIcon = LoadIcon (hInstance, MAKEINTRESOURCE (IDI_ICON1)) ;
	Mwc.lpszMenuName = TEXT("GRE");
	RegisterClassEx(&Mwc);

	// Create another window with another browser object embedded in it.
	Mhwnd = CreateWindowEx(0, TEXT("Mother"), TEXT("Lily GRE Vocabulary Suit"), 
			               WS_OVERLAPPEDWINDOW, 
						   CW_USEDEFAULT, CW_USEDEFAULT, 
						   CW_USEDEFAULT, CW_USEDEFAULT, 
						   NULL, NULL, hInstance, 0);
	ShowWindow(Mhwnd, nCmdShow);
	UpdateWindow(Mhwnd);

	// Load our DLL containing the OLE/COM code. We do this once-only. It's named "cwebpage.dll"
	if (Lilycwebdll = LoadLibrary(TEXT("Lilycwebpage.dll")))
	{
		// Get pointers to the EmbedBrowserObject, DisplayHTMLPage, DisplayHTMLStr, and UnEmbedBrowserObject
		// functions, and store them in some globals.

		// Get the address of the EmbedBrowserObject() function. NOTE: Only Reginald has this one
		lpEmbedBrowserObject = (EmbedBrowserObjectPtr *)GetProcAddress((HINSTANCE)Lilycwebdll, EMBEDBROWSEROBJECTNAME);

		// Get the address of the UnEmbedBrowserObject() function. NOTE: Only Reginald has this one
		lpUnEmbedBrowserObject = (UnEmbedBrowserObjectPtr *)GetProcAddress((HINSTANCE)Lilycwebdll, UNEMBEDBROWSEROBJECTNAME);

		// Get the address of the DisplayHTMLPagePtr() function
		lpDisplayHTMLPage = (DisplayHTMLStrPtr *)GetProcAddress((HINSTANCE)Lilycwebdll, DISPLAYHTMLPAGENAME);

		// Get the address of the DisplayHTMLStr() function
		lpDisplayHTMLStr = (DisplayHTMLStrPtr *)GetProcAddress((HINSTANCE)Lilycwebdll, DISPLAYHTMLSTRNAME);

		// Get the address of the ResizeBrowser() function
		lpResizeBrowser = (ResizeBrowserPtr *)GetProcAddress((HINSTANCE)Lilycwebdll, RESIZEBROWSERNAME);

		lpGetWebPtrs = (GetWebPtrsPtr *)GetProcAddress((HINSTANCE)Lilycwebdll, GETWEBPTRSNAME);

		// Register the class of our window to host the browser. 'WindowProc' is our message handler
		// and 'ClassName' is the class name. You can choose any class name you want.
		ZeroMemory(&wc, sizeof(WNDCLASSEX));
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.hInstance = hInstance;
		wc.lpfnWndProc = WndProc;
		wc.lpszClassName = TEXT("Googledic");
		RegisterClassEx(&wc);

		// Create another window with another browser object embedded in it.
		Googlehwnd = CreateWindowEx(0, TEXT("Googledic"), TEXT("Google Dictionary"), 
			                             WS_OVERLAPPED | WS_CHILD, 
										 0, 0, 
										 CW_USEDEFAULT, CW_USEDEFAULT, 
										 Mhwnd, (HMENU) 10, hInstance, 0);
		// For this window, display a URL. This could also be a HTML file on disk such as "c:\\myfile.htm".
		(*lpDisplayHTMLPage)(Googlehwnd, TEXT("http://www.google.com.tw/dictionary"));
		ShowWindow(Googlehwnd, SW_MAXIMIZE);
		UpdateWindow(Googlehwnd);
	}
	else{
		MessageBox(0, TEXT("Can't open Lilycwebpage.dll!"), TEXT("ERROR"), MB_OK);
	    return(-1);
	}

	HACCEL hAccel ;
	hAccel = LoadAccelerators (hInstance, TEXT("GRE_ACCELERATOR")) ;

	while (GetMessage(&msg, 0, 0, 0))
	{
		if(hDlgModeless == 0 || !IsDialogMessage (hDlgModeless, &msg) ){
			if( hDlgRecord == 0 || !IsDialogMessage(hDlgRecord, &msg) ){
			if(WBhwnd == 0)//Don't know where to get the WBhwnd handle, so put it here
				EnumChildWindows(Googlehwnd, WebEnumChildProc, (LPARAM) &WBhwnd);

			//Support for google suggest
			if(msg.hwnd == WBhwnd && msg.message >= WM_KEYDOWN && msg.message <= WM_KEYLAST && DoPageAction(p_IWebBrowser2, WEBPAGE_KEYSTROKE, &msg) == 0)
				continue;

			//Normal message loop
			if (!TranslateAccelerator (Mhwnd, hAccel, &msg)){
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			}}
	}

	// Free the DLL.
	FreeLibrary(Lilycwebdll);

	if(!ListBoxVec.empty()){
			SYSTEMTIME SystemTime; GetSystemTime(&SystemTime);
			std::stringstream ThisInstFileName;
			ThisInstFileName << SystemTime.wYear; ThisInstFileName << ' ';
			ThisInstFileName << ConvertMonthToString(SystemTime.wMonth); ThisInstFileName << ' ';
			ThisInstFileName << SystemTime.wDay; ThisInstFileName << ' ';
			ThisInstFileName << SystemTime.wHour; ThisInstFileName << '.';
			ThisInstFileName << SystemTime.wMinute; ThisInstFileName << '.';
			ThisInstFileName << SystemTime.wSecond; ThisInstFileName << ".txt";
			std::ofstream ThisInstFile(ThisInstFileName.str());
			int ListBoxVecSize = ListBoxVec.size();
			for(int i = 0; i != ListBoxVecSize; ++i){
				ThisInstFile << ListBoxVec[i];
				ThisInstFile << '\n';
			}
			ThisInstFile.close();
		}

	return(0);
}

LRESULT CALLBACK MWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
	switch (uMsg){
	case WM_CREATE:
		hInst = ((LPCREATESTRUCT) lParam)->hInstance ;
        return 0 ;
	case WM_COMMAND:
		hMenu = GetMenu (hwnd) ;
		switch (LOWORD (wParam))
          {
		  case ACCELERATOR_COPY:
			   p_IWebBrowser2->ExecWB(OLECMDID_COPY, OLECMDEXECOPT_DONTPROMPTUSER, NULL, NULL);
			   return 0;
		  case ACCELERATOR_PASTE:
			   p_IWebBrowser2->ExecWB(OLECMDID_PASTE, OLECMDEXECOPT_DONTPROMPTUSER, NULL, NULL);
			   return 0;
		  case ACCELERATOR_DELETE:
			   p_IWebBrowser2->ExecWB(OLECMDID_DELETE, OLECMDEXECOPT_DONTPROMPTUSER, NULL, NULL);
			   return 0;
		  case ACCELERATOR_SELECTALL:
			   p_IWebBrowser2->ExecWB(OLECMDID_SELECTALL, OLECMDEXECOPT_DONTPROMPTUSER, NULL, NULL);
			   return 0;
		  case ACCELERATOR_FIND:
			   MSG tempmsg;
			   return DoPageAction(p_IWebBrowser2, WEBPAGE_FIND, &tempmsg);
          case 65535:
			   if(hDlgModeless == 0){
				   hDlgModeless = CreateDialog (hInst, TEXT ("NEWWORD"), hwnd, NewWordDlgProc);
				   EnableMenuItem (hMenu, 65535, MF_GRAYED) ;
			   }
               return 0 ;
		  case 65534:
               DialogBox (hInst, TEXT ("ABOUT"), hwnd, AboutDlgProc);
               return 0 ;
		  case 65533:
			   if(hDlgRecord ==0){
				   hDlgRecord = CreateDialog(hInst, TEXT("RECORD"), hwnd, RecordDlgProc);
				   EnableMenuItem(hMenu, 65533, MF_GRAYED);
			   }
			   return 0;
          }
	case WM_SIZE:
		MoveWindow(Googlehwnd, 0, 0, LOWORD (lParam), HIWORD (lParam), 0);
		SendMessage(Googlehwnd, WM_SYSCOMMAND, SC_MAXIMIZE, lParam);
		if(!HollyFirst){
			SendMessage(hwnd, WM_COMMAND, 65535, lParam);
		    HollyFirst = 1;
		}
		return 1;
	case WM_SETFOCUS:
		SetFocus(WBhwnd);
		return 1;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 1;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

BOOL CALLBACK NewWordDlgProc (HWND hDlg, UINT message, 
                            WPARAM wParam, LPARAM lParam)
{
     static int left, top, szblength;
	 const HWND hwndEdit = FindWindowEx(hDlg, 0, TEXT("edit"), 0);
	 static TCHAR szBuffer[50], http[100], wstr[50];
	 static char charBuffer[50];
	 HFONT hFontNew;
	 switch (message)
     {
     case WM_INITDIALOG:
		  RECT rect, drect;
		  GetWindowRect(Mhwnd, &rect); GetWindowRect(hDlg, &drect);
		  left = (rect.right - rect.left)/2 - (drect.right - drect.left)/2 + rect.left;
		  top = (rect.bottom - rect.top)/2 - (drect.bottom - drect.top)/2 + rect.top;
		  if(left > GetSystemMetrics (SM_CXSCREEN) - 100) left = GetSystemMetrics (SM_CXSCREEN) - 100;
		  if(top > GetSystemMetrics (SM_CYSCREEN) - 100) top = GetSystemMetrics (SM_CYSCREEN) - 100;
		  SetWindowPos(hDlg, 0, left, top, 161, 87, SWP_NOSIZE);
		  PostMessage (hDlg, WM_NEXTDLGCTL, (WPARAM)hwndEdit, TRUE);

		  hFontNew = CreateFont(36,0,0,0,FW_DONTCARE,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_OUTLINE_PRECIS,
                                CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY, VARIABLE_PITCH,TEXT("MS Shell Dlg"));
          SendMessage (hwndEdit, WM_SETFONT, (WPARAM) hFontNew, 0) ;
		  SendMessage (hDlg, WM_COMMAND, (WPARAM) IDC_BUTTON1, 0) ;
          return FALSE ;
	 case WM_ACTIVATE:
		  if(wParam != WA_INACTIVE){
			  POINT point;
			  RECT New_Word_rect, GRect;
		      GetWindowRect(FindWindowEx(hDlg, 0, TEXT("button"), TEXT("New Word")), &New_Word_rect);
			  GetWindowRect(FindWindowEx(hDlg, 0, TEXT("button"), TEXT("Google Dictionary")), &GRect);
			  GetCursorPos(&point);
			  if(InsideRect(New_Word_rect, point) || InsideRect(GRect, point))
				  return FALSE;
			  else
				  PostMessage (hDlg, WM_NEXTDLGCTL, (WPARAM)hwndEdit, TRUE);
		  }
		  return TRUE;
     case WM_COMMAND:
          switch (LOWORD (wParam))
          {
          case IDOK:
			   ZeroMemory(szBuffer, 50); 
			   *(WORD*) szBuffer = 50 ;
			   szblength = SendMessage (hwndEdit, EM_GETLINE, 0, (LPARAM) szBuffer) ;
			   //http://www.google.com.tw/dictionary?langpair=en%7Cen&q=apprehend&hl=en&aq=f
               LookUpGooDic(Googlehwnd, szBuffer, http, 100, szblength);
			   (*lpDisplayHTMLPage)(Googlehwnd, http);
			   
			   ZeroMemory(szBuffer + szblength, 50 - szblength);
			   WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS | WC_COMPOSITECHECK | WC_DEFAULTCHAR, 
				                   szBuffer, -1, charBuffer, 50, 0, 0);
			   if(!CheckRepititiveWord(charBuffer, ListBoxVec)){
				   ListBoxVec.push_back(charBuffer);
				   if(ListBoxhwnd){
					   SendMessage (ListBoxhwnd, LB_INSERTSTRING, -1, (LPARAM) szBuffer) ;
				   }
			   }
               return TRUE ;
               
          case IDCANCEL:
			   EnableMenuItem (hMenu, 65535, MF_ENABLED) ;
			   DestroyWindow (hDlg) ;
               hDlgModeless = NULL ;
               return TRUE ;

		  case IDC_BUTTON1:
			   SYSTEMTIME SystemTime; FILETIME FileTime;
			   GetSystemTime(&SystemTime); SystemTimeToFileTime(&SystemTime, &FileTime);
			   srand(MAKELONG(FileTime.dwLowDateTime, FileTime.dwHighDateTime));
			   SendMessage (hwndEdit, EM_SETSEL, 0, -1) ;
			   MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, VocList[rand()%VocList.size()].c_str(), -1, wstr, 50);
			   SendMessage (hwndEdit, EM_REPLACESEL, 0, (LPARAM) wstr) ;
               return TRUE;
          case IDC_BUTTON2:
			   SendMessage(hDlg, WM_COMMAND, IDOK, lParam);
               return TRUE ;
          }
          break ;
     }
     return FALSE ;
}

LRESULT CALLBACK ListBoxWndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	static int ThereIsCopyPopMenu = 0;
	ptr_ThereIsCopyPopMenu = &ThereIsCopyPopMenu;
	int a = 0;
	if(message != WM_GETDLGCODE && wParam != 0&&message!=160&&message!=133&&message!=20&&message!=641&&message!=7&&message!=32&&message!=642){
		int a = 0;
	}
	static HMENU hListBoxMenu ;
	static POINT point;

	switch(message){
	case WM_GETDLGCODE:
		switch(wParam){
			case 0x03/*ctrl C*/:
				SendMessage(hDlgRecord, WM_CHAR, 0x03, lParam);
				return TRUE;
		}
		break;
	case WM_KILLFOCUS:
		ThereIsCopyPopMenu = 0;
		break;
	case WM_RBUTTONDOWN:
		hListBoxMenu = LoadMenu (hInst, TEXT("LISTBOXPOPUP")) ;
		hListBoxMenu = GetSubMenu (hListBoxMenu, 0) ;
		point.x = LOWORD (lParam) ;
		point.y = HIWORD (lParam) ;
		ClientToScreen (hwnd, &point) ;
		TrackPopupMenu (hListBoxMenu, TPM_RIGHTBUTTON, point.x, point.y, 0, hwnd, NULL) ;
		ThereIsCopyPopMenu = 1;
		break;
	case WM_LBUTTONDOWN:
		if(ThereIsCopyPopMenu){
			ThereIsCopyPopMenu = 0;
			return 0;
		}
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam)){
		case 65532:
			ThereIsCopyPopMenu = 0;
			SendMessage(hwnd, WM_GETDLGCODE, 0x03, lParam);
			break;
		}
		break;
	}
	return CallWindowProc((*OriListBoxProc), hwnd, message, wParam, lParam);
}

BOOL CALLBACK RecordDlgProc (HWND hDlg, UINT message, 
                            WPARAM wParam, LPARAM lParam)
{
	 static HWND OKhwnd;
	 static RECT IniRecordDlgRect, OKrect, DlgRect, Listrect;
	 static int ConstantWidth, ConstOKDistanceH, ConstListWidth, ConstListDistance, szblength;
	 static int OKwidth, SizeOfListBoxVec, iIndex, strLength, ItemsSelectedInListBox[1000], WhereArray;
	 static TCHAR RecordszBuffer[50], Recordhttp[100], RecordBigCharArray[10000];
	 static HFONT hFontNew;
	 static HGLOBAL hGlobal;
	 static LPVOID pGlobal;
	 if(message != 70&&message != 13&&message != 127&&message != 295&&message!=296&&message!=297&&message!=48&&message!=85&&message!=272&&message!=24&&message!=134&&message!=133&&message!=6&&message!=20&&message!=131&&message!=15&&message!=71&&message!=310&&message!=792&&message!=309&&message!=78&&message!=28&&message!=512&&message!=32&&message!=132&&message!=174&&message!=160&&message!=674&&message!=641&&message!=642&&message!=7&&message!=8&&message!=161&&message!=36&&message!=561&&message!=308&&message!=1024&&message!=31&&message!=533&&message!=40&&message!=273){
		int a = 0;
	 }
	 switch (message)
     {
     case WM_INITDIALOG:
		  OKhwnd = FindWindowEx(hDlg, 0, TEXT("button"), 0);
		  ListBoxhwnd = FindWindowEx(hDlg, 0, TEXT("listbox"), 0);
		  GetWindowRect(hDlg, &IniRecordDlgRect);
		  GetWindowRect(OKhwnd, &OKrect);
		  GetWindowRect(ListBoxhwnd, &Listrect);
		  ConstantWidth = IniRecordDlgRect.right - IniRecordDlgRect.left;
		  ConstListWidth = Listrect.right - Listrect.left;
		  OKwidth = OKrect.right - OKrect.left;
		  ConstOKDistanceH = IniRecordDlgRect.bottom - OKrect.top;
		  ConstListDistance = IniRecordDlgRect.bottom - Listrect.bottom;
		  SizeOfListBoxVec = ListBoxVec.size();

		  SendMessage (ListBoxhwnd, WM_SETREDRAW, FALSE, 0) ;
		  for(int i = 0; i != SizeOfListBoxVec; ++i){
			  MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, ListBoxVec[i].c_str(), -1, RecordszBuffer, 50);
			  SendMessage (ListBoxhwnd, LB_INSERTSTRING, -1, (LPARAM) RecordszBuffer) ;
		  }
		  for(int i = 0; i != iIndex; ++i){
			  SendMessage (ListBoxhwnd, LB_SETSEL, TRUE, ItemsSelectedInListBox[i]) ;
		  }
		  SendMessage (ListBoxhwnd, WM_SETREDRAW, TRUE, 0) ;

		  hFontNew = CreateFont(24,0,0,0,FW_DONTCARE,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_OUTLINE_PRECIS,
                                CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY, VARIABLE_PITCH,TEXT("MS Shell Dlg"));
          SendMessage (ListBoxhwnd, WM_SETFONT, (WPARAM) hFontNew, 0) ;
		  OriListBoxProc = (OriListBoxProcPtr*)SetWindowLong(ListBoxhwnd, GWL_WNDPROC, (LONG)ListBoxWndProc); 
		  PostMessage (hDlg, WM_NEXTDLGCTL, (WPARAM)ListBoxhwnd, TRUE);
          return TRUE ;
	 case WM_CHAR:
		  switch(wParam){
		  case 0x03 /*ctrl C*/:
			  ZeroMemory(RecordszBuffer, 50);
			  *(int *) RecordszBuffer = 50 ;
			  iIndex = SendMessage (ListBoxhwnd, LB_GETSELITEMS, 1000, (LPARAM)ItemsSelectedInListBox);
			  if(iIndex != LB_ERR){
				  ZeroMemory(RecordBigCharArray, 10000);
			      WhereArray = 0;
				  for(int i = 0; i != iIndex; i++){
					  SendMessage (ListBoxhwnd, LB_GETTEXT, ItemsSelectedInListBox[i], (LPARAM) RecordszBuffer) ;
					  strLength = _tcslen(RecordszBuffer);
					  memcpy(&RecordBigCharArray[WhereArray], RecordszBuffer, sizeof(TCHAR) * strLength);
					  RecordBigCharArray[WhereArray + strLength] = TEXT('\r'); //carriage return 
					  RecordBigCharArray[WhereArray + strLength + 1] = TEXT('\n'); //newline
					  WhereArray += _tcslen(RecordszBuffer) + 2;
				  }
				  RecordBigCharArray[WhereArray] = TEXT('\0');

				  strLength =  _tcslen(RecordBigCharArray);
				  hGlobal = GlobalAlloc (GHND | GMEM_SHARE, sizeof(TCHAR) * (strLength + 1)) ;
				  pGlobal = GlobalLock (hGlobal) ;
				  memcpy(pGlobal, RecordBigCharArray, sizeof(TCHAR) * (strLength + 1));
				  GlobalUnlock (hGlobal) ;
				  OpenClipboard (ListBoxhwnd) ;
			      EmptyClipboard () ;
				  SetClipboardData (CF_UNICODETEXT, hGlobal) ;
				  CloseClipboard () ;
			  }
			  break;
		  }
		  return TRUE;
	 case WM_SYSCOMMAND:
		  switch(wParam){
		  case 0xF002://Disable Horizontal size change
			  return TRUE;
		  case 0xF008://Transfer Diagonal size change to Vertical size change
			  SendMessage(hDlg, WM_SYSCOMMAND, 0xF006, lParam);
			  return TRUE;
		  }
		  break;
	 case WM_SIZE:
		  GetWindowRect(hDlg, &DlgRect);
		  SetWindowPos(OKhwnd, HWND_TOP, LOWORD(lParam)/2 - OKwidth/2 - 0, HIWORD(lParam) - ConstOKDistanceH + 8, 0, 0, SWP_NOSIZE);
		  SetWindowPos(ListBoxhwnd, HWND_TOP, 0, 0, ConstListWidth, HIWORD(lParam) - ConstListDistance - 3, SWP_NOMOVE);
		  UpdateWindow(hDlg);
		  return FALSE;
	 case WM_RBUTTONDOWN:
	 case WM_LBUTTONDOWN:
		  *ptr_ThereIsCopyPopMenu = 0;
		  return FALSE;
     case WM_COMMAND:
          switch (LOWORD (wParam))
          {
          case IDOK:
			   for(int i = 0; i != 50; ++i)RecordszBuffer[i] = 0;
			   *(WORD *) RecordszBuffer = 50 ;
			   iIndex = SendMessage (ListBoxhwnd, LB_GETCURSEL, 0, 0);
			   if(iIndex != LB_ERR){
				   szblength = SendMessage (ListBoxhwnd, LB_GETTEXT, iIndex, (LPARAM) RecordszBuffer) ;
				   LookUpGooDic(Googlehwnd, RecordszBuffer, Recordhttp, 100, szblength);
				   (*lpDisplayHTMLPage)(Googlehwnd, Recordhttp);
			   }
               return TRUE ;
               
          case IDCANCEL:
			   iIndex = SendMessage (ListBoxhwnd, LB_GETSELITEMS, 1000, (LPARAM)ItemsSelectedInListBox);
			   EnableMenuItem (hMenu, 65533, MF_ENABLED) ;
			   DestroyWindow (hDlg) ;
               hDlgRecord = NULL ;
               return TRUE ;
		  case IDC_LIST1:
			   switch(HIWORD(wParam)){
			   case LBN_DBLCLK:
				   SendMessage(hDlg, WM_COMMAND, IDOK, lParam);
			   }
			   return TRUE;
          }
          break ;
	 }
     return FALSE ;
}

BOOL CALLBACK AboutDlgProc (HWND hDlg, UINT message, 
                            WPARAM wParam, LPARAM lParam)
{
     switch (message)
     {
     case WM_INITDIALOG:
		  SetWindowPos(hDlg, 0, GetSystemMetrics (SM_CXSCREEN) / 2 - 395/2,  GetSystemMetrics (SM_CYSCREEN) / 2 - 240/2, 262, 105, SWP_NOSIZE);
          return FALSE ;
          
     case WM_COMMAND:
          switch (LOWORD (wParam))
          {
          case IDOK:
               EndDialog (hDlg, FALSE) ;
               return TRUE ;
               
          case IDCANCEL:
               EndDialog (hDlg, FALSE) ;
               return TRUE ;
          }
          break ;
     }
     return FALSE ;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	    case WM_USER:
			(*lpDisplayHTMLPage)(hwnd, TEXT("http://www.google.com.tw/dictionary"));
			return 0;
		case WM_SIZE:
		{
			// Resize the browser object to fit the window
			(*lpResizeBrowser)(hwnd, LOWORD(lParam), HIWORD(lParam));
			if((*lpGetWebPtrs)(Googlehwnd, &p_IWebBrowser2, 0) != S_OK) return -1;
			return(0);
		}
		case WM_CREATE:
		{
			if ((*lpEmbedBrowserObject)(hwnd)) return(-1);
			return(0);
		}
		case WM_DESTROY:
		{
			// Detach the browser object from this window, and free resources.
			(*lpUnEmbedBrowserObject)(hwnd);
			PostQuitMessage(0);
			return(TRUE);
		}
	}
	return(DefWindowProc(hwnd, uMsg, wParam, lParam));
}


