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
	VECTOR2D	OfsNoRad = VECTOR2D(0.f, 0.f);
	VECTOR2D	Ofs{};
	VECTOR2D	Size{};
	VECTOR2D	Scale = VECTOR2D(1.f, 1.f);
	VECTOR2D	Center = VECTOR2D(0.f, 0.f);
	float		Rad = deg2rad(0.f);
	char		padding[4]{};
	ColorRGBA	Color = ColorRGBA(255, 255, 255, 255);
};

class DrawUISystem;

class DrawModule {
	struct AnimParam;
	struct PartsParam {
	private:
		std::string			Name{};
		PartsType			Type{};

		Param2D				Base{};
		Param2D				Now{};
		Param2D				Before{};

		std::string			String{};

		VECTOR2D			Min{};
		VECTOR2D			Max{};
		std::string			ImagePath{};
		int					DrawModuleHandle{};
		char		padding[4]{};
		const GraphHandle*	ImageHandle{};
		bool				m_IsHitCheck{};
		char		padding2[7]{};
	public:
		void		SetString(std::string value) noexcept { String = value; }
		const auto&	GetName(void) const noexcept { return Name; }
	public:
		bool IsHitPoint(int x, int y, Param2D Parent) const noexcept;
		void Update(DrawUISystem* DrawUI, Param2D Parent) const noexcept;
		void Draw(DrawUISystem* DrawUI, Param2D Parent) const noexcept;
	public:
		void CalcAnim(const AnimParam& Anim, bool IsFirstFrame,float Per) noexcept {
			if (Anim.m_OfsNoRadChanged) {
				if (IsFirstFrame) {
					this->Before.OfsNoRad = this->Now.OfsNoRad;
				}
				this->Now.OfsNoRad = Lerp(this->Before.OfsNoRad, Anim.Target.OfsNoRad, Per);
			}
			if (Anim.m_OfsChanged) {
				if (IsFirstFrame) {
					this->Before.Ofs = this->Now.Ofs;
				}
				this->Now.Ofs = Lerp(this->Before.Ofs, Anim.Target.Ofs, Per);
			}
			if (Anim.m_SizeChanged) {
				if (IsFirstFrame) {
					this->Before.Size = this->Now.Size;
				}
				this->Now.Size = Lerp(this->Before.Size, Anim.Target.Size, Per);
			}
			if (Anim.m_ScaleChanged) {
				if (IsFirstFrame) {
					this->Before.Scale = this->Now.Scale;
				}
				this->Now.Scale = Lerp(this->Before.Scale, Anim.Target.Scale, Per);
			}
			if (Anim.m_CenterChanged) {
				if (IsFirstFrame) {
					this->Before.Center = this->Now.Center;
				}
				this->Now.Center = Lerp(this->Before.Center, Anim.Target.Center, Per);
			}
			if (Anim.m_RadChanged) {
				if (IsFirstFrame) {
					this->Before.Rad = this->Now.Rad;
				}
				this->Now.Rad = Lerp(this->Before.Rad, Anim.Target.Rad, Per);
			}
			if (Anim.m_ColorChanged) {
				if (IsFirstFrame) {
					this->Before.Color = this->Now.Color;
				}
				this->Now.Color = Lerp(this->Before.Color, Anim.Target.Color, Per);
			}
		}
		void SetDefault(void) noexcept {
			this->Before = this->Now = this->Base;
		}
		void AddChild(DrawUISystem* DrawUI, const char* ChildName, const char* ChildBranch, const char* FilePath, Param2D Param) noexcept;
		void SetByJson(DrawUISystem* DrawUI, nlohmann::json& d, std::string BranchName) noexcept;
	};
	struct AnimParam {
		size_t TargetID{};

		bool m_OfsNoRadChanged{};
		bool m_OfsChanged{};
		bool m_SizeChanged{};
		bool m_ScaleChanged{};
		bool m_CenterChanged{};
		bool m_RadChanged{};
		bool m_ColorChanged{};
		char		padding[1]{};

		Param2D	Target{};

		int StartFrame{};
		int EndFrame{};
	};
	struct AnimData {
		AnimType m_Type{ AnimType::None };
		std::vector<AnimParam> m_AnimParam;
		bool m_IsLoop{};
		char		padding[7]{};
	};
private:
	std::vector<PartsParam> m_PartsParam;
	std::vector<AnimData> m_AnimData;
	int m_AnimDataLastSelect{};
	int m_Frame{};
	bool m_IsSelect{};
	bool m_IsActive{ true };
	bool m_UseActive{};
	bool m_UseButton{};
	char		padding[4]{};
public:
	std::string BranchName{};
public:
	//コンストラクタ
	DrawModule(void) noexcept {
		m_PartsParam.reserve(128);
		m_AnimData.reserve(128);
	}
	DrawModule(const DrawModule& o) noexcept {
		this->m_PartsParam = o.m_PartsParam;
		this->m_AnimData = o.m_AnimData;
		this->m_AnimDataLastSelect = o.m_AnimDataLastSelect;
		this->m_Frame = o.m_Frame;
		this->m_IsSelect = o.m_IsSelect;
	}
	DrawModule(DrawModule&& o) noexcept {
		this->m_PartsParam = o.m_PartsParam;
		this->m_AnimData = o.m_AnimData;
		this->m_AnimDataLastSelect = o.m_AnimDataLastSelect;
		this->m_Frame = o.m_Frame;
		this->m_IsSelect = o.m_IsSelect;
	}
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
	bool IsSelectButton(void) const noexcept { return m_IsSelect; }

	bool IsActive(void) const noexcept { return m_IsActive; }

	void SetActive(bool value) noexcept { this->m_IsActive = value; }

