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
#include <Windows.h>
#include <direct.h>
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <thread>
#include <mutex>
#include <vector>
#include <queue>
#include <conio.h>
#include <KMP.h>
#include <urlEncode.h>
#include <sstream>
#include "Speak.h"
#include "MTRequest.h"
#include "vercommon.h"
#include "../pro/vs/resource.h"
using namespace std;

#define MAX_Thread   50
#define MAX_HtmlSize 409600

addrinfo  *pai;
addrinfo  *pmai;
string     cookie;
int		   responseCode;
string     response;
string     uid;
string     pwd;
string     type;
string     spec;
char                          cachedir[261];

char       pbuf[MAX_HtmlSize];

void mtGet(const string& path)
{
    string header;
	header = move((string)
              "GET " + path + " HTTP/1.1\r\n"
		      "Host: csujwc.its.csu.edu.cn\r\n"
	          "Connection: Keep-Alive\r\n"
			  "Cookie: " + cookie + "\r\n"
			  "\r\n"
              );

	MTRequest request(pai, header);
    response = move(request.response());
    responseCode = request.responseCode();
}

void mtPost(const string& path, const string& data)
{
	char nbuf[32];
	string header = move((string) 
              "POST " + path + " HTTP/1.1\r\n"
		      "Content-Type: application/x-www-form-urlencoded\r\n"
		      "Host: csujwc.its.csu.edu.cn\r\n"
			  "Content-Length: " + _itoa(data.length(), nbuf, 10) + "\r\n"
	          "Connection: Keep-Alive\r\n"
			  "Cookie: " + cookie + "\r\n"
			  "\r\n");

    MTRequest request(pai, header, data);
	response = move(request.response());
    responseCode = request.responseCode();
}

bool login()
{
    mtPost(
        "/_data/index_login.aspx", (string)
        "typeName=%D1%A7%C9%FA"
        "&Sel_Type=STU"
        "&UserID=" + uid +
        "&PassWord=" + pwd
        );
    if (KMP(response.c_str(), "���ڼ���Ȩ������...") >= 0)
    {
        int s = KMP(response.c_str(), "Set-Cookie:", 0, true);
        int e = response.find('\r', s);
        cookie = move(response.substr(s, e - s));

        return true;
    }
    return false;
}

struct Choise
{
    Choise(){}
    Choise(Choise&& a)
        : lesnid(move(a.lesnid)), clsid(move(a.clsid))
    {}

    std::string         lesnid;
    std::string         clsid;
};

struct Task
{
    Task(){}
    Task(Task&& a)
        : choises(move(a.choises))
    {}

    int                 id;
    std::queue<Choise>  choises;
};

struct lessoninfo {
    lessoninfo() {}
    lessoninfo(std::string&& v4s, std::string&& v4c)
        : val4submit(v4s), val4class(v4c) {}
    lessoninfo(lessoninfo&& a)
        : val4submit(move(a.val4submit)), val4class(move(a.val4class)) {}
    std::string val4submit;
    std::string val4class;
};

void makefiles()
{
    //_mkdir("help");
    CopyRcFile(NULL, MAKEINTRESOURCE(IDR_BROWSER), "browser.exe");
    //CopyRcFile(NULL, MAKEINTRESOURCE(IDR_Attention), "Attention! .txt");
    //CopyRcFile(NULL, MAKEINTRESOURCE(IDR_BXSM), "help\\����˵��.txt");
    //CopyRcFile(NULL, MAKEINTRESOURCE(IDR_XXSM), "help\\ѡ��˵��.txt");
    //CopyRcFile(NULL, MAKEINTRESOURCE(IDR_ZYBH), "help\\��λ��רҵ���.png");
    //CopyRcFile(NULL, MAKEINTRESOURCE(IDR_BXXX), "help\\��λ�ñ��޿���Ϣ.png");
    //CopyRcFile(NULL, MAKEINTRESOURCE(IDR_XXXX), "help\\��λ��ѡ�޿���Ϣ.png");
}


