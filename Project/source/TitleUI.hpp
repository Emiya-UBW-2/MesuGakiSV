#pragma once

#pragma warning(disable:5045)
#include "Draw/DrawUISystem.hpp"

#include "Util/Localize.hpp"

#pragma warning( push, 3 )
#include <functional>
#pragma warning( pop )
class TitleUI {
private:
	Draw::DrawUISystem* m_DrawUI{ nullptr };
	int				m_UIBase{ InvalidID };
	int				m_ButtonID[4] = { InvalidID,InvalidID,InvalidID,InvalidID };
	bool			m_IsActive{};
	char		padding[3]{};
	std::array<std::function<void()>, 4>	m_ButtonDo{};
public:
	TitleUI(void) noexcept {}
	TitleUI(const TitleUI&) = delete;
	TitleUI(TitleUI&&) = delete;
	TitleUI& operator=(const TitleUI&) = delete;
	TitleUI& operator=(TitleUI&&) = delete;
	virtual ~TitleUI(void) noexcept {}
public:
	bool		IsEnd(void) const noexcept { return !this->m_DrawUI->Get(this->m_UIBase).IsActive() && this->m_DrawUI->Get(this->m_UIBase).IsAnimeEnd(); }
	void		SetEnd() noexcept { this->m_DrawUI->Get(this->m_UIBase).SetActive(false); }
	bool		IsActive(void) const noexcept { return this->m_IsActive; }
	void		SetActive(bool value) noexcept { this->m_IsActive = value; }
	void		SetEvent(int ID, const std::function<void()>& value) noexcept { this->m_ButtonDo[static_cast<size_t>(ID)] = value; }
public:
	void		Init(void) noexcept {
		this->m_DrawUI = new Draw::DrawUISystem();
		this->m_DrawUI->Init("data/UI/Title/TitleUI.json");
		this->m_UIBase = this->m_DrawUI->GetID("");

		for (int loop = 0; loop < 4; ++loop) {
			std::string Path = "Tab";
			Path += std::to_string(loop + 1);
			this->m_ButtonID[loop] = this->m_DrawUI->GetID(Path.c_str());
		}
		this->m_DrawUI->Get(this->m_UIBase).SetActive(true);
	}
	void		Update(void) noexcept {
		auto* KeyMngr = Util::KeyParam::Instance();
		if (IsActive()) {
			if (KeyMngr->GetMenuKeyReleaseTrigger(Util::EnumMenu::Diside)) {
				for (int loop = 0; loop < 4; ++loop) {
					if (this->m_DrawUI->Get(this->m_ButtonID[loop]).IsSelectButton()) {
						if (this->m_ButtonDo[static_cast<size_t>(loop)]) {
							this->m_ButtonDo[static_cast<size_t>(loop)]();
						}
						break;
					}
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

class EndUI {
private:
	Draw::DrawUISystem* m_DrawUI{ nullptr };
	int				m_UIBase{ InvalidID };
	int				m_ButtonID{ InvalidID };
	int				m_CloseButtonID{ InvalidID };
	std::function<void()>	m_ButtonDo{};
public:
	EndUI(void) noexcept {}
	EndUI(const EndUI&) = delete;
	EndUI(EndUI&&) = delete;
	EndUI& operator=(const EndUI&) = delete;
	EndUI& operator=(EndUI&&) = delete;
	virtual ~EndUI(void) noexcept {}
public:
	bool		IsEnd(void) const noexcept { return !this->m_DrawUI->Get(this->m_UIBase).IsActive() && this->m_DrawUI->Get(this->m_UIBase).IsAnimeEnd(); }
	bool		IsActive(void) const noexcept { return this->m_DrawUI->Get(this->m_UIBase).IsActive(); }
	void		SetActive(bool value) noexcept { this->m_DrawUI->Get(this->m_UIBase).SetActive(value); }
	void		SetEvent(const std::function<void()>& value) noexcept { this->m_ButtonDo = value; }
public:
	void		Init(void) noexcept {
		this->m_DrawUI = new Draw::DrawUISystem();
		this->m_DrawUI->Init("data/UI/Title/EndUI.json");
		this->m_UIBase = this->m_DrawUI->GetID("");

		this->m_ButtonID = this->m_DrawUI->GetID("Button");
		this->m_CloseButtonID = this->m_DrawUI->GetID("CloseButton");
		SetActive(false);
	}
	void		Update(void) noexcept {
		auto* KeyMngr = Util::KeyParam::Instance();
		if (IsActive()) {
			if (KeyMngr->GetMenuKeyReleaseTrigger(Util::EnumMenu::Diside)) {
				if (this->m_DrawUI->Get(this->m_ButtonID).IsSelectButton()) {
					this->m_ButtonDo();
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