	void AddChild(DrawUISystem* DrawUI, const char* ChildName, const char* FilePath, Param2D Param = Param2D()) noexcept;
	void DeleteChild(const char* ChildName) noexcept;

	PartsParam* GetParts(const char* ChildName) const noexcept {
		for (auto& p : m_PartsParam) {
			if (p.GetName() == ChildName) {
				return (PartsParam*)&p;
			}
		}
		return nullptr;
	}
public:
	void Init(DrawUISystem* DrawUI, const char* Path, const char* Branch) noexcept;
	void Update(DrawUISystem* DrawUI, Param2D Param = Param2D()) noexcept {
		AnimType SelectNow = AnimType::Normal;
		if (m_UseActive) {
			if (m_IsActive) {
				SelectNow = AnimType::Active;
			}
			else {
				SelectNow = AnimType::DisActive;
			}
		}
		m_IsSelect = false;
		if (m_UseButton) {
			auto* DrawerMngr = MainDraw::Instance();
			auto* KeyMngr = KeyParam::Instance();

			bool IsSelect = false;
			for (auto& p : m_PartsParam) {
				if (p.IsHitPoint(DrawerMngr->GetMousePositionX(), DrawerMngr->GetMousePositionY(), Param)) {
					IsSelect = true;
					break;
				}
			}
			if (IsSelect) {
				SelectNow = AnimType::OnSelect;
				m_IsSelect = true;
			}

			if (m_IsSelect) {
				if (KeyMngr->GetMenuKeyPress(EnumMenu::Diside)) {
					SelectNow = AnimType::OnPress;
				}
			}
		}

		bool IsSelectAnim = false;
		for (auto& a : m_AnimData) {
			//アニメ選択
			if (SelectNow != a.m_Type) { continue; }
			IsSelectAnim = true;
			int index = static_cast<int>(&a - &m_AnimData.front());;
			//フレーム更新
			if (m_AnimDataLastSelect != index) {
				m_AnimDataLastSelect = index;
				this->m_Frame = 0;
			}
			else {
				auto Max = 0;
				for (auto& p : a.m_AnimParam) {
					Max = std::max(Max, p.EndFrame);
				}
				this->m_Frame = std::clamp(this->m_Frame + 1, 0, Max);
				if ((this->m_Frame == Max) && a.m_IsLoop) {
					this->m_Frame = 0;
				}
			}

			//
			for (auto& p : a.m_AnimParam) {
				if (p.StartFrame <= this->m_Frame && this->m_Frame <= p.EndFrame) {
					float Per = static_cast<float>(this->m_Frame - p.StartFrame) / static_cast<float>(p.EndFrame - p.StartFrame);
					m_PartsParam.at(p.TargetID).CalcAnim(p, (p.StartFrame == this->m_Frame), Per);
				}
			}
		}
		if (!IsSelectAnim) {
			m_AnimDataLastSelect = -1;
			for (auto& p : m_PartsParam) {
				p.SetDefault();
			}
		}

		for (auto& p : m_PartsParam) {
			p.Update(DrawUI, Param);
		}
	}
	void Draw(DrawUISystem* DrawUI, Param2D Param = Param2D()) noexcept {
		for (auto& p : m_PartsParam) {
			p.Draw(DrawUI, Param);
		}
		//DrawString(Param.OfsNoRad.x, Param.OfsNoRad.y, this->BranchName.c_str(), GetColor(255, 0, 0));
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
	int Add(const char* Path, const char* Branch) noexcept {
		int ID = static_cast<int>(m_DrawModule.size());
		m_DrawModule.emplace_back();
		m_DrawModule.back().Init(this, Path, Branch);
		return ID;
	}
	DrawModule& Get(int ID) noexcept { return m_DrawModule.at(static_cast<size_t>(ID)); }
	int GetID(const char* Value) noexcept {
		for (auto& d : m_DrawModule) {
			if (Value == d.BranchName) {
				return static_cast<int>(&d - &m_DrawModule.front());
			}
		}
		return -1;
	}
public:
	void AddChild(const char* Path, const char* FilePath, Param2D Param = Param2D()) noexcept {
		std::string Name = Path;
		std::string ChildName;
		if (Name.find("/") != std::string::npos) {
			ChildName = Name.substr(Name.rfind("/") + 1);
			Name = Name.substr(0, Name.rfind("/"));
		}
		else {
			ChildName = Path;
			Name = "";
		}
		Get(GetID(Name.c_str())).AddChild(this, ChildName.c_str(), FilePath, Param);
	}
	void DeleteChild(const char* Path) noexcept {
		std::string Name = Path;
		std::string ChildName;
		if (Name.find("/") != std::string::npos) {
			ChildName = Name.substr(Name.rfind("/") + 1);
			Name = Name.substr(0, Name.rfind("/"));
		}
		else {
			ChildName = Path;
			Name = "";
		}
		for (int loop = 0, max = static_cast<int>(this->m_DrawModule.size()); loop < max; ++loop) {
			auto& d = m_DrawModule.at(static_cast<size_t>(loop));
			if (d.BranchName.find(Path) != std::string::npos) {
				m_DrawModule.erase(m_DrawModule.begin() + loop);
				--loop;
				--max;
			}
		}
		for (auto& d : m_DrawModule) {
			if (d.BranchName == Name) {
				d.DeleteChild(ChildName.c_str());
				break;
			}
		}
	}
public:
	void Init(const char* Path) noexcept {
		Add(Path, "");
	}
	void Update(void) noexcept {
		if (m_DrawModule.size() == 0) { return; }
		m_DrawModule.at(0).Update(this);
	}
	void Draw(void) noexcept {
		if (m_DrawModule.size() == 0) { return; }
		m_DrawModule.at(0).Draw(this);
	}
	void Dispose(void) noexcept {
		m_DrawModule.clear();
	}
};

