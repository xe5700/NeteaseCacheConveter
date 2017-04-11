#pragma once
#include "stdafx.h"
enum cacheinfoerror
{
	Cache_OK,Read_IDX_ERROR, Read_INFO_ERROR, Size_Error
};
class exceptioncache : exception {
public:
	int eid;
	exceptioncache(int id) {
		eid=id;
	}
};
class cacheinfo {
private:
	ULONG64 SimplePow(ULONG64 X, UINT Y) {
		ULONG64 Z = X;
		for (UINT i = 0; i < Y; i++) {
			Z = Z*X;
		}
		return Z;
	}
	UINT NLength(ULONG64 n) {
		for (UINT i = 1;; i++) {
			ULONG64 n2 = SimplePow(10, i);
			if (n2 > n)return i;
		}
	}
	cacheinfoerror getMusicSize() {
		wstring fullCachePath = cache_path ;
		fullCachePath+=L"\\Cache\\";
		fullCachePath += cachefilename;
		fullCachePath += L".idx";
		FILE* json = _wfopen(fullCachePath.c_str(),L"r");
		if (!json) {
			return Read_IDX_ERROR;
		}
		Document d;
		char buf[1024];
		FileReadStream jfr(json, buf, sizeof(char) * 1024);
		d.ParseStream(jfr);
		fclose(json);
		if (d.HasMember("size")) {
			this->size= atoll(d["size"].GetString());
		} else {
			return Read_IDX_ERROR;
		}
		return Cache_OK;
	}
	cacheinfoerror getMusicFormat() {
		wstring fullCachePath = cache_path;
		fullCachePath += L"\\Cache\\";
		fullCachePath += cachefilename;
		fullCachePath += L".info";
		FILE* json = _wfopen(fullCachePath.c_str(), L"r");
		if (!json) {
			return Read_INFO_ERROR;
		}
		Document d;
		char buf[1024];
		FileReadStream jfr(json, buf, sizeof(char) * 1024);
		d.ParseStream(jfr);
		fclose(json);
		if (d.HasMember("format")) {
			this->format=(wchar_t*)_UTF8toUTF16(d["format"].GetString());
		} else {
			return Read_INFO_ERROR;
		}
		return Cache_OK;
	}
	wchar_t* getArrayName(Value d) {
		string names;
		for (UINT i2 = 0; i2<d.Size(); i2++) {
			names += d[i2]["name"].GetString();
			if (i2 + 1<d.Size())
				names += ";";
		}
		return (wchar_t*)_UTF8toUTF16(names.c_str());
	}
public:
	//Network infomation
	wchar_t* author;
	wchar_t* title;
	wchar_t* album;
	wchar_t* alias;
	//Local Infomation
	ULONG64 size;
	int id;
	wchar_t* cachefilename;
	wchar_t* md5;
	wchar_t* format;
	cacheinfo(wchar_t* name, ULONG64 size) {
		wstring default_name = name;
		wstring cachefilename=default_name.substr(0,default_name.find(L"."));
		this->cachefilename = new wchar_t[wcslen(cachefilename.c_str())+1];
		wcscpy(this->cachefilename, cachefilename.c_str());
		this->id=_wtoi(cachefilename.substr(0, cachefilename.find(L"-")).c_str());
		cacheinfoerror error = getMusicSize();
		if (error != Cache_OK) {
			throw error;
		}
		if (size != this->size) {
			throw Read_IDX_ERROR;
		}
		error = getMusicFormat();
		if (error != Cache_OK){
			throw error;
			return;
		}
	}
	~cacheinfo() {
		if (cachefilename != NULL)
			delete[] cachefilename;
		if (format != NULL)
			delete[] format;
		if (md5 != NULL)
			delete[] md5;
		if (author != NULL)
			delete[] author;
		if (title != NULL)
			delete[] title;
		if (album != NULL)
			delete[] album;
		if (alias != NULL)
			delete[] alias;
	}
	bool FindInfoInNeteaseJson(Value& d) {
			Value& songs = d["songs"];
			for (unsigned int i = 0; i < songs.Size(); i++) {
				int id = songs[i]["id"].GetInt();
				if (this->id	 == id) {
					const char* title = songs[i]["name"].GetString();
					this->title=(wchar_t*)_UTF8toUTF16(title);
					this->author = getArrayName(songs[i]["artists"].GetArray());
					const char* album= songs[i]["album"]["name"].GetString(); 
					this->album = (wchar_t*)_UTF8toUTF16(album);
					return true;
				}
			}
		return false;
	}
	wchar_t*  getsize() {
		wchar_t* size;
		size = new wchar_t[10];
		if (this->size < 1024) {
			_swprintf(size,L"%lldB",this->size);
		} else if (this->size < pow(1024, 2)) {
			_swprintf(size, L"%.2fKB", this->size/1024.0*1.00);
		} else if (this->size < pow(1024, 3)) {
			_swprintf(size, L"%.2fMB", this->size / 1024.0/1024.0*1.00);
		} else if (this->size < pow(1024, 4)) {
			_swprintf(size, L"%.2fGB", this->size / 1024.0 /1024.0 /1024.0*1.00);
		} else if (this->size < pow(1024, 5)) {
			_swprintf(size, L"%.2fTB", this->size / 1024.0 /1024.0 /1024.0*1.00);
		} else {
			double d = this->size / 1024.0 / 1024.0 / 1024.0 / 1024.0*1.00;
			_swprintf(size, L"%.2fPB", d);
		}
		return size;
	}
};