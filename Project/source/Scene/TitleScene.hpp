#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:5039)
#pragma warning(disable:5045)
#include "../Util/Enum.hpp"
#include "../Util/SceneManager.hpp"
#include "../Util/Key.hpp"
#include "../Draw/MainDraw.hpp"

#include "../Util/Util.hpp"
#pragma warning( push, 3 )
#include "../File/json.hpp"
#pragma warning( pop )

enum class PartsType {
	Box,
	Max,
};
static const char* PartsTypeStr[static_cast<int>(PartsType::Max)] = {
"Box",
};

enum class AnimType {
	None,
	Normal,
	OnSelect,
	OnPress,
	Max,
};

static const char* AnimTypeStr[static_cast<int>(AnimType::Max)] = {
	"None",
	"Normal",
	"OnSelect",
	"OnPress",
};

class DrawModule {
	struct Param2D {
		VECTOR2D	Pos{};
		VECTOR2D	Size{};
		VECTOR2D	Center{};
		ColorRGBA	Color{};
	};
	struct PartsParam {
		std::string Name{};
		PartsType Type{};

		Param2D	Base{};
		Param2D	Now{};
		Param2D	Before{};
	public:
		bool IsHitPoint(int x, int y, int xpos, int ypos) const noexcept {
			switch (this->Type) {
			case PartsType::Box:
			{
				int x1 = xpos + static_cast<int>(this->Base.Pos.x - this->Base.Size.x * (1.f - this->Base.Center.x));
				int y1 = ypos + static_cast<int>(this->Base.Pos.y - this->Base.Size.y * (1.f - this->Base.Center.y));
				int x2 = xpos + static_cast<int>(this->Base.Pos.x + this->Base.Size.x * this->Base.Center.x);
				int y2 = ypos + static_cast<int>(this->Base.Pos.y + this->Base.Size.y * this->Base.Center.y);
				if ((x1 < x && x <= x2) && (y1 < y && y <= y2)) {
					return true;
				}
			}
			break;
			case PartsType::Max:
				break;
			default:
				break;
			}
			return false;
		}

		void Draw(int xpos, int ypos) const noexcept {
			switch (this->Type) {
			case PartsType::Box:
			{
				DxLib::DrawBox(
					xpos + static_cast<int>(this->Now.Pos.x - this->Now.Size.x * (1.f - this->Now.Center.x)),
					ypos + static_cast<int>(this->Now.Pos.y - this->Now.Size.y * (1.f - this->Now.Center.y)),
					xpos + static_cast<int>(this->Now.Pos.x + this->Now.Size.x * this->Now.Center.x),
					ypos + static_cast<int>(this->Now.Pos.y + this->Now.Size.y * this->Now.Center.y),
					this->Now.Color.GetColor(), TRUE);
			}
			break;
			case PartsType::Max:
				break;
			default:
				break;
			}
		}
	};
	struct AnimParam {
		size_t TargetID{};

		bool m_PosChanged{};
		bool m_SizeChanged{};
		bool m_CenterChanged{};
		bool m_ColorChanged{};

		Param2D	Target{};

