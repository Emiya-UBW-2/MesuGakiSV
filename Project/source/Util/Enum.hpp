#pragma once

static constexpr float		Scale3DRate{ 12.5f };							/*1mに相当する3D空間上の長さ*/
static constexpr int		InvalidID{ -1 };								/*共通の無効値*/

enum class EnumScene :size_t {
	Title,
	Main,
};

enum class LanguageType :int {
	Jpn,
	Eng,
};
static const char* LanguageStr[] = {
"Jpn",
"Eng",
};
