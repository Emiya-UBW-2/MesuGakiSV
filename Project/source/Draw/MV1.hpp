#pragma warning(disable:4464)
#pragma once
#include "../Util/Enum.hpp"
#include "../Util/Util.hpp"

#include "ImageDraw.hpp"

namespace DXLibRef {
	class MV1 : public Util::DXHandle {
	public:
		class AnimControler {
			class MV1AnimeHandle : public Util::DXHandle {
				const MV1* m_pBaseModel{};
			public:
				MV1AnimeHandle(void) noexcept {}
				MV1AnimeHandle(const MV1AnimeHandle& o) noexcept { *this = o; }
				MV1AnimeHandle(MV1AnimeHandle&& o) noexcept { *this = o; }
				MV1AnimeHandle& operator=(const MV1AnimeHandle& o) noexcept {
					this->m_pBaseModel = o.m_pBaseModel;
					Util::DXHandle::SetHandleDirect(o.get());
					return *this;
				}
				MV1AnimeHandle& operator=(MV1AnimeHandle&& o) noexcept {
					this->m_pBaseModel = o.m_pBaseModel;
					Util::DXHandle::SetHandleDirect(o.get());
					return *this;
				}
				virtual ~MV1AnimeHandle(void) noexcept {}
			protected:
				void			Dispose_Sub(void) noexcept override {
					MV1DetachAnim(m_pBaseModel->get(), Util::DXHandle::get());
					m_pBaseModel = nullptr;
				}
			public:
				void			Attach(MV1* model, size_t AnimIndex, const MV1* model_haveanim = nullptr) noexcept {
					m_pBaseModel = model;
					if (model_haveanim && (m_pBaseModel->get() != model_haveanim->get())) {
						Util::DXHandle::SetHandleDirect(MV1AttachAnim(m_pBaseModel->get(), static_cast<int>(AnimIndex), model_haveanim->get()));
					}
					else {
						Util::DXHandle::SetHandleDirect(MV1AttachAnim(m_pBaseModel->get(), static_cast<int>(AnimIndex)));
					}
				}
				auto			GetTotalTime(void) const noexcept { return MV1GetAttachAnimTotalTime(m_pBaseModel->get(), Util::DXHandle::get()); }
				void			SetTime(float Time) noexcept { MV1SetAttachAnimTime(m_pBaseModel->get(), Util::DXHandle::get(), Time); }
				void			SetBlendRate(float Per) noexcept { MV1SetAttachAnimBlendRate(m_pBaseModel->get(), Util::DXHandle::get(), Per); }
			};
		private:
			MV1AnimeHandle	m_handle;
			float			m_per{ 0.f };
			float			m_time{ 0.f };
			float			m_AllTime{ 0.f };

