#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:4710)
#pragma warning(disable:4711)
#pragma warning(disable:5039)

#include "BaseObject.hpp"

class Stage :public BaseObject {
	Util::Matrix4x4			m_TargetMat;
public:
	Stage(void) noexcept {}
	Stage(const Stage&) = delete;
	Stage(Stage&&) = delete;
	Stage& operator=(const Stage&) = delete;
	Stage& operator=(Stage&&) = delete;
	virtual ~Stage(void) noexcept {}
private:
	int				GetFrameNum(void) noexcept override { return 0; }
	const char*		GetFrameStr(int) noexcept override { return nullptr; }
public:
	void SetMat(const Util::Matrix4x4& mat)noexcept {
		m_TargetMat = mat;
	}
	void SetupMat(const Util::Matrix4x4& mat)noexcept {
		SetMatrix(mat);
		m_TargetMat = GetMat();
	}
public:
	void Load_Sub(void) noexcept override {
	}
	void Init_Sub(void) noexcept override {
	}
	void Update_Sub(void) noexcept override {
		auto Mat = GetMat();
		Util::Easing(&Mat, m_TargetMat, 0.975f);
		SetMatrix(Mat);
	}
	void SetShadowDraw_Sub(void) const noexcept override {
		GetModel().DrawModel();
	}
	void CheckDraw_Sub(void) noexcept override {
	}
	void Draw_Sub(void) const noexcept override {
		GetModel().DrawModel();
	}
	void ShadowDraw_Sub(void) const noexcept override {
		GetModel().DrawModel();
	}
	void Dispose_Sub(void) noexcept override {
		DisposeModel();
	}
};

class Billboard :public BaseObject {
	const Draw::GraphHandle* m_Pic{};
	float m_cx{};
	float m_cy{};
	float m_scale{};
	float m_angle{};
	Util::Matrix4x4			m_TargetMat;
public:
	Billboard(void) noexcept {}
	Billboard(const Billboard&) = delete;
	Billboard(Billboard&&) = delete;
	Billboard& operator=(const Billboard&) = delete;
	Billboard& operator=(Billboard&&) = delete;
	virtual ~Billboard(void) noexcept {}
private:
	int				GetFrameNum(void) noexcept override { return 0; }
	const char* GetFrameStr(int) noexcept override { return nullptr; }
public:
	void SetupBillboardParam(float cx,float cy,float scale,float angle)noexcept {
		m_cx = cx;
		m_cy = cy;
		m_scale = scale;
		m_angle = angle;
	}
	void SetupMat(const Util::Matrix4x4& mat)noexcept {
		SetMatrix(mat);
		m_TargetMat = GetMat();
	}
public:
	void Load_Sub(void) noexcept override {
	}
	void Init_Sub(void) noexcept override {
		m_Pic = Draw::GraphPool::Instance()->Get(GetFilePath())->Get();
	}
	void Update_Sub(void) noexcept override {
		Util::Easing(&m_TargetMat, GetMat(), 0.975f);
	}
	void SetShadowDraw_Sub(void) const noexcept override {}
	void CheckDraw_Sub(void) noexcept override {}
	void Draw_Sub(void) const noexcept override {
		SetUseZBufferFlag(true);
		DrawBillboard3D(m_TargetMat.pos().get(), m_cx, m_cy, m_scale, m_angle, m_Pic->get(), true);
	}
	void ShadowDraw_Sub(void) const noexcept override {}
	void Dispose_Sub(void) noexcept override {
		DisposeModel();
	}
};
