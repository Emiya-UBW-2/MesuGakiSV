#pragma warning(disable:5259)
#include "DrawUISystem.hpp"

bool DrawModule::PartsParam::IsHitPoint(int x, int y, Param2D Parent) const noexcept {
	if (!m_IsHitCheck) { return false; }
	switch (this->Type) {
	case PartsType::Box:
	case PartsType::NineSlice:
	{
		float Rad = this->Now.Rad + Parent.Rad;
		VECTOR2D PosOfs = Parent.OfsNoRad + this->Now.OfsNoRad + this->Now.Ofs.Rotate(Rad);

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

		if (HitPointToSquare(VECTOR2D(static_cast<float>(x), static_cast<float>(y)),
			center + (VECTOR2D(x1, y1) - center).Rotate(Rad),
			center + (VECTOR2D(x2, y1) - center).Rotate(Rad),
			center + (VECTOR2D(x2, y2) - center).Rotate(Rad),
			center + (VECTOR2D(x1, y2) - center).Rotate(Rad)
		)) {
			return true;
		}
	}
	break;
	case PartsType::Image:
	case PartsType::String:
	case PartsType::Json:
	case PartsType::Max:
	default:
		break;
	}
	return false;
}
void DrawModule::PartsParam::Update(DrawUISystem* DrawUI, Param2D Parent) const noexcept {
	float Rad = this->Now.Rad + Parent.Rad;
	VECTOR2D PosOfs = Parent.OfsNoRad + this->Now.OfsNoRad + this->Now.Ofs.Rotate(Rad);

	VECTOR2D scale;
	scale.x = this->Now.Scale.x * Parent.Scale.x;
	scale.y = this->Now.Scale.y * Parent.Scale.y;

	ColorRGBA	Color = this->Now.Color.GetMult(Parent.Color);

	switch (this->Type) {
	case PartsType::Json:
	{
		Param2D Child;
		Child.OfsNoRad = PosOfs;
		Child.Rad = Rad;
		Child.Scale = scale;
		Child.Color = Color;
		DrawUI->Get(this->DrawModuleHandle).Update(DrawUI, Child);
	}
	break;
	case PartsType::Box:
	case PartsType::NineSlice:
	case PartsType::Image:
	case PartsType::String:
	case PartsType::Max:
	default:
		break;
	}
}
void DrawModule::PartsParam::Draw(DrawUISystem* DrawUI, Param2D Parent) const noexcept {
	float Rad = this->Now.Rad + Parent.Rad;
	VECTOR2D PosOfs = Parent.OfsNoRad + this->Now.OfsNoRad + this->Now.Ofs.Rotate(Rad);

	VECTOR2D scale;
	scale.x = this->Now.Scale.x * Parent.Scale.x;
	scale.y = this->Now.Scale.y * Parent.Scale.y;

	VECTOR2D Size;
	Size.x = this->Now.Size.x * scale.x;
	Size.y = this->Now.Size.y * scale.y;

	ColorRGBA	Color = this->Now.Color.GetMult(Parent.Color);

	float x1 = PosOfs.x - Size.x * this->Now.Center.x;
	float y1 = PosOfs.y - Size.y * this->Now.Center.y;
	float x2 = PosOfs.x + Size.x * (1.f - this->Now.Center.x);
	float y2 = PosOfs.y + Size.y * (1.f - this->Now.Center.y);

	switch (this->Type) {
	case PartsType::Box:
	{
		DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, Color.GetA());

		if (Rad == 0.f) {
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

			VECTOR2D  P1 = center + (VECTOR2D(x1, y1) - center).Rotate(Rad);
			VECTOR2D  P2 = center + (VECTOR2D(x2, y1) - center).Rotate(Rad);
			VECTOR2D  P3 = center + (VECTOR2D(x2, y2) - center).Rotate(Rad);
			VECTOR2D  P4 = center + (VECTOR2D(x1, y2) - center).Rotate(Rad);
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
		DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, Color.GetA());
		DxLib::SetDrawBright(Color.GetR(), Color.GetG(), Color.GetB());
		Draw9SliceGraph(
			VECTOR2D(x1, y1), VECTOR2D(x2, y2),
			this->Min, this->Max,
			VECTOR2D(1.f, 1.f) - this->Now.Center,
			Rad,
			this->ImageHandle->get(),
			true,
			false
		);
		DxLib::SetDrawBright(255, 255, 255);
		DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
	break;
	case PartsType::Image:
	{
		DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, Color.GetA());
		DxLib::SetDrawBright(Color.GetR(), Color.GetG(), Color.GetB());

		if (Rad == 0.f) {
			DxLib::DrawExtendGraph(
				static_cast<int>(x1), static_cast<int>(y1),
				static_cast<int>(x2), static_cast<int>(y2),
				this->ImageHandle->get(),
				true
			);
		}
		else {
			/*
			DxLib::DrawRotaGraph3(
				static_cast<int>((x2 + x1) / 2.f), static_cast<int>((y2 + y1) / 2.f),
				static_cast<int>((x2 + x1) / 2.f), static_cast<int>((y2 + y1) / 2.f),
				scale.x, scale.y, Rad, this->ImageHandle->get(), true);
			//*/
			DxLib::DrawRotaGraphFast(
				static_cast<int>((x2 + x1) / 2.f), static_cast<int>((y2 + y1) / 2.f),
				scale.y, Rad, this->ImageHandle->get(), true);
		}
		DxLib::SetDrawBright(255, 255, 255);
		DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
	break;
	case PartsType::String:
	{
		DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, Color.GetA());
		if (Rad == 0.f) {
			/*
			DxLib::DrawBox(
				static_cast<int>(x1), static_cast<int>(y1),
				static_cast<int>(x2), static_cast<int>(y2),
				Color.GetColor(),
				TRUE
			);
			this->m_FontHandle->DrawStringAutoFit(
				static_cast<int>(x1), static_cast<int>(y1),
				static_cast<int>(x2), static_cast<int>(y2),
				Color.GetColor(), GetColor(0, 0, 0), this->String);
			//*/
			FontPool::Instance()->Get(FontType::DIZ_UD_Gothic, 18, 3)->DrawString(
				FontXCenter::MIDDLE, FontYCenter::MIDDLE,
				static_cast<int>((x2 + x1) / 2.f), static_cast<int>((y2 + y1) / 2.f),
				Color.GetColor(), GetColor(0, 0, 0),
				SjistoUTF8(this->String));
		}
		else {
		}
		DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
	break;
	case PartsType::Json:
	{
		Param2D Child;
		Child.OfsNoRad = PosOfs;
		Child.Rad = Rad;
		Child.Scale = scale;
		Child.Color = Color;
		DrawUI->Get(this->DrawModuleHandle).Draw(DrawUI, Child);
	}
	break;
	case PartsType::Max:
	default:
		break;
	}
}

