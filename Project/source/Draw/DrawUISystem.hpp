#pragma once

#pragma warning(disable:4464)
#include "../Util/Util.hpp"
#include "../Util/Key.hpp"
#include "MainDraw.hpp"


enum class PartsType :size_t {
	Box,
	NineSlice,
	Max,
};
static const char* PartsTypeStr[static_cast<int>(PartsType::Max)] = {
	"Box",
	"NineSlice"
};

enum class AnimType :size_t {
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
		char		padding[4]{};
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
		char		padding2[3]{};
	public:
		bool IsHitPoint(int x, int y, VECTOR2D Pos, float Rad, VECTOR2D Scale) const noexcept {
			if (!m_IsHitCheck) { return false; }
			switch (this->Type) {
			case PartsType::Box:
			case PartsType::NineSlice:
			{
				VECTOR2D PosOfs = Pos + this->Now.OfsNoRad + this->Now.Ofs.Rotate(this->Now.Rad + Rad);

				VECTOR2D Size;
				Size.x = this->Now.Size.x * this->Now.Scale.x * Scale.x;
				Size.y = this->Now.Size.y * this->Now.Scale.y * Scale.y;

				float x1 = PosOfs.x - Size.x * (1.f - this->Now.Center.x);
				float y1 = PosOfs.y - Size.y * (1.f - this->Now.Center.y);
				float x2 = PosOfs.x + Size.x * this->Now.Center.x;
				float y2 = PosOfs.y + Size.y * this->Now.Center.y;

				VECTOR2D center;
				center.x = PosOfs.x + Size.x * (this->Now.Center.x - 0.5f) * 2.f;
				center.y = PosOfs.y + Size.y * (this->Now.Center.y - 0.5f) * 2.f;

				auto GetPoint = [&](float o1x, float o1y) {
					VECTOR2D ofs;
					ofs.x = o1x;
					ofs.y = o1y;
					return center + (ofs - center).Rotate(this->Now.Rad + Rad);
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
				VECTOR2D PosOfs = Pos + this->Now.OfsNoRad + this->Now.Ofs.Rotate(this->Now.Rad + Rad);

				VECTOR2D Size;
				Size.x = this->Now.Size.x * this->Now.Scale.x * Scale.x;
				Size.y = this->Now.Size.y * this->Now.Scale.y * Scale.y;

				float x1 = PosOfs.x - Size.x * (1.f - this->Now.Center.x);
				float y1 = PosOfs.y - Size.y * (1.f - this->Now.Center.y);
				float x2 = PosOfs.x + Size.x * this->Now.Center.x;
				float y2 = PosOfs.y + Size.y * this->Now.Center.y;

				VECTOR2D center;
				center.x = PosOfs.x + Size.x * (this->Now.Center.x - 0.5f) * 2.f;
				center.y = PosOfs.y + Size.y * (this->Now.Center.y - 0.5f) * 2.f;

				auto GetPoint = [&](float o1x, float o1y) {
					VECTOR2D ofs;
					ofs.x = o1x;
					ofs.y = o1y;
					return center + (ofs - center).Rotate(this->Now.Rad + Rad);
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
				VECTOR2D PosOfs = Pos + this->Now.OfsNoRad + this->Now.Ofs.Rotate(this->Now.Rad + Rad);

				VECTOR2D Size;
				Size.x = this->Now.Size.x * this->Now.Scale.x * Scale.x;
				Size.y = this->Now.Size.y * this->Now.Scale.y * Scale.y;

				float x1 = PosOfs.x - Size.x * (1.f - this->Now.Center.x);
				float y1 = PosOfs.y - Size.y * (1.f - this->Now.Center.y);
				float x2 = PosOfs.x + Size.x * this->Now.Center.x;
				float y2 = PosOfs.y + Size.y * this->Now.Center.y;
				DxLib::SetDrawBright(this->Now.Color.GetR(), this->Now.Color.GetG(), this->Now.Color.GetB());
				Draw9SliceGraph(
					VECTOR2D(x1, y1), VECTOR2D(x2, y2),
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
	char		padding[7]{};
public:
	//コンストラクタ
	DrawModule(void) noexcept {}
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
public:
	void Init(const char* Path) noexcept {
		m_PartsParam.clear();
		m_AnimData.clear();
		std::ifstream file(Path);
		nlohmann::json data = nlohmann::json::parse(file);
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



class DrawUISystem : public SingletonBase<DrawUISystem> {
private:
	friend class SingletonBase<DrawUISystem>;
private:
	std::vector<DrawModule> m_DrawModule;
private://コンストラクタ、デストラクタ
	DrawUISystem(void) noexcept {
		m_DrawModule.reserve(16);
	}
	DrawUISystem(const DrawUISystem&) = delete;
	DrawUISystem(DrawUISystem&&) = delete;
	DrawUISystem& operator=(const DrawUISystem&) = delete;
	DrawUISystem& operator=(DrawUISystem&&) = delete;
	~DrawUISystem(void) noexcept {}
public:
	void Add(const char* Path) noexcept {
		m_DrawModule.emplace_back();
		m_DrawModule.back().Init(Path);
	}
public:
	void Init(const char* Path) noexcept {
		Add(Path);
	}
	void Update(void) noexcept {
		if (m_DrawModule.size() == 0) { return; }
		m_DrawModule.at(0).Update(VECTOR2D(0.f, 0.f), deg2rad(0.f), VECTOR2D(1.f, 1.f));
	}
	void Draw(void) noexcept {
		if (m_DrawModule.size() == 0) { return; }
		m_DrawModule.at(0).Draw(VECTOR2D(0.f, 0.f), deg2rad(0.f), VECTOR2D(1.f, 1.f));
	}
	void Dispose(void) noexcept {
		m_DrawModule.clear();
	}
};

