#pragma once
#define NOMINMAX
#pragma warning( push, 3 )
#include "DxLib.h"
#include <string>
#include <vector>
#include <stdexcept>
#include <memory>
#pragma warning( pop )

#pragma warning(disable:4464)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:5039)
#include "../Util/Util.hpp"

// フォントハンドル
class GraphHandle : public DXHandle {
public:
	GraphHandle(void) noexcept {}
	GraphHandle(const GraphHandle&o) = delete;
	GraphHandle(GraphHandle&& o) noexcept {
		DXHandle::SetHandleDirect(o.get());
		o.SetHandleDirect(-1);
	}
	GraphHandle& operator=(const GraphHandle& o) = delete;
	GraphHandle& operator=(GraphHandle&& o) noexcept {
		DXHandle::SetHandleDirect(o.get());
		o.SetHandleDirect(-1);
		return *this;
	}
	virtual ~GraphHandle(void) noexcept {}
protected:
	void	Dispose_Sub(void) noexcept override {
		DeleteGraph(DXHandle::get());
	}
public:
	void Load_Tex(std::basic_string_view<TCHAR> FileName) noexcept {
		DXHandle::SetHandleDirect(DxLib::MV1LoadTextureWithStrLen(FileName.data(), FileName.length()));
	}
	void Load(std::basic_string_view<TCHAR> FileName, bool NotUse3DFlag = false) noexcept {
		DXHandle::SetHandleDirect(DxLib::LoadGraphWithStrLen(FileName.data(), FileName.length(), NotUse3DFlag));
	}
	void LoadDiv(std::basic_string_view<TCHAR> FileName, int AllNum, int XNum, int YNum, int XSize, int YSize, int* HandleArray, bool NotUse3DFlag = false) noexcept {
		DXHandle::SetHandleDirect(DxLib::LoadDivGraphWithStrLen(FileName.data(), FileName.length(), AllNum, XNum, YNum, XSize, YSize, HandleArray, NotUse3DFlag));
	}
	void DerivationGraph(int x, int y, int xsize, int ysize, const GraphHandle& baseImage) noexcept {
		DXHandle::SetHandleDirect(DxLib::DerivationGraph(x, y, xsize, ysize, baseImage.get()));
	}
	void Make(int SizeX, int SizeY, bool trns = false) noexcept {
		DXHandle::SetHandleDirect(DxLib::MakeScreen(SizeX, SizeY, (trns ? TRUE : FALSE)));
	}
	void MakeDepth(int SizeX, int SizeY) noexcept {
		// 深度を描画するテクスチャの作成( 2チャンネル浮動小数点32ビットテクスチャ )
		auto prevMip = GetCreateDrawValidGraphChannelNum();
		auto prevFloatType = GetDrawValidFloatTypeGraphCreateFlag();
		auto prevBit = GetCreateGraphChannelBitDepth();
		SetCreateDrawValidGraphChannelNum(2);
		SetDrawValidFloatTypeGraphCreateFlag(TRUE);
		SetCreateGraphChannelBitDepth(32);
		DXHandle::SetHandleDirect(DxLib::MakeScreen(SizeX, SizeY, FALSE));
		SetCreateDrawValidGraphChannelNum(prevMip);
		SetDrawValidFloatTypeGraphCreateFlag(prevFloatType);
		SetCreateGraphChannelBitDepth(prevBit);
	}
	void CreateGraphFromBmp(const BITMAPINFO* RGBBmpInfo, const void* RGBBmpImage, const BITMAPINFO* AlphaBmpInfo = nullptr, const void* AlphaBmpImage = nullptr, bool TextureFlag = true, bool ReverseFlag = false) noexcept {
		DXHandle::SetHandleDirect(DxLib::CreateGraphFromBmp(RGBBmpInfo, RGBBmpImage, AlphaBmpInfo, AlphaBmpImage, TextureFlag ? TRUE : FALSE, ReverseFlag ? TRUE : FALSE));
	}
public:
	void DrawGraph(int posx, int posy, bool trns) const noexcept {
		if (!DXHandle::IsActive()) { return; }
		DxLib::DrawGraph(posx, posy, DXHandle::get(), (trns ? TRUE : FALSE));
	}

	void DrawRotaGraph(int posx, int posy, float Exrate, float rad, bool trns) const noexcept {
		if (!DXHandle::IsActive()) { return; }
		DxLib::DrawRotaGraph(posx, posy, static_cast<double>(Exrate), static_cast<double>(rad), DXHandle::get(), (trns ? TRUE : FALSE));
	}

	void DrawRotaGraphF(float posx, float posy, float Exrate, float rad, bool trns) const noexcept {
		if (!DXHandle::IsActive()) { return; }
		DxLib::DrawRotaGraphF(posx, posy, static_cast<double>(Exrate), static_cast<double>(rad), DXHandle::get(), (trns ? TRUE : FALSE));
	}

	void DrawRotaGraph3(int posx, int posy,
		int cx, int cy,
		float ExtRateX, float ExtRateY, float rad, bool trns) const noexcept {
		if (!DXHandle::IsActive()) { return; }
		DxLib::DrawRotaGraph3(posx, posy, cx, cy, static_cast<double>(ExtRateX), static_cast<double>(ExtRateY), static_cast<double>(rad), DXHandle::get(), (trns ? TRUE : FALSE));
	}