void DrawModule::PartsParam::AddChild(DrawUISystem* DrawUI, const char* ChildName, const char* ChildBranch, const char* FilePath, Param2D Param) noexcept {
	this->Name = ChildName;
	this->Type = PartsType::Json;
	this->Base = Param;
	this->DrawModuleHandle = DrawUI->Add(FilePath, ChildBranch);
	SetDefault();
}

void DrawModule::PartsParam::SetByJson(DrawUISystem* DrawUI, nlohmann::json& d, std::string BranchName) noexcept {
	this->Name = d["Name"];
	{
		std::string TypeStr = d["Type"];
		for (int loop = 0; loop < static_cast<int>(PartsType::Max); ++loop) {
			if (TypeStr == PartsTypeStr[loop]) {
				this->Type = static_cast<PartsType>(loop);
				break;
			}
		}
	}
	if (d.contains("OfsNoRad")) {
		this->Base.OfsNoRad.SetByJson(d["OfsNoRad"]);
	}
	if (d.contains("Ofs")) {
		this->Base.Ofs.SetByJson(d["Ofs"]);
	}
	if (d.contains("Size")) {
		this->Base.Size.SetByJson(d["Size"]);
	}
	if (d.contains("Scale")) {
		this->Base.Scale.SetByJson(d["Scale"]);
	}
	if (d.contains("Center")) {
		this->Base.Center.SetByJson(d["Center"]);
	}
	if (d.contains("Rad")) {
		this->Base.Rad = d["Rad"];
		this->Base.Rad = deg2rad(this->Base.Rad);
	}
	if (d.contains("BaseColor")) {
		this->Base.Color.SetByJson(d["BaseColor"]);
	}
	else {
		this->Base.Color.Set(255, 255, 255, 255);
	}

	if (d.contains("FontID")) {
		this->String = d["FontID"];
	}
	if (d.contains("Min")) {
		this->Min.SetByJson(d["Min"]);
	}
	if (d.contains("Max")) {
		this->Max.SetByJson(d["Max"]);
	}

	if (d.contains("Image")) {
		this->ImagePath = d["Image"];
		this->ImageHandle = GraphPool::Instance()->Get(this->ImagePath)->Get();
	}
	if (d.contains("FilePath")) {
		std::string FilePath = d["FilePath"];
		std::string Child;
		if (BranchName == "") {
			Child = this->Name;
		}
		else {
			Child = BranchName + "/" + this->Name;
		}
		this->DrawModuleHandle = DrawUI->Add(FilePath.c_str(), Child.c_str());
	}
	;
	if (d.contains("IsHitCheck")) {
		this->m_IsHitCheck = d["IsHitCheck"];
	}

	this->SetDefault();
}

