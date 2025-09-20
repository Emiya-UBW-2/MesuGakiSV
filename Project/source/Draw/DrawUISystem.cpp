#pragma warning(disable:5259)
#include "DrawUISystem.hpp"


const DrawUISystem* SingletonBase<DrawUISystem>::m_Singleton = nullptr;

bool DrawModule::PartsParam::IsHitPoint(int x, int y, Param2D Parent) const noexcept {
	if (!m_IsHitCheck) { return false; }
	switch (this->Type) {
	case PartsType::Box:
	case PartsType::NineSlice:
	{
		VECTOR2D PosOfs = Parent.OfsNoRad + this->Now.OfsNoRad + this->Now.Ofs.Rotate(this->Now.Rad + Parent.Rad);

		VECTOR2D Size;
		Size.x = this->Now.Size.x * this->Now.Scale.x * Parent.Scale.x;
		Size.y = this->Now.Size.y * this->Now.Scale.y * Parent.Scale.y;

		float x1 = PosOfs.x - Size.x * this->Now.Center.x;
		float y1 = PosOfs.y - Size.y * this->Now.Center.y;
		float x2 = PosOfs.x + Size.x * (1.f - this->Now.Center.x);
		float y2 = PosOfs.y + Size.y * (1.f - this->Now.Center.y);

		VECTOR2D center;
		center.x = PosOfs.x + Size.x * (0.5f - this->Now.Center.x) * 2.f;
		center.y = PosOfs.y + Size.y * (0.5f - this->Now.Center.y) * 2.f;

		auto GetPoint = [&](VECTOR2D ofs) {
			return center + (ofs - center).Rotate(this->Now.Rad + Parent.Rad);
			};

		if (HitPointToSquare(VECTOR2D(static_cast<float>(x), static_cast<float>(y)),
			GetPoint(VECTOR2D(x1, y1)),
			GetPoint(VECTOR2D(x2, y1)),
			GetPoint(VECTOR2D(x2, y2)),
			GetPoint(VECTOR2D(x1, y2))
			)) {
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
void DrawModule::PartsParam::Update(Param2D Parent) const noexcept {
	VECTOR2D PosOfs = Parent.OfsNoRad + this->Now.OfsNoRad + this->Now.Ofs.Rotate(this->Now.Rad + Parent.Rad);

	VECTOR2D scale;
	scale.x = this->Now.Scale.x * Parent.Scale.x;
	scale.y = this->Now.Scale.y * Parent.Scale.y;

	switch (this->Type) {
	case PartsType::Json:
	{
		Param2D Child;
		Child.OfsNoRad = PosOfs;
		Child.Rad = this->Now.Rad + Parent.Rad;
		Child.Scale = scale;
		Child.Color = this->Now.Color.GetMult(Parent.Color);
		DrawUISystem::Instance()->Get(this->DrawModuleHandle).Update(Child);
	}
		break;
	case PartsType::Box:
	case PartsType::NineSlice:
	case PartsType::Max:
		break;
	default:
		break;
	}
}
void DrawModule::PartsParam::Draw(Param2D Parent) const noexcept {
	VECTOR2D PosOfs = Parent.OfsNoRad + this->Now.OfsNoRad + this->Now.Ofs.Rotate(this->Now.Rad + Parent.Rad);

	VECTOR2D scale;
	scale.x = this->Now.Scale.x * Parent.Scale.x;
	scale.y = this->Now.Scale.y * Parent.Scale.y;

	VECTOR2D Size;
	Size.x = this->Now.Size.x * scale.x;
	Size.y = this->Now.Size.y * scale.y;

	switch (this->Type) {
	case PartsType::Box:
	{
		float x1 = PosOfs.x - Size.x * this->Now.Center.x;
		float y1 = PosOfs.y - Size.y * this->Now.Center.y;
		float x2 = PosOfs.x + Size.x * (1.f - this->Now.Center.x);
		float y2 = PosOfs.y + Size.y * (1.f - this->Now.Center.y);

		ColorRGBA	Color = this->Now.Color.GetMult(Parent.Color);

		DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, Color.GetA());

		if (this->Now.Rad + Parent.Rad == 0.f) {
			DxLib::DrawBox(
				static_cast<int>(x1), static_cast<int>(y1),
				static_cast<int>(x2), static_cast<int>(y2),
				Color.GetColor(),
				TRUE
			);
		}
		else {
			VECTOR2D center;
			center.x = PosOfs.x + Size.x * (0.5f - this->Now.Center.x) * 2.f;
			center.y = PosOfs.y + Size.y * (0.5f - this->Now.Center.y) * 2.f;

			auto GetPoint = [&](VECTOR2D ofs) {
				return center + (ofs - center).Rotate(this->Now.Rad + Parent.Rad);
				};

			VECTOR2D  P1 = GetPoint(VECTOR2D(x1, y1));
			VECTOR2D  P2 = GetPoint(VECTOR2D(x2, y1));
			VECTOR2D  P3 = GetPoint(VECTOR2D(x2, y2));
			VECTOR2D  P4 = GetPoint(VECTOR2D(x1, y2));
			DxLib::DrawQuadrangle(
				static_cast<int>(P1.x), static_cast<int>(P1.y),
				static_cast<int>(P2.x), static_cast<int>(P2.y),
				static_cast<int>(P3.x), static_cast<int>(P3.y),
				static_cast<int>(P4.x), static_cast<int>(P4.y),
				Color.GetColor(),
				TRUE
			);
		}
		DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
	break;
	case PartsType::NineSlice:
	{
		float x1 = PosOfs.x - Size.x * this->Now.Center.x;
		float y1 = PosOfs.y - Size.y * this->Now.Center.y;
		float x2 = PosOfs.x + Size.x * (1.f - this->Now.Center.x);
		float y2 = PosOfs.y + Size.y * (1.f - this->Now.Center.y);

		ColorRGBA	Color = this->Now.Color.GetMult(Parent.Color);

		DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, Color.GetA());
		DxLib::SetDrawBright(Color.GetR(), Color.GetG(), Color.GetB());
		Draw9SliceGraph(
			VECTOR2D(x1, y1), VECTOR2D(x2, y2),
			this->Min, this->Max,
			VECTOR2D(1.f, 1.f) - this->Now.Center,
			this->Now.Rad + Parent.Rad,
			this->ImageHandle,
			true,
			false
		);
		DxLib::SetDrawBright(255, 255, 255);
		DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
	break;
	case PartsType::Json:
	{
		Param2D Child;
		Child.OfsNoRad = PosOfs;
		Child.Rad = this->Now.Rad + Parent.Rad;
		Child.Scale = scale;
		Child.Color = this->Now.Color.GetMult(Parent.Color);
		DrawUISystem::Instance()->Get(this->DrawModuleHandle).Draw(Child);
	}
		break;
	case PartsType::Max:
		break;
	default:
		break;
	}
}

void DrawModule::AddChild(const char* ChildName, const char* FilePath, Param2D Parent) noexcept {
	m_PartsParam.emplace_back();
	auto& Back = m_PartsParam.back();
	Back.Name = ChildName;
	Back.Type = PartsType::Json;
	Back.Base = Parent;

	std::string Child;
	if (BranchName == "") {
		Child = Back.Name;
	}
	else {
		Child = BranchName + "/" + Back.Name;
	}
	Back.DrawModuleHandle = DrawUISystem::Instance()->Add(FilePath, Child.c_str());

	Back.Before = Back.Now = Back.Base;
}

void DrawModule::DeleteChild(const char* ChildName) noexcept {
	for (int loop = 0, max = static_cast<int>(this->m_PartsParam.size()); loop < max; ++loop) {
		auto& p = m_PartsParam.at(static_cast<size_t>(loop));
		if (p.Name == ChildName) {
			m_PartsParam.erase(m_PartsParam.begin() + loop);
			break;
		}
	}
}

void DrawModule::Init(const char* Path, const char* Branch) noexcept {
	m_PartsParam.clear();
	m_AnimData.clear();
	std::ifstream file(Path);
	nlohmann::json data = nlohmann::json::parse(file);

	BranchName = Branch;

	for (auto& d : data["MainParts"]) {
		m_PartsParam.emplace_back();
		auto& Back = m_PartsParam.at(static_cast<size_t>(m_PartsParam.size() - 1));
		Back.Name = d["Name"];
		{
			std::string Type = d["Type"];
			for (int loop = 0; loop < static_cast<int>(PartsType::Max); ++loop) {
				if (Type == PartsTypeStr[loop]) {
					Back.Type = static_cast<PartsType>(loop);
					break;
				}
			}
		}
		if (d.contains("OfsNoRad")) {
			Back.Base.OfsNoRad.SetByJson(d["OfsNoRad"]);
		}
		if (d.contains("Ofs")) {
			Back.Base.Ofs.SetByJson(d["Ofs"]);
		}
		if (d.contains("Size")) {
			Back.Base.Size.SetByJson(d["Size"]);
		}
		if (d.contains("Scale")) {
			Back.Base.Scale.SetByJson(d["Scale"]);
		}
		if (d.contains("Center")) {
			Back.Base.Center.SetByJson(d["Center"]);
		}
		if (d.contains("Rad")) {
			Back.Base.Rad = d["Rad"];
			Back.Base.Rad = deg2rad(Back.Base.Rad);
		}
		if (d.contains("BaseColor")) {
			Back.Base.Color.SetByJson(d["BaseColor"]);
		}
		else {
			Back.Base.Color.Set(255, 255, 255, 255);
		}

		if (d.contains("Min")) {
			Back.Min.SetByJson(d["Min"]);
		}
		if (d.contains("Max")) {
			Back.Max.SetByJson(d["Max"]);
		}

		if (d.contains("Image")) {
			Back.ImagePath = d["Image"];
			Back.ImageHandle = LoadGraph(Back.ImagePath.c_str());
		}
		if (d.contains("FilePath")) {
			std::string FilePath = d["FilePath"];
			std::string Child;
			if (BranchName == "") {
				Child = Back.Name;
			}
			else {
				Child = BranchName + "/" + Back.Name;
			}
			Back.DrawModuleHandle = DrawUISystem::Instance()->Add(FilePath.c_str(), Child.c_str());
		}
		;
		if (d.contains("IsHitCheck")) {
			Back.m_IsHitCheck = d["IsHitCheck"];
		}

		Back.Before = Back.Now = Back.Base;
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

	m_IsActive = true;
	m_IsSelect = false;

	m_UseActive = false;
	for (auto& a : m_AnimData) {
		if (AnimType::Active != a.m_Type && AnimType::DisActive != a.m_Type) { continue; }
		m_UseActive = true;
	}

	m_UseButton = false;
	for (auto& a : m_AnimData) {
		if (AnimType::OnSelect != a.m_Type && AnimType::OnPress != a.m_Type) { continue; }
		m_UseButton = true;
	}
}