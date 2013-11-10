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
    L"��ӭʹ�����ϴ�ѧȫ�Զ�ѡ�������ͬʱҲ��ӭ�������ߵ���ҳ<spell>http://sowicm</spell>��com",
    L"���뵱�÷�ĳ�ʦ���Ǹ���˾��",
    L"��Ҫ���κ����ѵ�ͷ������һ���˷��κ����ѵ��ؾ�<silen msec=\"200\" />�����ʶ�<silence msec=\"100\" />�����������˧",
    L"�����Ի����˼�������Ȥ����ϵ�ң������������������Ϸ��������Ȥ����ϵ�ң�������<spell>3ds</spell>max����Ƶ��Ӱ����Ȥ����ϵ�ң�������������Ȥ����ϵ��"
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
