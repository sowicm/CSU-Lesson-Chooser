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
#include <WinSock2.h>
#include <string>
#include <iostream>
#include <fstream>
#include <thread>
#include <Windows.h>
#include "Person.h"
#include "LessonList.h"
#include "ClassList.h"
#include "Submiter.h"
#include "output.h"
#include "KMP.h"
#include "vercommon.h"
using namespace std;

Person::Person(const string& infofile)
    : filename(infofile), zone(spec)
{
    Task    task;
    Choise  choise;
    char    nbuf[32];

    logfile = "data\\logs\\log_" + filename;

	ifstream fin(ingdir + infofile);
    if (!fin)
    {
        what = "�������ļ�ʧ��!";
        return;
    }
    fin >> uid >> pwd >> type >> spec;

    log("���ڵ�¼...");
    
    if (!login())
    {
        fin.close();
        what = "��¼ʧ��!";
        log(what);
        return;
    }

    log("��¼�ɹ�!");
    
    needLessonlist(cookie, type, spec);

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
            needClassList(cookie, choise.lesnid);
            task.choises.push(move(choise));
        }
        tasks.push_back(move(task));
        ++task.id;
    }
    fin.close();

    numTask = tasks.size();
    string s = move((string) "��⵽" + _itoa(numTask, nbuf, 10) + "��ѡ������");
    log(s);
    
    numTaskDone    = 0;
    numTaskSuccess = 0;
    numTaskFailed  = 0;

    memset(bTaskDone, 0, sizeof(bTaskDone));
    memset(bTaskHas,  0, sizeof(bTaskHas));

    thread checker;
    while (true)
    {
        try
        {
            checker = thread(&Person::checkerEntry, this);
        }
        catch (...)
        {
            Sleep(200);
            continue;
        }
        break;
    }
    thread submiter;
    while (true)
    {
        try
        {
            submiter = thread(submitMain, this);
        }
        catch (...)
        {
            Sleep(200);
            continue;
        }
        break;
    }
   
    checker.join();
    submiter.join();
    
    log("����");

    ShellExecute(NULL, "open", logfile.c_str(), NULL, NULL, SW_NORMAL);

    what = move((string) "����" + _itoa(numTask, nbuf, 10) + "�����񣬳ɹ�" + _itoa(numTaskSuccess, nbuf, 10) + "����ʧ��" + _itoa(numTaskFailed, nbuf, 10) +"��");

#if 0
    vector<thread*> threads;
    thread*         pthr;
    for (auto itask = tasks.begin(); itask != tasks.end(); ++itask)
    {
        while (true)
        {
            pthr = new thread(submitMain, cookie, *itask);
            if (pthr != nullptr)
            {
                threads.push_back(pthr);
                break;
            }
            Sleep(AllocDelay);
        }
    }
    for (auto itr = threads.begin(); itr != threads.end(); ++itr)
    {
        pthr = *itr;
        pthr->join();
        delete pthr;
    }
#endif
}

bool Person::taskdone(int tid)
{
    lock_guard<mutex> lk(msubmitTask);
    return bTaskDone[tid];
}
bool Person::taskhas(int tid)
{
    lock_guard<mutex> lk(msubmitTask);
    return bTaskHas[tid];
}
bool Person::submitdone()
{
    lock_guard<mutex> lk(msubmitTask);
    return numTaskDone >= numTask;
}

void Person::checker_add(SubmitInfo&& e)
{
    lock_guard<mutex> lk(msubmitTask);
    if (bTaskDone[e.tid] || bTaskHas[e.tid])
        return;
    submitTask.push_back(e);
    bTaskHas[e.tid] = true;
}

void Person::checker_failed(int tid)
{
    lock_guard<mutex> lk(msubmitTask);
    char nbuf[32];
    string s = move((string) "ʧ�ܣ���" + _itoa(tid, nbuf, 10) + "������δ����ɡ�");
    log(s);
    if (!bTaskDone[tid])
    {
        bTaskDone[tid] = true;
        ++numTaskDone;
        ++numTaskFailed;
    }
}

