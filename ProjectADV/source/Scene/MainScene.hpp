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

enum class SpeakEnum : size_t {
	Speak,
	WaitTime,
	BlackOut,
	BlackIn,
	ResetImage,
	SetBG,
};

struct SpeakData {
	SpeakEnum								m_SpeakEnum{};
	std::u32string							m_Speaker;
	const Draw::GraphHandle*				m_Image{};
	Util::VECTOR2D							m_Pos;
	std::array<std::pair<std::u32string, size_t>, MesColumn>	m_Mes;
	float									m_Time{ 0 };
	float									m_SeekMax{ 0 };
	float									m_Seek{};
	size_t									m_ResetImage{};
public:
	void Init() noexcept {
		m_Speaker = U"";
		m_Image = nullptr;
		m_Pos.Set(0.f, 0.f);
		for (auto& m : this->m_Mes) {
			m.first = U"";
			m.second = 0;
		}
		this->m_Time = 0.f;
		this->m_SeekMax = 0.f;
		this->m_Seek = 0.f;
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
	void DrawStr(int alpha) noexcept {
		auto* Font = Draw::FontPool::Instance();

		int X1 = 1920 / 2 - 1920 / 2 + 64;
		int Y1 = 1080 - 36 - 12 - 200;
		int X2 = 1920 / 2 + 1920 / 2 - 64;
		int Y2 = 1080 - 36 - 12;

		DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha * 1 / 2);
		DxLib::DrawBox(
			X1, Y1,
			X2, Y2,
			ColorPalette::Gray50, TRUE);
		DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
		Font->Get(Draw::FontType::DIZ_UD_Gothic, 32, 3)->DrawString(
			Draw::FontXCenter::LEFT, Draw::FontYCenter::BOTTOM,
			X1 + 32, Y1 - 6,
			ColorPalette::White, ColorPalette::Black,
			Util::char32_to_utf8(this->m_Speaker));

		size_t Min = 0;
		for (auto& m : this->m_Mes) {
			size_t index2 = static_cast<size_t>(&m - &this->m_Mes.front());
			Font->Get(Draw::FontType::DIZ_UD_Gothic, 24, 3)->DrawString(
				Draw::FontXCenter::LEFT, Draw::FontYCenter::MIDDLE,
				X1 + 64, Y1 + 62 * static_cast<int>(index2) + 62 / 2,
				ColorPalette::White, ColorPalette::Black,
				Util::char32_to_utf8(m.first.substr(0,
					static_cast<size_t>(std::clamp<int>(static_cast<int>(this->m_Seek / 0.05f) - static_cast<int>(Min), 0, static_cast<int>(m.second)))
				)));
			Min += m.second;
		}
		DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
};
class SpeakScript {
	std::vector<SpeakData>		m_SpeakData{};
	size_t						m_NowPoint = 0;
	float						float_Mes{};
	bool						m_IsStart = false;
	bool						m_IsEnd = false;
	bool						m_Exit{ false };
	char		padding[2]{};
	float						m_Fade{ 1.f };
	float						m_FadeTimer{ 1.f };
	size_t						m_ResetImage{};
	const Draw::GraphHandle*				m_PrevBGImage{};
	const Draw::GraphHandle*				m_NowBGImage{};
	float						m_BGFade{ 1.f };
	float						m_BGFadeTimer{ 1.f };
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
					m_SpeakData.back().m_SpeakEnum = SpeakEnum::Speak;
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
					m_SpeakData.back().m_SeekMax += static_cast<float>(m.second) * 0.05f;
					++SeekMes;
				}
				else if (Args.at(0) == "BlackOut") {
					m_SpeakData.emplace_back();
					m_SpeakData.back().Init();
					m_SpeakData.back().m_SpeakEnum = SpeakEnum::BlackOut;
					m_SpeakData.back().m_Time = std::stof(Args.at(1)) / 1000.f;
				}
				else if (Args.at(0) == "BlackIn") {
					m_SpeakData.emplace_back();
					m_SpeakData.back().Init();
					m_SpeakData.back().m_SpeakEnum = SpeakEnum::BlackIn;
					m_SpeakData.back().m_Time = std::stof(Args.at(1)) / 1000.f;
				}
				else if (Args.at(0) == "WaitMS") {
					m_SpeakData.emplace_back();
					m_SpeakData.back().Init();
					m_SpeakData.back().m_SpeakEnum = SpeakEnum::WaitTime;
					m_SpeakData.back().m_SeekMax = std::stof(Args.at(1))/1000.f;
				}
				else if (Args.at(0) == "ResetImage") {
					m_SpeakData.emplace_back();
					m_SpeakData.back().Init();
					m_SpeakData.back().m_SpeakEnum = SpeakEnum::ResetImage;
					m_SpeakData.back().m_ResetImage = m_SpeakData.size();
				}
				else if (Args.at(0) == "SetBG") {
					m_SpeakData.emplace_back();
					m_SpeakData.back().Init();
					m_SpeakData.back().m_SpeakEnum = SpeakEnum::SetBG;
					m_SpeakData.back().m_Image = Draw::GraphPool::Instance()->Get(Args.at(1))->Get();
					m_SpeakData.back().m_Time = std::stof(Args.at(2)) / 1000.f;
				}
			}
		}
		FileStream.Close();
		m_IsEnd = false;
		float_Mes = 1080.f;

		m_NowPoint = 0;
		m_IsStart = false;
		m_IsEnd = false;

		this->m_Exit = false;
		this->m_Fade = 1.f;
		this->m_FadeTimer = 1.f;
		this->m_ResetImage = 0;
		m_PrevBGImage = nullptr;
		m_NowBGImage = nullptr;
		this->m_BGFade = 1.f;
		this->m_BGFadeTimer = 1.f;
	}

	void Update() noexcept {
		if (m_IsStart && !m_IsEnd) {
			auto* KeyMngr = Util::KeyParam::Instance();
			if (m_NowPoint < m_SpeakData.size()) {
				auto& Now = m_SpeakData.at(m_NowPoint);

				switch (Now.m_SpeakEnum) {
				case SpeakEnum::Speak:
					if (KeyMngr->GetBattleKeyTrigger(Util::EnumBattle::Attack) || KeyMngr->GetBattleKeyRepeat(Util::EnumBattle::Walk)) {
						if (Now.m_Seek >= Now.m_SeekMax) {
							++m_NowPoint;
						}
						else {
							Now.m_Seek = Now.m_SeekMax;
						}
					}
					break;
				case SpeakEnum::WaitTime:
					if (Now.m_Seek >= Now.m_SeekMax) {
						++m_NowPoint;
					}
					break;
				case SpeakEnum::BlackOut:
					if (Now.m_Seek != 0.f) {
						++m_NowPoint;
					}
					else {
						this->m_Exit = true;
						this->m_FadeTimer = Now.m_Time;
					}
					break;
				case SpeakEnum::BlackIn:
					if (Now.m_Seek != 0.f) {
						++m_NowPoint;
					}
					else {
						this->m_Exit = false;
						this->m_FadeTimer = Now.m_Time;
					}
					break;
				case SpeakEnum::ResetImage:
					if (Now.m_Seek != 0.f) {
						++m_NowPoint;
					}
					else {
						this->m_ResetImage = Now.m_ResetImage;
					}
					break;
				case SpeakEnum::SetBG:
					if (Now.m_Seek != 0.f) {
						++m_NowPoint;
					}
					else {
						this->m_PrevBGImage = this->m_NowBGImage;
						this->m_NowBGImage = Now.m_Image;
						this->m_BGFadeTimer = Now.m_Time;
						this->m_BGFade = 0.f;
					}
					break;
				default:
					break;
				}
				Now.m_Seek += DeltaTime;
			}
			else {
				m_IsEnd = true;
			}
		}
		Util::Easing(&float_Mes, ((!m_IsStart || m_IsEnd) ? 1080.f : 0.f), 0.9f);

		this->m_Fade = std::clamp(this->m_Fade + (this->m_Exit ? 1.f : -1.f) * DeltaTime / this->m_FadeTimer, 0.f, 1.f);
		if (this->m_BGFadeTimer == 0.f) {
			this->m_BGFade = 1.f;
		}
		else {
			this->m_BGFade = std::clamp(this->m_BGFade + DeltaTime / this->m_BGFadeTimer, 0.f, 1.f);
		}
	}

	void Draw() noexcept {
		auto* Font = Draw::FontPool::Instance();
		size_t LaskSpeak = 0;
		for (auto& Now : m_SpeakData) {
			size_t index = static_cast<size_t>(&Now - &m_SpeakData.front());
			if (Now.m_SpeakEnum == SpeakEnum::Speak) {
				LaskSpeak = index;
			}
			if (index == m_NowPoint) {
				break;
			}
		}
		//背景
		if (m_PrevBGImage) {
			m_PrevBGImage->DrawRotaGraph(1920 / 2, 1080 / 2, 1.f, 0.f, false);
		}
		if (m_NowBGImage) {
			DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(255.f * this->m_BGFade), 0, 255));
			m_NowBGImage->DrawRotaGraph(1920 / 2, 1080 / 2, 1.f, 0.f, false);
			DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		}
		//イメージ
		for (auto& Now : m_SpeakData) {
			size_t index = static_cast<size_t>(&Now - &m_SpeakData.front());
			if (Now.m_SpeakEnum == SpeakEnum::Speak) {
				if (index == m_NowPoint) {
					Now.DrawImage(Util::VECTOR2D::vget(Now.m_Pos.x, Now.m_Pos.y + float_Mes), 255);
				}
				else {
					if (this->m_ResetImage <= index && index <= LaskSpeak) {
						Now.DrawImage(Util::VECTOR2D::vget(Now.m_Pos.x, Now.m_Pos.y + float_Mes), 128);
					}
				}
			}
		}
		//ブラックアウト
		{
			DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(255.f * this->m_Fade), 0, 255));
			DxLib::DrawBox(0, 0, 1920, 1080, ColorPalette::Black, true);
			DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		}
		//テキスト
		for (auto& Now : m_SpeakData) {
			size_t index = static_cast<size_t>(&Now - &m_SpeakData.front());
			if (index == m_NowPoint) {
				if (Now.m_SpeakEnum == SpeakEnum::Speak) {
					Now.DrawStr(static_cast<int>((1080.f - float_Mes) / 1080.f * 255.f));
				}
			}
			else {
				if (index == LaskSpeak) {
					Now.DrawStr(static_cast<int>((1080.f - float_Mes) / 1080.f * 255.f));
				}
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
