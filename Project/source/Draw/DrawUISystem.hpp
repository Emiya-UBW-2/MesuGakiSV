#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:4710)
#include "../Util/Util.hpp"
#include "../Util/Key.hpp"

#pragma warning( push, 3 )
#include "../File/json.hpp"
#pragma warning( pop )

#include "MainDraw.hpp"
#include "FontDraw.hpp"
#include "ImageDraw.hpp"

namespace Draw {
	enum class PartsType :size_t {
		Box,
		NineSlice,
		Image,
		String,
		Json,
		Max,
	};
	static const char* PartsTypeStr[static_cast<int>(PartsType::Max)] = {
		"Box",
		"NineSlice",
		"Image",
		"String",
		"Json",
	};

	enum class AnimType :size_t {
		None,
		Normal,
		OnSelect,
		OnPress,
		Active,
		DisActive,
		Max,
	};

	static const char* AnimTypeStr[static_cast<int>(AnimType::Max)] = {
		"None",
		"Normal",
		"OnSelect",
		"OnPress",
		"Active",
		"DisActive",
	};

	struct Param2D {
		Util::VECTOR2D	OfsNoRad = Util::VECTOR2D(0.f, 0.f);
		Util::VECTOR2D	Ofs{};
		Util::VECTOR2D	Size{};
		Util::VECTOR2D	Scale = Util::VECTOR2D(1.f, 1.f);
		Util::VECTOR2D	Center = Util::VECTOR2D(0.f, 0.f);
		float			Rad = Util::deg2rad(0.f);
		char			padding[4]{};
		Util::ColorRGBA	Color = Util::ColorRGBA(255, 255, 255, 255);
	};

	class DrawUISystem;

	class DrawModule {
		struct AnimParam {
			size_t			m_TargetID{};

			bool			m_OfsNoRadChanged{};
			bool			m_OfsChanged{};
			bool			m_SizeChanged{};
			bool			m_ScaleChanged{};
			bool			m_CenterChanged{};
			bool			m_RadChanged{};
			bool			m_ColorChanged{};
			char		padding[1]{};

			Param2D			m_Target{};

			int				m_StartFrame{};
			int				m_EndFrame{};
		};
		class PartsParam {
			std::string			m_Name{};
			PartsType			m_Type{};

			Param2D				m_Base{};
			Param2D				m_Now{};
			Param2D				m_Before{};

			std::string			m_String{};

			Util::VECTOR2D		m_Min{};
			Util::VECTOR2D		m_Max{};
			std::string			m_ImagePath{};
			int					m_DrawModuleHandle{};
			char		padding[4]{};
			const GraphHandle* m_ImageHandle{};
			bool				m_IsHitCheck{};
			char		padding2[7]{};
		public:
			void			SetString(std::string value) noexcept { m_String = value; }
			const auto& GetName(void) const noexcept { return m_Name; }
		public:
			bool			IsHitPoint(int x, int y, Param2D Parent) const noexcept;
			void			Update(DrawUISystem* DrawUI, Param2D Parent) const noexcept;
			void			Draw(DrawUISystem* DrawUI, Param2D Parent) const noexcept;
		public:
			void			CalcAnim(const AnimParam& Anim, bool IsFirstFrame, float Per) noexcept;
			void			SetDefault(void) noexcept {
				this->m_Before = this->m_Now = this->m_Base;
			}
			void			AddChild(DrawUISystem* DrawUI, std::string_view ChildName, std::string_view ChildBranch, std::string_view FilePath, Param2D Param) noexcept;
			void			SetByJson(DrawUISystem* DrawUI, nlohmann::json& data, std::string BranchName) noexcept;
		};
		struct AnimData {
			AnimType				m_Type{ AnimType::None };
			std::vector<AnimParam>	m_AnimParam;
			bool					m_IsLoop{};
			char		padding[7]{};
		};
	private:
		std::vector<PartsParam>	m_PartsParam;
		std::vector<AnimData>	m_AnimData;
		int						m_AnimDataLastSelect{};
		int						m_Frame{};
		bool					m_IsSelect{};
		bool					m_IsActive{ true };
		bool					m_UseActive{};
		bool					m_UseButton{};
		char		padding[4]{};
		std::string				m_BranchName{};
	public:
		//コンストラクタ
		DrawModule(void) noexcept {
			m_PartsParam.reserve(128);
			m_AnimData.reserve(128);
		}
		DrawModule(const DrawModule& o) noexcept { *this = o; }
		DrawModule(DrawModule&& o) noexcept { *this = o; }
		DrawModule& operator=(const DrawModule& o) noexcept {
			this->m_PartsParam = o.m_PartsParam;
			this->m_AnimData = o.m_AnimData;
			this->m_AnimDataLastSelect = o.m_AnimDataLastSelect;
			this->m_Frame = o.m_Frame;
			this->m_IsSelect = o.m_IsSelect;
			return *this;
		}
		DrawModule& operator=(DrawModule&& o) noexcept {
			this->m_PartsParam = o.m_PartsParam;
			this->m_AnimData = o.m_AnimData;
			this->m_AnimDataLastSelect = o.m_AnimDataLastSelect;
			this->m_Frame = o.m_Frame;
			this->m_IsSelect = o.m_IsSelect;
			return *this;
		}
		//デストラクタ
		~DrawModule(void) noexcept {
			m_PartsParam.clear();
			m_AnimData.clear();
		}
	public:
		bool			IsSelectButton(void) const noexcept { return m_IsSelect; }
		bool			IsActive(void) const noexcept { return m_IsActive; }
		std::string		GetBranchName() const noexcept { return m_BranchName; }
		PartsParam* GetParts(std::string_view ChildName) const noexcept {
			for (auto& parts : m_PartsParam) {
				if (parts.GetName() == ChildName) {
					return (PartsParam*)&parts;
				}
			}
			return nullptr;
		}
	public:
		void			SetActive(bool value) noexcept { this->m_IsActive = value; }
		void			AddChild(DrawUISystem* DrawUI, std::string_view ChildName, std::string_view FilePath, Param2D Param = Param2D()) noexcept;
		void			DeleteChild(std::string_view ChildName) noexcept;
	public:
		void			Init(DrawUISystem* DrawUI, std::string_view Path, std::string_view Branch) noexcept;
		void			Update(DrawUISystem* DrawUI, Param2D Param = Param2D()) noexcept;
		void			Draw(DrawUISystem* DrawUI, Param2D Param = Param2D()) noexcept {
			for (auto& parts : m_PartsParam) {
				parts.Draw(DrawUI, Param);
			}
			//DrawString(Param.OfsNoRad.x, Param.OfsNoRad.y, GetBranchName().c_str(), GetColor(255, 0, 0));
		}
	};