void Person::submiter_query(vector<SubmitInfo>& recv)
{
    lock_guard<mutex> lk(msubmitTask);
    recv = move(submitTask);
}

void Person::submiter_back(const vector<SubmitResult>& res)
{
    lock_guard<mutex> lk(msubmitTask);
    for (auto itr = res.begin(); itr != res.end(); ++itr)
    {
        bTaskHas[itr->tid] = false;
        log(itr->success ? "ѡ�γɹ���" : "ѡ��ʧ��:" + itr->result);
        if (itr->success && !bTaskDone[itr->tid])
        {
            bTaskDone[itr->tid] = true;
            ++numTaskDone;
            ++numTaskSuccess;
        }
    }
    for (auto itr = submitTask.begin(); itr != submitTask.end();)
    {
        if (bTaskDone[itr->tid])
        {
            itr = submitTask.erase(itr);
        }
        else
        {
            ++itr;
        }
    }
}

void Person::checkerEntry()
{
    try
    {
        log("���ڻ�ȡ�γ��б�...");
        string s = move((string) type + "_" + spec);
        while (!lessonsdone.has(s))
        {
            Sleep(200);
        }

        log("�ɹ���ȡ�γ����ݣ�");
        SubmitInfo si;
        int n = tasks.size();
        bool first = true;
        while (!submitdone())
        {
            if (first)
                first = false;
            else
                Sleep(1000);

            for (auto itr = tasks.begin(); itr != tasks.end(); ++itr)
            {
                if (taskdone(itr->id) || taskhas(itr->id))
                    continue;

                if (itr->choises.size() < 1)
                {
                    checker_failed(itr->id);
                    continue;
                }

                auto c = itr->choises.front();
                mlessons.lock();
                si.lid4submit = lessons[c.lesnid].val4submit;
                si.lid4show   = c.lesnid;
                mlessons.unlock();
                if (si.lid4submit.empty())
                {
                    log("����û��" + c.lesnid + "���ſγ�");
                    itr->choises.pop();
                    continue;
                }
                log("���Ի�ȡ" + c.lesnid + "�İ༶�б�...");
                if (!classesdone.has(c.lesnid))
                    continue;

                log("�ɹ���ȡ" + c.lesnid + "�İ༶�б�!");
                mclasses.lock();
                si.cid = classes[c.clsid];
                mclasses.unlock();
                if (si.cid.empty())
                {
                    log("����" + c.lesnid + "û��" + c.clsid + "����༶");
                    itr->choises.pop();
                    continue;
                }
                si.tid = itr->id;
                checker_add(move(si));
                log("�����ύ" + c.lesnid + "�γ�" + c.clsid + "�༶������...");
                itr->choises.pop();
            }
        }
    }
    catch (...)
    {
        S_MSG("CK - error");
    }
}

extern addrinfo* pmai;

bool Person::login()
{
#if Use_Cookie
    string cookiepath = move((string) cachedir + 'c' + uid + ".cache");

    ifstream fin(cookiepath);
    if (fin)
    {
        fin >> cookie;
        fin.close();
        if (!cookie.empty())
            return true;
    }
#endif

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
#if Use_Cookie
        ofstream fout(cookiepath);
        if (fout)
        {
            fout << cookie;
            fout.close();
        }
#endif
        return true;
    }
    return false;
}

void Person::log(const string& str)
{
    lock_guard<mutex> lk(mlog);
    ofstream fout(logfile, ios::app | ios::binary);
    time_t tt = time(0);
    tm* t = localtime(&tt);
    if (fout)
    {
        fout << t->tm_mon + 1 << '/' << t->tm_mday << '/' <<  t->tm_year + 1900 << ' ' << t->tm_hour << ':' << t->tm_min << ':' << t->tm_sec << ' ' << str << "\r\n";
        fout.close();
    }
}

mutex mres;

void personMain(string fname)
{
    Person  person(fname);

    string fpath = move((string) ingdir + fname);
    
    MoveFile(fpath.c_str(), ((string)donedir + fname).c_str());

    mres.lock();
    ofstream fout("result.txt", ios::app);
    if (fout)
    {
        fout << fname + person.what << "\r\n";
        fout.close();
    }
    mres.unlock();
}