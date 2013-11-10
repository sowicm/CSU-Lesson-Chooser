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

#include <string>
#include <mutex>
#include <map>
#include "thrname.h"

struct lessoninfo {
    lessoninfo() {}
    lessoninfo(std::string&& v4s, std::string&& v4c)
        : val4submit(v4s), val4class(v4c) {}
    lessoninfo(lessoninfo&& a)
        : val4submit(move(a.val4submit)), val4class(move(a.val4class)) {}
    std::string val4submit;
    std::string val4class;
};

extern std::mutex                        mlessons;
extern std::map<std::string, lessoninfo> lessons;
extern thrname                           lessonsdone;

void needLessonlist(const std::string& cookie, const std::string& type, const std::string& spec);