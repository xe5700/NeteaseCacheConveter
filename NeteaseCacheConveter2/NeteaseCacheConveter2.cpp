// NeteaseCacheConveter2.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "CacheFile.h"
#include "CPP_CURL.h"
const wchar_t* cache_path;
const wchar_t* output_path;
vector<cacheinfo*> CacheInfos;
wchar_t* DefaultLang;
const char* neteas_api = "http://music.163.com/api/song/detail/?ids=";
void ImportCache(WIN32_FIND_DATA data, ULONG64 size) {
	cacheinfo *info = NULL;
	try {
		info = new cacheinfo(data.cFileName, size);
		CacheInfos.push_back(info);
	}
	catch (cacheinfoerror e) {
		switch (e)
		{
		case Read_IDX_ERROR:
			wcout << L"This cache file's idx file can't read" << endl;
			break;
		case Read_INFO_ERROR:
			wcout << L"(" << data.cFileName << L") This cache file's info file can't read" << endl;
			break;
		case Size_Error:
			wcout << L"(" << data.cFileName << L") This cache file not all downloaded" << endl;
			break;
		default:
			break;
		}
		if(info!=NULL)
		delete info;
	}
}
void findcache() {
	wstring p = cache_path;
	p += L"\\Cache\\*.uc";
	WIN32_FIND_DATA data;
	HANDLE H = FindFirstFile(p.c_str(), &data);
	ULONG64 size = (data.nFileSizeHigh * (MAXDWORD + 1)) + data.nFileSizeLow;
	if (size == 0) {
		goto k;
	}
	ImportCache(data, size);
k:
	while (FindNextFile(H, &data)) {
		size = (data.nFileSizeHigh * (MAXDWORD + 1)) + data.nFileSizeLow;
		if (size == 0)continue;
		ImportCache(data, size);
	}
}

int main()
{
	std::wcout.imbue(std::locale("chs"));
	FILE* conf_file=fopen("config.json","r");
	if (!conf_file) {
		conf_file = fopen("config.json","w");
		if (!conf_file)return -1;
		//cache_path = L"F:\\CloudMusicCache";
		//output_path = L"D:\\music";
		StringBuffer s;
		PrettyWriter<StringBuffer> writer(s);
		writer.StartObject();
		writer.Key("CachePath");
		writer.String("F:\\CloudMusicCache");
		writer.Key("OutputPath");
		writer.String("D:\\music");
		writer.EndObject();
		fputs(s.GetString(), conf_file);
		fclose(conf_file);
		return;
	}else {
		Document config;
		char buf[1024]="";
		FileReadStream frs(conf_file,buf,sizeof(char)*1024);
		config.ParseStream(frs);
		fclose(conf_file);
		cache_path = _UTF8toUTF16(config["CachePath"].GetString());
		output_path = _UTF8toUTF16(config["OutputPath"].GetString());
	}

	if (_waccess(cache_path, 00) == -1) {
		wcout << L"你的缓存文件夹不存在，请检查Config.json";
		return -1;
	}
	if (_waccess(output_path, 00) == -1) {
		wcout << L"你的输出文件夹不存在，请检查Config.json";
		return -1;
	}
	findcache();
	
	
	int now_get_index = 0;
	int now_size=0;
	int process = 0;
	Curl* INFODL = new Curl();
	string get_;
	while (now_get_index != CacheInfos.size()) {
		get_ = neteas_api;
		get_ += "[";
		for (vector<cacheinfo*>::iterator iter = CacheInfos.begin()+now_get_index; iter != CacheInfos.end(); iter++) {
			cacheinfo* info = *iter;
			char id[30];
			sprintf(id, "%d", info->id);
			get_ += id;
			now_get_index++;
			now_size++;
			if (now_size == 200) {
				cout << "stop" << endl;
				now_size = 0;
				break;
			}
			if (iter + 1 != CacheInfos.end()) {
				get_ += ",";
			}
		}
		get_ += "]";
		//cout << get_;
		INFODL->setopt(CURLOPT_URL, get_.c_str());
		INFODL->initDownloadMode();
		CURLcode code = INFODL->perform();
		int trytimes = 1;
		while (code != 0) {
			wcout << L"读取列表失败，正在重试第" << trytimes << "次，错误代码：" << code << endl;
			code = INFODL->perform();
			Sleep(20000);
			trytimes++;
			if (trytimes = 10)return -2;
		}
		char* SomeInfo = INFODL->outputDownloadData();
		Document d;
		d.Parse(SomeInfo);
		vector<cacheinfo*> CacheInfos2;
		for (vector<cacheinfo*>::iterator iter = CacheInfos.begin(); iter != CacheInfos.end(); iter++) {
			cacheinfo* info = *iter;
			bool b = info->FindInfoInNeteaseJson(d);
			//wcout << b<<endl;
			if (b) {
				const char * size = _UTF16toGBK(info->getsize());
				const char * author = _UTF16toGBK(info->author);
				const char * title = _UTF16toGBK(info->title);
				const char * album = _UTF16toGBK(info->album);
				cout << "正在处理 歌曲ID:" << info->id << " 歌曲名称：" << title << " 歌手：" << author << " 专辑名称：" << album << " 文件大小：" << size << endl;
				delete[] size;
				process++;
				wstring file_name;
				file_name= info->author;
				file_name+= L" - ";
				file_name+= info->title;
				file_name+= L".";
				file_name+= info->format;
				FormatFileName(&file_name);
				wstring album_name=info->album;
				FormatFileName(&album_name);
				wstring formatpath = output_path;
				formatpath += L"\\";
				formatpath += album_name;
				_tmkdir(formatpath.c_str());
				formatpath += L"\\";
				formatpath += file_name;
				wstring cache_file_path= cache_path;
				cache_file_path += L"\\cache\\";
				cache_file_path += info->cachefilename;
				cache_file_path += L".uc";
				wcout <<L"从 "<< cache_file_path << L" 复制到 " << formatpath<<endl;
				BOOL status=CopyFile(cache_file_path.c_str(),formatpath.c_str(),true);
				if (status) {
					TagLib::FileRef fp(formatpath.c_str());
					if (_wcsicmp((const wchar_t*)info->format, L"mp3") == 0) {
							fp.tag()->setArtist(author);
							fp.tag()->setAlbum(album);
							fp.tag()->setTitle(title);
					} else {
						fp.tag()->setArtist(TagLib::String((wchar_t*)info->author, TagLib::String::UTF16BE));
						fp.tag()->setAlbum(TagLib::String((wchar_t*)info->album, TagLib::String::UTF16BE));
						fp.tag()->setTitle(TagLib::String((wchar_t*)info->title, TagLib::String::UTF16BE));
					}
					fp.save();
					fp.swap(fp);
				}
			delete[] author;
			delete[] title;
			delete[] album;
				}

			}
		}
	
	
	for (vector<cacheinfo*>::iterator iter = CacheInfos.begin(); iter != CacheInfos.end(); iter++) {
		cacheinfo* info = *iter;
		delete info;
	}
		delete INFODL;
	cout <<"共处理 "<<process<<" 个文件" <<endl;
	system("pause");
	
    return 0;

}

