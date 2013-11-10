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
    if (KMP(response.c_str(), "正在加载权限数据...") >= 0)
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
    //CopyRcFile(NULL, MAKEINTRESOURCE(IDR_BXSM), "help\\必修说明.txt");
    //CopyRcFile(NULL, MAKEINTRESOURCE(IDR_XXSM), "help\\选修说明.txt");
    //CopyRcFile(NULL, MAKEINTRESOURCE(IDR_ZYBH), "help\\如何获得专业编号.png");
    //CopyRcFile(NULL, MAKEINTRESOURCE(IDR_BXXX), "help\\如何获得必修课信息.png");
    //CopyRcFile(NULL, MAKEINTRESOURCE(IDR_XXXX), "help\\如何获得选修课信息.png");
}


void work()
{
    GetTempPathA(MAX_PATH, cachedir);    
    puts(
        "中南大学全自动选课软件 - 标准版\n"
        "你正在使用的版本是云计算版，其特点是使用的人越多效果越好，能减轻服务器负担\n"
        "请自行承担使用此软件的任何后果\n"
        "出现任何问题请先大声朗读3遍万能咒语\"张日月明你好帅\"，如果问题仍未解决，那是你人品太差了\n"
        "欢迎关注我的主页： http://sowicm.com 虽然还没做好，但说不定你哪天进去就发现惊喜了\n"
        "欢迎关注我的博客： http://zi-jin.com \n"
        "欢迎关注我的微博:  http://weibo.com/zhizunmingshuai \n"
        "欢迎关注...\n"
        "好吧，一般而言看到这里的时候软件早就已经帮你把课选好了，很给力有木有！\n"
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
                puts("未找到任务文件，请按1自动生成必修示例文件，按2自动生成选修示例文件");
                char c = _getch();
                if (c == '1')
                {
                    CopyRcFile(NULL, MAKEINTRESOURCE(IDR_BXSL), "conf.txt");
                    puts("已生成必修示例文件conf.txt，请用记事本打开配合目录下“必修说明.txt”设置你想选的课");
                    system("pause");
                    return;
                }
                else if (c == '2')
                {
                    CopyRcFile(NULL, MAKEINTRESOURCE(IDR_XXSL), "conf.txt");
                    puts("已生成选修示例文件conf.txt，请用记事本打开配合目录下“选修说明.txt”设置你想选的课");
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
    
    cout << "正在登录..." << endl;
    
    if (!login())
    {
        fin.close();
        puts("登录失败!");
        system("pause");
        return;// -1;
    }

    cout << "登录成功!" << endl;

    cout << (string) "检测到" + _itoa(tasks.size(), nbuf, 10) + "个选课任务" << endl;

    cout << "正在获取课程列表..." << endl;

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
                        cout << "\r未到选课时间，" << delaySec-- << "秒后重试...";
                        Sleep(1000);
                    }
                    cout << "\r                                                \r";
                }
                else
                {
                    cout << "未处理的返回值：" << responseCode << endl;
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
    cout << "成功获取课程列表!" << endl;

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
                cout << "错误：没有" + c.lesnid + "这门课程" << endl;
                itr->choises.pop();
                continue;
            }

            if (classes[c.clsid].empty())
            {
                cout << "正在获取" + c.lesnid + "的班级列表..." << endl;

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
                cout << "成功获取" + c.lesnid + "的班级列表!" << endl;
            }
            cid = classes[c.clsid];
            if (cid.empty())
            {
                cout << "错误：" + c.lesnid + "没有" + c.clsid + "这个班级" << endl;
                itr->choises.pop();
                continue;
            }

            cout << "正在提交" + c.lesnid + "课程" + c.clsid + "班级的请求..." << endl;

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
                    cout << "选课成功：" << result << endl;
                    break;
                }
                else
                    cout << "选课失败：" << result << endl;
            }
            cout << "失败，尝试下一个选择..." << endl;
            itr->choises.pop();
        }
        if (itr->choises.size() == 0)
            cout << move((string) "失败：第" + _itoa(itr->id, nbuf, 10) + "个任务未能完成。");
    }
    cout << "选课任务已全部结束" << endl;
    system("pause");
}