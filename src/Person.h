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

#ifndef Person_h
#define Person_h

#include <string>
#include <vector>
#include <queue>
#include <mutex>
#include "RequesterBase.h"
#include "thrname.h"
#include "Submiter.h"

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

struct SubmitInfo {
                        SubmitInfo() {}
                        SubmitInfo(SubmitInfo&& a)
    {
        tid = a.tid;
        lid4show = move(a.lid4show);
        lid4submit = move(a.lid4submit);
        cid = move(a.cid);
    }
    bool                operator == (const SubmitInfo& a) const
    {
        return lid4show == a.lid4show;
    }
    bool                operator <  (const SubmitInfo& a) const
    {
        return lid4show < a.lid4show;
    }
    bool                operator >  (const SubmitInfo& a) const
    {
        return lid4show > a.lid4show;
    }

    int                 tid;
    //int                 sid;
    std::string         lid4show;
    std::string         lid4submit;
    std::string         cid;
};

class Person : public RequesterBase
{
public:
    Person(const std::string& infofile);

public:
    inline const std::string&               getCookie() const { return cookie; }
    bool                                    submitdone();
    void                                    submiter_query(std::vector<SubmitInfo>& recv);
    void                                    submiter_back(const std::vector<SubmitResult>& res);

    std::string                             what;

private:
    bool                                    login();

private:
    void                                    checkerEntry();

private:
    std::string const&                      filename;

private:
    bool                                    taskdone(int tid);
    bool                                    taskhas(int tid);

    void                                    checker_add(SubmitInfo&& e);
    void                                    checker_failed(int tid);
    std::vector<SubmitInfo>                 submitTask;
    std::mutex                              msubmitTask;
    bool                                    bTaskDone[1024];
    bool                                    bTaskHas[1024];
    int                                     numTaskDone;
    int                                     numTask;
    int                                     numTaskSuccess;
    int                                     numTaskFailed;

private:
    void                                    log(const std::string& str);
    std::string                             logfile;
    std::mutex                              mlog;

private:
    std::string                             uid;
    std::string                             pwd;
    std::string                             type;
    std::string                             spec;
    std::string&                            zone;
    std::vector<Task>                       tasks;
};

void personMain(std::string fname);

#endif