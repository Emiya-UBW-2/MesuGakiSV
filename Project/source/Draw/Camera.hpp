#pragma once
#pragma warning(disable:4464)
#include "../Util/Util.hpp"
#include "../Util/Algorithm.hpp"
#include "ImageDraw.hpp"

namespace DXLibRef {
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
		Util::Vector3DX					m_CamShake1;
		Util::Vector3DX					m_CamShake2;

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
			this->m_CamShake1 = Util::Vector3DX::zero();
			this->m_CamShake2 = Util::Vector3DX::zero();
		}
	private:
		Camera3D(void) noexcept {}
		Camera3D(const Camera3D&) = delete;
		Camera3D(Camera3D&&) = delete;
		Camera3D& operator=(const Camera3D&) = delete;
		Camera3D& operator=(Camera3D&&) = delete;

		virtual ~Camera3D(void) noexcept {}
	public:
		void Update(void) noexcept;
	};
}
