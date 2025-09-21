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
	Int,
	Max,
};
static const char* OptionSelectTypeStr[static_cast<int>(EnumOptionSelectType::Max)] = {
	"Bool",
	"Select",
	"Int",
};

class OptionParam : public SingletonBase<OptionParam> {
private:
	friend class SingletonBase<OptionParam>;
private:
	struct Param {
	private:
		EnumOptionSelectType		m_SelectType{};
		int							m_Value{};
		int							m_ValueMin{};
		int							m_ValueMax{};
		char	Padding[4]{};
		std::string					m_Type{};
		std::vector<std::string>	m_ValueList{};
	public:
		std::string GetType() const noexcept { return this->m_Type; }

		int GetSelectMax() const noexcept { return this->m_ValueMax; }
		int GetSelectMin() const noexcept { return this->m_ValueMin; }
		int GetSelect() const noexcept { return this->m_Value; }

		std::string GetValueNow() const noexcept {
			switch (this->m_SelectType) {
			case EnumOptionSelectType::Bool:
			case EnumOptionSelectType::Select:
				return this->m_ValueList.at(static_cast<size_t>(GetSelect()));
				break;
			case EnumOptionSelectType::Int:
				return std::to_string(GetSelect());
				break;
			case EnumOptionSelectType::Max:
			default:
				break;
			}
			return "";
		}

		bool IsActive() const noexcept {
			if (m_SelectType == EnumOptionSelectType::Bool) {
				return GetValueNow() == "True";
			}
			return false;
		}
	public:
		void SetSelect(int Value) noexcept {
			this->m_Value = std::clamp(Value, GetSelectMin(), GetSelectMax());
		}
		void SetSelect(std::string Value) noexcept {
			switch (this->m_SelectType) {
			case EnumOptionSelectType::Bool:
			case EnumOptionSelectType::Select:
				for (auto& v : this->m_ValueList) {
					if (v == Value) {
						SetSelect(static_cast<int>(&v - &this->m_ValueList.front()));
						break;
					}
				}
				break;
			case EnumOptionSelectType::Int:
				SetSelect(std::stoi(Value));
				break;
			case EnumOptionSelectType::Max:
			default:
				break;
			}
		}
	public:
		void SetByJson(nlohmann::json& data) noexcept {
			//Type
			this->m_Type = data["Type"];
			//SelectType
			std::string SelType = data["SelectType"];
			for (int loop = 0; loop < static_cast<int>(EnumOptionSelectType::Max); ++loop) {
				if (SelType == OptionSelectTypeStr[loop]) {
					this->m_SelectType = static_cast<EnumOptionSelectType>(loop);
					break;
				}
			}
			//Value
			switch (this->m_SelectType) {
			case EnumOptionSelectType::Bool:
				this->m_ValueList.emplace_back("False");
				this->m_ValueList.emplace_back("True");
				this->m_ValueMin = 0;
				this->m_ValueMax = static_cast<int>(this->m_ValueList.size()) - 1;
				break;
			case EnumOptionSelectType::Select:
				if (data.contains("Value")) {
					for (auto& v : data["Value"]) {
						this->m_ValueList.emplace_back(v);
					}
				}
				this->m_ValueMin = 0;
				this->m_ValueMax = static_cast<int>(this->m_ValueList.size()) - 1;
				break;
			case EnumOptionSelectType::Int:
				if (data.contains("Value")) {
					int loop = 0;
					for (auto& v : data["Value"]) {
						if (loop == 0) {
							this->m_ValueMin = v;
						}
						else if (loop == 1) {
							this->m_ValueMax = v;
						}
						++loop;
					}
				}
				break;
			case EnumOptionSelectType::Max:
				break;
			default:
				break;
			}
			//DefaultValue
			switch (this->m_SelectType) {
			case EnumOptionSelectType::Bool:
			case EnumOptionSelectType::Select:
			{
				std::string DefaultValue = data["DefaultValue"];
				for (auto& v : this->m_ValueList) {
					if (v == DefaultValue) {
						SetSelect(static_cast<int>(&v - &this->m_ValueList.front()));
						break;
					}
				}
			}
			break;
			case EnumOptionSelectType::Int:
			{
				int tmp = data["DefaultValue"];
				SetSelect(tmp);
			}
			break;
			case EnumOptionSelectType::Max:
				break;
			default:
				break;
			}
		}
		void Dispose(void) noexcept {
			this->m_ValueList.clear();
		}
	};

	std::vector<Param>	m_ParamList;
private:
	//コンストラクタ
	OptionParam(void) noexcept {
		std::ifstream file("data/ProjectSetting/Option.json");
		nlohmann::json jsonData = nlohmann::json::parse(file);
		for (auto& data : jsonData["data"]) {
			m_ParamList.emplace_back();
			m_ParamList.back().SetByJson(data);
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
		for (auto& param : m_ParamList) {
			param.Dispose();
		}
		m_ParamList.clear();
	}
public:
	const Param* GetParam(const char* Type) const noexcept {
		for (auto& param : m_ParamList) {
			if (param.GetType() == Type) {
				return &param;
			}
		}
		return nullptr;
	}
	void SetParam(const char* Type, int Param) noexcept {
		for (auto& param : m_ParamList) {
			if (param.GetType() == Type) {
				param.SetSelect(Param);
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
			for (auto& param : m_ParamList) {
				if (param.GetType() == Left) {
					param.SetSelect(Right);
					break;
				}
			}
		}
	}
	void Save(void) noexcept {
		OutputFileStream Ostream("Save/Option.dat");
		for (auto& param : m_ParamList) {
			std::string Line = param.GetType() + "=" + param.GetValueNow();
			Ostream.AddLine(Line);
		}
	}
};