		int StartFrame{};
		int EndFrame{};
	};
	struct AnimData {
		AnimType m_Type{ AnimType::None };
		bool m_IsLoop{};
		std::vector<AnimParam> m_AnimParam;
	};
private:
	std::vector<PartsParam> m_PartsParam;
	std::vector<AnimData> m_AnimData;
	int m_AnimDataLastSelect{};
	int m_Frame{};
	bool m_IsSelect{};
public:
	//コンストラクタ
	DrawModule(void) noexcept {}
	DrawModule(const DrawModule&) = delete;
	DrawModule(DrawModule&&) = delete;
	DrawModule& operator=(const DrawModule&) = delete;
	DrawModule& operator=(DrawModule&&) = delete;
	//デストラクタ
	~DrawModule(void) noexcept {
		m_PartsParam.clear();
		m_AnimData.clear();
	}
public:
	bool IsSelectButton(void) const noexcept { return m_IsSelect; }
public:
	void Init(void) noexcept {
		m_PartsParam.clear();
		m_AnimData.clear();
		std::ifstream file("data/Button001.json");
		nlohmann::json data = nlohmann::json::parse(file);
		//data["Type"];
		for (auto& d : data["MainParts"]) {
			m_PartsParam.emplace_back();
			m_PartsParam.back().Name = d["Name"];
			{
				std::string Type = d["Type"];
				for (int loop = 0; loop < static_cast<int>(PartsType::Max); ++loop) {
					if (Type == PartsTypeStr[loop]) {
						m_PartsParam.back().Type = static_cast<PartsType>(loop);
						break;
					}
				}
			}
			m_PartsParam.back().Base.Pos.SetByJson(d["Pos"]);
			m_PartsParam.back().Base.Size.SetByJson(d["Size"]);
			m_PartsParam.back().Base.Center.SetByJson(d["Center"]);
			m_PartsParam.back().Base.Color.SetByJson(d["BaseColor"]);
			m_PartsParam.back().Now = m_PartsParam.back().Base;
		}
		for (auto& d : data["Anim"]) {
			m_AnimData.emplace_back();
			//
			{
				std::string Type = d["Type"];
				for (int loop = 0; loop < static_cast<int>(AnimType::Max); ++loop) {
					if (Type == AnimTypeStr[loop]) {
						m_AnimData.back().m_Type = static_cast<AnimType>(loop);
						break;
					}
				}
			}
			//
			m_AnimData.back().m_IsLoop = d["Loop"];
			//
			for (auto& a : d["Anim"]) {
				m_AnimData.back().m_AnimParam.emplace_back();
				//
				{
					std::string Target = a["Target"];
					for (auto& p : m_PartsParam) {
						if (Target == p.Name) {
							m_AnimData.back().m_AnimParam.back().TargetID = static_cast<size_t>(&p - &m_PartsParam.front());
							break;
						}
					}
				}
				//
				if (a.contains("Pos")) {
					m_AnimData.back().m_AnimParam.back().m_PosChanged = true;
					m_AnimData.back().m_AnimParam.back().Target.Pos.SetByJson(a["Pos"]);
				}
				if (a.contains("Size")) {
					m_AnimData.back().m_AnimParam.back().m_SizeChanged = true;
					m_AnimData.back().m_AnimParam.back().Target.Size.SetByJson(a["Size"]);
				}
				if (a.contains("Center")) {
					m_AnimData.back().m_AnimParam.back().m_CenterChanged = true;
					m_AnimData.back().m_AnimParam.back().Target.Center.SetByJson(a["Center"]);
				}
				if (a.contains("Color")) {
					m_AnimData.back().m_AnimParam.back().m_ColorChanged = true;
					m_AnimData.back().m_AnimParam.back().Target.Color.SetByJson(a["Color"]);
				}
				//
				m_AnimData.back().m_AnimParam.back().StartFrame = a["StartFrame"];
				m_AnimData.back().m_AnimParam.back().EndFrame = a["EndFrame"];
			}
		}

		m_AnimDataLastSelect = -1;
		m_Frame = 0;
	}
	void Update(int xpos, int ypos) noexcept {
		AnimType SelectNow = AnimType::Normal;
		{
			auto* DrawerMngr = MainDraw::Instance();
			auto* KeyMngr = KeyParam::Instance();

			m_IsSelect = false;

			bool IsSelect = false;
			for (auto& p : m_PartsParam) {
				if (p.IsHitPoint(DrawerMngr->GetMousePositionX(), DrawerMngr->GetMousePositionY(), xpos, ypos)) {
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
					if (p.m_PosChanged) {
						if (p.StartFrame == this->m_Frame) {
							now.Before.Pos = now.Now.Pos;
						}
						now.Now.Pos = Lerp(now.Before.Pos, p.Target.Pos, Per);
					}
					if (p.m_SizeChanged) {
						if (p.StartFrame == this->m_Frame) {
							now.Before.Size = now.Now.Size;
						}
						now.Now.Size = Lerp(now.Before.Size, p.Target.Size, Per);
					}
					if (p.m_CenterChanged) {
						if (p.StartFrame == this->m_Frame) {
							now.Before.Center = now.Now.Center;
						}
						now.Now.Center = Lerp(now.Before.Center, p.Target.Center, Per);
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
	}
	void Draw(int xpos, int ypos) noexcept {
		for (auto& p : m_PartsParam) {
			p.Draw(xpos, ypos);
		}
	}
};

class TitleScene : public SceneBase {
	DrawModule m_DrawModule;
public:
	TitleScene(void) noexcept { SetID(static_cast<int>(EnumScene::Title)); }
	TitleScene(const TitleScene&) = delete;
	TitleScene(TitleScene&&) = delete;
	TitleScene& operator=(const TitleScene&) = delete;
	TitleScene& operator=(TitleScene&&) = delete;
	virtual ~TitleScene(void) noexcept {}
protected:
	void Init_Sub(void) noexcept override {
		m_DrawModule.Init();
	}
	void Update_Sub(void) noexcept override {
		auto* SceneMngr = SceneManager::Instance();
		auto* KeyMngr = KeyParam::Instance();
		if (m_DrawModule.IsSelectButton() && KeyMngr->GetMenuKeyReleaseTrigger(EnumMenu::Diside)) {
			SceneBase::SetNextScene(SceneMngr->GetScene(static_cast<int>(EnumScene::Main)));
			SceneBase::SetEndScene();
		}

		m_DrawModule.Update(110, 40);
	}
	void Draw_Sub(void) noexcept override {
		auto* DrawerMngr = MainDraw::Instance();
		DxLib::DrawBox(5, 5, DrawerMngr->GetDispWidth() - 5, DrawerMngr->GetDispHeight() - 5, GetColor(0, 0, 255), TRUE);

		m_DrawModule.Draw(110, 40);
	}
	void Dispose_Sub(void) noexcept override {}
};
