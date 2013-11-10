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
#include "..\res\�߼���.h"
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
    CopyRcFile(NULL, MAKEINTRESOURCE(IDR_RCDATA1), tododir ".����ʾ��.txt");
    CopyRcFile(NULL, MAKEINTRESOURCE(IDR_RCDATA2), tododir ".ѡ��ʾ��.txt");
    CopyRcFile(NULL, MAKEINTRESOURCE(IDR_RCDATA3), "Attention! .txt");
    CopyRcFile(NULL, MAKEINTRESOURCE(IDR_RCDATA4), "����.txt");
    CopyRcFile(NULL, MAKEINTRESOURCE(IDR_RCDATA5), "ѡ��.txt");
    CopyRcFile(NULL, MAKEINTRESOURCE(IDR_RCDATA6), "��λ��רҵ���.png");
    CopyRcFile(NULL, MAKEINTRESOURCE(IDR_RCDATA7), "��λ�ñ��޿���Ϣ.png");
    CopyRcFile(NULL, MAKEINTRESOURCE(IDR_RCDATA8), "��λ��ѡ�޿���Ϣ.png");
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
            L"��ӭʹ�����ϴ�ѧȫ�Զ�ѡ�������������������ǵ�һ�����б����򣬱������Ѿ��Զ��������ı���ͼƬ�İ����ļ�����鿴���ٴ����б�����"
            , true
            );
        puts(
            "���ϴ�ѧȫ�Զ�ѡ����� - �߼���\n"
            "������������ǵ�һ�����б����򣬱������Ѿ��Զ��������ı���ͼƬ�İ����ļ�����鿴���ٴ����б�����"
            );
        system("pause");
        return 0;
    }
    else
        fclose(fp);
    
    speak( words[rand() % numWords], true );
    GetTempPath(MAX_PATH, cachedir);
    puts(
        "���ϴ�ѧȫ�Զ�ѡ����� - �߼���\n"
        "������ʹ�õİ汾���Ƽ���棬���ص���ʹ�õ���Խ��Ч��Խ�ã��ܼ������������\n"
        "������ʹ�õİ汾��������ǿ�棬���Բ��ֵ���������л��������\n"
        "�����ге�ʹ�ô�������κκ��\n"
        "�����κ��������ȴ����ʶ�3����������\"�����������˧\"�����������δ�������������Ʒ̫����\n"
        "��ӭ��ע�ҵ���ҳ�� http://sowicm.com ��Ȼ��û���ã���˵�����������ȥ�ͷ��־�ϲ��\n"
        "��ӭ��ע�ҵĲ��ͣ� http://zi-jin.com \n"
        "��ӭ��ע�ҵ�΢��:  http://weibo.com/zhizunmingshuai \n"
        "��ӭ��ע...\n"
        "�ðɣ�һ����Կ��������ʱ���������Ѿ�����ѿ�ѡ���ˣ�����Ŀ¼����û��result.txt�ļ��������н����Enjoy it.\n"
        "���ˣ������һֱ���todoĿ¼�µ��ļ��������Զ�������Ҳ����˵����������������������������ֱ������ļ����У����������ѡ�κ��ֶ��رռ���"
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