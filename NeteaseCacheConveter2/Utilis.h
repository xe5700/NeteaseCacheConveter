#pragma once
#include "stdafx.h"
extern const wchar_t* _UTF8toUTF16(const char* source);
extern const char* _UTF16toUTF8(const wchar_t* source);
extern const char* _UTF16toGBK(const wchar_t* source);
extern void FormatFileName(wstring* filename);
typedef GenericDocument<UTF16<> > Document16;