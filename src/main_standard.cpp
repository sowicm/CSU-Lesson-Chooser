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

#include "common.h"
#include <Windows.h>
#include <time.h>
#include <thread>
#include "windows.h"
#include "work.h"
#include "Speak.h"
#include "vercommon.h"
using namespace std;

#pragma comment(lib, "ws2_32.lib")
#ifdef _DEBUG
#pragma comment(lib, "_LibDebug.lib")
#else
#pragma comment(lib, "_Lib.lib")
#endif


int APIENTRY wWinMain(HINSTANCE hInst,
                      HINSTANCE hPrevInst,
                      LPWSTR    lpCmdLine,
                      int       nCmdShow)
{
    FILE *fp = fopen("browser.exe", "rb");
    if (!fp)
    {
        //if (MessageBox(NULL, L"是否需要生成文本和图片的帮助文件？", L"欢迎使用中南大学全自动选课软件", MB_YESNO) == IDYES)
            makefiles();
    }
    else
    {
        fclose(fp);
    }

    initWindows(hInst);

    thread(work).detach();
    
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        //if (!IsWindow(g_hDlgInform) || !IsDialogMessage(g_hDlgInform, &msg))
        //{
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        //}
    }
}