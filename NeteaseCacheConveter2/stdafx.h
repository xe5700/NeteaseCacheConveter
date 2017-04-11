// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include "curl\curl.h"
#include <string>
#include <iostream>
#include <vector>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
using std::string;
using std::cout;
using std::wcout;
using std::wstring;
using std::exception;
using std::vector;
using std::endl;
using namespace rapidjson;
extern const wchar_t* cache_path;
extern const wchar_t* output_path;
#include  <stdlib.h>
#include <Windows.h>
#include "Utilis.h"
#include <tag.h>
#include <fileref.h>
#include<stdarg.h>
#include <typeinfo>
#include <flacfile.h>
// TODO:  在此处引用程序需要的其他头文件