			float			m_per_prev{ -1.f };
			float			m_time_prev{ -1.f };
			char		padding[4]{};
		public:
			AnimControler(void) noexcept {}
			AnimControler(const AnimControler& o) noexcept { *this = o; }
			AnimControler(AnimControler&& o) noexcept { *this = o; }
			AnimControler& operator=(const AnimControler& o) noexcept {
				this->m_handle = o.m_handle;
				this->m_per = o.m_per;
				this->m_time = o.m_time;
				this->m_AllTime = o.m_AllTime;

				this->m_per_prev = o.m_per_prev;
				this->m_time_prev = o.m_time_prev;
				return *this;
			}
			AnimControler& operator=(AnimControler&& o) noexcept {
				this->m_handle = o.m_handle;
				this->m_per = o.m_per;
				this->m_time = o.m_time;
				this->m_AllTime = o.m_AllTime;

				this->m_per_prev = o.m_per_prev;
				this->m_time_prev = o.m_time_prev;
				return *this;
			}
			virtual ~AnimControler(void) noexcept {}
		public:
			const auto&		GetPer(void) const noexcept { return this->m_per; }
			const auto&		GetTime(void) const noexcept { return this->m_time; }
			auto			GetTimePer(void) const noexcept { return (this->m_AllTime > 0) ? this->m_time / this->m_AllTime : 1.f; }
			auto			TimeStart(void) const noexcept { return this->m_time <= 0; }
			auto			TimeEnd(void) const noexcept { return this->m_time >= this->m_AllTime; }
		public:
			void			GoStart(void) noexcept { this->m_time = 0.f; }
			void			GoEnd(void) noexcept { this->m_time = this->m_AllTime; }
			void			Reset(void) noexcept {
				this->m_per = 0.f;
				this->m_time = 0.f;
			}
			void			SetPer(float value) noexcept { this->m_per = value; }
			void			SetTime(float value) noexcept { this->m_time = value; }
		public:
			void			Set(MV1* model, size_t AnimIndex, const MV1* model_haveanim = nullptr) noexcept {
				this->m_handle.Attach(model, AnimIndex, model_haveanim);
				Reset();
				UpdateAnim();
				this->m_AllTime = this->m_handle.GetTotalTime();
			}
			void			Update(bool loop, float speed) noexcept {
				this->m_time += 30.f / 60.f * speed;
				if (loop) {
					if (speed >= 0.f) {
						if (this->m_time >= this->m_AllTime) {
							this->m_time = 0.f;
						}
					}
					else {
						if (this->m_time <= 0.f) {
							this->m_time = this->m_AllTime;
						}
					}
				}
				else {
					if (speed >= 0.f) {
						if (this->m_time >= this->m_AllTime) {
							this->m_time = this->m_AllTime;
						}
					}
					else {
						if (this->m_time <= 0.f) {
							this->m_time = 0.f;
						}
					}
				}
			}
			void			UpdateAnim(void) noexcept {
				if (this->m_time_prev != this->m_time) {
					this->m_handle.SetTime(this->m_time);
					this->m_time_prev = this->m_time;
				}
				if (this->m_per_prev != this->m_per) {
					this->m_handle.SetBlendRate(this->m_per);
					this->m_per_prev = this->m_per;
				}
			}
		};
	private:
		std::vector<AnimControler>	m_AnimControler;
		size_t m_AnimControlerSize{};
	public:
		MV1(void) noexcept {}
		MV1(const MV1&) = delete;
		MV1(MV1&&) = delete;
		MV1& operator=(const MV1&) = delete;
		MV1& operator=(MV1&&) = delete;
		virtual ~MV1(void) noexcept {}
	protected:
		void			Dispose_Sub(void) noexcept override {
			MV1DeleteModel(Util::DXHandle::get());
			this->m_AnimControler.clear();
			this->m_AnimControlerSize = 0;
		}
	public:
		/*読み込み*/
		static void		Load(std::basic_string_view<TCHAR> FileName, MV1* targetMV1, int mode = DX_LOADMODEL_PHYSICS_LOADCALC/*, float Grav = GravityRate*/) noexcept {
			if (mode != DX_LOADMODEL_PHYSICS_LOADCALC) {
				MV1SetLoadModelUsePhysicsMode(mode);
			}
			/*
			if (Grav != GravityRate) {
				MV1SetLoadModelPhysicsWorldGravity(Grav);
			}
			//*/
			targetMV1->SetHandleDirect(DxLib::MV1LoadModelWithStrLen(FileName.data(), FileName.length()));
			/*
			if (Grav != GravityRate) {
				MV1SetLoadModelPhysicsWorldGravity(GravityRate);
			}
			//*/
			if (mode != DX_LOADMODEL_PHYSICS_LOADCALC) {
				MV1SetLoadModelUsePhysicsMode(DX_LOADMODEL_PHYSICS_LOADCALC);
			}
			targetMV1->m_AnimControler.clear();
			targetMV1->m_AnimControlerSize = 0;
			return;
		}
		static void		SetAnime(MV1* targetMV1, const MV1& animPaletteMV1) noexcept {
			// 元用、アニメ用が読めていない場合は待つ
			{
				while (ProcessMessage() == 0) {
					if ((CheckHandleASyncLoad(targetMV1->get()) == TRUE) && (CheckHandleASyncLoad(animPaletteMV1.get()) == TRUE)) {
						continue;
					}
					break;
				}
			}
			// エラー
			if (MV1GetAnimNum(animPaletteMV1.get()) < 0) {
				printfDx("error");
				WaitKey();
			}
			targetMV1->m_AnimControlerSize = static_cast<size_t>(MV1GetAnimNum(animPaletteMV1.get()));
			targetMV1->m_AnimControler.resize(targetMV1->m_AnimControlerSize);
			if (targetMV1->GetAnimNum() > 0) {
				for (size_t loop = 0; loop < targetMV1->GetAnimNum(); ++loop) {
					targetMV1->SetAnim(loop).Set(targetMV1, loop, &animPaletteMV1);
				}
			}
			return;
		}
		void			Duplicate(const MV1& o) noexcept {
			Util::DXHandle::SetHandleDirect(DxLib::MV1DuplicateModel(o.get()));
		}
	public:
		void Draw(bool IsBoned) noexcept {
			for (int loop = 0; loop < MV1GetTriangleListNum(this->get()); ++loop) {
				switch (MV1GetTriangleListVertexType(this->get(), loop)) {
				case DX_MV1_VERTEX_TYPE_1FRAME:
				case DX_MV1_VERTEX_TYPE_NMAP_1FRAME:
					if (IsBoned) { continue; }
					// 剛体メッシュの場合の処理
					MV1DrawTriangleList(this->get(), loop);
					break;
				case DX_MV1_VERTEX_TYPE_4FRAME:
				case DX_MV1_VERTEX_TYPE_NMAP_4FRAME:
					if (!IsBoned) { continue; }
					// スキニングメッシュの場合の処理
					MV1DrawTriangleList(this->get(), loop);
					break;
				default:
					break;
				}
			}
		}
	public:
		// getter
		auto			GetPosition(void) const noexcept { return MV1GetPosition(Util::DXHandle::get()); }
		auto			GetOpacityRate(void) const noexcept { return MV1GetOpacityRate(Util::DXHandle::get()); }
		Util::Matrix4x4		GetFrameLocalMatrix(int frame) const noexcept { return MV1GetFrameLocalMatrix(Util::DXHandle::get(), frame); }
		Util::Matrix4x4		GetFrameLocalWorldMatrix(int frame) const noexcept { return MV1GetFrameLocalWorldMatrix(Util::DXHandle::get(), frame); }
		Util::Matrix4x4		GetMatrix(void) const noexcept { return MV1GetMatrix(Util::DXHandle::get()); }
		Util::VECTOR3D		GetFramePosition(int frame) const noexcept { return MV1GetFramePosition(Util::DXHandle::get(), frame); }
		auto			GetFrameNum(void) const noexcept { return MV1GetFrameNum(Util::DXHandle::get()); }
		auto			GetFrameParent(int frame) const noexcept { return MV1GetFrameParent(Util::DXHandle::get(), frame); }
		auto			GetFrameChild(int frame, int p2) const noexcept { return MV1GetFrameChild(Util::DXHandle::get(), frame, p2); }
		auto			GetFrameChildNum(int frame) const noexcept { return MV1GetFrameChildNum(Util::DXHandle::get(), frame); }
		std::string		GetFrameName(int frame) const noexcept { return MV1GetFrameName(Util::DXHandle::get(), frame); }
		auto			GetMaterialNum(void) const noexcept { return MV1GetMaterialNum(Util::DXHandle::get()); }
		std::string		GetMaterialName(int material) const noexcept { return MV1GetMaterialName(Util::DXHandle::get(), material); }
		auto			GetMeshNum(void) const noexcept { return MV1GetMeshNum(Util::DXHandle::get()); }
		auto			GetMeshMaxPosition(int mesh) const noexcept { return MV1GetMeshMaxPosition(Util::DXHandle::get(), mesh); }
		auto			GetMeshMinPosition(int mesh) const noexcept { return MV1GetMeshMinPosition(Util::DXHandle::get(), mesh); }
		auto			GetMeshSemiTransState(int mesh) const noexcept { return (MV1GetMeshSemiTransState(Util::DXHandle::get(), mesh) == TRUE); }
		auto			SearchShape(const char* str) const noexcept { return MV1SearchShape(Util::DXHandle::get(), str); }

