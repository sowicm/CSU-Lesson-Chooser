// Copyright (c) 2012 sowicm

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

// If you edited this file, add your name and your email to following list,
// the above contents should NOT be modified.
//
// sowicm<sowicm@gmail.com>
//

#include <Windows.h>
#include <stdio.h>
#include <thread>
#include "windows.h"
#include "../pro/vs/resource.h"
#include "teacher.h"
#include "vercommon.h"
#include "Speak.h"
using std::thread;

LRESULT CALLBACK ConsoleProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK DlgProc    (HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK HookProc   (int code, WPARAM wParam, LPARAM lParam);

HWND  g_hDlg;
HHOOK g_hHook;
HWND  g_hConsole;

void initWindows(HINSTANCE hInst)
{
    AllocConsole();
    HWND& hConsoleWnd = g_hConsole;
    hConsoleWnd = GetConsoleWindow();
    
    SetConsoleTitle(L"CSU Lesson Chooser by Sowicm http://sowicm.com");

    DWORD dwStyle = GetWindowLong(hConsoleWnd, GWL_STYLE);
    dwStyle &= ~WS_CAPTION;
    SetWindowLong(hConsoleWnd, GWL_STYLE, dwStyle);
    
    RECT rc;
    g_hDlg = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
    GetWindowRect(g_hDlg, &rc);
    int dlgWidth, dlgHeight;
    dlgWidth = rc.right - rc.left;
    dlgHeight = rc.bottom - rc.top;
    GetWindowRect(hConsoleWnd, &rc);
    int conWidth, conHeight;
    conWidth = rc.right - rc.left;
    conHeight = rc.bottom - rc.top;// - 50;
    HWND hDesktop = GetDesktopWindow();
    GetWindowRect(hDesktop, &rc);
    int x = rc.left + (rc.right - rc.left - (dlgWidth + conWidth)) / 2;
    int y = rc.top  + (rc.bottom - rc.top - conHeight) / 2;
    SetWindowPos(g_hDlg, HWND_TOP, x, y, 0, 0, SWP_NOSIZE);
    MoveWindow(hConsoleWnd, x + dlgWidth, y, conWidth, conHeight, TRUE);
    UpdateWindow(hConsoleWnd);
    ShowWindow(g_hDlg, SW_SHOW);

    //g_hHook = SetWindowsHookEx(WH_GETMESSAGE, HookProc, hInst, GetWindowThreadProcessId(hConsoleWnd, 0));
    //SetWindowLong(hConsoleWnd, GWL_WNDPROC, (LONG)ConsoleProc);

    freopen("CONOUT$", "w+t", stdout);
    freopen("CONIN$", "r+t", stdin);
    SetForegroundWindow(g_hConsole);
}

#if 0
LRESULT CALLBACK ConsoleProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    switch (Msg)
    {
    case WM_MOVE:
        {
            short x, y;
            x = (short) LOWORD(lParam);
            y = (short) HIWORD(lParam);
            SendMessage(g_hDlg, WM_MOVE, 0, lParam);
        }            
    }
    return DefWindowProc(hWnd, Msg, wParam, lParam);
}

LRESULT CALLBACK HookProc(int _code, WPARAM _wParam, LPARAM _lParam)
{
    MSG* pMsg = (MSG*)_lParam;
    WPARAM wParam = pMsg->wParam;
    LPARAM lParam = pMsg->lParam;
    switch (pMsg->message)
    {
    case WM_MOVE:
        {
            short x, y;
            x = (short) LOWORD(lParam);
            y = (short) HIWORD(lParam);
            SendMessage(g_hDlg, WM_MOVE, 0, lParam);
        }            
    }
    return CallNextHookEx(g_hHook, _code, _wParam, _lParam);
}
#endif

