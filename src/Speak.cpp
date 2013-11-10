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
#include "Speak.h"
#include <sapi.h>
#include <sphelper.h>
#include <thread>

#pragma comment(lib, "sapi.lib")

void _speak(const wchar_t* str, bool del = false)
{
    CoInitialize(NULL);
    try
    {
        ISpVoice *pVoice;
        if (FAILED( CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void**)&pVoice) ))
            throw L"";
        CComPtr<ISpObjectToken> pOldToken;
        if (FAILED( pVoice->GetVoice(&pOldToken) ))
            throw L"";
        ISpObjectToken *pToken;
        if (FAILED( SpFindBestToken(SPCAT_VOICES, L"language=804", L"gender=female", &pToken) ))
            throw L"";
        if (pToken != pOldToken)
        {
            pVoice->SetRate(2);
            if (FAILED( pVoice->SetVoice(pToken) ))
                throw L"";
            pVoice->Speak(str, SPF_IS_XML, NULL);
            pVoice->Release();
        }
    }
    catch (...)
    {
    }
    if (del)
        delete[] (wchar_t*)str;
    CoUninitialize();
}

void speak(const wchar_t* str, bool sync)
{
    if (sync)
    {
        try
        {
            wchar_t *buf = new wchar_t[wcslen(str) + 1];
            wcscpy(buf, str);
            std::thread(_speak, buf, true).detach();
        }
        catch (...)
        {
        }
    }
    else
        _speak(str, false);
}