#pragma warning(disable:4464)
#pragma warning(disable:5259)

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
				// 改行して同じテキストとする
				h.m_Str += "\n";
				h.m_Str += RIGHT;
				IsHit = true;
				break;
			}
		}
		if (!IsHit) {
			this->havehandle.emplace_back(ID, RIGHT);
		}
	}
}
