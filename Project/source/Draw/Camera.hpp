#pragma once
#pragma warning(disable:4464)
#include "../Util/Util.hpp"
#include "../Util/Algorithm.hpp"
#include "ImageDraw.hpp"

namespace Camera {
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// カメラシェイク
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	class Camera3D : public Util::SingletonBase<Camera3D> {
	private:
		friend class Util::SingletonBase<Camera3D>;
	private:
		bool						m_SendCamShake{ false };
		char		padding[3]{};
		float						m_SendCamShakeTime{ 1.f };
		float						m_SendCamShakePower{ 1.f };
		float						m_CamShake{ 0.f };
		Util::VECTOR3D					m_CamShake1;
		Util::VECTOR3D					m_CamShake2;

		Draw::Camera3DInfo				m_MainCamera;				// カメラ
	public:
		const auto& GetMainCamera(void) const noexcept { return this->m_MainCamera; }
		auto& SetMainCamera(void) noexcept { return this->m_MainCamera; }
	public:
		const auto& GetCamShake(void) const noexcept { return this->m_CamShake2; }
	public:
		void			SetCamShake(float time, float power) noexcept {
			this->m_SendCamShake = true;
			this->m_SendCamShakeTime = time;
			this->m_SendCamShakePower = power;
		}
		void			StopCamShake() noexcept {
			this->m_SendCamShake = false;
			this->m_SendCamShakeTime = 1.f;
			this->m_SendCamShakePower = 1.f;
			this->m_CamShake = 0.f;
			this->m_CamShake1 = Util::VECTOR3D::zero();
			this->m_CamShake2 = Util::VECTOR3D::zero();
		}
	private:
		Camera3D(void) noexcept {}
		Camera3D(const Camera3D&) = delete;
		Camera3D(Camera3D&&) = delete;
		Camera3D& operator=(const Camera3D&) = delete;
		Camera3D& operator=(Camera3D&&) = delete;

		virtual ~Camera3D(void) noexcept {}
	public:
		static float GetRandf(float arg) noexcept { return -arg + static_cast<float>(DxLib::GetRand(static_cast<int>(arg * 2.f * 10000.f))) / 10000.f; }

		void Update(void) noexcept {
			if (this->m_SendCamShakeTime > 0.f) {
				if (this->m_SendCamShake) {
					this->m_SendCamShake = false;
					this->m_CamShake = this->m_SendCamShakeTime;
				}
				auto RandRange = this->m_CamShake / this->m_SendCamShakeTime * this->m_SendCamShakePower;
				this->m_CamShake1 = Util::Lerp(this->m_CamShake1, Util::VECTOR3D::vget(GetRandf(RandRange), GetRandf(RandRange), GetRandf(RandRange)), 0.8f);
				this->m_CamShake2 = Util::Lerp(this->m_CamShake2, this->m_CamShake1, 0.8f);
				this->m_CamShake = std::max(this->m_CamShake - 1.f / 60.f, 0.f);
			}
		}
	};
}
