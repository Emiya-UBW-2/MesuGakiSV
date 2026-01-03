#include "TitleScene.hpp"

CreditControl::CreditControl(void) noexcept {
	File::InputFileStream FileStream("data/Credit.txt");
	this->m_CreditCoulm = 0;
	while (true) {
		if (FileStream.ComeEof()) { break; }
		auto ALL = FileStream.SeekLineAndGetStr();
		if (ALL.find('=') != std::string::npos) {
			auto LEFT = File::InputFileStream::getleft(ALL, "=");
			auto RIGHT = File::InputFileStream::getright(ALL, "=");
			sprintfDx(this->m_CreditStr[static_cast<size_t>(this->m_CreditCoulm)].first, LEFT.c_str());
			sprintfDx(this->m_CreditStr[static_cast<size_t>(this->m_CreditCoulm)].second, RIGHT.c_str());
		}
		else {
			sprintfDx(this->m_CreditStr[static_cast<size_t>(this->m_CreditCoulm)].first, ALL.c_str());
			sprintfDx(this->m_CreditStr[static_cast<size_t>(this->m_CreditCoulm)].second, "");
		}
		++this->m_CreditCoulm;
	}
}
CreditControl::~CreditControl(void) noexcept {
	this->m_CreditCoulm = 0;
	for (auto& credit : this->m_CreditStr) {
		sprintfDx(credit.first, "");
		sprintfDx(credit.second, "");
	}
}

void CreditControl::Draw(int xmin, int ymin, int xmax) const noexcept {
	auto* Font = Draw::FontPool::Instance();
	int xp1, yp1;

	xp1 = xmin + (24);
	yp1 = ymin + LineHeight;
	int Height = LineHeight;
	for (auto& credit : this->m_CreditStr) {
		if (this->m_CreditCoulm < static_cast<int>(&credit - &this->m_CreditStr.front())) { break; }
		int xpos = xp1 + (6);
		int ypos = yp1 + Height / 2;

		Font->Get(Draw::FontType::DIZ_UD_Gothic, Height, 3)->DrawString(
			Draw::FontXCenter::LEFT, Draw::FontYCenter::MIDDLE,
			xpos, ypos,
			ColorPalette::White, ColorPalette::Black,
			//Util::SjistoUTF8(
				credit.first
			//)
		);

		xpos = xmax - (24);

		Font->Get(Draw::FontType::DIZ_UD_Gothic, Height, 3)->DrawString(
			Draw::FontXCenter::RIGHT, Draw::FontYCenter::MIDDLE,
			xpos, ypos,
			ColorPalette::White, ColorPalette::Black,
			//Util::SjistoUTF8(
				credit.second
			//)
		);
		yp1 += Height;
	}
}
