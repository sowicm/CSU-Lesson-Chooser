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
#include <Windows.h>
#include <fstream>
#include <vector>
#include <thread>
#include <mutex>
#include <KMP.h>
#include "Submiter.h"
#include "LessonList.h"
#include "ClassList.h"
#include "Person.h"
using namespace std;

extern addrinfo* pai;

class Submiter
{
public:
    Submiter(Person* pp);

private:
    void                        stRequestEntry();
    inline bool                 done() { return m_pp->submitdone(); }
    void                        update(set<SubmitInfo>& outinfos, string& outh, string& outd);
    void                        success(const vector<SubmitResult>& res);

    void                        SocketOpen(SOCKET& sck);
    void                        SocketClose(SOCKET& sck);

private:
    mutex                       msck;
    Person*                     m_pp;
    string                      cookie;
    mutex                       mupdate;
    set<SubmitInfo>             infos;
    string                      m_header;
    string                      m_data;
    set<SOCKET>                 m_opendedSockets;
    bool                        todel[1024];
};

void submitMain(Person* pp)
{
    Submiter submit(pp);
}

void Submiter::SocketOpen(SOCKET& sck)
{
    lock_guard<mutex> lk(msck);
    if (done())
    {
        sck = INVALID_SOCKET;
        return;
    }
    sck = socket(AF_INET, SOCK_STREAM, 0);
    if (sck != INVALID_SOCKET)
        m_opendedSockets.insert( sck );
}

void Submiter::SocketClose(SOCKET& sck)
{
    lock_guard<mutex> lk(msck);
    m_opendedSockets.erase( sck );
    closesocket(sck);
    sck = INVALID_SOCKET;
}

void Submiter::update(set<SubmitInfo>& outinfos, string& outh, string& outd)
{
    lock_guard<mutex>            lk(mupdate);
    vector<SubmitInfo>           recv;

    m_pp->submiter_query(recv);
    if (recv.size() > 0)
    {
        for (auto itr = recv.begin(); itr != recv.end(); ++itr)
        {
            m_data += "%2C" + itr->cid + "%23" + itr->lid4submit;
            infos.insert(move(*itr));
        }
        char nbuf[32];
        m_header = move((string)
            "POST /wsxk/stu_xszx_rpt.aspx?func=1 HTTP/1.1\r\n"
            "Content-Type: application/x-www-form-urlencoded\r\n"
            "Host: csujwc.its.csu.edu.cn\r\n"
            "Content-Length: " + _itoa(m_data.length(), nbuf, 10) + "\r\n"
            "Connection: Keep-Alive\r\n"
            "Cookie: " + cookie + "\r\n"
            "\r\n");
    }
    outinfos = infos;
    outh     = m_header;
    outd     = m_data;
}

void Submiter::success(const vector<SubmitResult>& res)
{
    lock_guard<mutex>    lk(mupdate);
    memset(todel, 0, sizeof(todel));
    for (auto itr = res.begin(); itr != res.end(); ++itr)
    {
        todel[itr->tid] = true;
    }
    for (auto itr = infos.begin(); itr != infos.end();)
    {
        if (todel[itr->tid])
        {
            itr = infos.erase(itr);
        }
        else
        {
            ++itr;
        }
    }
    m_pp->submiter_back(res);

    m_data = "id=TTT";
    for (auto itr = infos.begin(); itr != infos.end(); ++itr)
    {
        m_data += "%2C" + itr->cid + "%23" + itr->lid4submit;
    }
    char nbuf[32];
    m_header = move((string)
        "POST /wsxk/stu_xszx_rpt.aspx?func=1 HTTP/1.1\r\n"
        "Content-Type: application/x-www-form-urlencoded\r\n"
        "Host: csujwc.its.csu.edu.cn\r\n"
        "Content-Length: " + _itoa(m_data.length(), nbuf, 10) + "\r\n"
        "Connection: Keep-Alive\r\n"
        "Cookie: " + cookie + "\r\n"
        "\r\n");
}

