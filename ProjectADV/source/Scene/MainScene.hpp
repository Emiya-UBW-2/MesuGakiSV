#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:4710)
#pragma warning(disable:4711)
#pragma warning(disable:5039)

#include "../OptionWindow.hpp"
#include "../PauseUI.hpp"

#include "../Util/SceneManager.hpp"


struct SpeakData {
	std::string m_Speaker;
	const Draw::GraphHandle* m_Image{};
	Util::VECTOR2D m_Pos;
	std::string m_Mes1;
	std::string m_Mes2;
};
class SpeakScript {
	std::vector<SpeakData>	m_SpeakData;
	int total_Mes1{};
	int total_Mes2{};

	float float_Mes{};

	float seek_Mes{};
	size_t m_NowPoint = 0;
	bool m_IsStart = false;
	bool m_IsEnd = false;
	char		padding[6]{};
public:
	bool IsEnd() const noexcept { return m_IsEnd; }
	void SetStoryStart() noexcept {
		m_IsStart = true;
	}
public:
	void Load(const char* Path) noexcept {
		m_SpeakData.clear();

		File::InputFileStream FileStream;
		FileStream.Open(Path);
		while (true) {
			if (FileStream.ComeEof()) { break; }
			std::vector<std::string> Args;
			File::GetArgs(FileStream.SeekLineAndGetStr(), &Args);
			//
			{
				if (Args.at(0) == "speaker") {
					m_SpeakData.emplace_back();
					m_SpeakData.back().m_Speaker = Args.at(1);
					m_SpeakData.back().m_Mes1 = "";
					m_SpeakData.back().m_Mes2 = "";
				}
				else if (Args.at(0) == "position") {
					if (Args.at(1) == "LEFT") {
						m_SpeakData.back().m_Pos.x = 1920 / 2 - 864 / 2 - 100 + 512 / 2;
						m_SpeakData.back().m_Pos.y = 1080 - 512 / 2;
					}
					else if (Args.at(1) == "RIGHT") {
						m_SpeakData.back().m_Pos.x = 1920 / 2 + 864 / 2 + 100 - 512 / 2;
						m_SpeakData.back().m_Pos.y = 1080 - 512 / 2;
					}
				}
				else if (Args.at(0) == "image") {
					m_SpeakData.back().m_Image = Draw::GraphPool::Instance()->Get(Args.at(1))->Get();
				}
				else if (Args.at(0) == "mes1") {
					m_SpeakData.back().m_Mes1 = Args.at(1);
				}
				else if (Args.at(0) == "mes2") {
					m_SpeakData.back().m_Mes2 = Args.at(1);
				}
			}
		}
		FileStream.Close();
		m_IsEnd = false;
		float_Mes = 1080.f;

		seek_Mes = 0.f;
		m_NowPoint = 0;
		m_IsStart = false;
		m_IsEnd = false;
	}

	void Update() noexcept {
		if (m_IsStart && !m_IsEnd) {
			auto* KeyMngr = Util::KeyParam::Instance();
			if (m_NowPoint < m_SpeakData.size()) {
				auto& Now = m_SpeakData.at(m_NowPoint);
				total_Mes1 = static_cast<int>(Now.m_Mes1.length() / 2);
				total_Mes2 = static_cast<int>(Now.m_Mes2.length() / 2);

				seek_Mes += DeltaTime / 0.1f;

				if (KeyMngr->GetBattleKeyTrigger(Util::EnumBattle::Attack) || KeyMngr->GetBattleKeyRepeat(Util::EnumBattle::Walk)) {
					if (static_cast<int>(seek_Mes) >= total_Mes1 + total_Mes2) {
						++m_NowPoint;
						seek_Mes = 0.f;
					}
					else {
						seek_Mes = static_cast<float>(total_Mes1 + total_Mes2);
					}
				}
			}
			else {
				m_IsEnd = true;
			}
		}
		Util::Easing(&float_Mes, ((!m_IsStart || m_IsEnd) ? 1080.f : 0.f), 0.9f);
	}

