#pragma once

#pragma warning(disable:4464)
#include "../Util/Util.hpp"
#include "../Util/Key.hpp"
#include "MainDraw.hpp"


enum class PartsType :size_t {
	Box,
	NineSlice,
	Json,
	Max,
};
static const char* PartsTypeStr[static_cast<int>(PartsType::Max)] = {
	"Box",
	"NineSlice",
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
	struct PartsParam {
		std::string Name{};
		PartsType Type{};

		Param2D	Base{};
		Param2D	Now{};
		Param2D	Before{};

		VECTOR2D	Min{};
		VECTOR2D	Max{};
		std::string	ImagePath{};
		int			DrawModuleHandle{};
		int			ImageHandle{};
		bool		m_IsHitCheck{};
		char		padding2[7]{};
	public:
		bool IsHitPoint(int x, int y, Param2D Parent) const noexcept;
		void Update(DrawUISystem* DrawUI, Param2D Parent) const noexcept;
		void Draw(DrawUISystem* DrawUI, Param2D Parent) const noexcept;
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
					auto& now = m_PartsParam.at(p.TargetID);
					float Per = static_cast<float>(this->m_Frame - p.StartFrame) / static_cast<float>(p.EndFrame - p.StartFrame);
					if (p.m_OfsNoRadChanged) {
						if (p.StartFrame == this->m_Frame) {
							now.Before.OfsNoRad = now.Now.OfsNoRad;
						}
						now.Now.OfsNoRad = Lerp(now.Before.OfsNoRad, p.Target.OfsNoRad, Per);
					}
					if (p.m_OfsChanged) {
						if (p.StartFrame == this->m_Frame) {
							now.Before.Ofs = now.Now.Ofs;
						}
						now.Now.Ofs = Lerp(now.Before.Ofs, p.Target.Ofs, Per);
					}
					if (p.m_SizeChanged) {
						if (p.StartFrame == this->m_Frame) {
							now.Before.Size = now.Now.Size;
						}
						now.Now.Size = Lerp(now.Before.Size, p.Target.Size, Per);
					}
					if (p.m_ScaleChanged) {
						if (p.StartFrame == this->m_Frame) {
							now.Before.Scale = now.Now.Scale;
						}
						now.Now.Scale = Lerp(now.Before.Scale, p.Target.Scale, Per);
					}
					if (p.m_CenterChanged) {
						if (p.StartFrame == this->m_Frame) {
							now.Before.Center = now.Now.Center;
						}
						now.Now.Center = Lerp(now.Before.Center, p.Target.Center, Per);
					}
					if (p.m_RadChanged) {
						if (p.StartFrame == this->m_Frame) {
							now.Before.Rad = now.Now.Rad;
						}
						now.Now.Rad = Lerp(now.Before.Rad, p.Target.Rad, Per);
					}
					if (p.m_ColorChanged) {
						if (p.StartFrame == this->m_Frame) {
							now.Before.Color = now.Now.Color;
						}
						now.Now.Color = Lerp(now.Before.Color, p.Target.Color, Per);
					}
				}
			}
		}
		if (!IsSelectAnim) {
			m_AnimDataLastSelect = -1;
			for (auto& p : m_PartsParam) {
				p.Before = p.Now = p.Base;
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

