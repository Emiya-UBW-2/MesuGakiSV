#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:4710)
#pragma warning(disable:4711)
#pragma warning(disable:5039)

#include "../OptionWindow.hpp"
#include "../PauseUI.hpp"
#include "../TitleUI.hpp"

#include "../Util/SceneManager.hpp"

#include "../Draw/MV1.hpp"

constexpr size_t MesColumn = 3;

enum class SpeakEnum : size_t {
	Speak,
	Model,
	Image,
	WaitTime,
	BlackOut,
	BlackIn,
	ResetImage,
	ResetModel,
	SetBG,
	SetBGM,
};

struct SpeakData {
	SpeakEnum								m_SpeakEnum{};
	std::u32string							m_Speaker;
	const Draw::Graphhave*					m_Image{};
	const Draw::MV1have*					m_MV1{};
	Util::VECTOR2D							m_Pos;
	std::array<std::pair<std::u32string, size_t>, MesColumn>	m_Mes;
	size_t									m_MV1AnimSelect{ 0 };
	Sound::SoundUniqueID					m_BGMID{ InvalidID };
	float									m_Timer{ 0 };
	float									m_SeekMax{ 0 };
	float									m_TimeMax{ 0 };
	unsigned int							m_Color{ ColorPalette::White };
	unsigned int							m_EdgeColor{ ColorPalette::Black };
	char		padding[4]{};
	bool									m_AutoGoNext{};
	char		padding2[7]{};
public:
	void Init(void) noexcept {
		this->m_Speaker = U"";
		this->m_Image = nullptr;
		this->m_MV1 = nullptr;
		this->m_MV1AnimSelect = 0;
		this->m_BGMID = InvalidID;
		this->m_Pos.Set(0.f, 0.f);
		for (auto& m : this->m_Mes) {
			m.first = U"";
			m.second = 0;
		}
		this->m_Timer = 0.f;
		this->m_SeekMax = 0.f;
		this->m_TimeMax = 0.f;
	}
public:
	void DrawImage(Util::VECTOR2D pos, int alpha) const noexcept {
		if (this->m_Image) {
			if (alpha != 255) {
				DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				SetDrawBright(0, 0, 0);
				this->m_Image->Get()->DrawRotaGraph(static_cast<int>(pos.x), static_cast<int>(pos.y), 1.f, 0.f, true);
				SetDrawBright(255, 255, 255);
				DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
			}
			this->m_Image->Get()->DrawRotaGraph(static_cast<int>(pos.x), static_cast<int>(pos.y), 1.f, 0.f, true);
			DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		}
	}
	void DrawModel(void) const noexcept {
		if (this->m_MV1) {
			this->m_MV1->Get()->DrawModel();
		}
	}
	void DrawStr(float seek, int alpha) const noexcept {
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
		Font->Get(Draw::FontType::DIZ_UD_Gothic, 32, 2)->DrawString(
			Draw::FontXCenter::LEFT, Draw::FontYCenter::BOTTOM,
			X1 + 32, Y1 - 6,
			this->m_Color, this->m_EdgeColor,
			Util::char32_to_utf8(this->m_Speaker));

		size_t Min = 0;
		for (auto& m : this->m_Mes) {
			size_t index2 = static_cast<size_t>(&m - &this->m_Mes.front());

			auto StrMax = static_cast<size_t>(std::clamp<int>(static_cast<int>(seek) - static_cast<int>(Min), 0, static_cast<int>(m.second)));
			auto Mes = m.first.substr(0, StrMax);

			//打ち込み風
			if (StrMax != 0 && (index2 == MesColumn - 1 || (this->m_Mes.at(index2 + 1).second == 0))) {
				if ((seek < this->m_SeekMax) || (static_cast<int>(seek / 2.f) % 10 < 5)) {
					Mes += U"|";
				}
			}

			Font->Get(Draw::FontType::DIZ_UD_Gothic, 24, 2)->DrawString(
				Draw::FontXCenter::LEFT, Draw::FontYCenter::MIDDLE,
				X1 + 64, Y1 + 62 * static_cast<int>(index2) + 62 / 2,
				this->m_Color, this->m_EdgeColor,
				Util::char32_to_utf8(Mes));
			Min += m.second;
		}
		DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
	}
};
class SpeakScript {
	std::vector<SpeakData>		m_SpeakData{};
	size_t						m_NowPoint{ 0 };
	float						m_Fade{ 1.f };
	float						m_FadeTimer{ 1.f };
	float						m_BGFade{ 1.f };
	float						m_BGFadeTimer{ 1.f };

