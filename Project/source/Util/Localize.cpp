#pragma warning(disable:4464)
#pragma warning(disable:5259)
#pragma warning( push, 3 )
#include <string>
#pragma warning( pop )

#include "Localize.hpp"

#include "Enum.hpp"
#include "../File/FileStream.hpp"


const LocalizePool* SingletonBase<LocalizePool>::m_Singleton = nullptr;

LocalizePool::LocalizePool(void) noexcept {
	Load();
}
void LocalizePool::Load(void) noexcept {
	Dispose();
	std::string Path = "data/Localize/";
	Path += LanguageStr[0];
	Path += ".txt";
	this->havehandle.reserve(256);
	InputFileStream FileStream(Path.c_str());
	while (true) {
		if (FileStream.ComeEof()) { break; }
		auto ALL = FileStream.SeekLineAndGetStr();
		if (ALL == "") { continue; }
		//=の右側の文字をカンマ区切りとして識別する
		auto LEFT = InputFileStream::getleft(ALL, "=");
		auto RIGHT = InputFileStream::getright(ALL, "=");
		LocalizeID ID = (LocalizeID)(std::stoi(LEFT));
		bool IsHit = false;
		for (auto& h : this->havehandle) {
			if (h.m_ID == ID) {
				std::string Before = h.m_Str;
				sprintfDx(h.m_Str, "%s\n%s", Before.c_str(), RIGHT.c_str());// 改行して同じテキストとする
				IsHit = true;
				break;
			}
		}
		if (!IsHit) {
			this->havehandle.emplace_back((LocalizeID)(std::stoi(LEFT)), RIGHT.c_str());
		}
	}
}
