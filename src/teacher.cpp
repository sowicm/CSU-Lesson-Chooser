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
    // 在选课前2天设置好，选课当天提前半个小时运行，然后就不用管了。
    //speak(L"程序需要操控你的计算机进行教学，请问需要继续吗？需要请说<emph>要</emph>，不需要请说<emph>不要</emph>，"
    //      L"如果超过三秒钟未应答，教学将自动开始");

    speak(L"下面我将给你演示如何写必修课的设置文件以及如何获取所需的信息。");

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

    speak(L"首先请打开记事本");

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

    speak(L"然后输入你的学号和密码");
    inputString(hWndInput, "0909111024\n123456");
    
    speak(L"由于要选的是必修课，所以新起一行，输入1");
    inputString(hWndInput, "\n1");

    speak(L"然后必修课需要你的专业编号，可以点击这里");   

    //GetWindowRect(g_hDlg, &rcWnd);
    //GetClientRect(g_hDlg, &rcClient);
    hWnd = GetDlgItem(g_hDlg, IDC_BUTTON3);
    GetWindowRect(hWnd, &rc);
    moveTo(rc.left + 60, rc.top + 10);
    click();

    speak(L"这样就打开了查看专业编号的网页");
    do
    {
        ReadFile(hRead, buf, 1024, &dwRead, NULL);
        buf[dwRead] = '\0';
    } while (KMP(buf, "/ZNPK/KBFB_ClassSel_rpt.aspx") < 0);

    speak(L"网页已经成功打开了，从下拉框选择你的年级，院系，和专业");
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
    speak(L"OK，把年级4位数", true);
    moveTo(half + 47, 98);//357, 98);
    Sleep(2000);
    speak(L"后面接上专业编号4位数组成的8位数就是我们需要的结果", true);
    moveTo(half + 340, 96);//650, 96);
    
    speak(L"把它写入记事本");
    SetForegroundWindow(hNotepad);
    Sleep(1000);
    inputString(hWndInput, " 20110909");
    Sleep(1000);

    speak(L"关闭浏览器");
    GetWindowRect(GetDesktopWindow(), &rc);
    moveTo(rc.right - 35, rc.top + 15);
    click();
    
    speak(L"下面为大家演示如何获取课程信息。假设我想选C加加和微积分两门课，首先点击这里。");
    hWnd = GetDlgItem(g_hDlg, IDC_BUTTON4);
    GetWindowRect(hWnd, &rc);
    moveTo(rc.left + 60, rc.top + 10);
    click();
    
    do
    {
        ReadFile(hRead, buf, 1024, &dwRead, NULL);
        buf[dwRead] = '\0';
    } while (KMP(buf, "/ZNPK/KBFB_LessonSel_rpt.aspx") < 0);
    speak(L"然后在这里选格式二，不然看不到班级号！");
    moveTo(half + 83, 92);//393, 92);
    click();
    speak(L"然后在这里输入C加加");
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
    //inputChinese(hWnd, "微积分");//inputString(hWnd, "微积分");

    //SetClipboardData(CF_TEXT, "微积分");
    //SendMessage(GetForegroundWindow(), WM_PASTE, 0, 0);
    speak(L"点下拉框");
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
    speak(L"检索");
    moveTo(half + 585, 68);//895, 68);
    click();
    do
    {
        ReadFile(hRead, buf, 1024, &dwRead, NULL);
        buf[dwRead] = '\0';
    } while (KMP(buf, "/ZNPK/KBFB_LessonSel_rpt.aspx") < 0);
    moveTo(half + 365, 73);//675, 73);
    speak(L"这里");
    moveTo(266, 178);
    speak(L"还有这里，是课程号。");
    moveTo(157, 214);
    speak(L"这一竖排是班号", true);
    int x = dwidth * 8 / 100 + 55; // 157
    moveTo(x, 521);
    moveTo(x, 214);
    moveTo(x, 521);
    speak(L"我们可以用同样的方法得到微积分的课程信息，就不赘述，现在切回记事本。");

    SetForegroundWindow(hNotepad);
    moveTo(800, 200);
    speak(L"新起一行，输入要选的第一个课程的编号。刚才我们已经看到C加加的编号是<spell>02010011</spell>");
    inputString(hWndInput, "\n02010011");
    speak(L"然后再新起一行，输入左方括号。");
    inputString(hWndInput, "\n[");
    speak(L"假设我最想选的班，班号为030，就新起一行，输入030。");
    inputString(hWndInput, "\n030");
    speak(L"如果时间冲突或者已经被选满了，怎么办呢？假设如果这个班选不上，我想选026班，则再新起一行，输入026。");
    inputString(hWndInput, "\n026");
    speak(L"依次类推，把你想选的班按优先级从上往下全部输入在这里，程序会自动从第一个开始，选不上则自动选下一个。"
          L"别忘了把括号括回来，新起一行输入右中括号");
    inputString(hWndInput, "\n]");
    speak(L"同样现在我也设置微积分想选的班级，直接按相同的格式添加到后面即可。");
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
    speak(L"到此设置文件就已经写好了，然后只要把这个文件保存在程序目录，文件名为<spell>conf</spell>，程序就设置好了，"
          L"选课那天你只要双击打开本程序就行了，到此我的教学任务已经结束了，如果仍然有疑问，请点击检查新版访问主页并留言。拜拜！");
    CloseHandle(hWrite);
    CloseHandle(hRead);
    hWrite = NULL;
}

