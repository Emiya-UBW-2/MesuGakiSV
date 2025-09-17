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
	NineSlice,
	Max,
};
static const char* PartsTypeStr[static_cast<int>(PartsType::Max)] = {
"Box",
"NineSlice"
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
		VECTOR2D	OfsNoRad{};
		VECTOR2D	Ofs{};
		VECTOR2D	Size{};
		VECTOR2D	Scale{};
		VECTOR2D	Center{};
		float		Rad{};
		ColorRGBA	Color{};
	};
	struct PartsParam {
		std::string Name{};
		PartsType Type{};

		Param2D	Base{};
		Param2D	Now{};
		Param2D	Before{};

		VECTOR2D	Min{};
		VECTOR2D	Max{};
		std::string	ImagePath{};
		int			ImageHandle{};
		bool		m_IsHitCheck{};
	public:
		bool IsHitPoint(int x, int y, VECTOR2D Pos, float Rad, VECTOR2D Scale) const noexcept {
			if (!m_IsHitCheck) { return false; }
			switch (this->Type) {
			case PartsType::Box:
			case PartsType::NineSlice:
			{
				VECTOR2D PosOfs;
				PosOfs.x = Pos.x + this->Now.OfsNoRad.y + this->Now.Ofs.x * std::cos(this->Now.Rad + Rad) - this->Now.Ofs.y * std::sin(this->Now.Rad + Rad);
				PosOfs.y = Pos.y + this->Now.OfsNoRad.y + this->Now.Ofs.x * std::sin(this->Now.Rad + Rad) + this->Now.Ofs.y * std::cos(this->Now.Rad + Rad);

				float x1 = PosOfs.x - this->Now.Size.x * this->Now.Scale.x * Scale.x * (1.f - this->Now.Center.x);
				float y1 = PosOfs.y - this->Now.Size.y * this->Now.Scale.y * Scale.y * (1.f - this->Now.Center.y);
				float x2 = PosOfs.x + this->Now.Size.x * this->Now.Scale.x * Scale.x * this->Now.Center.x;
				float y2 = PosOfs.y + this->Now.Size.y * this->Now.Scale.y * Scale.y * this->Now.Center.y;

				float centerX = PosOfs.x + this->Now.Size.x * this->Now.Scale.x * Scale.x * (this->Now.Center.x - 0.5f) * 2.f;
				float centerY = PosOfs.y + this->Now.Size.y * this->Now.Scale.y * Scale.y * (this->Now.Center.y - 0.5f) * 2.f;

				auto GetPoint = [&](float o1x, float o1y) {
					float X = o1x - centerX;
					float Y = o1y - centerY;
					VECTOR2D Answer;
					Answer.x = centerX + X * std::cos(this->Now.Rad + Rad) - Y * std::sin(this->Now.Rad + Rad);
					Answer.y = centerY + X * std::sin(this->Now.Rad + Rad) + Y * std::cos(this->Now.Rad + Rad);
					return Answer;
					};

				if (HitPointToSquare(VECTOR2D(static_cast<float>(x), static_cast<float>(y)), GetPoint(x1, y1), GetPoint(x2, y1), GetPoint(x2, y2), GetPoint(x1, y2))) {
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

		void Draw(VECTOR2D Pos, float Rad, VECTOR2D Scale) const noexcept {
			DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, this->Now.Color.GetA());
			switch (this->Type) {
			case PartsType::Box:
			{
				VECTOR2D PosOfs;
				PosOfs.x = Pos.x + this->Now.OfsNoRad.y + this->Now.Ofs.x * std::cos(this->Now.Rad + Rad) - this->Now.Ofs.y * std::sin(this->Now.Rad + Rad);
				PosOfs.y = Pos.y + this->Now.OfsNoRad.y + this->Now.Ofs.x * std::sin(this->Now.Rad + Rad) + this->Now.Ofs.y * std::cos(this->Now.Rad + Rad);

				float x1 = PosOfs.x - this->Now.Size.x * this->Now.Scale.x * Scale.x * (1.f - this->Now.Center.x);
				float y1 = PosOfs.y - this->Now.Size.y * this->Now.Scale.y * Scale.y * (1.f - this->Now.Center.y);
				float x2 = PosOfs.x + this->Now.Size.x * this->Now.Scale.x * Scale.x * this->Now.Center.x;
				float y2 = PosOfs.y + this->Now.Size.y * this->Now.Scale.y * Scale.y * this->Now.Center.y;

				float centerX = PosOfs.x + this->Now.Size.x * this->Now.Scale.x * Scale.x * (this->Now.Center.x - 0.5f) * 2.f;
				float centerY = PosOfs.y + this->Now.Size.y * this->Now.Scale.y * Scale.y * (this->Now.Center.y - 0.5f) * 2.f;

				auto GetPoint = [&](float o1x, float o1y) {
					float X = o1x - centerX;
					float Y = o1y - centerY;
					VECTOR2D Answer;
					Answer.x = centerX + X * std::cos(this->Now.Rad + Rad) - Y * std::sin(this->Now.Rad + Rad);
					Answer.y = centerY + X * std::sin(this->Now.Rad + Rad) + Y * std::cos(this->Now.Rad + Rad);
					return Answer;
					};
				VECTOR2D  P1 = GetPoint(x1, y1);
				VECTOR2D  P2 = GetPoint(x2, y1);
				VECTOR2D  P3 = GetPoint(x2, y2);
				VECTOR2D  P4 = GetPoint(x1, y2);
				DxLib::DrawQuadrangle(
					static_cast<int>(P1.x), static_cast<int>(P1.y),
					static_cast<int>(P2.x), static_cast<int>(P2.y),
					static_cast<int>(P3.x), static_cast<int>(P3.y),
					static_cast<int>(P4.x), static_cast<int>(P4.y),
					this->Now.Color.GetColor(),
					TRUE
					);
			}
			break;
			case PartsType::NineSlice:
			{
				VECTOR2D PosOfs;
				PosOfs.x = Pos.x + this->Now.OfsNoRad.y + this->Now.Ofs.x * std::cos(this->Now.Rad + Rad) - this->Now.Ofs.y * std::sin(this->Now.Rad + Rad);
				PosOfs.y = Pos.y + this->Now.OfsNoRad.y + this->Now.Ofs.x * std::sin(this->Now.Rad + Rad) + this->Now.Ofs.y * std::cos(this->Now.Rad + Rad);

				float x1 = PosOfs.x - this->Now.Size.x * this->Now.Scale.x * Scale.x * (1.f - this->Now.Center.x);
				float y1 = PosOfs.y - this->Now.Size.y * this->Now.Scale.y * Scale.y * (1.f - this->Now.Center.y);
				float x2 = PosOfs.x + this->Now.Size.x * this->Now.Scale.x * Scale.x * this->Now.Center.x;
				float y2 = PosOfs.y + this->Now.Size.y * this->Now.Scale.y * Scale.y * this->Now.Center.y;
				DxLib::SetDrawBright(this->Now.Color.GetR(), this->Now.Color.GetG(), this->Now.Color.GetB());
				Draw9SliceGraph(
					VECTOR2D(x1,y1), VECTOR2D(x2,y2),
					this->Min, this->Max,
					this->Now.Center,
					this->Now.Rad + Rad,
					this->ImageHandle,
					true,
					false
				);
				DxLib::SetDrawBright(255, 255, 255);
			}
			break;
			case PartsType::Max:
				break;
			default:
				break;
			}
			DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		}
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
	void Init(const char* Path) noexcept {
		m_PartsParam.clear();
		m_AnimData.clear();
		std::ifstream file(Path);
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
			if (d.contains("OfsNoRad")) {
				m_PartsParam.back().Base.OfsNoRad.SetByJson(d["OfsNoRad"]);
			}
			if (d.contains("Ofs")) {
				m_PartsParam.back().Base.Ofs.SetByJson(d["Ofs"]);
			}
			if (d.contains("Size")) {
				m_PartsParam.back().Base.Size.SetByJson(d["Size"]);
			}
			if (d.contains("Scale")) {
				m_PartsParam.back().Base.Scale.SetByJson(d["Scale"]);
			}
			if (d.contains("Center")) {
				m_PartsParam.back().Base.Center.SetByJson(d["Center"]);
			}
			if (d.contains("Rad")) {
				m_PartsParam.back().Base.Rad = d["Rad"];
				m_PartsParam.back().Base.Rad = deg2rad(m_PartsParam.back().Base.Rad);
			}
			if (d.contains("BaseColor")) {
				m_PartsParam.back().Base.Color.SetByJson(d["BaseColor"]);
			}
			else {
				m_PartsParam.back().Base.Color.Set(255, 255, 255, 255);
			}

			if (d.contains("Min")) {
				m_PartsParam.back().Min.SetByJson(d["Min"]);
			}
			if (d.contains("Max")) {
				m_PartsParam.back().Max.SetByJson(d["Max"]);
			}

			if (d.contains("Image")) {
				m_PartsParam.back().ImagePath = d["Image"];
				m_PartsParam.back().ImageHandle = LoadGraph(m_PartsParam.back().ImagePath.c_str());
			}

			if (d.contains("IsHitCheck")) {
				m_PartsParam.back().m_IsHitCheck = d["IsHitCheck"];
			}

			m_PartsParam.back().Before = m_PartsParam.back().Now = m_PartsParam.back().Base;
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
				if (a.contains("OfsNoRad")) {
					m_AnimData.back().m_AnimParam.back().m_OfsNoRadChanged = true;
					m_AnimData.back().m_AnimParam.back().Target.OfsNoRad.SetByJson(a["OfsNoRad"]);
				}
				if (a.contains("Ofs")) {
					m_AnimData.back().m_AnimParam.back().m_OfsChanged = true;
					m_AnimData.back().m_AnimParam.back().Target.Ofs.SetByJson(a["Ofs"]);
				}
				if (a.contains("Size")) {
					m_AnimData.back().m_AnimParam.back().m_SizeChanged = true;
					m_AnimData.back().m_AnimParam.back().Target.Size.SetByJson(a["Size"]);
				}
				if (a.contains("Scale")) {
					m_AnimData.back().m_AnimParam.back().m_ScaleChanged = true;
					m_AnimData.back().m_AnimParam.back().Target.Scale.SetByJson(a["Scale"]);
				}
				if (a.contains("Center")) {
					m_AnimData.back().m_AnimParam.back().m_CenterChanged = true;
					m_AnimData.back().m_AnimParam.back().Target.Center.SetByJson(a["Center"]);
				}
				if (a.contains("Rad")) {
					m_AnimData.back().m_AnimParam.back().m_RadChanged = true;
					m_AnimData.back().m_AnimParam.back().Target.Rad = a["Rad"];
					m_AnimData.back().m_AnimParam.back().Target.Rad = deg2rad(m_AnimData.back().m_AnimParam.back().Target.Rad);
				}
				if (a.contains("Color")) {
					m_AnimData.back().m_AnimParam.back().m_ColorChanged = true;
					m_AnimData.back().m_AnimParam.back().Target.Color.SetByJson(a["Color"]);
				}
				else {
					m_AnimData.back().m_AnimParam.back().Target.Color.Set(255, 255, 255, 255);
				}
				//
				m_AnimData.back().m_AnimParam.back().StartFrame = a["StartFrame"];
				m_AnimData.back().m_AnimParam.back().EndFrame = a["EndFrame"];
			}
		}

		m_AnimDataLastSelect = -1;
		m_Frame = 0;
	}
	void Update(VECTOR2D Pos, float Rad, VECTOR2D Scale) noexcept {
		AnimType SelectNow = AnimType::Normal;
		{
			auto* DrawerMngr = MainDraw::Instance();
			auto* KeyMngr = KeyParam::Instance();

			m_IsSelect = false;

			bool IsSelect = false;
			for (auto& p : m_PartsParam) {
				if (p.IsHitPoint(DrawerMngr->GetMousePositionX(), DrawerMngr->GetMousePositionY(), Pos, Rad, Scale)) {
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
	}
	void Draw(VECTOR2D Pos, float Rad, VECTOR2D Scale) noexcept {
		for (auto& p : m_PartsParam) {
			p.Draw(Pos, Rad, Scale);
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
		m_DrawModule.Init("data/Button001.json");
	}
	void Update_Sub(void) noexcept override {
		auto* SceneMngr = SceneManager::Instance();
		auto* KeyMngr = KeyParam::Instance();
		if (m_DrawModule.IsSelectButton() && KeyMngr->GetMenuKeyReleaseTrigger(EnumMenu::Diside)) {
			SceneBase::SetNextScene(SceneMngr->GetScene(static_cast<int>(EnumScene::Main)));
			SceneBase::SetEndScene();
		}

		m_DrawModule.Update(VECTOR2D(400.f, 200.f), deg2rad(10.f), VECTOR2D(2.f, 1.f));
	}
	void Draw_Sub(void) noexcept override {
		auto* DrawerMngr = MainDraw::Instance();
		DxLib::DrawBox(5, 5, DrawerMngr->GetDispWidth() - 5, DrawerMngr->GetDispHeight() - 5, GetColor(0, 0, 255), TRUE);

		m_DrawModule.Draw(VECTOR2D(400.f, 200.f), deg2rad(10.f), VECTOR2D(2.f, 1.f));
	}
	void Dispose_Sub(void) noexcept override {}
};
