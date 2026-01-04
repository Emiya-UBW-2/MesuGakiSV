#pragma once

#pragma warning(disable:5045)
#include "Draw/DrawUISystem.hpp"

#include "Util/Localize.hpp"

#include "Util/Sound.hpp"

#pragma warning( push, 3 )
#include <functional>
#pragma warning( pop )
class PauseUI {
private:
	Draw::DrawUISystem*						m_DrawUI{ nullptr };
	int										m_UIBase{ InvalidID };
	int										m_ButtonID[5] = { InvalidID,InvalidID,InvalidID,InvalidID,InvalidID };
	bool									m_IsActive{};
	char		padding[7]{};
	std::array<std::function<void()>, 5>	m_ButtonDo{};

	Sound::SoundUniqueID cancelID{ InvalidID };
	Sound::SoundUniqueID cursorID{ InvalidID };
	Sound::SoundUniqueID NGID{ InvalidID };
	Sound::SoundUniqueID OKID{ InvalidID };
	int					isSelectSoundPrev{ InvalidID };
	char		padding2[4]{};
public:
	PauseUI(void) noexcept {}
	PauseUI(const PauseUI&) = delete;
	PauseUI(PauseUI&&) = delete;
	PauseUI& operator=(const PauseUI&) = delete;
	PauseUI& operator=(PauseUI&&) = delete;
	virtual ~PauseUI(void) noexcept {}
public:
	bool		IsEnd(void) const noexcept { return !this->m_DrawUI->Get(this->m_UIBase).IsActive() && this->m_DrawUI->Get(this->m_UIBase).IsAnimeEnd(); }
	void		SetEnd(void) noexcept { this->m_DrawUI->Get(this->m_UIBase).SetActive(false); }
	bool		IsActive(void) const noexcept { return this->m_IsActive; }
	void		SetActive(bool value) noexcept { this->m_IsActive = value; }
	void		SetEvent(int ID, const std::function<void()>& value) noexcept { this->m_ButtonDo[static_cast<size_t>(ID)] = value; }
public:
	void		Init(void) noexcept {
		cancelID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/UI/cancel.wav", false);
		cursorID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/UI/cursor.wav", false);
		NGID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/UI/ng.wav", false);
		OKID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/UI/ok.wav", false);

		this->m_DrawUI = new Draw::DrawUISystem();
		this->m_DrawUI->Init("data/UI/Pause/PauseUI.json");
		this->m_UIBase = this->m_DrawUI->GetID("");

		for (int loop = 0; loop < 5; ++loop) {
			std::string Path = "Tab";
			Path += std::to_string(loop + 1);
			this->m_ButtonID[loop] = this->m_DrawUI->GetID(Path.c_str());
		}
		this->m_DrawUI->Get(this->m_UIBase).SetActive(true);
	}
	void		Update(void) noexcept {
		auto* KeyMngr = Util::KeyParam::Instance();
		if (IsActive()) {
			int IsSelect = InvalidID;
			for (int loop = 0; loop < 3; ++loop) {
				if (this->m_DrawUI->Get(this->m_ButtonID[loop]).IsSelectButton()) {
					IsSelect = loop;
					break;
				}
			}
			if (IsSelect != InvalidID && (IsSelect != isSelectSoundPrev)) {
				Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, cursorID)->Play(DX_PLAYTYPE_BACK, TRUE);
			}
			isSelectSoundPrev = IsSelect;

			if (KeyMngr->GetMenuKeyReleaseTrigger(Util::EnumMenu::Diside)) {
				for (int loop = 0; loop < 5; ++loop) {
					if (this->m_DrawUI->Get(this->m_ButtonID[loop]).IsSelectButton()) {
						Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, OKID)->Play(DX_PLAYTYPE_BACK, TRUE);
						if (this->m_ButtonDo[static_cast<size_t>(loop)]) {
							this->m_ButtonDo[static_cast<size_t>(loop)]();
						}
						break;
					}
				}
			}
		}
		this->m_DrawUI->Get(this->m_UIBase).SetActive(IsActive());
		this->m_DrawUI->Update(IsActive());
	}
	void		Draw(void) noexcept {
		this->m_DrawUI->Draw();
	}
	void		Dispose(void) noexcept {
		delete this->m_DrawUI;
	}
};