		auto			GetParentFrameWorldMatrix(int frame) const noexcept { return GetFrameLocalWorldMatrix(GetFrameParent(frame)); }
		auto			GetChildFrameWorldMatrix(int frame, int ID) const noexcept { return GetFrameLocalWorldMatrix(GetFrameChild(frame, ID)); }
	public:
		// 描画
		auto			DrawModel(void) const noexcept { return MV1DrawModel(Util::DXHandle::get()) == TRUE; }
		auto			DrawFrame(int frame) const noexcept { return MV1DrawFrame(Util::DXHandle::get(), frame) == TRUE; }
		auto			DrawMesh(int mesh) const noexcept { return MV1DrawMesh(Util::DXHandle::get(), mesh) == TRUE; }
	public:
		/*モデル*/
		auto			SetPosition(const Util::VECTOR3D& pos) const noexcept { return MV1SetPosition(Util::DXHandle::get(), pos.get()) == TRUE; }
		auto			SetRotationZYAxis(const Util::VECTOR3D& zaxis, const Util::VECTOR3D& yaxis, float zrad) const noexcept { return MV1SetRotationZYAxis(Util::DXHandle::get(), zaxis.get(), yaxis.get(), zrad) == TRUE; }
		auto			SetMatrix(const Util::Matrix4x4& mat) const noexcept { return MV1SetMatrix(Util::DXHandle::get(), mat.get()) == TRUE; }
		auto			SetOpacityRate(float alpha) const noexcept { return MV1SetOpacityRate(Util::DXHandle::get(), alpha) == TRUE; }
		auto			SetScale(const Util::VECTOR3D& scale) const noexcept { return MV1SetScale(Util::DXHandle::get(), scale.get()) == TRUE; }
		auto			SetTextureGraphHandle(int texindex, const Draw::GraphHandle& texHandle, bool trans) const noexcept { return MV1SetTextureGraphHandle(Util::DXHandle::get(), texindex, texHandle.get(), trans ? TRUE : FALSE) == TRUE; }
		auto			SetFrameLocalMatrix(int frame, Util::Matrix4x4 mat) const noexcept { return MV1SetFrameUserLocalMatrix(Util::DXHandle::get(), frame, mat.get()) == TRUE; }
		void			ResetFrameUserLocalMatrix(int frame) const noexcept { MV1ResetFrameUserLocalMatrix(Util::DXHandle::get(), frame); }
		void			SetMaterialDrawAlphaTestAll(bool Enable, int mode, int param) const noexcept { MV1SetMaterialDrawAlphaTestAll(Util::DXHandle::get(), Enable ? TRUE : FALSE, mode, param); }
		void			SetMaterialTypeAll(int mode) const noexcept { MV1SetMaterialTypeAll(Util::DXHandle::get(), mode); }
		auto			SetShapeRate(int Shapeindex, float Per) const noexcept { return MV1SetShapeRate(Util::DXHandle::get(), Shapeindex, Per) == TRUE; }

