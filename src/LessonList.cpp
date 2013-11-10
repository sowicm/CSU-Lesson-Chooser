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
#include <vector>
#include <thread>
#include <map>
#include <fstream>
#include <sstream>
#include <KMP.h>
#include "LessonList.h"
#include "RequesterBase.h"
#include "output.h"
#include "thrname.h"
#include "urlencode.h"
#include "vercommon.h"
using namespace std;

extern time_t           didTime;
extern mutex            mdidtime;

mutex                   mlessons;
map<string, lessoninfo> lessons;
static thrname          lesnlis;
thrname                 lessonsdone;

class LessonList : public RequesterBase
{
public:
                                    LessonList(const string& cookie, const string& type, const string& spec);


};

LessonList::LessonList(const string& c, const string& type, const string& spec)
{
    char nbuf[32];
    cookie = c;
    string data;
    if (type == "1")
    {
        data = move((string) "sel_lx=0&SelSpeciality=" + spec + "&Submit=%BC%EC%CB%F7&kc=");
    }
    else if (type == "2")
    {
        data = move((string) "sel_lx=2&sel_xq=" + spec + "&Submit=%BC%EC%CB%F7&kc=");
    }

    while (true)
    {
        mtPost("/wsxk/stu_xszx_rpt.aspx", data);

        if (responseCode == 200)
            break;
        else if (responseCode == 302)
        {
            mdidtime.lock();
            if (didTime + 3 < time(0))
            {
                puts("未到选课时间, 3s后重试...");
                didTime = time(0);
            }
            mdidtime.unlock();
#if WaitWhileNotBegin
            Sleep(3000);
#endif
        }
        else
        {
            string s = move((string) "未处理的返回值: " + _itoa(responseCode, nbuf, 10));
            mdidtime.lock();
            puts(s.c_str());
            mdidtime.unlock();
        }
    }
    
    const char* pstart;
    const char* pend;

    map<string, lessoninfo> lessons;

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
    
    string fname = move((string) cachedir + 'l' + type + '_' + spec + ".cache");
    FILE *fp = fopen(fname.c_str(), "wb");
    if (fp != nullptr)
    {
        fwrite(sbuf.c_str(), 1, sbuf.length(), fp);
        fclose(fp);
    }    
    
    mlessons.lock();
    for (auto itr = lessons.begin(); itr != lessons.end(); ++itr)
        ::lessons.insert(move(*itr));
    mlessons.unlock();
}

void lessonListMain(const string& cookie, const string& type, const string& spec)
{
    LessonList ll(cookie, type, spec);

    //lesnlis.del(type + "_" + spec);
    lessonsdone.add(type + "_" + spec);
}

void needLessonlist(const string& cookie, const std::string& type, const std::string& spec)
{
    string id = move((string) type + "_" + spec);
    if (!lesnlis.add(id))
        return;

    string fname = move((string) cachedir + 'l' + type + '_' + spec + ".cache");

    ifstream fin(fname);
    string s1, s2, s3;
    if (fin)
    {
        mlessons.lock();
        while (fin >> s1 >> s2 >> s3)
            ::lessons[move(s1)] = lessoninfo(move(s2), move(s3));
        mlessons.unlock();
		fin.close();
        lessonsdone.add(move(id));
        return;
    }

    char pbuf[MAX_HtmlSize];
    _get((string) "/clc/l" + type + '_' + spec, pbuf);
    int responseCode;
    if (sscanf(pbuf, "HTTP/1.1 %d", &responseCode) > 0 && responseCode == 200)
    {
        int i = KMP(pbuf, "\r\n\r\n", 0, true);
        if (i > 0)
        {
            istringstream iss(pbuf + i);
            ofstream fout(fname);
            if (fout)
            {
                mlessons.lock();
                while (iss >> s1 >> s2 >> s3)
                {
                    fout << s1 + ' ' + s2 + ' ' + s3 + '\n';
                    ::lessons[move(s1)] = lessoninfo(move(s2), move(s3));
                }
                mlessons.unlock();
                fout.close();
            }
            else
            {
                mlessons.lock();
                while (iss >> s1 >> s2 >> s3)
                    ::lessons[move(s1)] = lessoninfo(move(s2), move(s3));
                mlessons.unlock();
            }
            lessonsdone.add(move(id));
            return;
        }
    }

    while (true)
    {
        try
        {
            thread(lessonListMain, cookie, type, spec).detach();
        }
        catch (...)
        {
            Sleep(200);
            continue;
        }
        break;
    }
}