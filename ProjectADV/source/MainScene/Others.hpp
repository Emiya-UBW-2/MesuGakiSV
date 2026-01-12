#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:4710)
#pragma warning(disable:4711)
#pragma warning(disable:5039)

#include "BaseObject.hpp"

class Stage :public BaseObject {
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
	void Load_Sub(void) noexcept override {
	}
	void Init_Sub(void) noexcept override {
	}
	void Update_Sub(void) noexcept override {
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

class Table :public BaseObject {
public:
	Table(void) noexcept {}
	Table(const Table&) = delete;
	Table(Table&&) = delete;
	Table& operator=(const Table&) = delete;
	Table& operator=(Table&&) = delete;
	virtual ~Table(void) noexcept {}
private:
	int				GetFrameNum(void) noexcept override { return 0; }
	const char* GetFrameStr(int) noexcept override { return nullptr; }
public:
	void Load_Sub(void) noexcept override {
	}
	void Init_Sub(void) noexcept override {
	}
	void Update_Sub(void) noexcept override {
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