		void			SetDifColorScale(COLOR_F color) const noexcept { MV1SetDifColorScale(Util::DXHandle::get(), color); }
		void			SetAmbColorScale(COLOR_F color) const noexcept { MV1SetAmbColorScale(Util::DXHandle::get(), color); }
		void			SetSpcColorScale(COLOR_F color) const noexcept { MV1SetSpcColorScale(Util::DXHandle::get(), color); }
		void			SetMaterialDifColor(int material, COLOR_F color) const noexcept { MV1SetMaterialDifColor(Util::DXHandle::get(), material, color); }
		void			SetMaterialSpcColor(int material, COLOR_F color) const noexcept { MV1SetMaterialSpcColor(Util::DXHandle::get(), material, color); }
		void			SetMaterialAmbColor(int material, COLOR_F color) const noexcept { MV1SetMaterialAmbColor(Util::DXHandle::get(), material, color); }
		void			SetMaterialSpcPower(int material, float power) const noexcept { MV1SetMaterialSpcPower(Util::DXHandle::get(), material, power); }
		void			SetMaterialDrawAddColorAll(int r, int g, int b) const noexcept { MV1SetMaterialDrawAddColorAll(Util::DXHandle::get(), r, g, b); }
		void			SetMaterialOpacityRate(int material, float alpha) const noexcept { MV1SetMeshOpacityRate(Util::DXHandle::get(), material, alpha); }
		
