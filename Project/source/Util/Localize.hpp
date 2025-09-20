#pragma once
#include "Util.hpp"

class LocalizePool : public SingletonBase<LocalizePool> {
private:
	friend class SingletonBase<LocalizePool>;
public:
	typedef int LocalizeID;
private:
	struct LocalizeStr {
		LocalizeID	m_ID{ 0 };
		char		m_Str[512]{};
	public:
		LocalizeStr(LocalizeID ID, const char* Str) {
			this->m_ID = ID;
			sprintfDx(this->m_Str, Str);
		}
	};

	std::vector<LocalizeStr> havehandle;
private:
	LocalizePool(void) noexcept;
	LocalizePool(const LocalizePool&) = delete;
	LocalizePool(LocalizePool&&) = delete;
	LocalizePool& operator=(const LocalizePool&) = delete;
	LocalizePool& operator=(LocalizePool&&) = delete;
public:
	void Load(void) noexcept;
	void Dispose(void) noexcept {
		this->havehandle.clear();
		this->havehandle.shrink_to_fit();
	}
public:
	const char* Get(LocalizeID type) noexcept {
		for (auto& s : this->havehandle) {
			if (s.m_ID == type) {
				return s.m_Str;
			}
		}
		return "";
	}
};
