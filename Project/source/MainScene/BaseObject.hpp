#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:4710)
#pragma warning(disable:4711)
#pragma warning(disable:5039)

#include "../Draw/MV1.hpp"

struct Frames {
	int first{};
	Util::Matrix4x4 second{};
	Util::Matrix4x4 third{};
};

class BaseObject {
protected:
	Util::Matrix4x4			MyMat;
	Draw::MV1				ModelID{};
	std::vector<Frames>		m_Frames;
public:
	BaseObject(void) noexcept {}
	BaseObject(const BaseObject&) = delete;
	BaseObject(BaseObject&&) = delete;
	BaseObject& operator=(const BaseObject&) = delete;
	BaseObject& operator=(BaseObject&&) = delete;
	virtual ~BaseObject(void) noexcept {}
public:
	virtual int	GetFrameNum(void) noexcept { return 0; }
	virtual const char* GetFrameStr(int) noexcept { return nullptr; }
public:
	bool			HaveFrame(int frame) const noexcept { return this->m_Frames[static_cast<size_t>(frame)].first != InvalidID; }
	const auto& GetFrame(int frame) const noexcept { return this->m_Frames[static_cast<size_t>(frame)].first; }
	const auto& GetFrameBaseLocalMat(int frame) const noexcept { return this->m_Frames[static_cast<size_t>(frame)].second; }
	const auto& GetFrameBaseLocalWorldMat(int frame) const noexcept { return this->m_Frames[static_cast<size_t>(frame)].third; }
	const Util::Matrix4x4& GetMat(void) const noexcept { return MyMat; }
	
	Util::Matrix4x4	GetFrameLocalMatrix(int frame) const noexcept { return ModelID.GetFrameLocalMatrix(GetFrame(frame)); }
	Util::Matrix4x4	GetFrameLocalWorldMatrix(int frame) const noexcept { return ModelID.GetFrameLocalWorldMatrix(GetFrame(frame)); }

	auto& SetAnim(size_t frame) noexcept { return ModelID.SetAnim(frame); }
	void ResetFrameUserLocalMatrix(int frame) noexcept { ModelID.ResetFrameUserLocalMatrix(GetFrame(frame)); }
	void SetFrameLocalMatrix(int frame, Util::Matrix4x4 mat) const noexcept { ModelID.SetFrameLocalMatrix(GetFrame(frame), mat); }
	void SetMatrix(const Util::Matrix4x4& mat) noexcept {
		MyMat = mat;
		ModelID.SetMatrix(MyMat);
	}
public:
	void Load(void) noexcept {
		Load_Sub();
	}
	void Init(void) noexcept {
		// フレーム
		{
			this->m_Frames.clear();
			if (GetFrameNum() > 0) {
				this->m_Frames.resize(static_cast<size_t>(GetFrameNum()));
				for (auto& f : this->m_Frames) {
					f.first = InvalidID;
				}
				size_t count = 0;
				for (int frameNum = 0, Max = this->ModelID.GetFrameNum(); frameNum < Max; ++frameNum) {
					if (Util::UTF8toSjis(this->ModelID.GetFrameName(frameNum)) == GetFrameStr(static_cast<int>(count))) {
						// そのフレームを登録
						this->m_Frames[count].first = frameNum;
						this->m_Frames[count].second = Util::Matrix4x4::Mtrans(this->ModelID.GetFrameLocalMatrix(this->m_Frames[count].first).pos());
						this->m_Frames[count].third = this->ModelID.GetFrameLocalWorldMatrix(this->m_Frames[count].first);
					}
					else if (frameNum < Max - 1) {
						continue;// 飛ばす
					}
					++count;
					frameNum = 0;
					if (count >= this->m_Frames.size()) {
						break;
					}
				}
			}
		}
		Init_Sub();
	}
	void Update(void) noexcept {
		Update_Sub();
	}
	void SetShadowDraw(void) const noexcept {
		SetShadowDraw_Sub();
	}
	void CheckDraw(void) noexcept {
		CheckDraw_Sub();
	}
	void Draw(void) const noexcept {
		Draw_Sub();
	}
	void ShadowDraw(void) const noexcept {
		ShadowDraw_Sub();
	}
	void Dispose(void) noexcept {
		Dispose_Sub();
	}
protected:
	virtual void Load_Sub(void) noexcept = 0;
	virtual void Init_Sub(void) noexcept = 0;
	virtual void Update_Sub(void) noexcept = 0;
	virtual void SetShadowDraw_Sub(void) const noexcept = 0;
	virtual void CheckDraw_Sub(void) noexcept = 0;
	virtual void Draw_Sub(void) const noexcept = 0;
	virtual void ShadowDraw_Sub(void) const noexcept = 0;
	virtual void Dispose_Sub(void) noexcept = 0;
};