	float						m_BGMFade{ 1.f };
	float						m_BGMFadeTimer{ 1.f };
	float						m_Seek{ 0.f };
	float						m_Time{ 0.f };

	bool						m_IsStart{ false };
	bool						m_IsEnd{ false };
	bool						m_Exit{ false };
	char		padding[1]{};
	int							m_GoNext{ InvalidID };
	const Draw::Graphhave*				m_PrevBGImage{};
	const Draw::Graphhave*				m_NowBGImage{};

	Sound::SoundUniqueID			m_PrevBGMID{ InvalidID };
	Sound::SoundUniqueID			m_NowBGMID{ InvalidID };
public:
	bool IsEnd(void) const noexcept { return this->m_IsEnd; }
	void SetStoryStart(void) noexcept { this->m_IsStart = true; }
	size_t GetNowPoint(void) const noexcept { return this->m_NowPoint; }
	int GetNext(void) const noexcept { return this->m_GoNext; }
public:
	void Load(const char* Path) noexcept {
		this->m_SpeakData.clear();

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
					this->m_SpeakData.emplace_back();
					this->m_SpeakData.back().Init();
					this->m_SpeakData.back().m_SpeakEnum = SpeakEnum::Speak;
					this->m_SpeakData.back().m_Speaker = Util::utf8_to_char32(Args.at(1));
					SeekMes = 0;
				}
				else if (Args.at(0) == "MesColor") {
					this->m_SpeakData.back().m_Color = GetColor(std::stoi(Args.at(1)), std::stoi(Args.at(2)), std::stoi(Args.at(3)));
				}
				else if (Args.at(0) == "MesEdgeColor") {
					this->m_SpeakData.back().m_EdgeColor = GetColor(std::stoi(Args.at(1)), std::stoi(Args.at(2)), std::stoi(Args.at(3)));
				}
				else if (Args.at(0) == "AutoGoNext") {
					this->m_SpeakData.back().m_AutoGoNext = true;
				}
				else if (Args.at(0) == "Mes") {
					auto& m = this->m_SpeakData.back().m_Mes.at(SeekMes);
					m.first = Util::utf8_to_char32(Args.at(1));
					m.second = m.first.length();
					this->m_SpeakData.back().m_SeekMax += static_cast<float>(m.second);
					++SeekMes;
				}
				else if (Args.at(0) == "Model") {
					this->m_SpeakData.emplace_back();
					this->m_SpeakData.back().Init();
					this->m_SpeakData.back().m_SpeakEnum = SpeakEnum::Model;
					if (Args.at(1) != "NULL") {
						this->m_SpeakData.back().m_MV1 = Draw::MV1Pool::Instance()->Get(Args.at(1)).get();
					}
					this->m_SpeakData.back().m_MV1AnimSelect = static_cast<size_t>(std::stoi(Args.at(2)));
				}
				else if (Args.at(0) == "Image") {
					this->m_SpeakData.emplace_back();
					this->m_SpeakData.back().Init();
					this->m_SpeakData.back().m_SpeakEnum = SpeakEnum::Image;
					if (Args.at(1) != "NULL") {
						this->m_SpeakData.back().m_Image = Draw::GraphPool::Instance()->Get(Args.at(1)).get();
					}
				}
				else if (Args.at(0) == "Position") {
					if (Args.at(1) == "LEFT") {
						this->m_SpeakData.back().m_Pos.x = 1920 / 2 - 864 / 2 - 100 + 512 / 2;
						this->m_SpeakData.back().m_Pos.y = 1080 - 512 / 2;
					}
					else if (Args.at(1) == "RIGHT") {
						this->m_SpeakData.back().m_Pos.x = 1920 / 2 + 864 / 2 + 100 - 512 / 2;
						this->m_SpeakData.back().m_Pos.y = 1080 - 512 / 2;
					}
				}
				else if (Args.at(0) == "BlackOut") {
					this->m_SpeakData.emplace_back();
					this->m_SpeakData.back().Init();
					this->m_SpeakData.back().m_SpeakEnum = SpeakEnum::BlackOut;
					this->m_SpeakData.back().m_Timer = std::stof(Args.at(1)) / 1000.f;
				}
				else if (Args.at(0) == "BlackIn") {
					this->m_SpeakData.emplace_back();
					this->m_SpeakData.back().Init();
					this->m_SpeakData.back().m_SpeakEnum = SpeakEnum::BlackIn;
					this->m_SpeakData.back().m_Timer = std::stof(Args.at(1)) / 1000.f;
				}
				else if (Args.at(0) == "WaitMS") {
					this->m_SpeakData.emplace_back();
					this->m_SpeakData.back().Init();
					this->m_SpeakData.back().m_SpeakEnum = SpeakEnum::WaitTime;
					this->m_SpeakData.back().m_TimeMax = std::stof(Args.at(1))/1000.f;
				}
				else if (Args.at(0) == "ResetImage") {
					this->m_SpeakData.emplace_back();
					this->m_SpeakData.back().Init();
					this->m_SpeakData.back().m_SpeakEnum = SpeakEnum::ResetImage;
				}
				else if (Args.at(0) == "ResetModel") {
					this->m_SpeakData.emplace_back();
					this->m_SpeakData.back().Init();
					this->m_SpeakData.back().m_SpeakEnum = SpeakEnum::ResetModel;
				}
				else if (Args.at(0) == "SetBG") {
					this->m_SpeakData.emplace_back();
					this->m_SpeakData.back().Init();
					this->m_SpeakData.back().m_SpeakEnum = SpeakEnum::SetBG;
					this->m_SpeakData.back().m_Image = Draw::GraphPool::Instance()->Get(Args.at(1)).get();
					this->m_SpeakData.back().m_Timer = std::stof(Args.at(2)) / 1000.f;
				}
				else if (Args.at(0) == "SetBGM") {
					this->m_SpeakData.emplace_back();
					this->m_SpeakData.back().Init();
					this->m_SpeakData.back().m_SpeakEnum = SpeakEnum::SetBGM;
					if (Args.at(1) == "NULL" || Args.at(1) == "") {
						this->m_SpeakData.back().m_BGMID = InvalidID;
					}
					else {
						this->m_SpeakData.back().m_BGMID = Sound::SoundPool::Instance()->GetUniqueID(Sound::SoundType::BGM, 1, Args.at(1), false);
					}
					this->m_SpeakData.back().m_Timer = std::stof(Args.at(2)) / 1000.f;
				}
				else if (Args.at(0) == "SetNext") {
					this->m_GoNext = std::stoi(Args.at(1));
				}
			}
		}
		FileStream.Close();
	}
	void Start(void) noexcept {
		this->m_NowPoint = 0;
		this->m_Fade = 1.f;
		this->m_FadeTimer = 1.f;

		this->m_BGFade = 1.f;
		this->m_BGFadeTimer = 1.f;

		this->m_BGMFade = 1.f;
		this->m_BGMFadeTimer = 1.f;

		if (this->m_PrevBGMID != InvalidID) {
			Sound::SoundPool::Instance()->Get(Sound::SoundType::BGM, this->m_PrevBGMID)->StopAll();
		}
		if (this->m_NowBGMID != InvalidID) {
			Sound::SoundPool::Instance()->Get(Sound::SoundType::BGM, this->m_NowBGMID)->StopAll();
		}
		this->m_PrevBGMID = InvalidID;
		this->m_NowBGMID = InvalidID;

		this->m_PrevBGImage = nullptr;
		this->m_NowBGImage = nullptr;

		this->m_IsStart = false;
		this->m_IsEnd = false;
		this->m_Exit = false;
		this->m_Seek = 0.f;
		this->m_Time = 0.f;
	}
	void Step(void) noexcept {
		if (this->m_NowPoint < this->m_SpeakData.size()) {
			const auto& Now = this->m_SpeakData.at(this->m_NowPoint);
			switch (Now.m_SpeakEnum) {
			case SpeakEnum::Speak:
				++this->m_NowPoint;
				break;
			case SpeakEnum::Model:
				++this->m_NowPoint;
				if (Now.m_MV1) {
					auto* pModel = (Draw::MV1*)Now.m_MV1->Get();
					pModel->SetAnim(Now.m_MV1AnimSelect).SetTime(0.f);
				}
				break;
			case SpeakEnum::Image:
				++this->m_NowPoint;
				break;
			case SpeakEnum::WaitTime:
				++this->m_NowPoint;
				break;
			case SpeakEnum::BlackOut:
				this->m_Exit = true;
				this->m_FadeTimer = Now.m_Timer;
				++this->m_NowPoint;
				break;
			case SpeakEnum::BlackIn:
				this->m_Exit = false;
				this->m_FadeTimer = Now.m_Timer;
				++this->m_NowPoint;
				break;
			case SpeakEnum::ResetImage:
				++this->m_NowPoint;
				break;
			case SpeakEnum::ResetModel:
				++this->m_NowPoint;
				break;
			case SpeakEnum::SetBG:
				this->m_PrevBGImage = this->m_NowBGImage;
				this->m_NowBGImage = Now.m_Image;
				this->m_BGFadeTimer = Now.m_Timer;
				this->m_BGFade = 0.f;
				++this->m_NowPoint;
				break;
			case SpeakEnum::SetBGM:
				this->m_PrevBGMID = this->m_NowBGMID;
				this->m_NowBGMID = Now.m_BGMID;
				this->m_BGMFadeTimer = Now.m_Timer;
				this->m_BGMFade = 0.f;
				++this->m_NowPoint;
				break;
			default:
				break;
			}
		}
	}

	void Update(void) noexcept {
#if _DEBUG
		printfDx("NowPoint[%d]\n", this->m_NowPoint);
#endif
		if (this->m_IsStart && !this->m_IsEnd) {
			auto* KeyMngr = Util::KeyParam::Instance();
			if (this->m_NowPoint < this->m_SpeakData.size()) {
				const auto& Now = this->m_SpeakData.at(this->m_NowPoint);
				switch (Now.m_SpeakEnum) {
				case SpeakEnum::Speak:
					this->m_Seek += DeltaTime / 0.05f;
					if (KeyMngr->GetBattleKeyTrigger(Util::EnumBattle::Attack) || KeyMngr->GetBattleKeyRepeat(Util::EnumBattle::Aim)) {
						if (this->m_Seek >= Now.m_SeekMax) {
							++this->m_NowPoint;
							this->m_Seek = 0.f;
						}
						else {
							this->m_Seek = Now.m_SeekMax;
						}
					}
					if (Now.m_AutoGoNext) {
						if (this->m_Seek >= Now.m_SeekMax) {
							++this->m_NowPoint;
							this->m_Seek = 0.f;
						}
					}
					break;
				case SpeakEnum::Model:
					++this->m_NowPoint;
					if (Now.m_MV1) {
						auto* pModel = (Draw::MV1*)Now.m_MV1->Get();
						pModel->SetAnim(Now.m_MV1AnimSelect).SetTime(0.f);
					}
					break;
				case SpeakEnum::Image:
					++this->m_NowPoint;
					break;
				case SpeakEnum::WaitTime:
					this->m_Time += DeltaTime;
					if (this->m_Time >= Now.m_TimeMax) {
						++this->m_NowPoint;
						this->m_Time = 0.f;
					}
					break;
				case SpeakEnum::BlackOut:
					this->m_Exit = true;
					this->m_FadeTimer = Now.m_Timer;
					++this->m_NowPoint;
					break;
				case SpeakEnum::BlackIn:
					this->m_Exit = false;
					this->m_FadeTimer = Now.m_Timer;
					++this->m_NowPoint;
					break;
				case SpeakEnum::ResetImage:
					++this->m_NowPoint;
					break;
				case SpeakEnum::ResetModel:
					++this->m_NowPoint;
					break;
				case SpeakEnum::SetBG:
					this->m_PrevBGImage = this->m_NowBGImage;
					this->m_NowBGImage = Now.m_Image;
					this->m_BGFadeTimer = Now.m_Timer;
					this->m_BGFade = 0.f;
					++this->m_NowPoint;
					break;
				case SpeakEnum::SetBGM:
					//BGM強制停止
					if (this->m_BGMFade != 1.f) {
						if (this->m_PrevBGMID != InvalidID) {
							Sound::SoundPool::Instance()->Get(Sound::SoundType::BGM, this->m_PrevBGMID)->StopAll();
						}
						if (this->m_NowBGMID != InvalidID) {
							Sound::SoundPool::Instance()->Get(Sound::SoundType::BGM, this->m_NowBGMID)->StopAll();
						}
					}
					this->m_PrevBGMID = this->m_NowBGMID;
					this->m_NowBGMID = Now.m_BGMID;
					this->m_BGMFadeTimer = Now.m_Timer;
					this->m_BGMFade = 0.f;
					++this->m_NowPoint;
					break;
				default:
					break;
				}
			}
			else {
				this->m_IsEnd = true;
			}
		}
		this->m_Fade = std::clamp(this->m_Fade + (this->m_Exit ? 1.f : -1.f) * DeltaTime / this->m_FadeTimer, 0.f, 1.f);
		if (this->m_BGFadeTimer == 0.f) {
			this->m_BGFade = 1.f;
		}
		else {
			this->m_BGFade = std::clamp(this->m_BGFade + DeltaTime / this->m_BGFadeTimer, 0.f, 1.f);
		}
		{
			float Prev = this->m_BGMFade;
			if (this->m_BGMFadeTimer == 0.f) {
				this->m_BGMFade = 1.f;
			}
			else {
				this->m_BGMFade = std::clamp(this->m_BGMFade + DeltaTime / this->m_BGMFadeTimer, 0.f, 1.f);
			}
			if ((Prev != this->m_BGMFade)) {
				if (Prev == 0.f) {
					if (this->m_NowBGMID != InvalidID) {
						Sound::SoundPool::Instance()->Get(Sound::SoundType::BGM, this->m_NowBGMID)->Play(DX_PLAYTYPE_LOOP, TRUE);
					}
				}
				if (this->m_PrevBGMID != InvalidID) {
					Sound::SoundPool::Instance()->Get(Sound::SoundType::BGM, this->m_PrevBGMID)->SetLocalVolume(static_cast<int>((1.f - this->m_BGMFade) * 255.f));
				}
				if (this->m_NowBGMID != InvalidID) {
					Sound::SoundPool::Instance()->Get(Sound::SoundType::BGM, this->m_NowBGMID)->SetLocalVolume(static_cast<int>((this->m_BGMFade) * 255.f));
				}
				if (this->m_BGMFade == 1.f) {
					if (this->m_PrevBGMID != InvalidID) {
						Sound::SoundPool::Instance()->Get(Sound::SoundType::BGM, this->m_PrevBGMID)->StopAll();
					}
				}
			}
		}
		{
			size_t LaskMV1 = 0;
			for (const auto& Now : this->m_SpeakData) {
				size_t index = static_cast<size_t>(&Now - &this->m_SpeakData.front());
				if (Now.m_SpeakEnum == SpeakEnum::Model) {
					if (Now.m_MV1) {
						LaskMV1 = index;
					}
				}
				if (index == this->m_NowPoint) {
					break;
				}
			}
			const auto& Now = this->m_SpeakData.at(LaskMV1);
			if (Now.m_MV1) {
				auto* pModel = (Draw::MV1*)Now.m_MV1->Get();

				//どれか1にしておく
				bool AllZero = true;
				for (size_t loop = 0; loop < pModel->GetAnimNum(); ++loop) {
					if (pModel->SetAnim(loop).GetPer() > 0.5f) {
						AllZero = false;
						break;
					}
				}
				if (AllZero) {
					pModel->SetAnim(Now.m_MV1AnimSelect).SetPer(1.f);
				}

				for (size_t loop = 0; loop < pModel->GetAnimNum(); ++loop) {
					pModel->SetAnim(loop).SetPer(std::clamp(pModel->SetAnim(loop).GetPer() + ((Now.m_MV1AnimSelect == loop) ? DeltaTime : -DeltaTime) / 0.25f, 0.f, 1.f));
					pModel->SetAnim(loop).Update(false, 1.0f);
				}
				pModel->PhysicsCalculation(DeltaTime);
				pModel->FlipAnimAll();
			}
		}
	}

	void Dispose(void) noexcept {
		if (this->m_PrevBGMID != InvalidID) {
			Sound::SoundPool::Instance()->Get(Sound::SoundType::BGM, this->m_PrevBGMID)->StopAll();
		}
		if (this->m_NowBGMID != InvalidID) {
			Sound::SoundPool::Instance()->Get(Sound::SoundType::BGM, this->m_NowBGMID)->StopAll();
		}
	}

	void DrawBG(void) noexcept {
		//背景
		if (this->m_PrevBGImage) {
			this->m_PrevBGImage->Get()->DrawRotaGraph(1920 / 2, 1080 / 2, 1.f, 0.f, false);
		}
		if (this->m_NowBGImage) {
			DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(255.f * this->m_BGFade), 0, 255));
			this->m_NowBGImage->Get()->DrawRotaGraph(1920 / 2, 1080 / 2, 1.f, 0.f, false);
			DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		}
	}
	void Draw3D(void) noexcept {
		size_t LaskImage = 0;
		size_t ResetImage = 0;
		size_t LaskMV1 = 0;
		size_t ResetMV1 = 0;
		for (const auto& Now : this->m_SpeakData) {
			size_t index = static_cast<size_t>(&Now - &this->m_SpeakData.front());
			if (Now.m_SpeakEnum == SpeakEnum::Image) {
				LaskImage = index;
			}
			if (Now.m_SpeakEnum == SpeakEnum::ResetImage) {
				ResetImage = index;
			}
			if (Now.m_SpeakEnum == SpeakEnum::Model) {
				if (Now.m_MV1) {
					LaskMV1 = index;
				}
			}
			if (Now.m_SpeakEnum == SpeakEnum::ResetModel) {
				ResetMV1 = index;
			}
			if (index == this->m_NowPoint) {
				break;
			}
		}
		//イメージ
		for (const auto& Now : this->m_SpeakData) {
			size_t index = static_cast<size_t>(&Now - &this->m_SpeakData.front());
			if (Now.m_SpeakEnum == SpeakEnum::Image) {
				if (ResetImage <= index) {
					if (index == LaskImage) {
						Now.DrawImage(Now.m_Pos, 255);
					}
					else {
						if (index < LaskImage) {
							Now.DrawImage(Now.m_Pos, 128);
						}
					}
				}
			}
			if (Now.m_SpeakEnum == SpeakEnum::Model) {
				if (ResetMV1 <= index) {
					if (index == LaskMV1) {
						Now.DrawModel();
					}
					else {
						if (index < LaskMV1) {
							Now.DrawModel();
						}
					}
				}
			}
		}
	}
	void Draw(void) noexcept {
		size_t LaskSpeak = 0;
		for (const auto& Now : this->m_SpeakData) {
			size_t index = static_cast<size_t>(&Now - &this->m_SpeakData.front());
			if (Now.m_SpeakEnum == SpeakEnum::Speak) {
				LaskSpeak = index;
			}
			if (index == this->m_NowPoint) {
				break;
			}
		}
		//ブラックアウト
		{
			DxLib::SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp(static_cast<int>(255.f * this->m_Fade), 0, 255));
			DxLib::DrawBox(0, 0, 1920, 1080, ColorPalette::Black, true);
			DxLib::SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
		}
		//テキスト
		for (const auto& Now : this->m_SpeakData) {
			size_t index = static_cast<size_t>(&Now - &this->m_SpeakData.front());
			if (Now.m_SpeakEnum == SpeakEnum::Speak) {
				if (index == this->m_NowPoint) {
					Now.DrawStr(this->m_Seek, 255);
				}
				else {
					if (index == LaskSpeak) {
						Now.DrawStr(Now.m_SeekMax, 255);
					}
				}
			}
		}
	}
};


