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
#include <KMP.h>
#include <math.h>
#include "teacher.h"
#include "Speak.h"
#include "../pro/vs/resource.h"

HANDLE hWrite = NULL;
extern HWND g_hDlg;

void openUrlWithBrowser(LPTSTR url)
{
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    if (hWrite != NULL)
    {
        si.dwFlags = STARTF_USESTDHANDLES;
        si.hStdOutput = hWrite;
    }
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));
    wchar_t cmdLine[261] = L"browser.exe ";
    wcscat(cmdLine, url);
    if (CreateProcess(NULL, cmdLine, NULL, NULL, hWrite != NULL, 0, NULL, NULL, &si, &pi))
    {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}

void moveTo(int x, int y)
{
    POINT pt;
    GetCursorPos(&pt);
    double dx, dy;
    dx = x - pt.x;
    dy = y - pt.y;
    double dist = sqrt(dx * dx + dy * dy);
    double tm = dist / 3;
    dx /= tm;
    dy /= tm;
    for (int i = 0; i < tm; ++i)
    {
        SetCursorPos(pt.x + i * dx, pt.y + i * dy);
        Sleep(10);
    }
    SetCursorPos(x, y);
}

inline void click()
{
    INPUT input[2];
    ZeroMemory(input, sizeof(INPUT) * 2);
    input[0].type = INPUT_MOUSE;
    input[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    input[1].type = INPUT_MOUSE;
    input[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(2, input, sizeof(INPUT));
}
inline void mouseDown()
{
    INPUT input;
    ZeroMemory(&input, sizeof(INPUT));
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(1, &input, sizeof(INPUT));
}
inline void mouseUp()
{
    INPUT input;
    ZeroMemory(&input, sizeof(INPUT));
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, &input, sizeof(INPUT));
}
inline void inputString(HWND hWnd, const char* str)
{
    while (*str)
    {
        SendMessageA(hWnd, WM_CHAR, *str++ & 0xff, 0);
        Sleep(50);
    }
}
inline void inputChinese(HWND hWnd, const char* str)
{
    while (*str)
    {
        PostMessageA(hWnd, WM_CHAR, *str++ & 0xff, 0);
        SendMessageA(hWnd, WM_CHAR, *str++ & 0xff, 0);
        Sleep(50);
    }
}
inline void inputCpp()
{
    INPUT input[2];
    ZeroMemory(input, sizeof(INPUT) * 2);
    const char chars[] = {'C', VK_ADD, VK_ADD, 0};
    const char *s = chars;
    while (*s)
    {
        input[0].type  = INPUT_KEYBOARD;
        input[0].ki.wVk = *s;
        input[1].type = INPUT_KEYBOARD;
        input[1].ki.wVk = *s;
        input[1].mi.dwFlags = KEYEVENTF_KEYUP;
        SendInput(2, input, sizeof(INPUT));
        Sleep(100);
        ++s;
    }
}

void teachNecessary()
{
    // ��ѡ��ǰ2�����úã�ѡ�ε�����ǰ���Сʱ���У�Ȼ��Ͳ��ù��ˡ�
    //speak(L"������Ҫ�ٿ���ļ�������н�ѧ��������Ҫ��������Ҫ��˵<emph>Ҫ</emph>������Ҫ��˵<emph>��Ҫ</emph>��"
    //      L"�������������δӦ�𣬽�ѧ���Զ���ʼ");

    speak(L"�����ҽ�������ʾ���д���޿ε������ļ��Լ���λ�ȡ�������Ϣ��");

    SECURITY_ATTRIBUTES sa;
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;
    sa.nLength = sizeof(sa);
    HANDLE hRead;
    CreatePipe(&hRead, &hWrite, &sa, 0);

    RECT rc;
    HWND hWnd;
    char buf[1024];
    DWORD dwRead;

    GetWindowRect(GetDesktopWindow(), &rc);
    int dwidth = rc.right - rc.left;
    int half = (rc.right - rc.left - 660) / 2;
    
    /*
    GetWindowRect(g_hDlg, &rc);
    moveTo(rc.left + 50, rc.top + 15);
    mouseDown();
    moveTo(rc.left - 100, rc.top);
    mouseUp();
    */

    speak(L"������򿪼��±�");

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    CreateProcessA(NULL, "notepad", NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
    Sleep(1000);
    HWND hNotepad = GetForegroundWindow();

    DWORD curThreadId = GetCurrentThreadId();
    Sleep(200);
    AttachThreadInput(pi.dwThreadId, curThreadId, true);
    HWND hWndInput = GetFocus();
    AttachThreadInput(pi.dwThreadId, curThreadId, false);

    GetWindowRect(hNotepad, &rc);
    moveTo(rc.left + 50, rc.top + 15);
    mouseDown();
    moveTo(700, 150);
    mouseUp();
    //SetWindowPos(hNotepad, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE);

    speak(L"Ȼ���������ѧ�ź�����");
    inputString(hWndInput, "0909111024\n123456");
    
    speak(L"����Ҫѡ���Ǳ��޿Σ���������һ�У�����1");
    inputString(hWndInput, "\n1");

    speak(L"Ȼ����޿���Ҫ���רҵ��ţ����Ե������");   

    //GetWindowRect(g_hDlg, &rcWnd);
    //GetClientRect(g_hDlg, &rcClient);
    hWnd = GetDlgItem(g_hDlg, IDC_BUTTON3);
    GetWindowRect(hWnd, &rc);
    moveTo(rc.left + 60, rc.top + 10);
    click();

    speak(L"�����ʹ��˲鿴רҵ��ŵ���ҳ");
    do
    {
        ReadFile(hRead, buf, 1024, &dwRead, NULL);
        buf[dwRead] = '\0';
    } while (KMP(buf, "/ZNPK/KBFB_ClassSel_rpt.aspx") < 0);

    speak(L"��ҳ�Ѿ��ɹ����ˣ���������ѡ������꼶��Ժϵ����רҵ");
    moveTo(half + 72, 90); //382, 90);
    click();
    moveTo(half + 46, 130);//356, 130);
    click();
    do
    {
        ReadFile(hRead, buf, 1024, &dwRead, NULL);
        buf[dwRead] = '\0';
    } while (KMP(buf, "/XSXJ/Private/List_ZYBJ.aspx") < 0);
    moveTo(half + 271, 92);//581, 92);
    click();
    moveTo(half + 180, 272);//490, 272);
    click();
    do
    {
        ReadFile(hRead, buf, 1024, &dwRead, NULL);
        buf[dwRead] = '\0';
    } while (KMP(buf, "/XSXJ/Private/List_NJYXZY.aspx") < 0);
    moveTo(half + 451, 89);//761, 89);
    click();
    moveTo(half + 375, 253);//685, 253);
    click();
    speak(L"OK�����꼶4λ��", true);
    moveTo(half + 47, 98);//357, 98);
    Sleep(2000);
    speak(L"�������רҵ���4λ����ɵ�8λ������������Ҫ�Ľ��", true);
    moveTo(half + 340, 96);//650, 96);
    
    speak(L"����д����±�");
    SetForegroundWindow(hNotepad);
    Sleep(1000);
    inputString(hWndInput, " 20110909");
    Sleep(1000);

    speak(L"�ر������");
    GetWindowRect(GetDesktopWindow(), &rc);
    moveTo(rc.right - 35, rc.top + 15);
    click();
    
    speak(L"����Ϊ�����ʾ��λ�ȡ�γ���Ϣ����������ѡC�ӼӺ�΢�������ſΣ����ȵ�����");
    hWnd = GetDlgItem(g_hDlg, IDC_BUTTON4);
    GetWindowRect(hWnd, &rc);
    moveTo(rc.left + 60, rc.top + 10);
    click();
    
    do
    {
        ReadFile(hRead, buf, 1024, &dwRead, NULL);
        buf[dwRead] = '\0';
    } while (KMP(buf, "/ZNPK/KBFB_LessonSel_rpt.aspx") < 0);
    speak(L"Ȼ��������ѡ��ʽ������Ȼ�������༶�ţ�");
    moveTo(half + 83, 92);//393, 92);
    click();
    speak(L"Ȼ������������C�Ӽ�");
    moveTo(half + 283, 65);//593, 65);
    click();
    inputCpp();
#if 0
    DWORD pid;
    GetWindowThreadProcessId(GetForegroundWindow(), &pid);
    AttachThreadInput(pid, curThreadId, true);
    hWnd = GetFocus();
    AttachThreadInput(pid, curThreadId, false);
#endif
    //hWnd = GetForegroundWindow();
    //inputChinese(hWnd, "΢����");//inputString(hWnd, "΢����");

    //SetClipboardData(CF_TEXT, "΢����");
    //SendMessage(GetForegroundWindow(), WM_PASTE, 0, 0);
    speak(L"��������");
    moveTo(half + 443, 69);//753, 69);
    click();
    do
    {
        ReadFile(hRead, buf, 1024, &dwRead, NULL);
        buf[dwRead] = '\0';
    } while (KMP(buf, "/ZNPK/Private/List_XNXQKC.aspx") < 0);
    Sleep(1000);
    click();
    moveTo(half + 398, 104);//708, 104);
    click();
    speak(L"����");
    moveTo(half + 585, 68);//895, 68);
    click();
    do
    {
        ReadFile(hRead, buf, 1024, &dwRead, NULL);
        buf[dwRead] = '\0';
    } while (KMP(buf, "/ZNPK/KBFB_LessonSel_rpt.aspx") < 0);
    moveTo(half + 365, 73);//675, 73);
    speak(L"����");
    moveTo(266, 178);
    speak(L"��������ǿγ̺š�");
    moveTo(157, 214);
    speak(L"��һ�����ǰ��", true);
    int x = dwidth * 8 / 100 + 55; // 157
    moveTo(x, 521);
    moveTo(x, 214);
    moveTo(x, 521);
    speak(L"���ǿ�����ͬ���ķ����õ�΢���ֵĿγ���Ϣ���Ͳ�׸���������лؼ��±���");

    SetForegroundWindow(hNotepad);
    moveTo(800, 200);
    speak(L"����һ�У�����Ҫѡ�ĵ�һ���γ̵ı�š��ղ������Ѿ�����C�Ӽӵı����<spell>02010011</spell>");
    inputString(hWndInput, "\n02010011");
    speak(L"Ȼ��������һ�У����������š�");
    inputString(hWndInput, "\n[");
    speak(L"����������ѡ�İ࣬���Ϊ030��������һ�У�����030��");
    inputString(hWndInput, "\n030");
    speak(L"���ʱ���ͻ�����Ѿ���ѡ���ˣ���ô���أ�������������ѡ���ϣ�����ѡ026�࣬��������һ�У�����026��");
    inputString(hWndInput, "\n026");
    speak(L"�������ƣ�������ѡ�İఴ���ȼ���������ȫ�����������������Զ��ӵ�һ����ʼ��ѡ�������Զ�ѡ��һ����"
          L"�����˰�����������������һ��������������");
    inputString(hWndInput, "\n]");
    speak(L"ͬ��������Ҳ����΢������ѡ�İ༶��ֱ�Ӱ���ͬ�ĸ�ʽ��ӵ����漴�ɡ�");
    inputString(hWndInput,
        "\n13070031\n"
        "[\n"
        "041\n"
        "015\n"
        "040\n"
        "028\n"
        "016\n"
        "042\n"
        "]\n");
    speak(L"���������ļ����Ѿ�д���ˣ�Ȼ��ֻҪ������ļ������ڳ���Ŀ¼���ļ���Ϊ<spell>conf</spell>����������ú��ˣ�"
          L"ѡ��������ֻҪ˫���򿪱���������ˣ������ҵĽ�ѧ�����Ѿ������ˣ������Ȼ�����ʣ���������°������ҳ�����ԡ��ݰݣ�");
    CloseHandle(hWrite);
    CloseHandle(hRead);
    hWrite = NULL;
}

void teachOptional()
{
    //speak(L"������Ҫ�ٿ���ļ�������н�ѧ��������Ҫ��������Ҫ��˵<emph>Ҫ</emph>������Ҫ��˵<emph>��Ҫ</emph>��"
    //      L"�������������δӦ�𣬽�ѧ���Զ���ʼ");

    speak(L"�����ҽ�������ʾ���дѡ�޿ε������ļ��Լ���λ�ȡ�������Ϣ��");

    SECURITY_ATTRIBUTES sa;
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;
    sa.nLength = sizeof(sa);
    HANDLE hRead;
    CreatePipe(&hRead, &hWrite, &sa, 0);

    RECT rc;
    HWND hWnd;
    char buf[1024];
    DWORD dwRead;
    
    GetWindowRect(GetDesktopWindow(), &rc);
    int dwidth = rc.right - rc.left;
    int half = (dwidth - 660) / 2;
    

    /*
    GetWindowRect(g_hDlg, &rc);
    moveTo(rc.left + 50, rc.top + 15);
    mouseDown();
    moveTo(rc.left - 100, rc.top);
    mouseUp();
    */

    speak(L"������򿪼��±�");

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    CreateProcessA(NULL, "notepad", NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
    Sleep(1000);
    HWND hNotepad = GetForegroundWindow();

    DWORD curThreadId = GetCurrentThreadId();
    Sleep(200);
    AttachThreadInput(pi.dwThreadId, curThreadId, true);
    HWND hWndInput = GetFocus();
    AttachThreadInput(pi.dwThreadId, curThreadId, false);

    GetWindowRect(hNotepad, &rc);
    moveTo(rc.left + 50, rc.top + 15);
    mouseDown();
    moveTo(700, 150);
    mouseUp();
    //SetWindowPos(hNotepad, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE);

    speak(L"Ȼ���������ѧ�ź�����");
    inputString(hWndInput, "0909111024\n123456");
    
    speak(L"����Ҫѡ����ѡ�޿Σ���������һ�У�����2");
    inputString(hWndInput, "\n2");

    speak(L"���������ʾ��λ�ȡѡ�޿εĿγ���Ϣ���ȿ��Ե������");   

    //GetWindowRect(g_hDlg, &rcWnd);
    //GetClientRect(g_hDlg, &rcClient);
    hWnd = GetDlgItem(g_hDlg, IDC_BUTTON5);
    GetWindowRect(hWnd, &rc);
    moveTo(rc.left + 60, rc.top + 10);
    click();

    speak(L"�����ʹ��˲鿴ѡ�޿ε���ҳ");
    do
    {
        ReadFile(hRead, buf, 1024, &dwRead, NULL);
        buf[dwRead] = '\0';
    } while (KMP(buf, "/ZNPK/KBFB_RXKBSel_rpt.aspx") < 0);

    speak(L"��ҳ�Ѿ��ɹ����ˣ���������ѡ��У��");
    moveTo(half + 346, 62);//656, 62);
    click();

    speak(L"��ע�⣬������������ڣ�У�����ڵڼ�λ�����ı�ž��Ǽ���������У���ı�ž���9��");
    moveTo(half + 283, 243);//593, 243);
    click();

    moveTo(half + 583, 62);//893, 62);
    click();
    do
    {
        ReadFile(hRead, buf, 1024, &dwRead, NULL);
        buf[dwRead] = '\0';
    } while (KMP(buf, "/ZNPK/KBFB_RXKBSel_rpt.aspx") < 0);
    speak(L"��������");
    speak(L"���﷽��������ľ��ǿγ̺�", true);
    int x = dwidth * 5 / 100 + 21; // 85
    moveTo(x, 154);
    moveTo(x, 554);
    moveTo(x, 154);
    moveTo(x, 554);
    moveTo(x, 154);
    speak(L"������ǰ��", true);
    x = dwidth / 2 + 21; // 661
    moveTo(x, 154);
    moveTo(x, 554);
    moveTo(x, 154);
    moveTo(x, 554);
    moveTo(x, 154);

    speak(L"���ڻص����±�");
    SetForegroundWindow(hNotepad);
    moveTo(800, 200);
    speak(L"������У����ţ���������У����������������9");
    inputString(hWndInput, " 9");
    speak(L"���ڼ���������ѡ�Ŀ���һ������ſγ̣����������࣬���ѡ��������Σ�����ѡ������˺ſγ̵Ķ��Űࡣ"
          L"��ֻ��Ҫ������һ�У������ҵĵ�һ��ѡ��Ŀγ̺źͰ�š�һ������ŵĵ�һ���ࡣ");
    inputString(hWndInput, "\n1-02 001");
    speak(L"���ѡ���ϻ��еڶ����ࡣ");
    inputString(hWndInput, "\n1-02 002");
    speak(L"���ſ�ѡ���ϣ�������˺ŵĶ��Ű�Ҳ����");
    inputString(hWndInput, "\n2-08 002");
    speak(L"�������ƣ�������ѡ�Ŀγ̰༶�����ȼ���������ȫ�����������������Զ��ӵ�һ����ʼ�����ʱ���ͻ�����Ѿ���ѡ�������Զ�ѡ��һ����");
    speak(L"���������ļ����Ѿ�д���ˣ�Ȼ��ֻҪ������ļ������ڳ���Ŀ¼���ļ���Ϊ<spell>conf</spell>����������ú��ˣ�"
          L"ѡ��������ֻҪ˫���򿪱���������ˣ������ҵĽ�ѧ�����Ѿ������ˣ������Ȼ�����ʣ���������°������ҳ�����ԡ��ݰݣ�");
    CloseHandle(hWrite);
    CloseHandle(hRead);
    hWrite = NULL;
}