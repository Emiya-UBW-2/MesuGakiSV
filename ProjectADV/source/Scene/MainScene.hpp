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

constexpr size_t MesColumn = 3;

struct SpeakData {
	std::u32string							m_Speaker;
	const Draw::GraphHandle*				m_Image{};
	Util::VECTOR2D							m_Pos;
	std::array<std::pair<std::u32string, size_t>, MesColumn>	m_Mes;
	size_t									m_MesMaxAll{ 0 };
public:
	void Init() noexcept {
		m_Speaker = U"";
		m_Image = nullptr;
		m_Pos.Set(0.f, 0.f);
		for (auto& m : this->m_Mes) {
			m.first = U"";
			m.second = 0;
		}
		this->m_MesMaxAll = 0;
	}
	void DrawImage(Util::VECTOR2D pos, int alpha) noexcept {
		if (this->m_Image) {
			if (alpha != 255) {
				DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				SetDrawBright(0, 0, 0);
				this->m_Image->DrawRotaGraph(static_cast<int>(pos.x), static_cast<int>(pos.y), 1.f, 0.f, true);
				SetDrawBright(255, 255, 255);
				DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
			}
			this->m_Image->DrawRotaGraph(static_cast<int>(pos.x), static_cast<int>(pos.y), 1.f, 0.f, true);
			DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		}
	}
};
class SpeakScript {
	std::vector<SpeakData>		m_SpeakData{};
	float						float_Mes{};
	float						seek_Mes{};
	size_t						m_NowPoint = 0;
	bool						m_IsStart = false;
	bool						m_IsEnd = false;
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
		size_t	SeekMes{ 0 };
		while (true) {
			if (FileStream.ComeEof()) { break; }
			std::vector<std::string> Args;
			File::GetArgs(FileStream.SeekLineAndGetStr(), &Args);
			//
			{
				if (Args.at(0) == "Speaker") {
					m_SpeakData.emplace_back();
					m_SpeakData.back().Init();
					m_SpeakData.back().m_Speaker = Util::utf8_to_char32(Args.at(1));
					SeekMes = 0;
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
					auto& m = m_SpeakData.back().m_Mes.at(SeekMes);
					m.first = Util::utf8_to_char32(Args.at(1));
					m.second = m.first.length();
					m_SpeakData.back().m_MesMaxAll += m.second;
					++SeekMes;
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

				seek_Mes += DeltaTime / 0.05f;

				if (KeyMngr->GetBattleKeyTrigger(Util::EnumBattle::Attack) || KeyMngr->GetBattleKeyRepeat(Util::EnumBattle::Walk)) {
					if (static_cast<size_t>(seek_Mes) >= Now.m_MesMaxAll) {
						++m_NowPoint;
						seek_Mes = 0.f;
					}
					else {
						seek_Mes = static_cast<float>(Now.m_MesMaxAll);
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
		for (auto& Now : m_SpeakData) {
			size_t index = static_cast<size_t>(&Now - &m_SpeakData.front());
			if (index < m_NowPoint) {
				Now.DrawImage(Util::VECTOR2D::vget(Now.m_Pos.x, Now.m_Pos.y + float_Mes), 128);
			}
			if (index == m_NowPoint) {
				Now.DrawImage(Util::VECTOR2D::vget(Now.m_Pos.x, Now.m_Pos.y + float_Mes), 255);

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
					Util::char32_to_utf8(Now.m_Speaker));

				size_t Min = 0;
				for (auto& m : Now.m_Mes) {
					size_t index2 = static_cast<size_t>(&m - &Now.m_Mes.front());
					Font->Get(Draw::FontType::DIZ_UD_Gothic, 24, 3)->DrawString(
						Draw::FontXCenter::LEFT, Draw::FontYCenter::MIDDLE,
						X1 + 64, Y1 + 62 * static_cast<int>(index2) + 62 / 2,
						ColorPalette::White, ColorPalette::Black,
						Util::char32_to_utf8(m.first.substr(0, 
							static_cast<size_t>(std::clamp<int>(static_cast<int>(seek_Mes) - static_cast<int>(Min), 0, static_cast<int>(m.second)))
						)));
					Min += m.second;
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
