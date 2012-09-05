#include <windows.h>
#include <tchar.h>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include "resource1.h"
#include "Lilycwebpage.h"
#include "mshtmcid.h" //For DoPageAction WEBPAGE_FIND

#pragma once

#if defined(_M_IA64)
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='IA64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#if defined(_M_X64)
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.6000.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#if defined(_M_IX86)
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#define WEBPAGE_KEYSTROKE 10000
#define WEBPAGE_FIND      10001

BOOL CALLBACK WebEnumChildProc(HWND hWnd, LPARAM lParam);
BOOL DoPageAction(IWebBrowser2* p_IWebBrowser2, WPARAM wParam, LPMSG lpmsg);
BOOL LookUpGooDic(HWND Googlehwnd, const TCHAR* szBuffer, TCHAR* http, int httpSize, int szblength);
std::string ConvertMonthToString(int Month);
BOOL InsideRect(RECT, POINT);
BOOL CheckRepititiveWord(const char*, std::vector<std::string>);