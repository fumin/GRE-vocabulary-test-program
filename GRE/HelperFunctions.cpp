#include "HelperFunctions.h"

BOOL CALLBACK WebEnumChildProc(HWND hWnd, LPARAM lParam)
{
	TCHAR WinClass[128];
	int clen = GetClassName(hWnd, WinClass, 127);
	HWND *phWnd = (HWND *) lParam;
	WinClass[clen] = 0;

	if (_tcscmp(WinClass, TEXT("Internet Explorer_Server")) == 0) {
		*phWnd = hWnd;
		return FALSE;
	}
	return TRUE;
}

BOOL DoPageAction(IWebBrowser2* p_IWebBrowser2, WPARAM wParam, LPMSG lpmsg){
	BOOL result;

	switch(wParam){
	case WEBPAGE_FIND:
		IOleCommandTarget* pIOleComT;
		p_IWebBrowser2->QueryInterface(IID_IOleCommandTarget, (LPVOID*)&pIOleComT);
		result = pIOleComT->Exec(&CGID_MSHTML, IDM_FIND, OLECMDEXECOPT_DONTPROMPTUSER, NULL, NULL);
		pIOleComT->Release();
		break;
	case WEBPAGE_KEYSTROKE:
		IOleInPlaceActiveObject* pIOIPAO;
		if (SUCCEEDED(p_IWebBrowser2->QueryInterface(IID_IOleInPlaceActiveObject, (LPVOID*)&pIOIPAO)))
			result = pIOIPAO->TranslateAccelerator(lpmsg);
		pIOIPAO->Release();
		break;
	}
	return result;
}

BOOL LookUpGooDic(HWND Googlehwnd, const TCHAR* szBuffer, TCHAR* http, int httpSize, int szblength){
	for(int i = 0; i != httpSize; ++i) http[i] = 0;
	const TCHAR httpfront[] = TEXT("http://www.google.com.tw/dictionary?langpair=en%7Cen&q="), httpback[] = TEXT("&hl=en&aq=f");
	memcpy(&http[0],             httpfront, _tcslen(httpfront) * sizeof(TCHAR));
	memcpy(&http[_tcslen(http)], szBuffer, szblength * sizeof(TCHAR));
	memcpy(&http[_tcslen(http)], httpback, _tcslen(httpback) * sizeof(TCHAR));
	return TRUE;
}

std::string ConvertMonthToString(int Month){
	switch(Month){
	case 1: return std::string("January");
	case 2: return std::string("Fabruary");
	case 3: return std::string("March");
	case 4: return std::string("April");
	case 5: return std::string("May");
	case 6: return std::string("June");
	case 7: return std::string("July");
	case 8: return std::string("August");
	case 9: return std::string("September");
    case 10: return std::string("October");
	case 11: return std::string("November");
	case 12: return std::string("December");
	}
	return std::string("");
}

BOOL InsideRect(RECT rect, POINT p){
	if(p.x <= rect.right && p.x >= rect.left && p.y <= rect.bottom && p.y >= rect.top)return TRUE;
	else return FALSE;
}

BOOL CheckRepititiveWord(const char* p_char, std::vector<std::string> list){
	int S = list.size();
	for(int i = 0; i != S; ++i)
		if(!strcmp(p_char, list[i].c_str())) return TRUE;
	return FALSE;
}