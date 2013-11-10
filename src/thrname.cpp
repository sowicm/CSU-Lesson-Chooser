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

#include "thrname.h"

using namespace std;

bool thrname::has(const string& s)
{
	lock_guard<mutex> lk(m);
	return d.find(s) != d.end();
}

bool thrname::add(const string& s)
{
	lock_guard<mutex> lk(m);
    if (d.find(s) != d.end()) // if has
        return false;
	d.insert(s);
    return true;
}

bool thrname::add(string&& s)
{
    lock_guard<mutex> lk(m);
    if (d.find(s) != d.end())
        return false;
    d.insert(s);
    return true;
}

void thrname::del(const string& s)
{
	lock_guard<mutex> lk(m);
	d.erase(s);
}