	void DrawExtendGraph(int posx1, int posy1, int posx2, int posy2, bool trns) const noexcept {
		if (!DXHandle::IsActive()) { return; }
		DxLib::DrawExtendGraph(posx1, posy1, posx2, posy2, DXHandle::get(), (trns ? TRUE : FALSE));
	}
	void FillGraph(int r, int g, int b) const noexcept {
		if (!DXHandle::IsActive()) { return; }
		DxLib::FillGraph(DXHandle::get(), r, g, b);
	}

	template <typename... Args>
	void GraphFilter(int FilterType /* DX_GRAPH_FILTER_GAUSS 等 */, Args&&... args) const noexcept {
		DxLib::GraphFilter(DXHandle::get(), FilterType, args...);
	}

	template <typename... Args>
	void GraphFilterBlt(const GraphHandle& targetImage, int FilterType /* DX_GRAPH_FILTER_GAUSS 等 */, Args&&... args) const noexcept {
		DxLib::GraphFilterBlt(targetImage.get(), DXHandle::get(), FilterType, args...);
	}

	template <typename... Args>
	void GraphBlend(const GraphHandle& BlendImage, int BlendRatio /* ブレンド効果の影響度( 0:０％  255:１００％ ) */, int BlendType /* DX_GRAPH_BLEND_ADD 等 */, Args&&... args) const noexcept {
		DxLib::GraphBlend(DXHandle::get(), BlendImage.get(), BlendRatio, BlendType, args...);
	}

	template <typename... Args>
	void GraphBlendBlt(const GraphHandle& BaseImage, const GraphHandle& BlendImage, int BlendRatio /* ブレンド効果の影響度( 0:０％  255:１００％ ) */, int BlendType /* DX_GRAPH_BLEND_ADD 等 */, Args&&... args) const noexcept {
		DxLib::GraphBlendBlt(BaseImage.get(), BlendImage.get(), DXHandle::get(), BlendRatio, BlendType, args...);
	}


	// GetGraphSize
	void GetSize(int* xsize, int* ysize) const noexcept {
		if (!DXHandle::IsActive()) { return; }
		GetGraphSize(DXHandle::get(), xsize, ysize);
	}
	// 
	void SetDraw_Screen(bool Clear = true) const noexcept {
		SetDrawScreen(DXHandle::get());
		if (Clear) {
			ClearDrawScreen();
		}
	}
	// 
	void SetUseTextureToShader(int ID) const noexcept {
		DxLib::SetUseTextureToShader(ID, DXHandle::get());
	}
	void SetRenderTargetToShader(int ID, int SurfaceIndex = 0, int MipLevel = 0) const noexcept {
		DxLib::SetRenderTargetToShader(ID, DXHandle::get(), SurfaceIndex, MipLevel);
	}
public:
	// 
	static void SetDraw_Screen(int handle, bool Clear = true) noexcept {
		SetDrawScreen(handle);
		if (Clear) {
			ClearDrawScreen();
		}
	}
	// 
	/*
	static void SetUseTextureToShader(int ID, int handle) noexcept {
		DxLib::SetUseTextureToShader(ID, handle);
	}
	//
	static void SetRenderTargetToShader(int ID, int handle, int SurfaceIndex = 0, int MipLevel = 0) noexcept {
		DxLib::SetRenderTargetToShader(ID, handle, SurfaceIndex, MipLevel);
	}
	//*/

	static void LoadDiv(std::basic_string_view<TCHAR> FileName, int AllNum, int XNum, int YNum, int  XSize, int  YSize, std::vector<GraphHandle>* Handles, bool NotUse3DFlag = false) noexcept {
		int* HandleArray = new int[static_cast<size_t>(AllNum)];
		DxLib::LoadDivGraphWithStrLen(FileName.data(), FileName.length(), AllNum, XNum, YNum, XSize, YSize, HandleArray, NotUse3DFlag);

		Handles->clear();
		for (size_t loop = 0; loop < static_cast<size_t>(AllNum); ++loop) {
			Handles->emplace_back();
			Handles->back().SetHandleDirect(HandleArray[loop]);
		}
		delete[] HandleArray;

		return;
	}
};
//
class Graphhave {
	// カスタム項目
	std::string		m_FilePath;
	GraphHandle		m_Handle;
public:
	Graphhave(std::string FilePath) noexcept;
	Graphhave(const Graphhave&) = delete;
	Graphhave(Graphhave&&) = delete;
	Graphhave& operator=(const Graphhave&) = delete;
	Graphhave& operator=(Graphhave&&) = delete;

	~Graphhave(void) noexcept {
		this->m_Handle.Dispose();
	}
public:
	bool			Equal(std::string FilePath) const noexcept {
		return (this->m_FilePath == FilePath);
	}
public:
	const GraphHandle* Get(void) const noexcept { return &m_Handle; }
};
// フォントプール
class GraphPool : public SingletonBase<GraphPool> {
private:
	friend class SingletonBase<GraphPool>;
public:
private:
	std::vector<std::unique_ptr<Graphhave>>	m_Pools;
private:
	GraphPool(void) noexcept {}
	GraphPool(const GraphPool&) = delete;
	GraphPool(GraphPool&&) = delete;
	GraphPool& operator=(const GraphPool&) = delete;
	GraphPool& operator=(GraphPool&&) = delete;
public:
	std::unique_ptr<Graphhave>& Get(std::string FilePath) noexcept {
		auto Find = std::find_if(this->m_Pools.begin(), this->m_Pools.end(), [&](const std::unique_ptr<Graphhave>& tgt) {return tgt->Equal(FilePath); });
		if (Find != this->m_Pools.end()) {
			return *Find;
		}
		this->m_Pools.emplace_back(std::make_unique<Graphhave>(FilePath));
		return this->m_Pools.back();
	}
};
