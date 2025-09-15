#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:5039)
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
	struct PartsParam {
		std::string Name{};
		PartsType Type{};
		int BasePosX{};
		int BasePosY{};
		int BaseSizeX{};
		int BaseSizeY{};
		float BaseCenterX{};
		float BaseCenterY{};
		int BaseColorR{};
		int BaseColorG{};
		int BaseColorB{};

		int PosX{};
		int PosY{};
		int BeforePosX{};
		int BeforePosY{};

		int SizeX{};
		int SizeY{};
		int BeforeSizeX{};
		int BeforeSizeY{};

		float CenterX{};
		float CenterY{};
		float BeforeCenterX{};
		float BeforeCenterY{};

		int ColorR{};
		int ColorG{};
		int ColorB{};
		int BeforeColorR{};
		int BeforeColorG{};
		int BeforeColorB{};
	public:
	};
	struct AnimParam {
		size_t TargetID{};

		bool m_PosChanged{};
		int PosX{};
		int PosY{};

		bool m_SizeChanged{};
		int SizeX{};
		int SizeY{};

		bool m_CenterChanged{};
		float CenterX{};
		float CenterY{};

		bool m_ColorChanged{};
		int ColorR{};
		int ColorG{};
		int ColorB{};

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
			{
				int loop = 0;
				for (const int& dp : d["Pos"]) {
					if (loop == 0) {
						m_PartsParam.back().BasePosX = dp;
					}
					else {
						m_PartsParam.back().BasePosY = dp;
					}
					++loop;
				}
			}
			{
				int loop = 0;
				for (const int& dp : d["Size"]) {
					if (loop == 0) {
						m_PartsParam.back().BaseSizeX = dp;
					}
					else {
						m_PartsParam.back().BaseSizeY = dp;
					}
					++loop;
				}
			}
			{
				int loop = 0;
				for (const float& dp : d["Center"]) {
					if (loop == 0) {
						m_PartsParam.back().BaseCenterX = dp;
					}
					else {
						m_PartsParam.back().BaseCenterY = dp;
					}
					++loop;
				}
			}
			{
				int loop = 0;
				for (const int& dp : d["BaseColor"]) {
					if (loop == 0) {
						m_PartsParam.back().BaseColorR = dp;
					}
					else if (loop == 1) {
						m_PartsParam.back().BaseColorG = dp;
					}
					else {
						m_PartsParam.back().BaseColorB = dp;
					}
					++loop;
				}
			}
			m_PartsParam.back().PosX = m_PartsParam.back().BasePosX;
			m_PartsParam.back().PosY = m_PartsParam.back().BasePosY;

			m_PartsParam.back().SizeX = m_PartsParam.back().BaseSizeX;
			m_PartsParam.back().SizeY = m_PartsParam.back().BaseSizeY;

			m_PartsParam.back().CenterX = m_PartsParam.back().BaseCenterX;
			m_PartsParam.back().CenterY = m_PartsParam.back().BaseCenterY;

			m_PartsParam.back().ColorR = m_PartsParam.back().BaseColorR;
			m_PartsParam.back().ColorG = m_PartsParam.back().BaseColorG;
			m_PartsParam.back().ColorB = m_PartsParam.back().BaseColorB;
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
					int loop = 0;
					for (const int& dp : a["Pos"]) {
						if (loop == 0) {
							m_AnimData.back().m_AnimParam.back().PosX = dp;
						}
						else {
							m_AnimData.back().m_AnimParam.back().PosY = dp;
						}
						++loop;
					}
				}
				if (a.contains("Size")) {
					m_AnimData.back().m_AnimParam.back().m_SizeChanged = true;
					int loop = 0;
					for (const int& dp : a["Size"]) {
						if (loop == 0) {
							m_AnimData.back().m_AnimParam.back().SizeX = dp;
						}
						else {
							m_AnimData.back().m_AnimParam.back().SizeY = dp;
						}
						++loop;
					}
				}
				if (a.contains("Center")) {
					m_AnimData.back().m_AnimParam.back().m_CenterChanged = true;
					int loop = 0;
					for (const float& dp : a["Center"]) {
						if (loop == 0) {
							m_AnimData.back().m_AnimParam.back().CenterX = dp;
						}
						else {
							m_AnimData.back().m_AnimParam.back().CenterY = dp;
						}
						++loop;
					}
				}
				if (a.contains("Color")) {
					m_AnimData.back().m_AnimParam.back().m_ColorChanged = true;
					int loop = 0;
					for (const int& dp : a["Color"]) {
						if (loop == 0) {
							m_AnimData.back().m_AnimParam.back().ColorR = dp;
						}
						else if (loop == 1) {
							m_AnimData.back().m_AnimParam.back().ColorG = dp;
						}
						else {
							m_AnimData.back().m_AnimParam.back().ColorB = dp;
						}
						++loop;
					}
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

			auto& p = m_PartsParam.back();//とりあえず最後部で
			int x1 = xpos + p.BasePosX - static_cast<int>(static_cast<float>(p.BaseSizeX) * (1.f - p.BaseCenterX));
			int y1 = ypos + p.BasePosY - static_cast<int>(static_cast<float>(p.BaseSizeY) * (1.f - p.BaseCenterX));
			int x2 = xpos + p.BasePosX + static_cast<int>(static_cast<float>(p.BaseSizeX) * p.BaseCenterX);
			int y2 = ypos + p.BasePosY + static_cast<int>(static_cast<float>(p.BaseSizeY) * p.BaseCenterY);

			if (
				(x1 < DrawerMngr->GetMousePositionX() && DrawerMngr->GetMousePositionX() <= x2) &&
				(y1 < DrawerMngr->GetMousePositionY() && DrawerMngr->GetMousePositionY() <= y2)
				) {
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
					auto& Now = m_PartsParam.at(p.TargetID);
					if (p.m_PosChanged) {
						if (p.StartFrame == this->m_Frame) {
							Now.BeforePosX = Now.PosX;
							Now.BeforePosY = Now.PosY;
						}
						float Per = static_cast<float>(this->m_Frame - p.StartFrame) / static_cast<float>(p.EndFrame - p.StartFrame);
						Now.PosX = Now.BeforePosX + static_cast<int>(static_cast<float>(p.PosX - Now.BeforePosX) * Per);
						Now.PosY = Now.BeforePosY + static_cast<int>(static_cast<float>(p.PosY - Now.BeforePosY) * Per);
					}
					if (p.m_SizeChanged) {
						if (p.StartFrame == this->m_Frame) {
							Now.BeforeSizeX = Now.SizeX;
							Now.BeforeSizeY = Now.SizeY;
						}
						float Per = static_cast<float>(this->m_Frame - p.StartFrame) / static_cast<float>(p.EndFrame - p.StartFrame);
						Now.SizeX = Now.BeforeSizeX + static_cast<int>(static_cast<float>(p.SizeX - Now.BeforeSizeX) * Per);
						Now.SizeY = Now.BeforeSizeY + static_cast<int>(static_cast<float>(p.SizeY - Now.BeforeSizeY) * Per);
					}
					if (p.m_CenterChanged) {
						if (p.StartFrame == this->m_Frame) {
							Now.BeforeCenterX = Now.CenterX;
							Now.BeforeCenterY = Now.CenterY;
						}
						float Per = static_cast<float>(this->m_Frame - p.StartFrame) / static_cast<float>(p.EndFrame - p.StartFrame);
						Now.CenterX = Now.BeforeCenterX + static_cast<int>(static_cast<float>(p.CenterX - Now.BeforeCenterX) * Per);
						Now.CenterY = Now.BeforeCenterY + static_cast<int>(static_cast<float>(p.CenterY - Now.BeforeCenterY) * Per);
					}
					if (p.m_ColorChanged) {
						if (p.StartFrame == this->m_Frame) {
							Now.BeforeColorR = Now.ColorR;
							Now.BeforeColorG = Now.ColorG;
							Now.BeforeColorB = Now.ColorB;
						}
						float Per = static_cast<float>(this->m_Frame - p.StartFrame) / static_cast<float>(p.EndFrame - p.StartFrame);
						Now.ColorR = Now.BeforeColorR + static_cast<int>(static_cast<float>(p.ColorR - Now.BeforeColorR) * Per);
						Now.ColorG = Now.BeforeColorG + static_cast<int>(static_cast<float>(p.ColorG - Now.BeforeColorG) * Per);
						Now.ColorB = Now.BeforeColorB + static_cast<int>(static_cast<float>(p.ColorB - Now.BeforeColorB) * Per);
					}
				}
			}
		}
		if (!IsSelectAnim) {
			m_AnimDataLastSelect = -1;
			for (auto& p : m_PartsParam) {
				p.BeforePosX = p.PosX = p.BasePosX;
				p.BeforePosY = p.PosY = p.BasePosY;

				p.BeforeSizeX = p.SizeX = p.BaseSizeX;
				p.BeforeSizeY = p.SizeY = p.BaseSizeY;

				p.BeforeCenterX = p.CenterX = p.BaseCenterX;
				p.BeforeCenterY = p.CenterY = p.BaseCenterY;

				p.BeforeColorR = p.ColorR = p.BaseColorR;
				p.BeforeColorG = p.ColorG = p.BaseColorG;
				p.BeforeColorB = p.ColorB = p.BaseColorB;
			}
		}
	}
	void Draw(int xpos, int ypos) noexcept {
		for (auto& p : m_PartsParam) {
			switch (p.Type) {
			case PartsType::Box:
			{
				DxLib::DrawBox(
					xpos + p.PosX - static_cast<int>(static_cast<float>(p.SizeX) * (1.f - p.CenterX)),
					ypos + p.PosY - static_cast<int>(static_cast<float>(p.SizeY) * (1.f - p.CenterX)),
					xpos + p.PosX + static_cast<int>(static_cast<float>(p.SizeX) * p.CenterX),
					ypos + p.PosY + static_cast<int>(static_cast<float>(p.SizeY) * p.CenterY),
					GetColor(p.ColorR, p.ColorG, p.ColorB), TRUE);
			}
				break;
			case PartsType::Max:
				break;
			default:
				break;
			}
		}
	}
};

class TitleScene : public SceneBase {
	DrawModule m_DrawModule;
public:
	TitleScene(void) noexcept { SetID(static_cast<int>(EnumScene::Title)); }
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