class SaveUI {
private:
	Draw::DrawUISystem* m_DrawUI{ nullptr };
	int						m_UIBase{ InvalidID };
	int						m_ButtonID[5] = { InvalidID,InvalidID,InvalidID,InvalidID,InvalidID };
	int						m_CloseButtonID{ InvalidID };
	int						m_SelectButtonID{ InvalidID };
	//char		padding[4]{};
	std::function<void()>	m_ButtonDo{};
	Sound::SoundUniqueID	m_cancelID{ InvalidID };
	Sound::SoundUniqueID	m_cursorID{ InvalidID };
	Sound::SoundUniqueID	m_NGID{ InvalidID };
	Sound::SoundUniqueID	m_OKID{ InvalidID };
	bool					m_isSelectSoundPrev{ false };
	char		padding2[7]{};
public:
	SaveUI(void) noexcept {}
	SaveUI(const SaveUI&) = delete;
	SaveUI(SaveUI&&) = delete;
	SaveUI& operator=(const SaveUI&) = delete;
	SaveUI& operator=(SaveUI&&) = delete;
	virtual ~SaveUI(void) noexcept {}
public:
	bool		IsEnd(void) const noexcept { return !this->m_DrawUI->Get(this->m_UIBase).IsActive() && this->m_DrawUI->Get(this->m_UIBase).IsAnimeEnd(); }
	bool		IsActive(void) const noexcept { return this->m_DrawUI->Get(this->m_UIBase).IsActive(); }
	void		SetActive(bool value) noexcept { this->m_DrawUI->Get(this->m_UIBase).SetActive(value); }
	void		SetEvent(const std::function<void()>& value) noexcept { this->m_ButtonDo = value; }

	int			GetSelect(void) const noexcept { return m_SelectButtonID; }
public:
	void		Init(void) noexcept {
		this->m_cancelID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/UI/cancel.wav", false);
		this->m_cursorID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/UI/cursor.wav", false);
		this->m_NGID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/UI/ng.wav", false);
		this->m_OKID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::SE, 3, "data/Sound/UI/ok.wav", false);

		this->m_DrawUI = new Draw::DrawUISystem();
		this->m_DrawUI->Init("data/UI/Title/ContinueUI.json");
		this->m_UIBase = this->m_DrawUI->GetID("");
		for (int loop = 0; loop < 5; ++loop) {
			std::string Path = "Button";
			Path += std::to_string(loop + 1);
			this->m_ButtonID[loop] = this->m_DrawUI->GetID(Path.c_str());
		}
		this->m_CloseButtonID = this->m_DrawUI->GetID("CloseButton");
		SetActive(false);
	}
	void		Update(void) noexcept {
		auto* KeyMngr = Util::KeyParam::Instance();
		if (IsActive()) {
			m_SelectButtonID = InvalidID;

			bool IsSelect = false;
			for (int loop = 0; loop < 5; ++loop) {
				if (this->m_DrawUI->Get(this->m_ButtonID[loop]).IsSelectButton()) {
					IsSelect = true;
				}
			}
			if (this->m_DrawUI->Get(this->m_CloseButtonID).IsSelectButton()) {
				IsSelect = true;
			}
			if (IsSelect && (IsSelect != this->m_isSelectSoundPrev)) {
				Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_cursorID)->Play(DX_PLAYTYPE_BACK, TRUE);
			}
			this->m_isSelectSoundPrev = IsSelect;

			if (KeyMngr->GetMenuKeyTrigger(Util::EnumMenu::Diside)) {
				for (int loop = 0; loop < 5; ++loop) {
					if (this->m_DrawUI->Get(this->m_ButtonID[loop]).IsSelectButton()) {
						if (this->m_DrawUI->Get(this->m_ButtonID[loop]).IsActive()) {
							Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_OKID)->Play(DX_PLAYTYPE_BACK, TRUE);
						}
						else {
							Sound::SoundPool::Instance()->Get(Sound::SoundType::SE, this->m_NGID)->Play(DX_PLAYTYPE_BACK, TRUE);
						}
					}
				}
			}
			if (KeyMngr->GetMenuKeyReleaseTrigger(Util::EnumMenu::Diside)) {
				for (int loop = 0; loop < 5; ++loop) {
					if (this->m_DrawUI->Get(this->m_ButtonID[loop]).IsSelectButton()) {
						if (this->m_DrawUI->Get(this->m_ButtonID[loop]).IsActive()) {
							m_SelectButtonID = loop;
							this->m_ButtonDo();
						}
					}
				}
				if (this->m_DrawUI->Get(this->m_CloseButtonID).IsSelectButton()) {
					SetActive(false);
				}
			}
		}
		this->m_DrawUI->Update(IsActive());
	}
	void		Draw(void) noexcept {
		this->m_DrawUI->Draw();
	}
	void		Dispose(void) noexcept {
		delete this->m_DrawUI;
	}
};
