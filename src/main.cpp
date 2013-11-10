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

#include <WS2tcpip.h>
#include <Windows.h>
#include <thread>
#include <direct.h>
#include "common.h"
#include "Person.h"
#include "vercommon.h"
#include "Speak.h"
#include "..\res\高级版.h"
#include "windows.h"
using namespace std;

#pragma comment(lib, "ws2_32.lib")


addrinfo*                     pai;
addrinfo*                     pmai;

time_t                        didTime = 0;
mutex                         mdidtime;
char                          cachedir[261];

void checkdir(const string& dir, bool bMove)
{
	WIN32_FIND_DATA		      ffd;
	HANDLE				      hFind;
    string                    srcpath;
    string                    dstpath;

    hFind = FindFirstFile((dir + "*.*").c_str(), &ffd);
	if (hFind == INVALID_HANDLE_VALUE)
		return;
	do
	{
        if (ffd.cFileName[0] != '.')
        {
            puts(ffd.cFileName);
            while (true)
            {
                try
                {
                    thread(personMain, ffd.cFileName).detach();
                }
                catch (...)
                {
                    Sleep(200);
                    continue;
                }
                break;
            }
            if (bMove)
            {
                srcpath = move((string) dir + ffd.cFileName);
                dstpath  = move((string) ingdir  + ffd.cFileName);
                MoveFile(srcpath.c_str(), dstpath.c_str());
            }
        }
    } while (FindNextFile(hFind, &ffd));
}

void watcher()
{
    checkdir(ingdir, false);

    while (true)
    {
        checkdir(tododir, true);
        Sleep(3000);
    }
}

void makefiles()
{
    _mkdir("data");
    _mkdir(tododir);
    _mkdir(donedir);
    _mkdir(ingdir);
    _mkdir(logsdir);
    CopyRcFile(NULL, MAKEINTRESOURCE(IDR_RCDATA1), tododir ".必修示例.txt");
    CopyRcFile(NULL, MAKEINTRESOURCE(IDR_RCDATA2), tododir ".选修示例.txt");
    CopyRcFile(NULL, MAKEINTRESOURCE(IDR_RCDATA3), "Attention! .txt");
    CopyRcFile(NULL, MAKEINTRESOURCE(IDR_RCDATA4), "必修.txt");
    CopyRcFile(NULL, MAKEINTRESOURCE(IDR_RCDATA5), "选修.txt");
    CopyRcFile(NULL, MAKEINTRESOURCE(IDR_RCDATA6), "如何获得专业编号.png");
    CopyRcFile(NULL, MAKEINTRESOURCE(IDR_RCDATA7), "如何获得必修课信息.png");
    CopyRcFile(NULL, MAKEINTRESOURCE(IDR_RCDATA8), "如何获得选修课信息.png");
}

int main()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|_CRTDBG_LEAK_CHECK_DF);
    initWindows();
    srand(time(0));
    FILE *fp = fopen("Attention! .txt", "rb");

    makefiles();
    if (!fp)
    {
        speak(
            L"欢迎使用中南大学全自动选课软件，看起来你好像是第一次运行本程序，本程序已经自动生成了文本和图片的帮助文件，请查看后再次运行本程序。"
            , true
            );
        puts(
            "中南大学全自动选课软件 - 高级版\n"
            "看起来你好像是第一次运行本程序，本程序已经自动生成了文本和图片的帮助文件，请查看后再次运行本程序。"
            );
        system("pause");
        return 0;
    }
    else
        fclose(fp);
    
    speak( words[rand() % numWords], true );
    GetTempPath(MAX_PATH, cachedir);
    puts(
        "中南大学全自动选课软件 - 高级版\n"
        "你正在使用的版本是云计算版，其特点是使用的人越多效果越好，能减轻服务器负担\n"
        "你正在使用的版本是威力加强版，所以部分低配电脑运行会出现问题\n"
        "请自行承担使用此软件的任何后果\n"
        "出现任何问题请先大声朗读3遍万能咒语\"张日月明你好帅\"，如果问题仍未解决，那是你人品太差了\n"
        "欢迎关注我的主页： http://sowicm.com 虽然还没做好，但说不定你哪天进去就发现惊喜了\n"
        "欢迎关注我的博客： http://zi-jin.com \n"
        "欢迎关注我的微博:  http://weibo.com/zhizunmingshuai \n"
        "欢迎关注...\n"
        "好吧，一般而言看到这里的时候软件早就已经帮你把课选好了，看看目录下有没有result.txt文件，里面有结果，Enjoy it.\n"
        "对了，程序会一直监测todo目录下的文件，不会自动结束（也就是说你想新增加任务不用重新启动程序，直接添加文件就行），所以完成选课后手动关闭即可"
        );


	WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData))
        return false;

    if (!getAddrInfo())
        return -1;

    thread watcherThread(watcher);
    watcherThread.join();

	WSACleanup();
}