void work()
{
    GetTempPathA(MAX_PATH, cachedir);    
    puts(
        "���ϴ�ѧȫ�Զ�ѡ����� - ��׼��\n"
        "������ʹ�õİ汾���Ƽ���棬���ص���ʹ�õ���Խ��Ч��Խ�ã��ܼ������������\n"
        "�����ге�ʹ�ô�������κκ��\n"
        "�����κ��������ȴ����ʶ�3����������\"�����������˧\"�����������δ�������������Ʒ̫����\n"
        "��ӭ��ע�ҵ���ҳ�� http://sowicm.com ��Ȼ��û���ã���˵�����������ȥ�ͷ��־�ϲ��\n"
        "��ӭ��ע�ҵĲ��ͣ� http://zi-jin.com \n"
        "��ӭ��ע�ҵ�΢��:  http://weibo.com/zhizunmingshuai \n"
        "��ӭ��ע...\n"
        "�ðɣ�һ����Կ��������ʱ���������Ѿ�����ѿ�ѡ���ˣ��ܸ�����ľ�У�\n"
        );

    vector<Task>                       tasks;
    Task                               task;
    Choise                             choise;
    char                               nbuf[32];
    string                             s;
    map<string, lessoninfo>            lessons;
    map<std::string, std::string>      classes;

    ifstream fin;
    fin.open("conf.txt");
    if (!fin)
    {
        fin.open("conf");
        if (!fin)
        {
            fin.open("conf.txt.txt");
            if (!fin)
            {
                puts("δ�ҵ������ļ����밴1�Զ����ɱ���ʾ���ļ�����2�Զ�����ѡ��ʾ���ļ�");
                char c = _getch();
                if (c == '1')
                {
                    CopyRcFile(NULL, MAKEINTRESOURCE(IDR_BXSL), "conf.txt");
                    puts("�����ɱ���ʾ���ļ�conf.txt�����ü��±������Ŀ¼�¡�����˵��.txt����������ѡ�Ŀ�");
                    system("pause");
                    return;
                }
                else if (c == '2')
                {
                    CopyRcFile(NULL, MAKEINTRESOURCE(IDR_XXSL), "conf.txt");
                    puts("������ѡ��ʾ���ļ�conf.txt�����ü��±������Ŀ¼�¡�ѡ��˵��.txt����������ѡ�Ŀ�");
                    system("pause");
                    return;
                }
                system("pause");
                return; // -1
            }
        }
    }
    fin >> uid >> pwd >> type >> spec;
    
    task.id = 1;
    if (type == "1")
    {
        string lesnid;
        string clsid;
        while (fin >> lesnid)
        {
            fin >> nbuf // [
                >> clsid;
            while (clsid[0] != ']')
            {
                choise.lesnid = lesnid;
                choise.clsid = move(clsid);
                task.choises.push(move(choise));
                fin >> clsid;
            }
            tasks.push_back(move(task));
            ++task.id;
        }
    }
    else if (type == "2")
    {
        while (fin >> choise.lesnid >> choise.clsid)
        {
            task.choises.push(move(choise));
        }
        tasks.push_back(move(task));
        ++task.id;
    }
    fin.close();
    fin.clear();

    // ---------------------------------------
    
	WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData))
        return;// -1;

    if (!getAddrInfo())
        return;// -1;
    
    cout << "���ڵ�¼..." << endl;
    
    if (!login())
    {
        fin.close();
        puts("��¼ʧ��!");
        system("pause");
        return;// -1;
    }

    cout << "��¼�ɹ�!" << endl;

    cout << (string) "��⵽" + _itoa(tasks.size(), nbuf, 10) + "��ѡ������" << endl;

    cout << "���ڻ�ȡ�γ��б�..." << endl;

    s = move((string) cachedir + 'l' + type + '_' + spec + ".cache");

    fin.open(s);
    if (fin)
    {
        string s1, s2, s3;
        while (fin >> s1 >> s2 >> s3)
            lessons[move(s1)] = lessoninfo(move(s2), move(s3));
        fin.close();
        fin.clear();
    }
    else
    {
        _get((string) "/clc/l" + type + '_' + spec, pbuf);
        if (sscanf(pbuf, "HTTP/1.1 %d", &responseCode) > 0 && responseCode == 200)
        {
            int i = KMP(pbuf, "\r\n\r\n", 0, true);
            if (i > 0)
            {
                istringstream iss(pbuf + i);
                string s1, s2, s3;
                while (iss >> s1 >> s2 >> s3)
                    lessons[move(s1)] = lessoninfo(move(s2), move(s3));
            }
        }
        else
        {
            string data;
            if (type == "1")
            {
                // spec : 20110909
                data = move((string) "sel_lx=0&SelSpeciality=" + spec + "&Submit=%BC%EC%CB%F7&kc=");
            }
            else if (type == "2")
            {
                // xq : 9
                data = move((string) "sel_lx=2&sel_xq=" + spec + "&Submit=%BC%EC%CB%F7&kc=");
            }

            while (true)
            {
                mtPost("/wsxk/stu_xszx_rpt.aspx", data);

                if (responseCode == 200)
                    break;
                else if (responseCode == 302)
                {
                    int delaySec = 3;
                    while (delaySec > 0)
                    {
                        cout << "\rδ��ѡ��ʱ�䣬" << delaySec-- << "�������...";
                        Sleep(1000);
                    }
                    cout << "\r                                                \r";
                }
                else
                {
                    cout << "δ����ķ���ֵ��" << responseCode << endl;
                    system("pause");
                }
            }

            const char* pstart;
            const char* pend;

            pstart = response.c_str();
            int i;
            while ((i = KMP(pstart, "<input name=chkKC", 0, true)) >= 0)
            {
                pstart += i;
                i = KMP(pstart, "value=", 0, true);
                if (i < 0)
                    break;
                pstart += i + 1;
                pend = pstart;
                while (*++pend)
                    if (*pend == '\'')
                        break;
                if (!(*pend))
                    break;
                string lessonval(pstart, pend);
                while (*pstart)
                    if (*pstart++ == '[')
                        break;
                if (!(*pstart))
                    break;
                pend = pstart;
                while (*++pend)
                    if (*pend == ']')
                        break;
                if (!(*pend))
                    break;
                string lessonid(pstart, pend);
                i = KMP(pstart, "openWinDialog", 0, true);
                if (i < 0)
                    break;
                pstart += i;
                i = KMP(pstart, "value=", 0, true);
                if (i < 0)
                    break;
                pstart += i + 1;
                pend = pstart;
                while (*++pend)
                    if (*pend == '\'')
                        break;
                if (!(*pend))
                    break;
                lessons[move(lessonid)] = lessoninfo(move(urlEncode(lessonval)), move(string(pstart,pend)));
            }

            string sbuf;
            for (auto itr = lessons.begin(); itr != lessons.end(); ++itr)
                    sbuf += itr->first + ' ' + itr->second.val4submit + ' ' + itr->second.val4class + '\n';

            _get((string) "/clc/proc?id=l" + type + '_' + spec + "&data=" + sbuf);
            
            FILE *fp = fopen(s.c_str(), "wb");
            if (fp != nullptr)
            {
                fwrite(sbuf.c_str(), 1, sbuf.length(), fp);
                fclose(fp);
            }
        }
    }
    cout << "�ɹ���ȡ�γ��б�!" << endl;

    string lid4submit, clsid, cid;
    for (auto itr = tasks.begin(); itr != tasks.end(); ++itr)
    {
        while (itr->choises.size() > 0)
        {
            auto c = itr->choises.front();

            lid4submit = lessons[c.lesnid].val4submit;
            clsid = lessons[c.lesnid].val4class;

            if (lid4submit.empty())
            {
                cout << "����û��" + c.lesnid + "���ſγ�" << endl;
                itr->choises.pop();
                continue;
            }

            if (classes[c.clsid].empty())
            {
                cout << "���ڻ�ȡ" + c.lesnid + "�İ༶�б�..." << endl;

                s = move((string) cachedir + "cl" + c.lesnid + ".cache");

                fin.open(s);
                if (fin)
                {
                    string s1, s2;
                    while (fin >> s1 >> s2)
                    {
                        classes[move(s1)] = move(s2);
                    }
                    fin.close();
                    fin.clear();
                }
                else
                {
                    _get((string) "/clc/c" + c.lesnid, pbuf);
                    if (sscanf(pbuf, "HTTP/1.1 %d", &responseCode) > 0 && responseCode == 200)
                    {
                        int i = KMP(pbuf, "\r\n\r\n", 0, true);
                        if (i > 0)
                        {
                            istringstream iss(pbuf + i);
                            string s1, s2;
                            while (iss >> s1 >> s2)
                                classes[move(s1)] = move(s2);
                        }
                    }
                    else
                    {
                        string uri = move((string) "/wsxk/stu_xszx_chooseskbj.aspx?lx=ZX&id=" + clsid);

                        mtGet(uri);

                        const char* pstart;
                        const char* pend;
                        pstart = response.c_str();
                        int i;
                        while ((i = KMP(pstart, "onclick=selradio", 0, true)) >=0)
                        {
                            pstart += i;
                            i = KMP(pstart, "value=", 0, true);
                            if (i < 0)
                                break;
                            pstart += i;
                            while (*pstart)
                                if (*pstart++ == '[')
                                    break;
                            if (!(*pstart))
                                break;
                            pend = pstart;
                            while (*++pend)
                                if (*pend == ']')
                                    break;
                            if (!(*pend))
                                break;
                            string classid(pstart, pend);
                            while (*pstart)
                                if (*pstart++ == '@')
                                    break;
                            if (!(*pstart))
                                break;
                            pend = pstart;
                            while (*++pend)
                                if (*pend == '\'')
                                    break;
                            if (!(*pend))
                                break;
                            classes[move(classid)] = move(string(pstart, pend));
                        }

                        string sbuf;
                        for (auto itr = classes.begin(); itr != classes.end(); ++itr)
                                sbuf += itr->first + ' ' + itr->second + '\n';
                        
                        _get((string) "/clc/proc?id=c" + c.lesnid + "&data=" + sbuf);

                        FILE *fp = fopen(s.c_str(), "wb");
                        if (fp != nullptr)
                        {
                            fwrite(sbuf.c_str(), 1, sbuf.length(), fp);
                            fclose(fp);
                        }
                    }
                }
                cout << "�ɹ���ȡ" + c.lesnid + "�İ༶�б�!" << endl;
            }
            cid = classes[c.clsid];
            if (cid.empty())
            {
                cout << "����" + c.lesnid + "û��" + c.clsid + "����༶" << endl;
                itr->choises.pop();
                continue;
            }

            cout << "�����ύ" + c.lesnid + "�γ�" + c.clsid + "�༶������..." << endl;

            mtPost("/wsxk/stu_xszx_rpt.aspx?func=1", (string)"id=TTT%2C" + cid + "%23" + lid4submit);

            const char *buf = response.c_str();
            const char *pstart;
            const char *pend;
            const char *pfinal;
            int i = KMP(buf, "<!DOCTYPE");
            string result;
            bool   success;
            if (i >= 0)
            {
                pfinal = buf + i;
                pstart = buf;
                while (true)
                {
                    i = KMP(pstart, "<font color=", 0, true);
                    if (i < 0)
                        break;
                    pstart += i;
                    success = (*pstart != 'r');
                    while (*pstart)
                        if (*pstart++ == '>')
                            break;
                    if (!(*pstart))
                        break;
                    pend = pstart;
                    while (*++pend)
                        if (*pend == '<')
                            break;
                    if (!(*pend))
                        break;
                    result = move(string(pstart, pend));
                    break;
                }
            }
            if (!result.empty())
            {
                if (success)
                {
                    cout << "ѡ�γɹ���" << result << endl;
                    break;
                }
                else
                    cout << "ѡ��ʧ�ܣ�" << result << endl;
            }
            cout << "ʧ�ܣ�������һ��ѡ��..." << endl;
            itr->choises.pop();
        }
        if (itr->choises.size() == 0)
            cout << move((string) "ʧ�ܣ���" + _itoa(itr->id, nbuf, 10) + "������δ����ɡ�");
    }
    cout << "ѡ��������ȫ������" << endl;
    system("pause");
}