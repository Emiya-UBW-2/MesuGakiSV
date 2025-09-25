#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:5039)
#pragma warning(disable:5045)

#include "../OptionWindow.hpp"
#include "../TitleUI.hpp"

#include "../Util/Enum.hpp"
#include "../Util/SceneManager.hpp"
#include "../Util/Key.hpp"
#include "../Util/Util.hpp"

#include "../Draw/MainDraw.hpp"

class TitleScene : public Util::SceneBase {
	OptionWindow m_OptionWindow;
	TitleUI m_TitleUI;
public:
	TitleScene(void) noexcept { SetID(static_cast<int>(EnumScene::Title)); }
	TitleScene(const TitleScene&) = delete;
	TitleScene(TitleScene&&) = delete;
	TitleScene& operator=(const TitleScene&) = delete;
	TitleScene& operator=(TitleScene&&) = delete;
	virtual ~TitleScene(void) noexcept {}
protected:
	void Init_Sub(void) noexcept override {
		this->m_OptionWindow.Init();
		this->m_TitleUI.Init();

		this->m_TitleUI.SetEvent(0, [this]() {
			this->m_TitleUI.SetEnd();
			});
		this->m_TitleUI.SetEvent(1, []() {
			});
		this->m_TitleUI.SetEvent(2, [this]() {
			this->m_OptionWindow.SetActive(true);
			});
		this->m_TitleUI.SetEvent(3, []() {
			});
	}
	void Update_Sub(void) noexcept override {
		this->m_TitleUI.SetActive(!this->m_OptionWindow.IsActive());
		this->m_TitleUI.Update();
		if (this->m_TitleUI.IsEnd()) {
			this->m_OptionWindow.SetActive(false);
			SceneBase::SetNextScene(Util::SceneManager::Instance()->GetScene(static_cast<int>(EnumScene::Main)));
			SceneBase::SetEndScene();
		}
		this->m_OptionWindow.Update();
	}
	void BGDraw_Sub(void) noexcept override {
	}
	void Draw_Sub(void) noexcept override {
	}
	void UIDraw_Sub(void) noexcept override {
		this->m_TitleUI.Draw();
		this->m_OptionWindow.Draw();
	}
	void Dispose_Sub(void) noexcept override {
		this->m_TitleUI.Dispose();
		this->m_OptionWindow.Dispose();
	}
};
