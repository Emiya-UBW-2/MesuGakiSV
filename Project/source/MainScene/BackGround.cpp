#pragma warning(disable:5259)
#include "BackGround.hpp"

#include "../MainScene/PlayerManager.hpp"

const BackGround* Util::SingletonBase<BackGround>::m_Singleton = nullptr;

void BackGround::ChangeOffset(void) noexcept {
	this->m_Offset += VecT;
	++this->m_VOfs;

	m_pVoxel.at(0) = &m_Voxel.at(static_cast<size_t>((m_VOfs + 0) % 2));
	m_pVoxel.at(1) = &m_Voxel.at(static_cast<size_t>((m_VOfs + 1) % 2));

	if ((this->m_VOfs - 1) % 5 == 0) {
		m_NextMap = "Map" + std::to_string(1);
	}
	else {
		m_NextMap = "Map" + std::to_string(GetRand(100) % (5 - 1) + 2);
	}

	m_Jobs.JobStart();

	size_t loop = 1;
	for (auto& m : GetMapInfo()) {
		if (m.m_InfoType == InfoType::WayPoint) {
			PlayerManager::Instance()->SetCharacter().at(loop)->SetPos(GetWorldPos(m.m_pos) + m_Offset);
			++loop;
		}
	}
}