		void			SaveModelToMV1File(std::basic_string_view<TCHAR> FileName, int  SaveType = MV1_SAVETYPE_NORMAL, int AnimMHandle = InvalidID, int AnimNameCheck = TRUE, int Normal8BitFlag = 1, int Position16BitFlag = 1, int Weight8BitFlag = 0, int Anim16BitFlag = 1) const noexcept { MV1SaveModelToMV1FileWithStrLen(Util::DXHandle::get(), FileName.data(), FileName.length(), SaveType, AnimMHandle, AnimNameCheck, Normal8BitFlag, Position16BitFlag, Weight8BitFlag, Anim16BitFlag); }
		void			SetPrioritizePhysicsOverAnimFlag(bool flag) const noexcept { MV1SetPrioritizePhysicsOverAnimFlag(Util::DXHandle::get(), flag ? TRUE : FALSE); }

		/*アニメーション*/
		AnimControler&	SetAnim(size_t animeID) noexcept { return this->m_AnimControler[animeID]; }
		const auto&		GetAnim(size_t animeID) const noexcept { return this->m_AnimControler[animeID]; }
		size_t			GetAnimNum(void) const noexcept { return this->m_AnimControlerSize; }
		auto			UpdateAnim(size_t animeID) noexcept { SetAnim(animeID).UpdateAnim(); }
		auto			UpdateAnimAll(void) noexcept {
			for (auto& a : this->m_AnimControler) {
				a.UpdateAnim();
			}
		}
		/*物理演算*/
		auto			PhysicsResetState(void) const noexcept { return MV1PhysicsResetState(Util::DXHandle::get()) == TRUE; }
		auto			PhysicsCalculation(float TimeMS) const noexcept { return MV1PhysicsCalculation(Util::DXHandle::get(), TimeMS) == TRUE; }
		/*当たり判定*/
		auto			SetupCollInfo(int x = 32, int y = 8, int z = 32, int frame = InvalidID, int mesh = InvalidID) const noexcept {
			return MV1SetupCollInfo(Util::DXHandle::get(), frame, x, y, z, mesh) == 0;
		}
		auto			TerminateCollInfo(int frame = InvalidID, int mesh = InvalidID) const noexcept {
			return MV1TerminateCollInfo(Util::DXHandle::get(),frame, mesh) == 0;
		}

		auto			RefreshCollInfo(int frame = InvalidID, int mesh = InvalidID) const noexcept {
			return MV1RefreshCollInfo(Util::DXHandle::get(), frame, mesh) == 0;
		}
		auto			CollCheck_Line(const Util::VECTOR3D& start, const Util::VECTOR3D& end, int frame = InvalidID, int mesh = InvalidID) const noexcept {
			return MV1CollCheck_Line(Util::DXHandle::get(), frame, start.get(), end.get(), mesh);
		}
		auto			CollCheck_Sphere(const Util::VECTOR3D& startpos, float range, int frame = InvalidID, int mesh = InvalidID) const noexcept {
			return MV1CollCheck_Sphere(Util::DXHandle::get(), frame, startpos.get(), range, mesh);
		}
		auto			CollCheck_Capsule(const Util::VECTOR3D& startpos, const Util::VECTOR3D& endpos, float range, int frame = InvalidID, int mesh = InvalidID) const noexcept {
			return MV1CollCheck_Capsule(Util::DXHandle::get(), frame, startpos.get(), endpos.get(), range, mesh);
		}
	};
};