	class DrawUISystem {
		std::vector<DrawModule> m_DrawModule;
	public://コンストラクタ、デストラクタ
		DrawUISystem(void) noexcept {
			m_DrawModule.reserve(128);
		}
		DrawUISystem(const DrawUISystem&) = delete;
		DrawUISystem(DrawUISystem&&) = delete;
		DrawUISystem& operator=(const DrawUISystem&) = delete;
		DrawUISystem& operator=(DrawUISystem&&) = delete;
		~DrawUISystem(void) noexcept {}
	public:
		int				Add(std::string_view Path, std::string_view Branch) noexcept {
			int ID = static_cast<int>(m_DrawModule.size());
			m_DrawModule.emplace_back();
			m_DrawModule.back().Init(this, Path, Branch);
			return ID;
		}
		DrawModule& Get(int ID) noexcept { return m_DrawModule.at(static_cast<size_t>(ID)); }
		int				GetID(std::string_view Value) noexcept {
			for (auto& Module : m_DrawModule) {
				if (Value == Module.GetBranchName()) {
					return static_cast<int>(&Module - &m_DrawModule.front());
				}
			}
			return -1;
		}
	public:
		void			AddChild(std::string_view Path, std::string_view FilePath, Param2D Param = Param2D()) noexcept {
			std::string Name;
			std::string ChildName;
			if (Path.find("/") != std::string::npos) {
				ChildName = Path.substr(Path.rfind("/") + 1);
				Name = Path.substr(0, Path.rfind("/"));
			}
			else {
				ChildName = Path;
				Name = "";
			}
			Get(GetID(Name.c_str())).AddChild(this, ChildName.c_str(), FilePath, Param);
		}
		void			DeleteChild(std::string_view Path) noexcept {
			std::string Name;
			std::string ChildName;
			if (Path.find("/") != std::string::npos) {
				ChildName = Path.substr(Path.rfind("/") + 1);
				Name = Path.substr(0, Path.rfind("/"));
			}
			else {
				ChildName = Path;
				Name = "";
			}
			//子供のモジュールを削除
			for (int loop = 0, max = static_cast<int>(this->m_DrawModule.size()); loop < max; ++loop) {
				auto& Module = m_DrawModule.at(static_cast<size_t>(loop));
				if (Module.GetBranchName().find(Path) != std::string::npos) {
					m_DrawModule.erase(m_DrawModule.begin() + loop);
					--loop;
					--max;
				}
			}
			for (auto& Module : m_DrawModule) {
				if (Module.GetBranchName() == Name) {
					Module.DeleteChild(ChildName.c_str());
					break;
				}
			}
		}
	public:
		void			Init(std::string_view Path) noexcept {
			Add(Path, "");
		}
		void			Update(void) noexcept {
			if (m_DrawModule.size() == 0) { return; }
			m_DrawModule.at(0).Update(this);
		}
		void			Draw(void) noexcept {
			if (m_DrawModule.size() == 0) { return; }
			m_DrawModule.at(0).Draw(this);
		}
		void			Dispose(void) noexcept {
			m_DrawModule.clear();
		}
	};
}
