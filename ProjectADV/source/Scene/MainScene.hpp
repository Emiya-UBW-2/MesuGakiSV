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

constexpr int MesColumn = 3;

struct SpeakData {
	std::string							m_Speaker;
	const Draw::GraphHandle*			m_Image{};
	Util::VECTOR2D						m_Pos;
	size_t								MesMax{ 0 };
	std::array<std::string, MesColumn>	m_Mes;
};
class SpeakScript {
	std::vector<SpeakData>		m_SpeakData{};
	float						float_Mes{};
	float						seek_Mes{};
	size_t						m_NowPoint = 0;
	bool						m_IsStart = false;
	bool						m_IsEnd = false;
	char		padding[6]{};
	std::array<int, MesColumn>	m_MesMax{};
	char		padding2[4]{};
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
				if (Args.at(0) == "Speaker") {
					m_SpeakData.emplace_back();
					m_SpeakData.back().m_Speaker = Args.at(1);
					m_SpeakData.back().MesMax = 0;
					m_SpeakData.back().m_Mes.at(0) = "";
					m_SpeakData.back().m_Mes.at(1) = "";
					m_SpeakData.back().m_Mes.at(2) = "";
				}
				else if (Args.at(0) == "Position") {
					if (Args.at(1) == "LEFT") {
						m_SpeakData.back().m_Pos.x = 1920 / 2 - 864 / 2 - 100 + 512 / 2;
						m_SpeakData.back().m_Pos.y = 1080 - 512 / 2;
					}
					else if (Args.at(1) == "RIGHT") {
						m_SpeakData.back().m_Pos.x = 1920 / 2 + 864 / 2 + 100 - 512 / 2;
						m_SpeakData.back().m_Pos.y = 1080 - 512 / 2;
					}
				}
				else if (Args.at(0) == "Image") {
					m_SpeakData.back().m_Image = Draw::GraphPool::Instance()->Get(Args.at(1))->Get();
				}
				else if (Args.at(0) == "Mes") {
					m_SpeakData.back().m_Mes.at(m_SpeakData.back().MesMax) = Args.at(1);
					m_SpeakData.back().MesMax++;
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
				for (int loop2 = 0; loop2 < MesColumn; ++loop2) {
					m_MesMax.at(static_cast<size_t>(loop2)) = static_cast<int>(Now.m_Mes.at(static_cast<size_t>(loop2)).length() / 2);
				}

				seek_Mes += DeltaTime / 0.05f;

				int Max = 0;
				for (int loop2 = 0; loop2 < MesColumn; ++loop2) {
					Max += m_MesMax.at(static_cast<size_t>(loop2));
				}
				if (KeyMngr->GetBattleKeyTrigger(Util::EnumBattle::Attack) || KeyMngr->GetBattleKeyRepeat(Util::EnumBattle::Walk)) {
					if (static_cast<int>(seek_Mes) >= Max) {
						++m_NowPoint;
						seek_Mes = 0.f;
					}
					else {
						seek_Mes = static_cast<float>(Max);
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
				if (Now.m_Image) {
					DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
					SetDrawBright(0, 0, 0);
					Now.m_Image->DrawRotaGraph(static_cast<int>(Now.m_Pos.x), static_cast<int>(Now.m_Pos.y + float_Mes), 1.f, 0.f, true);
					SetDrawBright(255, 255, 255);
					DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
					Now.m_Image->DrawRotaGraph(static_cast<int>(Now.m_Pos.x), static_cast<int>(Now.m_Pos.y + float_Mes), 1.f, 0.f, true);
					DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				}
			}
			if (loop == m_NowPoint) {
				//DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(255.f* seek_Mes), 0, 255));
				if (Now.m_Image) {
					Now.m_Image->DrawRotaGraph(static_cast<int>(Now.m_Pos.x), static_cast<int>(Now.m_Pos.y + float_Mes), 1.f, 0.f, true);
				}
				DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>((1080.f - float_Mes) / 1080.f * 128.f));

				int X1 = 1920 / 2 - 1920 / 2 + 64;
				int Y1 = 1080 - 36 - 12 - 200;
				int X2 = 1920 / 2 + 1920 / 2 - 64;
				int Y2 = 1080 - 36 - 12;

				DxLib::DrawBox(
					X1, Y1,
					X2, Y2,
					ColorPalette::Gray50, TRUE);
				DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>((1080.f - float_Mes) / 1080.f * 255.f));
				Font->Get(Draw::FontType::DIZ_UD_Gothic, 32, 3)->DrawString(
					Draw::FontXCenter::LEFT, Draw::FontYCenter::BOTTOM,
					X1 + 32, Y1 - 6,
					ColorPalette::White, ColorPalette::Black,
					Util::SjistoUTF8(Now.m_Speaker));

				int Min = 0;
				for (int loop2 = 0; loop2 < MesColumn; ++loop2) {
					Font->Get(Draw::FontType::DIZ_UD_Gothic, 24, 3)->DrawString(
						Draw::FontXCenter::LEFT, Draw::FontYCenter::MIDDLE,
						X1 + 64, Y1 + 62 * loop2 + 62 / 2,
						ColorPalette::White, ColorPalette::Black,
						Util::SjistoUTF8(Now.m_Mes.at(static_cast<size_t>(loop2)).substr(0, static_cast<size_t>(std::clamp(static_cast<int>(seek_Mes) - Min, 0, m_MesMax.at(static_cast<size_t>(loop2))) * 2))));
					Min += m_MesMax.at(static_cast<size_t>(loop2));
				}

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