Submiter::Submiter(Person* pp)
    : m_pp(pp)
{
    cookie = pp->getCookie();
    m_data = "id=TTT";

    thread                      threads[MAX_ThreadForOneRequest];
    //vector<thread*>             threads;
    //vector<thread*>::iterator   itr;
    //thread*                     pthr;
    //DWORD					    dwExitCode;
    //int                         tries;
    int i = 0;
    while (!pp->submitdone() && i < MAX_ThreadForOneRequest)
    {
        try
        {
            threads[i++] = thread(&Submiter::stRequestEntry, this);
            //Sleep(ThreadCreateDelay);
        }
        catch (...)
        {
            --i;
            //S_MSG("SM - MTR - Start error");
        }
    }

    /*
    try
    {
    for (itr = threads.begin(); itr != threads.end();)
    {
    pthr = *itr;
    GetExitCodeThread(pthr->native_handle()._Hnd, &dwExitCode);
    if (dwExitCode != STILL_ACTIVE)
    {
    pthr->join();
    delete pthr;
    itr = threads.erase(itr);
    }
    else
    ++itr;
    }
    }
    catch (...)
    {
    S_MSG("SM - MTR - Dynamic recycle error");
    }
    */
    while (!pp->submitdone())
    {
        Sleep(300);
    }

    msck.lock();
    for (auto itr = m_opendedSockets.begin(); itr != m_opendedSockets.end(); ++itr)
    {
        closesocket( *itr );
    }
    msck.unlock();

    for (i = 0; i < MAX_ThreadForOneRequest; ++i)
        if (threads[i].joinable())
            threads[i].join();
}


void Submiter::stRequestEntry()
{
    SOCKET sck = INVALID_SOCKET;
    bool first = true;

    char *buf = nullptr;
    int received;

    set<SubmitInfo>    infos;
    string             data;
    string             header;
    const char*        pstart;
    const char*        pend;
    const char*        pfinal;

    try
    {
        while (!done() && (buf = new char[MAX_HtmlSize]) == nullptr)
        {
            Sleep(RequestThreadDelay);
        }
        while (!done())
        {
            if (first)
                first = false;
            else
                Sleep(RequestThreadDelay);

            update(infos, header, data);
            if (infos.size() < 1)
                continue;

            SocketOpen(sck);
            if (INVALID_SOCKET == sck)
                continue;
            try
            {
                if (connect(sck, pai->ai_addr, pai->ai_addrlen) == SOCKET_ERROR)
                    throw L"";

                if (send(sck, header.c_str(), header.length(), 0) == SOCKET_ERROR)
                    throw L"";

                if (send(sck, data.c_str(), data.length(), 0) == SOCKET_ERROR)
                    throw L"";
#if 0
                int t;
                while ((t = recv(sck, buf + received, MAX_HtmlSize - received, 0)) > 0)
                {
                    received += t;
                }
#endif
                if ((received = recv(sck, buf, MAX_HtmlSize, 0)) == SOCKET_ERROR)
                    throw L"";

                buf[received] = '\0';
            }
            catch (...)
            {
                SocketClose(sck);
                continue;
            }
            SocketClose(sck);
            if (received > 0)
            {
                int i = KMP(buf, "\r\n\r\n", 0, true);
                if (i < 0)
                    continue;
                pstart += i;
                char filename[256];
                sprintf(filename, "data\\logs\\r_%llu.txt", GetTickCount64());
                ofstream fout(filename);
                if (fout.is_open())
                {
                    fout << pstart;
                    fout.close();
                }

                int status;
                if (sscanf(buf, "HTTP/1.1 %d", &status) > 0)
                {
                    //if (status == 200 || status == 302)
                    if (status >= 200 && status < 400)
                    {
                        vector<SubmitResult> res;
                        SubmitResult         sr;
                        SubmitInfo           in;

                        i = KMP(buf, "<!DOCTYPE");
                        if (i >= 0)
                        {
                            pfinal = buf + i;
                            pstart = buf;
                            while (pstart < pfinal)
                            {
                                i = KMP(pstart, "<font color=", 0, true);
                                if (i < 0)
                                    break;
                                pstart += i;
                                sr.success = (*pstart != 'r');
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
                                sr.result = move(string(pstart, pend));
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
                                in.lid4show = move(string(pstart, pend));
                                auto itr = infos.find(in);
                                if (itr != infos.end())
                                {
                                    sr.tid = itr->tid;
                                    res.push_back(sr);
                                }
                            }
                            if (res.size() > 0)
                                success(res);
                        }
                    }
                }
            }
        }
    }
    catch (...)
    {
    }
    if (buf != nullptr)
        delete[] buf;
}