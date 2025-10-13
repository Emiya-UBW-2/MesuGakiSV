﻿#pragma once

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

enum class PHYSICS_SETUP {
	DISABLE,
	LOADCALC,
	REALTIME,
};

class ResourceModel;
class BaseObject;

using SharedModel = std::shared_ptr<ResourceModel>;
using SharedObj = std::shared_ptr<BaseObject>;

class ResourceModel {
protected:
	Draw::MV1				ModelID{};
	std::vector<Frames>		m_Frames;
private:
	PHYSICS_SETUP								m_PHYSICS_SETUP{ PHYSICS_SETUP::DISABLE };
	char		padding[4]{};
	std::string									m_FilePath;
	std::string									m_ObjFileName;
	std::string									m_ColFileName;
	bool										m_IsEndLoadData{};
	char		padding2[7]{};
public:
	ResourceModel(void) noexcept { this->m_IsEndLoadData = false; }
	ResourceModel(const ResourceModel&) = delete;
	ResourceModel(ResourceModel&&) = delete;
	ResourceModel& operator=(const ResourceModel&) = delete;
	ResourceModel& operator=(ResourceModel&&) = delete;

	virtual ~ResourceModel(void) noexcept {}
public:
public:
	auto			GetPathCompare(std::string_view filepath, std::string_view objfilename, std::string_view colfilename) const noexcept {
		return ((this->m_FilePath == filepath) && (this->m_ObjFileName == objfilename) && (this->m_ColFileName == colfilename));
	}
public:
	bool			GetIsEndLoadData() const noexcept { return this->m_IsEndLoadData; }
	bool			HaveFrame(int frame) const noexcept { return this->m_Frames[static_cast<size_t>(frame)].first != InvalidID; }
	const auto& GetFrame(int frame) const noexcept { return this->m_Frames[static_cast<size_t>(frame)].first; }
	const auto& GetFrameBaseLocalMat(int frame) const noexcept { return this->m_Frames[static_cast<size_t>(frame)].second; }
	const auto& GetFrameBaseLocalWorldMat(int frame) const noexcept { return this->m_Frames[static_cast<size_t>(frame)].third; }

	auto& SetModel(void) noexcept { return this->ModelID; }
	const auto& GetModel(void) const noexcept { return this->ModelID; }

	Util::Matrix4x4	GetFrameLocalMatrix(int frame) const noexcept { return ModelID.GetFrameLocalMatrix(GetFrame(frame)); }
	Util::Matrix4x4	GetFrameLocalWorldMatrix(int frame) const noexcept { return ModelID.GetFrameLocalWorldMatrix(GetFrame(frame)); }

	auto& SetAnim(size_t frame) noexcept { return ModelID.SetAnim(frame); }
	void ResetFrameUserLocalMatrix(int frame) noexcept { ModelID.ResetFrameUserLocalMatrix(GetFrame(frame)); }
	void SetFrameLocalMatrix(int frame, Util::Matrix4x4 mat) const noexcept { ModelID.SetFrameLocalMatrix(GetFrame(frame), mat); }
public:
	void LoadStart(PHYSICS_SETUP TYPE, std::string_view path, std::string_view objname, std::string_view colname) noexcept {
		this->m_PHYSICS_SETUP = TYPE;
		this->m_FilePath = path;
		this->m_ObjFileName = objname;
		this->m_ColFileName = colname;
		std::string FileName = this->m_FilePath + this->m_ObjFileName + ".mv1";
		switch (this->m_PHYSICS_SETUP) {
		case PHYSICS_SETUP::DISABLE:
			Draw::MV1::Load(FileName, &ModelID, DX_LOADMODEL_PHYSICS_DISABLE);
			break;
		case PHYSICS_SETUP::LOADCALC:
			Draw::MV1::Load(FileName, &ModelID, DX_LOADMODEL_PHYSICS_LOADCALC);
			break;
		case PHYSICS_SETUP::REALTIME:
			Draw::MV1::Load(FileName, &ModelID, DX_LOADMODEL_PHYSICS_REALTIME);
			break;
		default:
			break;
		}
	}
	void LoadEnd(const std::shared_ptr<BaseObject>& pObj) noexcept;

