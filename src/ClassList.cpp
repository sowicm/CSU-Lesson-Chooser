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
#include "ClassList.h"
#include "LessonList.h"
#include "RequesterBase.h"
#include "output.h"
#include "urlEncode.h"
#include "thrname.h"
#include "Threads.h"
#include "KMP.h"
#include "vercommon.h"
using namespace std;


mutex                         mclasses;
map<std::string, std::string> classes;
thrname                       classesdone;
static thrname                clslis;

class ClassList : public RequesterBase
{
public:
                                    ClassList(const string& cookie, const string& lesnid);


};

ClassList::ClassList(const string& c, const string& lesnid)
{
    cookie = c;
    string clsid;

    while (true)
    {
        mlessons.lock();
        clsid = lessons[lesnid].val4class;
        mlessons.unlock();
        if (clsid.empty())
        {
            Sleep(200);
            continue;
        }
        break;
    }
    
    string uri = move((string) "/wsxk/stu_xszx_chooseskbj.aspx?lx=ZX&id=" + clsid);

    mtGet(uri);

    map<std::string, std::string> classes;

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

    _get((string) "/clc/proc?id=c" + lesnid + "&data=" + sbuf);


    string fname = move((string) cachedir + "cl" + lesnid + ".cache");
    FILE *fp = fopen(fname.c_str(), "wb");
    if (fp != nullptr)
    {
        fwrite(sbuf.c_str(), 1, sbuf.length(), fp);
        fclose(fp);
    }


    mclasses.lock();
    for (auto itr = classes.begin(); itr != classes.end(); ++itr)
        ::classes.insert(move(*itr));
    mclasses.unlock();
}

void classListMain(const string& cookie, string lesnid)
{
    ClassList cl(cookie, lesnid);

    classesdone.add(move(lesnid));
}

void needClassList(const std::string& cookie, const std::string& lesnid)
{
    if (!clslis.add(lesnid))
        return;

    string fname = move((string) cachedir + "cl" + lesnid + ".cache");
    
    string s1, s2;
    ifstream fin(fname);    
    if (fin)
    {
        mclasses.lock();
        while (fin >> s1 >> s2)
            ::classes[move(s1)] = move(s2);
        mclasses.unlock();
        fin.close();
        classesdone.add(lesnid);
        return;
    }

    char pbuf[MAX_HtmlSize];
    _get((string) "/clc/c" + lesnid, pbuf);
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
                mclasses.lock();
                while (iss >> s1 >> s2)
                {
                    fout << s1 + ' ' + s2 + '\n';
                    classes[move(s1)] = move(s2);
                }
                mclasses.unlock();
                fout.close();
            }
            else
            {
                mclasses.lock();
                while (iss >> s1 >> s2)
                    classes[move(s1)] = move(s2);
                mclasses.unlock();
            }
            classesdone.add(lesnid);
            return;
        }
    }

    while (true)
    {
        try
        {
            thread(classListMain, cookie, lesnid).detach();
        }
        catch (...)
        {
            Sleep(200);
            continue;
        }
        break;
    }
}