#include "Camera.hpp"
#pragma warning(disable:5259)
const DXLibRef::Camera3D* Util::SingletonBase<DXLibRef::Camera3D>::m_Singleton = nullptr;

namespace DXLibRef {
	static float GetRandf(float arg) noexcept { return -arg + static_cast<float>(GetRand(static_cast<int>(arg * 2.f * 10000.f))) / 10000.f; }

	// --------------------------------------------------------------------------------------------------
	// 
	// --------------------------------------------------------------------------------------------------
	void Camera3D::Update(void) noexcept {
		if (this->m_SendCamShakeTime > 0.f) {
			if (this->m_SendCamShake) {
				this->m_SendCamShake = false;
				this->m_CamShake = this->m_SendCamShakeTime;
			}
			auto RandRange = this->m_CamShake / this->m_SendCamShakeTime * this->m_SendCamShakePower;
			this->m_CamShake1 = Util::Lerp(this->m_CamShake1, Util::Vector3DX::vget(GetRandf(RandRange), GetRandf(RandRange), GetRandf(RandRange)), 0.8f);
			this->m_CamShake2 = Util::Lerp(this->m_CamShake2, this->m_CamShake1, 0.8f);
			this->m_CamShake = std::max(this->m_CamShake - 1.f / 60.f, 0.f);
		}
	}
};