INT_PTR CALLBACK DlgProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    switch (Msg)
    {
    case WM_INITDIALOG:
        {
            HWND hDesktop = GetDesktopWindow();
            RECT rcDesktop, rcDlg;
            GetWindowRect(hDesktop, &rcDesktop);
            return TRUE;
        }

#if 1
    case WM_WINDOWPOSCHANGED:
        {
            WINDOWPOS* pwp = (WINDOWPOS*)lParam;
            RECT rc;
            GetWindowRect(hWnd, &rc);
            int offset = rc.right - rc.left;
            GetWindowRect(g_hConsole, &rc);
            MoveWindow(g_hConsole, pwp->x + offset, pwp->y, rc.right - rc.left, rc.bottom - rc.top, FALSE);
            break;
        }
#else
    case WM_MOVE:
        {
            //short x = LOWORD( lParam );
            //short y = HIWORD( lParam );
            RECT rc;
            GetWindowRect(hWnd, &rc);
            LONG& x = rc.left;
            LONG& y = rc.top;
            //SetWindowPos(g_hConsole, HWND_TOP, x + rc.right - rc.left, y, 0, 0, SWP_NOSIZE);
            int offset = rc.right - rc.left;
            GetWindowRect(g_hConsole, &rc);
            MoveWindow(g_hConsole, x + offset, y, rc.right - rc.left, rc.bottom - rc.top, FALSE);
            break;
        }
#endif

    case WM_CLOSE:
        PostQuitMessage(0);
        SetForegroundWindow(g_hConsole);
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_BUTTON10:
            {
                speak(
                    L"你好，本软件需要你在选课前几天，也就是选课公告放出来之后，按照本软件的指导查询课程信息，自己排好理想课表，"
                    L"然后把这个课表按格式写入设置文件，注意，以上工作可以在选课公告出来后尽快做好，选课当天你要做的事情仅仅是双"
                    L"击运行本程序，并且可以在选课开始前运行，哪怕你选课当天"
                    L"想出去玩，你可以选课的前一天运行本软件，然后保持电脑处于开机状态，程序也会在选课正式开始后自动帮你完成你前"
                    L"几天设置的选课计划。关于如何设置选课，请点击教我如何选必修，或，教我如何选选修的按钮。"
                    , true );
                return TRUE;
            }
        case IDC_BUTTON1:
            {
                if (IDYES == MessageBox(NULL, L"程序需要操控你的计算机进行教学，请问需要继续吗？（同时请确保打开扬声器或插入耳机）", L"教学", MB_YESNO))
                    thread(teachNecessary).detach();
                return TRUE;
            }
        case IDC_BUTTON2:
            {
                if (IDYES == MessageBox(NULL, L"程序需要操控你的计算机进行教学，请问需要继续吗？（同时请确保打开扬声器或插入耳机）", L"教学", MB_YESNO))
                    thread(teachOptional).detach();
                return TRUE;
            }
        case IDC_BUTTON3:
            {
                openUrlWithBrowser(L"http://csujwc.its.csu.edu.cn/ZNPK/KBFB_ClassSel.aspx");
                return TRUE;
            }
        case IDC_BUTTON4:
            {
                openUrlWithBrowser(L"http://csujwc.its.csu.edu.cn/ZNPK/KBFB_LessonSel.aspx");
                return TRUE;
            }
        case IDC_BUTTON5:
            {
                openUrlWithBrowser(L"http://csujwc.its.csu.edu.cn/ZNPK/KBFB_RXKBSel.aspx");
                return TRUE;
            }
        case IDC_BUTTON6:
            {
                ShellExecute(NULL, L"open", L"http://zi-jin.com/archives/533", NULL, NULL, SW_NORMAL);
                return TRUE;
            }
        case IDC_BUTTON7:
            {
                MessageBox(NULL, L"当前版本: "VERSION, L"关于", MB_OK);
                return TRUE;
            }
        case IDC_BUTTON8:
            {
                ShellExecute(NULL, L"open", L"http://item.taobao.com/item.htm?id=14478765070", NULL, NULL, SW_NORMAL);
                return TRUE;
            }

        case IDC_BUTTON9:
            {
                srand(time(0));
                speak( words[rand() % numWords], true );
                return TRUE;
            }
        }
    }
    return FALSE;
}