class MainScene : public Util::SceneBase {
	OptionWindow					m_OptionWindow;
	PauseUI							m_PauseUI;
	SaveUI							m_SaveUI;
	ContinueUI						m_ContinueUI;

	bool							m_Exit{ false };
	bool							m_IsSceneEnd{ false };
	bool							m_IsPauseActive{ false };
	bool							m_IsResetMouse{ false };
	bool							m_IsSeek{ true };
	char		padding[3]{};
	float							m_Fade{ 1.f };

	Sound::SoundUniqueID			m_OKID{ InvalidID };

	SpeakScript						m_SpeakScript{};

	int								m_TargetPoint{ 0 };
	int								m_NowPhase{ 0 };
	//char		padding[4]{};
public:
	MainScene(void) noexcept { SetID(static_cast<int>(EnumScene::Main)); }
	MainScene(const MainScene&) = delete;
	MainScene(MainScene&&) = delete;
	MainScene& operator=(const MainScene&) = delete;
	MainScene& operator=(MainScene&&) = delete;
	virtual ~MainScene(void) noexcept {}
private:
	void Load(int Slot) noexcept {
		std::string Path = "Save/Slot" + std::to_string(Slot) + ".dat";
		if (Util::IsFileExist(Path.c_str())) {
			File::InputFileStream Istream(Path);
			while (!Istream.ComeEof()) {
				std::string Line = File::InputFileStream::getleft(Istream.SeekLineAndGetStr(), "//");
				std::string Left = File::InputFileStream::getleft(Line, "=");
				std::string Right = File::InputFileStream::getright(Line, "=");
				if ("NowPhase" == Left) {
					m_NowPhase = std::stoi(Right);
				}
				else if ("NowPoint" == Left) {
					m_TargetPoint = std::stoi(Right);
				}
				else {
					break;
				}
			}
		}
	}
	void Save(int Slot) noexcept {
		std::string Path = "Save/Slot" + std::to_string(Slot) + ".dat";
		File::OutputFileStream Ostream(Path);
		{
			std::string Line = (std::string)("NowPhase") + "=" + std::to_string(m_NowPhase);
			Ostream.AddLine(Line);
		}
		{
			std::string Line = (std::string)("NowPoint") + "=" + std::to_string(this->m_SpeakScript.GetNowPoint());
			Ostream.AddLine(Line);
		}
	}
private:
	void Seek(void) noexcept {
		if (Param::CommonParam::Instance()->m_IsLoad == InvalidID) {
			m_NowPhase = 0;
			m_TargetPoint = 0;
		}
		else {
			Load(Param::CommonParam::Instance()->m_IsLoad);
		}
	}
	void Start(void) noexcept {
		this->m_SpeakScript.Start();
		//目標地点まで最速スキップ
		for (int loop = 0; loop < m_TargetPoint; ++loop) {
			this->m_SpeakScript.Step();
		}
		this->m_SpeakScript.SetStoryStart();
	}
protected:
	void Load_Sub(void) noexcept override;
	void Init_Sub(void) noexcept override;
	void Update_Sub(void) noexcept override;
	void BGDraw_Sub(void) noexcept override;
	void SetShadowDrawRigid_Sub(void) noexcept override {}
	void SetShadowDraw_Sub(void) noexcept override {}
	void Draw_Sub(void) noexcept override;
	void DepthDraw_Sub(void) noexcept override {}
	void ShadowDrawFar_Sub(void) noexcept override {}
	void ShadowDraw_Sub(void) noexcept override {}
	void UIDraw_Sub(void) noexcept override;
	void Dispose_Sub(void) noexcept override;
};
