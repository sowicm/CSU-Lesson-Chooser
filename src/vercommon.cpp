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
#include <WS2tcpip.h>
#include <stdio.h>
#include "vercommon.h"
using namespace std;

const int  numWords = 4;
wchar_t words[numWords][128] = 
{
    L"欢迎使用中南大学全自动选课软件，同时也欢迎访问作者的主页<spell>http://sowicm</spell>点com",
    L"不想当裁缝的厨师不是个好司机",
    L"不要向任何困难低头，教你一个克服任何困难的秘诀<silen msec=\"200\" />大声朗读<silence msec=\"100\" />张日月明你好帅",
    L"如果你对机器人技术感兴趣，联系我，如果你对软件开发和游戏开发感兴趣，联系我，如果你对<spell>3ds</spell>max和视频电影感兴趣，联系我，如果你对我有兴趣，联系我"
};

bool getAddrInfo()
{
    addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    while (getaddrinfo("csujwc.its.csu.edu.cn", "80", &hints, &pai) != 0)
    {
        Sleep(2000);
    }
    
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    while (getaddrinfo("zi-jin.com", "80", &hints, &pmai) != 0)
    {
        Sleep(2000);
    }

    return true;
}

void CopyRcFile(HMODULE hModule, LPCTSTR lpName, const char* path)
{
    HRSRC   hResInfo    = FindResource(hModule, lpName, RT_RCDATA);
    DWORD   dwSize      = SizeofResource(hModule, hResInfo);
    HGLOBAL hResData    = LoadResource(hModule, hResInfo);
    LPVOID  pResLock    = LockResource(hResData);
    FILE *fp = fopen(path, "wb");
    if (fp)
    {
        fwrite(pResLock, 1, dwSize, fp);
        fclose(fp);
    }
    FreeResource(hResData);
}


void _get(const string& path, char* buf)
{
    string header = move((string)
              "GET " + path + " HTTP/1.1\r\n"
		      "Host: zi-jin.com\r\n"
	          "Connection: Keep-Alive\r\n"
			  "\r\n"
              );
}
