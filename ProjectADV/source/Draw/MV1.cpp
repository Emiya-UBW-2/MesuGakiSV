#include "MV1.hpp"
const Draw::MV1Pool* Util::SingletonBase<Draw::MV1Pool>::m_Singleton = nullptr;

namespace Draw {
	MV1have::MV1have(std::string_view FilePath) noexcept {
		this->m_FilePath = FilePath;
		MV1::Load(this->m_FilePath, &this->m_Handle/*, DX_LOADMODEL_PHYSICS_REALTIME*/);
	}
}