void DrawModule::AddChild(DrawUISystem* DrawUI, const char* ChildName, const char* FilePath, Param2D Param) noexcept {
	m_PartsParam.emplace_back();

	std::string ChildBranch;
	if (BranchName == "") {
		ChildBranch = ChildName;
	}
	else {
		ChildBranch = BranchName + "/" + ChildName;
	}
	m_PartsParam.back().AddChild(DrawUI, ChildName, ChildBranch.c_str(), FilePath, Param);
}

void DrawModule::DeleteChild(const char* ChildName) noexcept {
	for (int loop = 0, max = static_cast<int>(this->m_PartsParam.size()); loop < max; ++loop) {
		auto& p = m_PartsParam.at(static_cast<size_t>(loop));
		if (p.GetName() == ChildName) {
			m_PartsParam.erase(m_PartsParam.begin() + loop);
			break;
		}
	}
}

void DrawModule::Init(DrawUISystem* DrawUI, const char* Path, const char* Branch) noexcept {
	m_PartsParam.clear();
	m_AnimData.clear();
	std::ifstream file(Path);
	nlohmann::json data = nlohmann::json::parse(file);

	BranchName = Branch;

	for (auto& d : data["MainParts"]) {
		m_PartsParam.emplace_back();
		m_PartsParam.back().SetByJson(DrawUI, d, BranchName);
	}
	for (auto& d : data["Anim"]) {
		m_AnimData.emplace_back();
		auto& AnimBack = m_AnimData.back();
		//
		{
			std::string Type = d["Type"];
			for (int loop = 0; loop < static_cast<int>(AnimType::Max); ++loop) {
				if (Type == AnimTypeStr[loop]) {
					AnimBack.m_Type = static_cast<AnimType>(loop);
					break;
				}
			}
		}
		//
		AnimBack.m_IsLoop = d["Loop"];
		//
		for (auto& a : d["Anim"]) {
			AnimBack.m_AnimParam.emplace_back();
			auto& AnimParamBack = AnimBack.m_AnimParam.back();
			//
			{
				std::string Target = a["Target"];
				for (auto& p : m_PartsParam) {
					if (Target == p.GetName()) {
						AnimParamBack.TargetID = static_cast<size_t>(&p - &m_PartsParam.front());
						break;
					}
				}
			}
			//
			if (a.contains("OfsNoRad")) {
				AnimParamBack.m_OfsNoRadChanged = true;
				AnimParamBack.Target.OfsNoRad.SetByJson(a["OfsNoRad"]);
			}
			if (a.contains("Ofs")) {
				AnimParamBack.m_OfsChanged = true;
				AnimParamBack.Target.Ofs.SetByJson(a["Ofs"]);
			}
			if (a.contains("Size")) {
				AnimParamBack.m_SizeChanged = true;
				AnimParamBack.Target.Size.SetByJson(a["Size"]);
			}
			if (a.contains("Scale")) {
				AnimParamBack.m_ScaleChanged = true;
				AnimParamBack.Target.Scale.SetByJson(a["Scale"]);
			}
			if (a.contains("Center")) {
				AnimParamBack.m_CenterChanged = true;
				AnimParamBack.Target.Center.SetByJson(a["Center"]);
			}
			if (a.contains("Rad")) {
				AnimParamBack.m_RadChanged = true;
				AnimParamBack.Target.Rad = a["Rad"];
				AnimParamBack.Target.Rad = deg2rad(AnimParamBack.Target.Rad);
			}
			if (a.contains("Color")) {
				AnimParamBack.m_ColorChanged = true;
				AnimParamBack.Target.Color.SetByJson(a["Color"]);
			}
			else {
				AnimParamBack.Target.Color.Set(255, 255, 255, 255);
			}
			//
			AnimParamBack.StartFrame = a["StartFrame"];
			AnimParamBack.EndFrame = a["EndFrame"];
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