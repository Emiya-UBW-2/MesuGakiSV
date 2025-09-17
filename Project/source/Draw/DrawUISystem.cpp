#pragma warning(disable:5259)
#include "DrawUISystem.hpp"


const DrawUISystem* SingletonBase<DrawUISystem>::m_Singleton = nullptr;

bool DrawModule::PartsParam::IsHitPoint(int x, int y, VECTOR2D Pos, float Rad, VECTOR2D Scale) const noexcept {
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
	case PartsType::Json:
		break;
	case PartsType::Max:
		break;
	default:
		break;
	}
	return false;
}
void DrawModule::PartsParam::Update(VECTOR2D Pos, float Rad, VECTOR2D Scale) const noexcept {
	VECTOR2D PosOfs = Pos + this->Now.OfsNoRad + this->Now.Ofs.Rotate(this->Now.Rad + Rad);

	VECTOR2D scale;
	scale.x = this->Now.Scale.x * Scale.x;
	scale.y = this->Now.Scale.y * Scale.y;

	switch (this->Type) {
	case PartsType::Json:
		DrawUISystem::Instance()->Get(this->DrawModuleHandle).Update(PosOfs, this->Now.Rad + Rad, scale);
		break;
	case PartsType::Box:
	case PartsType::NineSlice:
	case PartsType::Max:
		break;
	default:
		break;
	}
}
void DrawModule::PartsParam::Draw(VECTOR2D Pos, float Rad, VECTOR2D Scale) const noexcept {
	VECTOR2D PosOfs = Pos + this->Now.OfsNoRad + this->Now.Ofs.Rotate(this->Now.Rad + Rad);

	VECTOR2D scale;
	scale.x = this->Now.Scale.x * Scale.x;
	scale.y = this->Now.Scale.y * Scale.y;

	VECTOR2D Size;
	Size.x = this->Now.Size.x * scale.x;
	Size.y = this->Now.Size.y * scale.y;

	switch (this->Type) {
	case PartsType::Box:
	{
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
		DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, this->Now.Color.GetA());
		DxLib::DrawQuadrangle(
			static_cast<int>(P1.x), static_cast<int>(P1.y),
			static_cast<int>(P2.x), static_cast<int>(P2.y),
			static_cast<int>(P3.x), static_cast<int>(P3.y),
			static_cast<int>(P4.x), static_cast<int>(P4.y),
			this->Now.Color.GetColor(),
			TRUE
		);
		DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
	break;
	case PartsType::NineSlice:
	{
		float x1 = PosOfs.x - Size.x * (1.f - this->Now.Center.x);
		float y1 = PosOfs.y - Size.y * (1.f - this->Now.Center.y);
		float x2 = PosOfs.x + Size.x * this->Now.Center.x;
		float y2 = PosOfs.y + Size.y * this->Now.Center.y;
		DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, this->Now.Color.GetA());
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
		DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
	break;
	case PartsType::Json:
		DrawUISystem::Instance()->Get(this->DrawModuleHandle).Draw(PosOfs, this->Now.Rad + Rad, scale);
		break;
	case PartsType::Max:
		break;
	default:
		break;
	}
}

void DrawModule::Init(const char* Path) noexcept {
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
		if (d.contains("FilePath")) {
			std::string FilePath = d["FilePath"];
			m_PartsParam.back().DrawModuleHandle = DrawUISystem::Instance()->Add(FilePath.c_str());
		}
		;
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