	void CopyModel(const std::shared_ptr<ResourceModel>& pBase) noexcept {
		this->m_PHYSICS_SETUP = pBase->m_PHYSICS_SETUP;
		this->m_FilePath = pBase->m_FilePath;
		this->m_ObjFileName = pBase->m_ObjFileName;
		this->m_ColFileName = pBase->m_ColFileName;
		// model
		this->ModelID.Duplicate(pBase->ModelID);
		// フレーム
		this->m_Frames.resize(pBase->m_Frames.size());
		for (size_t index = 0; auto& f : this->m_Frames) {
			f.first = pBase->m_Frames[index].first;
			if (f.first != InvalidID) {
				f.second = pBase->m_Frames[index].second;
				f.third = pBase->m_Frames[index].third;
			}
			++index;
		}
	}

	void			DisposeModel(void) noexcept {
		this->ModelID.Dispose();
	}
};

class BaseObject : public ResourceModel {
protected:
	Util::Matrix4x4			MyMat;
	int						m_UniqueID{ 0 };
	bool										m_IsDelete{ false };
	char		padding[3]{};
public:
	BaseObject(void) noexcept {}
	BaseObject(const BaseObject&) = delete;
	BaseObject(BaseObject&&) = delete;
	BaseObject& operator=(const BaseObject&) = delete;
	BaseObject& operator=(BaseObject&&) = delete;
	virtual ~BaseObject(void) noexcept {}
public:
	const Util::Matrix4x4& GetMat(void) const noexcept { return MyMat; }
	int GetObjectID(void) const noexcept { return m_UniqueID; }
	const auto& GetIsDelete(void) const noexcept { return this->m_IsDelete; }

	void SetMatrix(const Util::Matrix4x4& mat) noexcept {
		MyMat = mat;
		ModelID.SetMatrix(MyMat);
	}
	void SetObjectID(int ID) noexcept { m_UniqueID = ID; }
	void				SetDelete(void) noexcept { this->m_IsDelete = true; }
public:
	virtual int	GetFrameNum(void) noexcept = 0;// { return 0; }
	virtual const char* GetFrameStr(int) noexcept = 0;// { return nullptr; }
public:
	void Load(void) noexcept {
		Load_Sub();
	}
	void Init(void) noexcept {
		// フレーム
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

class ObjectManager : public Util::SingletonBase<ObjectManager> {
private:
	friend class Util::SingletonBase<ObjectManager>;
private:
	std::vector<SharedModel>	m_ModelList;
	std::vector<SharedObj>		m_ObjectList;
	int							m_LastUniqueID{ 0 };
	char		padding[4]{};
	//switchs						m_ResetP;
private:
	ObjectManager(void) noexcept {
		this->m_ModelList.reserve(24);
		this->m_ObjectList.reserve(256);
	}
	ObjectManager(const ObjectManager&) = delete;
	ObjectManager(ObjectManager&&) = delete;
	ObjectManager& operator=(const ObjectManager&) = delete;
	ObjectManager& operator=(ObjectManager&&) = delete;
private:
	void			AddObject(const SharedObj& NewObj) noexcept;
	void			LoadModelAfter(const SharedObj& pObj, const SharedObj& pAnim, std::string_view filepath, std::string_view objfilename = "model", std::string_view colfilename = "col") noexcept;
public:
	//あらかじめモデルを読み込んでおく
	const SharedModel* LoadModel(std::string_view path, std::string_view objname = "model", std::string_view colname = "col") noexcept;
	//モデル未使用バージョン
	void			InitObject(const SharedObj& pObj) noexcept;
	//モデル使用バージョン
	void			InitObject(const SharedObj& pObj, std::string_view filepath, std::string_view objfilename = "model", std::string_view colfilename = "col") noexcept;
	//アニメーション兼務モデルバージョン
	void			InitObject(const SharedObj& pObj, const SharedObj& pAnim, std::string_view filepath, std::string_view objfilename = "model", std::string_view colfilename = "col") noexcept;
public:
	SharedObj* GetObj(int UniqueID) noexcept;
public:
	void			DelObj(const SharedObj& ptr) noexcept;
public:
	void			UpdateObject(void) noexcept;
	void			Draw(void) noexcept {
		for (auto& object : this->m_ObjectList) {
			if (!object) { continue; }
			object->CheckDraw();
			object->Draw();
		}
	}
	void			Draw_SetShadow(void) noexcept {
		for (auto& object : this->m_ObjectList) {
			if (!object) { continue; }
			object->SetShadowDraw();
		}
	}
	void			Draw_Shadow(void) noexcept {
		for (auto& object : this->m_ObjectList) {
			if (!object) { continue; }
			object->ShadowDraw();
		}
	}
	void			Draw_Depth(void) noexcept {
		for (auto& object : this->m_ObjectList) {
			if (!object) { continue; }
			object->Draw();
		}
	}
	void			DeleteAll(void) noexcept;
};
