#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:4505)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:5039)
#pragma warning(disable:5045)
#include "Util.hpp"
#include "../File/FileStream.hpp"
#pragma warning( push, 3 )
#include "../File/json.hpp"
#pragma warning( pop )

enum class EnumOptionSelectType : size_t {
	Bool,
	Select,
	Max,
};
static const char* OptionSelectTypeStr[static_cast<int>(EnumOptionSelectType::Max)] = {
	"Bool",
	"Select",
};

class OptionParam : public SingletonBase<OptionParam> {
private:
	friend class SingletonBase<OptionParam>;
private:
	struct Param {
		EnumOptionSelectType		m_SelectType{};
		int							m_Value{};
		char		padding[4]{};
		std::string					m_Type{};
		std::vector<std::string>	m_ValueList{};
	public:
		const std::string GetValueNow() const noexcept { return this->m_ValueList.at(static_cast<size_t>(this->m_Value)); }
		bool IsActive() const noexcept {
			if (m_SelectType == EnumOptionSelectType::Bool) {
				return GetValueNow() == "True";
			}
			return false;
		}
	};

	std::vector<Param>	m_ParamList;
private:
	//コンストラクタ
	OptionParam(void) noexcept {
		std::ifstream file("data/ProjectSetting/Option.json");
		nlohmann::json data = nlohmann::json::parse(file);
		for (auto& d : data["data"]) {
			m_ParamList.emplace_back();
			//Type
			m_ParamList.back().m_Type = d["Type"];
			//SelectType
			std::string SelType = d["SelectType"];
			for (int loop = 0; loop < static_cast<int>(EnumOptionSelectType::Max); ++loop) {
				if (SelType == OptionSelectTypeStr[loop]) {
					m_ParamList.back().m_SelectType = static_cast<EnumOptionSelectType>(loop);
					break;
				}
			}
			//Value
			switch (m_ParamList.back().m_SelectType) {
			case EnumOptionSelectType::Bool:
				m_ParamList.back().m_ValueList.emplace_back("False");
				m_ParamList.back().m_ValueList.emplace_back("True");
				break;
			case EnumOptionSelectType::Select:
				if (d.contains("Value")) {
					for (auto& v : d["Value"]) {
						m_ParamList.back().m_ValueList.emplace_back(v);
					}
				}
				break;
			case EnumOptionSelectType::Max:
				break;
			default:
				break;
			}
			//DefaultValue
			std::string DefaultValue = d["DefaultValue"];
			for (auto& v : m_ParamList.back().m_ValueList) {
				if (v == DefaultValue) {
					m_ParamList.back().m_Value = static_cast<int>(&v - &m_ParamList.back().m_ValueList.front());
					break;
				}
			}
		}
		Load();
	}
	OptionParam(const OptionParam&) = delete;
	OptionParam(OptionParam&&) = delete;
	OptionParam& operator=(const OptionParam&) = delete;
	OptionParam& operator=(OptionParam&&) = delete;
	//デストラクタ
	~OptionParam(void) noexcept {
		Save();
		for (auto& p : m_ParamList) {
			p.m_ValueList.clear();
		}
		m_ParamList.clear();
	}
public:
	const Param* GetParam(const char* Type) const noexcept {
		for (auto& p : m_ParamList) {
			if (p.m_Type == Type) {
				return &p;
			}
		}
		return nullptr;
	}
	void SetParam(const char* Type, int Param) noexcept {
		for (auto& p : m_ParamList) {
			if (p.m_Type == Type) {
				p.m_Value = Param;
				break;
			}
		}
	}
public:
	void Load(void) noexcept {
		InputFileStream Istream("Save/Option.dat");
		while (!Istream.ComeEof()) {
			std::string Line = InputFileStream::getleft(Istream.SeekLineAndGetStr(), "//");
			std::string Left = InputFileStream::getleft(Line, "=");
			std::string Right = InputFileStream::getright(Line, "=");
			for (auto& p : m_ParamList) {
				if (p.m_Type == Left) {
					for (auto& v : p.m_ValueList) {
						if (v == Right) {
							p.m_Value = static_cast<int>(&v - &p.m_ValueList.front());
							break;
						}
					}
					break;
				}
			}
		}
	}
	void Save(void) noexcept {
		OutputFileStream Ostream("Save/Option.dat");
		for (auto& p : m_ParamList) {
			std::string Line = p.m_Type + "=" + p.GetValueNow();
			Ostream.AddLine(Line);
		}
	}
};