	void Draw() noexcept {
		auto* Font = Draw::FontPool::Instance();

		for (size_t loop = 0; loop < m_SpeakData.size(); ++loop) {
			auto& Now = m_SpeakData.at(loop);
			if (loop < m_NowPoint) {
				DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				SetDrawBright(0, 0, 0);
				Now.m_Image->DrawRotaGraph(static_cast<int>(Now.m_Pos.x), static_cast<int>(Now.m_Pos.y + float_Mes), 1.f, 0.f, true);
				SetDrawBright(255, 255, 255);
				DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
				Now.m_Image->DrawRotaGraph(static_cast<int>(Now.m_Pos.x), static_cast<int>(Now.m_Pos.y + float_Mes), 1.f, 0.f, true);
				DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			}
			if (loop == m_NowPoint) {
				//DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(255.f* seek_Mes), 0, 255));
				Now.m_Image->DrawRotaGraph(static_cast<int>(Now.m_Pos.x), static_cast<int>(Now.m_Pos.y + float_Mes), 1.f, 0.f, true);

				DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>((1080.f - float_Mes) / 1080.f * 128.f));
				DxLib::DrawBox(
					1920 / 2 - 864 / 2 + 64, 1080 - 36 - 12 - 125,
					1920 / 2 + 864 / 2 - 64, 1080 - 36 - 12,
					ColorPalette::Gray50, TRUE);
				DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>((1080.f - float_Mes) / 1080.f * 255.f));
				Font->Get(Draw::FontType::DIZ_UD_Gothic, 24, 3)->DrawString(
					Draw::FontXCenter::LEFT, Draw::FontYCenter::BOTTOM,
					1920 / 2 - 864 / 2 + 64 + 32, 1080 - 36 - 12 - 125 - 6,
					ColorPalette::White, ColorPalette::Black,
					Util::SjistoUTF8(Now.m_Speaker));
				Font->Get(Draw::FontType::DIZ_UD_Gothic, 32, 3)->DrawString(
					Draw::FontXCenter::LEFT, Draw::FontYCenter::MIDDLE,
					1920 / 2 - 864 / 2 + 64 + 64, 1080 - 36 - 12 - 125 + 62 / 2,
					ColorPalette::White, ColorPalette::Black,
					Util::SjistoUTF8(Now.m_Mes1.substr(0, static_cast<size_t>(std::clamp(static_cast<int>(seek_Mes), 0, total_Mes1) * 2))));
				Font->Get(Draw::FontType::DIZ_UD_Gothic, 32, 3)->DrawString(
					Draw::FontXCenter::LEFT, Draw::FontYCenter::MIDDLE,
					1920 / 2 - 864 / 2 + 64 + 64, 1080 - 36 - 12 - 125 + 62 + 62 / 2,
					ColorPalette::White, ColorPalette::Black,
					Util::SjistoUTF8(Now.m_Mes2.substr(0, static_cast<size_t>(std::clamp(static_cast<int>(seek_Mes) - total_Mes1, 0, total_Mes2) * 2))));
				DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			}
		}
	}
};


class MainScene : public Util::SceneBase {
	OptionWindow					m_OptionWindow;
	PauseUI							m_PauseUI;
	bool							m_Exit{ false };
	bool							m_IsSceneEnd{ false };
	bool							m_IsPauseActive{ false };
	bool							m_IsResetMouse{ false };
	float							m_Fade{ 1.f };

	Sound::SoundUniqueID			m_OKID{ InvalidID };
	Sound::SoundUniqueID			m_EnviID{ InvalidID };

	Sound::SoundUniqueID			m_NormalBGMID{ InvalidID };
	const Draw::GraphHandle*		m_BackScreen{};

	SpeakScript						m_SpeakScript{};
public:
	MainScene(void) noexcept { SetID(static_cast<int>(EnumScene::Main)); }
	MainScene(const MainScene&) = delete;
	MainScene(MainScene&&) = delete;
	MainScene& operator=(const MainScene&) = delete;
	MainScene& operator=(MainScene&&) = delete;
	virtual ~MainScene(void) noexcept {}
private:
protected:
	void Load_Sub(void) noexcept override;
	void Init_Sub(void) noexcept override;
	void Update_Sub(void) noexcept override;
	void BGDraw_Sub(void) noexcept override {}
	void SetShadowDrawRigid_Sub(void) noexcept override {}
	void SetShadowDraw_Sub(void) noexcept override {}
	void Draw_Sub(void) noexcept override {}
	void DepthDraw_Sub(void) noexcept override {}
	void ShadowDrawFar_Sub(void) noexcept override {}
	void ShadowDraw_Sub(void) noexcept override {}
	void UIDraw_Sub(void) noexcept override;
	void Dispose_Sub(void) noexcept override;
};
