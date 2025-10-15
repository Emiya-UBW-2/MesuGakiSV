#pragma once

#include <functional>

#include "Util/Util.hpp"

#include <openvr.h>
#pragma comment(lib,"..\\..\\openvr\\openvr_api.lib")

namespace DXLibRef {
	enum class VR_PAD {
		TRIGGER,
		SIDEBUTTON,
		TOUCHPAD,
		TOPBUTTON1,
		TOPBUTTON2,
	};
	class VRControl : public Util::SingletonBase<VRControl> {
	private:
		friend class Util::SingletonBase<VRControl>;
	private:
		class VRDevice {
		private:
			int							m_ID{ 0 };
			char						m_DeviceNumber{ 0 };
			vr::ETrackedDeviceClass 	m_type{ vr::TrackedDeviceClass_Invalid };
			bool						m_isInitialized{ false };
			bool						m_isActive{ false };
			// 位置やボタン判定
			uint64_t					m_ButtonPressFlag{ 0 };
			uint64_t					m_ButtonTouchFlag{ 0 };
			Util::VECTOR3D				m_TouchPadPoint{ Util::VECTOR3D::zero() };
			//
			Util::VECTOR3D				m_Pos;
			Util::Matrix3x3				m_Mat;
		public:
			const auto& GetID(void) const noexcept { return this->m_ID; }
			auto			IsActive(void) const noexcept { return this->m_isInitialized && this->m_isActive; }
			bool			PadPress(VR_PAD ID) const noexcept {
				switch (ID) {
				case VR_PAD::TRIGGER:
					return(this->m_ButtonPressFlag & vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_SteamVR_Trigger));
				case VR_PAD::SIDEBUTTON:
					return(this->m_ButtonPressFlag & vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_Grip));
				case VR_PAD::TOUCHPAD:
					return(this->m_ButtonPressFlag & vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_SteamVR_Touchpad));
				case VR_PAD::TOPBUTTON1:
					return(this->m_ButtonPressFlag & vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_ApplicationMenu));
				case VR_PAD::TOPBUTTON2:
					return(this->m_ButtonPressFlag & vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_IndexController_B));
				default:
					return false;
				}
			}
			bool			PadTouch(VR_PAD ID) const noexcept {
				switch (ID) {
				case VR_PAD::TRIGGER:
					return(this->m_ButtonTouchFlag & vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_SteamVR_Trigger));
				case VR_PAD::SIDEBUTTON:
					return(this->m_ButtonTouchFlag & vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_Grip));
				case VR_PAD::TOUCHPAD:
					return(this->m_ButtonTouchFlag & vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_SteamVR_Touchpad));
				case VR_PAD::TOPBUTTON1:
					return(this->m_ButtonTouchFlag & vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_ApplicationMenu));
				case VR_PAD::TOPBUTTON2:
					return(this->m_ButtonTouchFlag & vr::ButtonMaskFromId(vr::EVRButtonId::k_EButton_IndexController_B));
				default:
					return false;
				}
			}
			const auto& GetTouchPadPoint(void) const noexcept { return this->m_TouchPadPoint; }
			const auto& GetPos(void) const noexcept { return this->m_Pos; }
			const auto& GetMat(void) const noexcept { return this->m_Mat; }
		public:
			void Init(int ID, char Num, vr::ETrackedDeviceClass Type) noexcept {
				this->m_ID = ID;
				this->m_DeviceNumber = Num;
				this->m_type = Type;
				this->m_isInitialized = true;
				this->m_isActive = false;
				Reset();
			}
			void Update(vr::IVRSystem* SystemPtr) noexcept {
				vr::TrackedDevicePose_t tmp;
				switch (this->m_type) {
				case vr::TrackedDeviceClass_HMD:
					SystemPtr->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding, 0.0f, &tmp, 1);
					this->m_ButtonPressFlag = 0;
					this->m_ButtonTouchFlag = 0;
					this->m_TouchPadPoint = Util::VECTOR3D::zero();
					this->m_isActive = tmp.bPoseIsValid;
					this->m_Pos = Util::VECTOR3D::vget(tmp.mDeviceToAbsoluteTracking.m[0][3], tmp.mDeviceToAbsoluteTracking.m[1][3], -tmp.mDeviceToAbsoluteTracking.m[2][3]);
					for (int x = 0; x < 3; ++x) {
						for (int y = 0; y < 3; ++y) {
							this->m_Mat.get().m[x][y] = tmp.mDeviceToAbsoluteTracking.m[x][y];
						}
					}
					this->m_Mat = Util::Matrix3x3::Axis1(
						Util::VECTOR3D::vget(tmp.mDeviceToAbsoluteTracking.m[0][1], tmp.mDeviceToAbsoluteTracking.m[1][1], tmp.mDeviceToAbsoluteTracking.m[2][1]),
						Util::VECTOR3D::vget(tmp.mDeviceToAbsoluteTracking.m[0][2], tmp.mDeviceToAbsoluteTracking.m[1][2], tmp.mDeviceToAbsoluteTracking.m[2][2]) * -1.f
					);
					break;
				case vr::TrackedDeviceClass_Controller:
				case vr::TrackedDeviceClass_TrackingReference:
				case vr::TrackedDeviceClass_GenericTracker:
				{
					vr::VRControllerState_t night;
					SystemPtr->GetControllerStateWithPose(vr::TrackingUniverseStanding, this->m_DeviceNumber, &night, sizeof(night), &tmp);
					this->m_ButtonPressFlag = night.ulButtonPressed;
					this->m_ButtonTouchFlag = night.ulButtonTouched;
					this->m_TouchPadPoint = Util::VECTOR3D::vget(night.rAxis[0].x, night.rAxis[0].y, 0);
					this->m_isActive = tmp.bPoseIsValid;
					this->m_Pos = Util::VECTOR3D::vget(tmp.mDeviceToAbsoluteTracking.m[0][3], tmp.mDeviceToAbsoluteTracking.m[1][3], -tmp.mDeviceToAbsoluteTracking.m[2][3]);
					if (this->m_isActive && (this->m_type == vr::TrackedDeviceClass_Controller)) {
						clsDx();
						printfDx("[%5.2f,%5.2f,%5.2f]\n", tmp.mDeviceToAbsoluteTracking.m[0][0], tmp.mDeviceToAbsoluteTracking.m[1][0], tmp.mDeviceToAbsoluteTracking.m[2][0]);
						printfDx("[%5.2f,%5.2f,%5.2f]\n", tmp.mDeviceToAbsoluteTracking.m[0][1], tmp.mDeviceToAbsoluteTracking.m[1][1], tmp.mDeviceToAbsoluteTracking.m[2][1]);
						printfDx("[%5.2f,%5.2f,%5.2f]\n", tmp.mDeviceToAbsoluteTracking.m[0][2], tmp.mDeviceToAbsoluteTracking.m[1][2], tmp.mDeviceToAbsoluteTracking.m[2][2]);
					}
					this->m_Mat = Util::Matrix3x3::Axis1(
						Util::VECTOR3D::vget(tmp.mDeviceToAbsoluteTracking.m[0][0], tmp.mDeviceToAbsoluteTracking.m[1][0], -tmp.mDeviceToAbsoluteTracking.m[2][0]),
						Util::VECTOR3D::vget(tmp.mDeviceToAbsoluteTracking.m[0][1], tmp.mDeviceToAbsoluteTracking.m[1][1], -tmp.mDeviceToAbsoluteTracking.m[2][1]),
						Util::VECTOR3D::vget(tmp.mDeviceToAbsoluteTracking.m[0][2], tmp.mDeviceToAbsoluteTracking.m[1][2], -tmp.mDeviceToAbsoluteTracking.m[2][2])
					);
				}
				break;
				default:
					break;
				}
			}
			void Reset(void) noexcept {
				this->m_ButtonPressFlag = 0;
				this->m_ButtonTouchFlag = 0;
				this->m_TouchPadPoint = Util::VECTOR3D::zero();
				this->m_Pos = Util::VECTOR3D::zero();
				this->m_Mat = Util::Matrix3x3::identity();
			}
		};
	private:
		vr::IVRSystem*				m_VR_SystemPtr{ nullptr };
		vr::EVRInitError			m_VR_ErrorHandle{ vr::VRInitError_None };
		std::vector<VRDevice>	m_VR_DeviceInfo;
		char						m_VR_HMDID{ InvalidID };
		char						m_VR_Hand1ID{ InvalidID };
		char						m_VR_Hand2ID{ InvalidID };
		std::vector<char>			m_VR_TrackerID;
		bool						m_VR_PrevHMDIsActive{ false };
		bool						m_VR_HMD_StartFlag{ true };
		Util::Matrix4x4				m_VR_HMD_StartPoint;
	private:// コンストラクタ
		VRControl(void) noexcept{}
		VRControl(const VRControl&) = delete;
		VRControl(VRControl&&) = delete;
		VRControl& operator=(const VRControl&) = delete;
		VRControl& operator=(VRControl&&) = delete;
	public:
		const VRDevice* Get_VR_Hand1Device(void) const noexcept { return (this->m_VR_Hand1ID >= 0) ? &this->m_VR_DeviceInfo.at(this->m_VR_Hand1ID) : nullptr; }
		const VRDevice* Get_VR_Hand2Device(void) const noexcept { return (this->m_VR_Hand2ID >= 0) ? &this->m_VR_DeviceInfo.at(this->m_VR_Hand2ID) : nullptr; }
		const VRDevice* GetTrackerDevice(int sel) const noexcept { return (0 <= sel && sel < this->m_VR_TrackerID.size()) ? &this->m_VR_DeviceInfo.at(this->m_VR_TrackerID.at(sel)) : nullptr; }
		void			ResetHMD(void) noexcept {
			this->m_VR_PrevHMDIsActive = false;
			this->m_VR_HMD_StartFlag = true;
		}
		void			GetHMDPosition(Util::VECTOR3D* pos_, Util::Matrix3x3* mat) noexcept {
			auto* HMDPtr = (this->m_VR_HMDID >= 0) ? &this->m_VR_DeviceInfo.at(this->m_VR_HMDID) : nullptr;
			if (HMDPtr) {
				*mat = HMDPtr->GetMat();
				if (!HMDPtr->IsActive()) {
					this->m_VR_HMD_StartFlag = true;
				}
				if (this->m_VR_HMD_StartFlag && HMDPtr->IsActive() != this->m_VR_PrevHMDIsActive) {
					this->m_VR_HMD_StartFlag = false;
					// 
					Util::VECTOR3D pos = HMDPtr->GetPos(); pos.y = 0.f;
					Util::VECTOR3D tmp = mat->zvec(); tmp.y = 0.f; tmp = tmp.normalized();
					float rad = std::atan2f(tmp.x, -tmp.z);
					this->m_VR_HMD_StartPoint = Util::Matrix4x4::RotAxis(Util::VECTOR3D::up(), rad) * Util::Matrix4x4::Mtrans(pos);
				}
				this->m_VR_PrevHMDIsActive = HMDPtr->IsActive();
				*pos_ = HMDPtr->GetPos() - this->m_VR_HMD_StartPoint.pos();
				*mat = *mat * Util::Matrix3x3::Get33DX(this->m_VR_HMD_StartPoint);
			}
			else {
				*pos_ = Util::VECTOR3D::zero();
				*mat = Util::Matrix3x3::identity();
				ResetHMD();
			}
		}

		void			GetHand1Position(Util::VECTOR3D* pos_, Util::Matrix3x3* mat) noexcept {
			auto* Hand1Ptr = (this->m_VR_Hand1ID >= 0) ? &this->m_VR_DeviceInfo.at(this->m_VR_Hand1ID) : nullptr;
			if (Hand1Ptr) {
				*mat = Hand1Ptr->GetMat();
				*pos_ = Hand1Ptr->GetPos() - this->m_VR_HMD_StartPoint.pos();
				*mat = *mat * Util::Matrix3x3::Get33DX(this->m_VR_HMD_StartPoint);
			}
			else {
				*pos_ = Util::VECTOR3D::zero();
				*mat = Util::Matrix3x3::identity();
			}
		}

		void			GetHand2Position(Util::VECTOR3D* pos_, Util::Matrix3x3* mat) noexcept {
			auto* Hand2Ptr = (this->m_VR_Hand2ID >= 0) ? &this->m_VR_DeviceInfo.at(this->m_VR_Hand2ID) : nullptr;
			if (Hand2Ptr) {
				*mat = Hand2Ptr->GetMat();
				*pos_ = Hand2Ptr->GetPos() - this->m_VR_HMD_StartPoint.pos();
				*mat = *mat * Util::Matrix3x3::Get33DX(this->m_VR_HMD_StartPoint);
			}
			else {
				*pos_ = Util::VECTOR3D::zero();
				*mat = Util::Matrix3x3::identity();
			}
		}

		void			Haptic(char id_, unsigned short times) noexcept {
			if ((id_ != InvalidID) && this->m_VR_SystemPtr) {
				this->m_VR_SystemPtr->TriggerHapticPulse(this->m_VR_DeviceInfo[id_].GetID(), 2, times);
			}
		}
		auto			GetEyePosition(char eye_type) noexcept {
			auto* HMDPtr = (this->m_VR_HMDID >= 0) ? &this->m_VR_DeviceInfo.at(this->m_VR_HMDID) : nullptr;
			if (HMDPtr) {
				const vr::HmdMatrix34_t tmpmat = vr::VRSystem()->GetEyeToHeadTransform((vr::EVREye)eye_type);
				return Util::Matrix3x3::Vtrans(Util::VECTOR3D::vget(tmpmat.m[0][3], tmpmat.m[1][3], tmpmat.m[2][3]), HMDPtr->GetMat());
			}
			return Util::VECTOR3D::zero();
		}
	public:
		void Init(void) noexcept;
		void Update(void) noexcept;
		void Submit(char eye_type) noexcept;
		void SubmitDraw(char eye_type, const int& MainDrawScreen) noexcept;
		void WaitSync(void) noexcept;
		void Dispose(void) noexcept;
	};
};
