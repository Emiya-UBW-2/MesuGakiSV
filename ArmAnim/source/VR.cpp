#include "VR.hpp"

const DXLibRef::VRControl* Util::SingletonBase<DXLibRef::VRControl>::m_Singleton = nullptr;

namespace DXLibRef {
	// --------------------------------------------------------------------------------------------------
	// 
	// --------------------------------------------------------------------------------------------------
	void VRControl::Init(void) noexcept {
		// VRが使えるかチェック
		this->m_VR_ErrorHandle = vr::VRInitError_None;
		this->m_VR_SystemPtr = vr::VR_Init(&this->m_VR_ErrorHandle, vr::VRApplication_Scene);
		if (this->m_VR_ErrorHandle != vr::VRInitError_None) {
			this->m_VR_SystemPtr = nullptr;
			return;
		}
		// デバイスセット
		this->m_VR_TrackerID.clear();
		char DeviceCount = 0;
		bool IsFirstHand = true;
		for (char loop = 0; loop < 8; ++loop) {
			auto DeviceID = DeviceCount;
			auto DeviceType = this->m_VR_SystemPtr->GetTrackedDeviceClass(loop);
			switch (DeviceType) {
			case vr::TrackedDeviceClass_HMD:
				this->m_VR_HMDID = DeviceID;
				++DeviceCount;
				break;
			case vr::TrackedDeviceClass_Controller:
				if (IsFirstHand) {
					IsFirstHand = false;
					this->m_VR_Hand1ID = DeviceID;
				}
				else {
					this->m_VR_Hand2ID = DeviceID;
				}
				++DeviceCount;
				break;
			case vr::TrackedDeviceClass_GenericTracker:
				this->m_VR_TrackerID.emplace_back(DeviceID);
				++DeviceCount;
				break;
			case vr::TrackedDeviceClass_TrackingReference:
				++DeviceCount;
				break;
			default:
				continue;
			}
			this->m_VR_DeviceInfo.resize(DeviceCount);
			this->m_VR_DeviceInfo.back().Init(DeviceID, loop, DeviceType);
		}
	}
	void VRControl::Update(void) noexcept {
		if (!this->m_VR_SystemPtr) { return; }
		for (auto& c : this->m_VR_DeviceInfo) {
			c.Update(this->m_VR_SystemPtr);
		}
	}
	void VRControl::Submit(char eye_type) noexcept {
		vr::Texture_t tex = { (void*)GetUseDirect3D11BackBufferTexture2D(), vr::ETextureType::TextureType_DirectX,vr::EColorSpace::ColorSpace_Auto };
		vr::VRCompositor()->Submit((vr::EVREye)eye_type, &tex, NULL, vr::Submit_Default);
	}
	void VRControl::SubmitDraw(char eye_type, const int& MainDrawScreen) noexcept {
		if (!this->m_VR_SystemPtr) { return; }
		// 合成したものをBACKに持ってきて
		SetDrawScreen(DX_SCREEN_BACK);
		ClearDrawScreen();
		{
			DrawGraph(0, 0, MainDrawScreen, false);
		}
		// それぞれの目にDX_SCREEN_BACKの内容を送信
		this->Submit(eye_type);
	}
	void VRControl::WaitSync(void) noexcept {
		if (!this->m_VR_SystemPtr) { return; }
		vr::TrackedDevicePose_t tmp;
		vr::VRCompositor()->WaitGetPoses(&tmp, 1, NULL, 1);
	}
	void VRControl::Dispose(void) noexcept {
		if (!this->m_VR_SystemPtr) { return; }
		// vr::VR_Shutdown();
		this->m_VR_SystemPtr = nullptr;
	}
}