void teachOptional()
{
    //speak(L"程序需要操控你的计算机进行教学，请问需要继续吗？需要请说<emph>要</emph>，不需要请说<emph>不要</emph>，"
    //      L"如果超过三秒钟未应答，教学将自动开始");

    speak(L"下面我将给你演示如何写选修课的设置文件以及如何获取所需的信息。");

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

    speak(L"首先请打开记事本");

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

    speak(L"然后输入你的学号和密码");
    inputString(hWndInput, "0909111024\n123456");
    
    speak(L"由于要选的是选修课，所以新起一行，输入2");
    inputString(hWndInput, "\n2");

    speak(L"下面给你演示如何获取选修课的课程信息，先可以点击这里");   

    //GetWindowRect(g_hDlg, &rcWnd);
    //GetClientRect(g_hDlg, &rcClient);
    hWnd = GetDlgItem(g_hDlg, IDC_BUTTON5);
    GetWindowRect(hWnd, &rc);
    moveTo(rc.left + 60, rc.top + 10);
    click();

    speak(L"这样就打开了查看选修课的网页");
    do
    {
        ReadFile(hRead, buf, 1024, &dwRead, NULL);
        buf[dwRead] = '\0';
    } while (KMP(buf, "/ZNPK/KBFB_RXKBSel_rpt.aspx") < 0);

    speak(L"网页已经成功打开了，从下拉框选择校区");
    moveTo(half + 346, 62);//656, 62);
    click();

    speak(L"请注意，在这个下拉框内，校区排在第几位，它的编号就是几。所以新校区的编号就是9。");
    moveTo(half + 283, 243);//593, 243);
    click();

    moveTo(half + 583, 62);//893, 62);
    click();
    do
    {
        ReadFile(hRead, buf, 1024, &dwRead, NULL);
        buf[dwRead] = '\0';
    } while (KMP(buf, "/ZNPK/KBFB_RXKBSel_rpt.aspx") < 0);
    speak(L"表格出来了");
    speak(L"这里方括号里面的就是课程号", true);
    int x = dwidth * 5 / 100 + 21; // 85
    moveTo(x, 154);
    moveTo(x, 554);
    moveTo(x, 154);
    moveTo(x, 554);
    moveTo(x, 154);
    speak(L"这里就是班号", true);
    x = dwidth / 2 + 21; // 661
    moveTo(x, 154);
    moveTo(x, 554);
    moveTo(x, 154);
    moveTo(x, 554);
    moveTo(x, 154);

    speak(L"现在回到记事本");
    SetForegroundWindow(hNotepad);
    moveTo(800, 200);
    speak(L"先输入校区编号，假设是新校区，我在这里输入9");
    inputString(hWndInput, " 9");
    speak(L"现在假设我最想选的课是一杠零二号课程，它有两个班，如果选不上这个课，我想选二杠零八号课程的二号班。"
          L"我只需要再新起一行，输入我的第一个选择的课程号和班号。一杠零二号的第一个班。");
    inputString(hWndInput, "\n1-02 001");
    speak(L"这个选不上还有第二个班。");
    inputString(hWndInput, "\n1-02 002");
    speak(L"这门课选不上，二杠零八号的二号班也不错。");
    inputString(hWndInput, "\n2-08 002");
    speak(L"依次类推，把你想选的课程班级按优先级从上往下全部输入在这里，程序会自动从第一个开始，如果时间冲突或者已经被选满了则自动选下一个。");
    speak(L"到此设置文件就已经写好了，然后只要把这个文件保存在程序目录，文件名为<spell>conf</spell>，程序就设置好了，"
          L"选课那天你只要双击打开本程序就行了，到此我的教学任务已经结束了，如果仍然有疑问，请点击检查新版访问主页并留言。拜拜！");
    CloseHandle(hWrite);
    CloseHandle(hRead);
    hWrite = NULL;
}