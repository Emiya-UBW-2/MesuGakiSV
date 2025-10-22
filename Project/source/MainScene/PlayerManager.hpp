#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:4710)
#pragma warning(disable:4711)
#pragma warning(disable:5039)

#include "../Util/Util.hpp"
#include "EarlyCharacter.hpp"

class PlayerManager : public Util::SingletonBase<PlayerManager> {
private:
	friend class Util::SingletonBase<PlayerManager>;
private:
	std::vector<std::shared_ptr<EarlyCharacter>>	m_Character;
private:
	PlayerManager(void) noexcept {}
	PlayerManager(const PlayerManager&) = delete;
	PlayerManager(PlayerManager&&) = delete;
	PlayerManager& operator=(const PlayerManager&) = delete;
	PlayerManager& operator=(PlayerManager&&) = delete;
	virtual ~PlayerManager(void) noexcept { Dispose(); }
public:
	void Load() noexcept {
		ObjectManager::Instance()->LoadModel("data/Early/");
		this->m_Character.resize(5);
	}
	void Init(void) noexcept {
		size_t loop = 0;
		for (auto& m : BackGround::Instance()->GetMapInfo()) {
			if (m.m_InfoType == InfoType::WayPoint) {
				this->m_Character.at(loop) = std::make_shared<EarlyCharacter>();
				ObjectManager::Instance()->InitObject(this->m_Character.at(loop), this->m_Character.at(loop), "data/Early/");
				this->m_Character.at(loop)->SetPos(BackGround::Instance()->GetWorldPos(m.m_pos));
				++loop;
				if (loop >= this->m_Character.size()) { break; }
			}
		}
	}
	void SetTarget(const Util::VECTOR3D& pos) noexcept {
		for (auto& m : this->m_Character) {
			m->SetTarget(pos);
		}
	}
	void Dispose(void) noexcept {
		for (auto& m : this->m_Character) {
			m.reset();
		}
		this->m_Character.clear();
	}
public:
	const auto& GetCharacter() const noexcept { return m_Character; }
	auto& SetCharacter() noexcept { return m_Character; }
};
