#pragma once

static constexpr int		InvalidID{ -1 };								//共通の無効値
static constexpr float		FrameRate{ 60.f };								//フレームレート
static constexpr float		DeltaTime{ 1.f / FrameRate };					//1F単位の経過時間
static constexpr int		DispWidth{ 1280 };								//キャンバスのXサイズ
static constexpr int		DispHeight{ 720 };								//キャンバスのYサイズ
