#include "stdafx.h"
char* readJsonFile(FILE* F) {
	int size = 1024;
	char* out = (char*)malloc(size * sizeof(char*));
	int i = 0;
	while (char c = fgetc(F) != EOF) {
		if (i >= size) {
			size = size * 2;
			out = (char*)realloc(out, size * sizeof(char*));
		}
		out[i] = c;
		i++;
	}
	if (i >= size) {
		size = size + 1;
		out = (char*)realloc(out, size * sizeof(char*));
	}
	out[i] = '\0';
	return out;
}
const wchar_t* _UTF8toUTF16(const char* source) {
	StringStream sources(source);
	GenericStringBuffer<UTF16<> > target;

	while (sources.Peek() != '\0') {
		if (!Transcoder<UTF8<>, UTF16<>>::Transcode(sources, target)) {
			break;
		}
	}
	wchar_t* returninfo = new wchar_t[wcslen(target.GetString()) + 1];
	wcscpy(returninfo, target.GetString());
	return returninfo;
}
const char* _UTF16toUTF8(const wchar_t* source) {
	GenericStringStream<UTF16<>> sources(source);
	GenericStringBuffer<UTF8<> > target;

	while (sources.Peek() != '\0') {
		if (!Transcoder<UTF16<>, UTF8<>>::Transcode(sources, target)) {
			break;
		}
	}
	char* returninfo = new char[strlen(target.GetString()) + 1];
	strcpy(returninfo, target.GetString());
	return returninfo;
}
const char* _UTF16toGBK(const wchar_t* source) {
	DWORD len = WideCharToMultiByte(CP_ACP, NULL, source, -1, NULL, 0, NULL, FALSE);
	char* gbk = new char[len];
	if (WideCharToMultiByte(CP_ACP, NULL, source, -1, gbk, len, NULL, FALSE)) {
	return gbk;
	}
	delete[] gbk;
	return NULL;
}
char* gtu(char* utf8) {
	wchar_t* unicode;
	DWORD wsize = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	unicode = new wchar_t[wsize];
	if (wsize == 0)return NULL;
	if (MultiByteToWideChar(CP_UTF8, 0, utf8, -1, unicode, wsize)) {
		DWORD len = WideCharToMultiByte(CP_ACP, NULL, unicode, -1, NULL, 0, NULL, FALSE);
		char* gbk = new char[len];
		if (WideCharToMultiByte(CP_ACP, NULL, unicode, -1, gbk, len, NULL, FALSE)) {
			delete[] unicode;
			return gbk;
		}
		//system("pause");
		delete[] unicode;
		delete[] gbk;
		return NULL;
	}
	delete[] unicode;
	return NULL;
}
wchar_t* FormatFileName(wchar_t* filename) {
	size_t l=wcslen(filename);
	wchar_t *newfilename = new wchar_t[l];
	int n = 0;
	for (size_t i = 0; i < l; i++) {
		while (filename[i] == '\\' || filename[i] == '/' || filename[i] == ':' || filename[i] == '*' || filename[i] == '?' || filename[i] == '\"' || filename[i] == '<' || filename[i] == '>' || filename[i] == '|') {
			i++;
		}
		newfilename[n]=filename[i];
		n++;
	}
	newfilename[n] = '/0';
	return newfilename;
}
void ReplaceDelete(wstring* str, wchar_t find_data) {
	while (int i = str->find(find_data)!=-1) {
		str = &str->replace(i, 1, L"");
	}
}
void ReplaceDelete(wstring* str, wchar_t* find_data) {
	for (int i = 0; i < wcslen(find_data); i++) {
		ReplaceDelete(str, find_data[i]);
	}
}
void FormatFileName(wstring* filename) {
	ReplaceDelete(filename, L"\\/:*?\\<>|");
}

