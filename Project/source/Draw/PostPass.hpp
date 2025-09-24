#pragma once

#pragma warning(disable:4464)
#pragma warning(disable:4505)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:5039)
#pragma warning(disable:5045)
#define NOMINMAX
#pragma warning( push, 3 )
#include "DxLib.h"
#include <array>
#include <functional>
#include <memory>
#pragma warning( pop )
#include "ImageDraw.hpp"

#include "../Util/Util.hpp"
#include "../Util/Algorithm.hpp"
#include "../Util/Option.hpp"

#include "../Draw/MainDraw.hpp"


namespace DXLibRef {
	constexpr float		Scale3DRate{ 12.5f };							/*1mに相当する3D空間上の長さ*/
	constexpr int		InvalidID{ -1 };								/*共通の無効値*/

	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	/*シェーダー																																*/
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// シェーダーを使用する際の補助クラス
	class ShaderController {
	public:
		// 2Dにシェーダーを適用する際に使用する画面サイズの頂点情報
		class ScreenVertex {
			VERTEX3DSHADER Screen_vertex[6] = {};
		public:
			// 頂点データの取得
			const auto* GetScreenVertex(void) const noexcept { return Screen_vertex; }
			// 頂点データの準備
			void			SetScreenVertex(int dispx, int dispy) noexcept {
				int xp1 = 0;
				int yp1 = dispy;
				int xp2 = dispx;
				int yp2 = 0;
				Screen_vertex[0].pos = VGet(static_cast<float>(xp1), static_cast<float>(yp1), 0.0f);
				Screen_vertex[1].pos = VGet(static_cast<float>(xp2), static_cast<float>(yp1), 0.0f);
				Screen_vertex[2].pos = VGet(static_cast<float>(xp1), static_cast<float>(yp2), 0.0f);
				Screen_vertex[3].pos = VGet(static_cast<float>(xp2), static_cast<float>(yp2), 0.0f);
				Screen_vertex[0].dif = GetColorU8(255, 255, 255, 255);
				Screen_vertex[1].dif = GetColorU8(255, 255, 255, 255);
				Screen_vertex[2].dif = GetColorU8(255, 255, 255, 255);
				Screen_vertex[3].dif = GetColorU8(255, 255, 255, 255);
				Screen_vertex[0].u = 0.0f; Screen_vertex[0].v = 0.0f;
				Screen_vertex[1].u = 1.0f; Screen_vertex[1].v = 0.0f;
				Screen_vertex[2].u = 0.0f; Screen_vertex[3].v = 1.0f;
				Screen_vertex[3].u = 1.0f; Screen_vertex[2].v = 1.0f;
				Screen_vertex[4] = Screen_vertex[2];
				Screen_vertex[5] = Screen_vertex[1];
			}
		};
		// 影用の深度記録画像を作成した際のカメラのビュー行列と射影行列
		struct LIGHTCAMERA_MATRIX {
			MATRIX ViewMatrix;
			MATRIX ProjectionMatrix;
		};
		// DXLIBから引っ張ってきたシェーダー用の定義
		typedef float DX_D3D11_SHADER_FLOAT2[2];
		typedef float DX_D3D11_SHADER_FLOAT4[4];
		//
		struct DX_D3D11_GS_CONST_BUFFER_BASE {
			DX_D3D11_SHADER_FLOAT4		ProjectionMatrix[4]{};											// ビュー　→　プロジェクション行列
			DX_D3D11_SHADER_FLOAT4		ViewMatrix[3]{};												// ワールド　→　ビュー行列
		};
	private:
		// シェーダーハンドル
		int					m_VertexShaderhandle{ InvalidID };
		int					m_GeometryShaderhandle{ InvalidID };
		int					m_PixelShaderhandle{ InvalidID };
		// シェーダーに渡す追加パラメーターを配するハンドル
		std::array<int, 3>	LightCameraMatrixConstantBufferHandle{};	// 影用の深度記録画像を作成した際のカメラのビュー行列と射影行列を設定するための定数バッファ
		std::array<int, 4>	m_VertexShadercbhandle{};
		int					m_GeometryShaderMatcbhandle{ InvalidID };
		int					m_PixelShaderSendDispSizeHandle{ InvalidID };
		std::array<int, 3>	m_PixelShadercbhandle{};
		ScreenVertex	m_ScreenVertex;					// 頂点データ
	public:
		ShaderController(void) noexcept {
			// シェーダーハンドル
			this->m_VertexShaderhandle = InvalidID;
			this->m_GeometryShaderhandle = InvalidID;
			this->m_PixelShaderhandle = InvalidID;
			// シェーダーに渡す追加パラメーターを配するハンドル
			for (auto& h : this->m_VertexShadercbhandle) {
				h = InvalidID;
			}
			this->m_GeometryShaderMatcbhandle = InvalidID;
			this->m_PixelShaderSendDispSizeHandle = InvalidID;
			for (auto& h : this->m_PixelShadercbhandle) {
				h = InvalidID;
			}
		}
		~ShaderController(void) noexcept {
			Dispose();
		}
	public:
		// 初期化
		void			Init(const char* VertexShader, const char* PixelShader) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			// 頂点シェーダー周り
			for (auto& h : this->m_VertexShadercbhandle) {
				h = CreateShaderConstantBuffer(sizeof(float) * 4);
			}
			this->m_VertexShaderhandle = LoadVertexShader(VertexShader);
			// ピクセルシェーダ―周り
			this->m_PixelShaderSendDispSizeHandle = CreateShaderConstantBuffer(sizeof(float) * 4);
			for (auto& h : this->m_PixelShadercbhandle) {
				h = CreateShaderConstantBuffer(sizeof(float) * 4);
			}
			this->m_PixelShaderhandle = LoadPixelShader(PixelShader);
			// 影用の深度記録画像を作成した際のカメラのビュー行列と射影行列を設定するための定数バッファの作成
			for (auto& h : LightCameraMatrixConstantBufferHandle) {
				h = CreateShaderConstantBuffer(sizeof(LIGHTCAMERA_MATRIX));
			}
		}
		void			AddGeometryShader(const char* GeometryShader) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			this->m_GeometryShaderMatcbhandle = CreateShaderConstantBuffer(sizeof(DX_D3D11_GS_CONST_BUFFER_BASE));
			this->m_GeometryShaderhandle = LoadGeometryShader(GeometryShader);
		}
		// 後始末
		void			Dispose(void) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			// 頂点シェーダー周り
			for (auto& h : LightCameraMatrixConstantBufferHandle) {
				DeleteShaderConstantBuffer(h);
			}
			for (auto& h : this->m_VertexShadercbhandle) {
				DeleteShaderConstantBuffer(h);
			}
			DeleteShader(this->m_VertexShaderhandle);
			// 
			DeleteShaderConstantBuffer(this->m_GeometryShaderMatcbhandle);
			DeleteShader(this->m_GeometryShaderhandle);
			// ピクセルシェーダ―周り
			DeleteShaderConstantBuffer(this->m_PixelShaderSendDispSizeHandle);
			for (auto& h : this->m_PixelShadercbhandle) {
				DeleteShaderConstantBuffer(h);
			}
			DeleteShader(this->m_PixelShaderhandle);
		}
	public:
		// 頂点シェーダ―のSlot番目のレジスタに情報をセット(Slot>=4)
		void			SetVertexCameraMatrix(int Slot, const Util::Matrix4x4DX& View, const Util::Matrix4x4DX& Projection) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			auto& BufferHandle = LightCameraMatrixConstantBufferHandle[static_cast<size_t>(Slot - 4)];
			// 設定したカメラのビュー行列と射影行列を取得しておく
			LIGHTCAMERA_MATRIX* LightCameraMatrixConst = (LIGHTCAMERA_MATRIX*)GetBufferShaderConstantBuffer(BufferHandle);
			LightCameraMatrixConst->ViewMatrix = View.get();
			LightCameraMatrixConst->ProjectionMatrix = Projection.get();

			UpdateShaderConstantBuffer(BufferHandle);
			SetShaderConstantBuffer(BufferHandle, DX_SHADERTYPE_VERTEX, Slot);		// 影用深度記録画像を描画したときのカメラのビュー行列と射影行列を定数に設定する
		}
		// 頂点シェーダ―のSlot番目のレジスタに情報をセット(Slot>=4)
		void			SetVertexParam(int Slot, float param1, float param2, float param3, float param4) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			auto& BufferHandle = this->m_VertexShadercbhandle[static_cast<size_t>(Slot - 4)];
			FLOAT4* f4 = (FLOAT4*)GetBufferShaderConstantBuffer(BufferHandle);		// 頂点シェーダー用の定数バッファのアドレスを取得
			f4->x = param1;
			f4->y = param2;
			f4->z = param3;
			f4->w = param4;
			UpdateShaderConstantBuffer(BufferHandle);								// 頂点シェーダー用の定数バッファを更新して書き込んだ内容を反映する
			SetShaderConstantBuffer(BufferHandle, DX_SHADERTYPE_VERTEX, Slot);		// 頂点シェーダーの定数バッファを定数バッファレジスタ４にセット
		}
		// シェーダ―のSlot番目のレジスタに情報をセット(Slot>=4)
		void			SetGeometryCONSTBUFFER(int Slot, const MATRIX* ViewMatrix, const MATRIX* ProjectionMatrix) const noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			if (this->m_GeometryShaderhandle == InvalidID) { return; }
			auto& BufferHandle = this->m_GeometryShaderMatcbhandle;
			DX_D3D11_GS_CONST_BUFFER_BASE* LightCameraMatrixConst = (DX_D3D11_GS_CONST_BUFFER_BASE*)GetBufferShaderConstantBuffer(BufferHandle);

			// ビュー変換用行列をセットする
			LightCameraMatrixConst->ViewMatrix[0][0] = ViewMatrix->m[0][0];
			LightCameraMatrixConst->ViewMatrix[0][1] = ViewMatrix->m[1][0];
			LightCameraMatrixConst->ViewMatrix[0][2] = ViewMatrix->m[2][0];
			LightCameraMatrixConst->ViewMatrix[0][3] = ViewMatrix->m[3][0];
			LightCameraMatrixConst->ViewMatrix[1][0] = ViewMatrix->m[0][1];
			LightCameraMatrixConst->ViewMatrix[1][1] = ViewMatrix->m[1][1];
			LightCameraMatrixConst->ViewMatrix[1][2] = ViewMatrix->m[2][1];
			LightCameraMatrixConst->ViewMatrix[1][3] = ViewMatrix->m[3][1];
			LightCameraMatrixConst->ViewMatrix[2][0] = ViewMatrix->m[0][2];
			LightCameraMatrixConst->ViewMatrix[2][1] = ViewMatrix->m[1][2];
			LightCameraMatrixConst->ViewMatrix[2][2] = ViewMatrix->m[2][2];
			LightCameraMatrixConst->ViewMatrix[2][3] = ViewMatrix->m[3][2];
			// 投影変換用行列をセットする
			LightCameraMatrixConst->ProjectionMatrix[0][0] = ProjectionMatrix->m[0][0];
			LightCameraMatrixConst->ProjectionMatrix[0][1] = ProjectionMatrix->m[1][0];
			LightCameraMatrixConst->ProjectionMatrix[0][2] = ProjectionMatrix->m[2][0];
			LightCameraMatrixConst->ProjectionMatrix[0][3] = ProjectionMatrix->m[3][0];
			LightCameraMatrixConst->ProjectionMatrix[1][0] = ProjectionMatrix->m[0][1];
			LightCameraMatrixConst->ProjectionMatrix[1][1] = ProjectionMatrix->m[1][1];
			LightCameraMatrixConst->ProjectionMatrix[1][2] = ProjectionMatrix->m[2][1];
			LightCameraMatrixConst->ProjectionMatrix[1][3] = ProjectionMatrix->m[3][1];
			LightCameraMatrixConst->ProjectionMatrix[2][0] = ProjectionMatrix->m[0][2];
			LightCameraMatrixConst->ProjectionMatrix[2][1] = ProjectionMatrix->m[1][2];
			LightCameraMatrixConst->ProjectionMatrix[2][2] = ProjectionMatrix->m[2][2];
			LightCameraMatrixConst->ProjectionMatrix[2][3] = ProjectionMatrix->m[3][2];
			LightCameraMatrixConst->ProjectionMatrix[3][0] = ProjectionMatrix->m[0][3];
			LightCameraMatrixConst->ProjectionMatrix[3][1] = ProjectionMatrix->m[1][3];
			LightCameraMatrixConst->ProjectionMatrix[3][2] = ProjectionMatrix->m[2][3];
			LightCameraMatrixConst->ProjectionMatrix[3][3] = ProjectionMatrix->m[3][3];

			UpdateShaderConstantBuffer(BufferHandle);
			SetShaderConstantBuffer(BufferHandle, DX_SHADERTYPE_GEOMETRY, Slot);		// 影用深度記録画像を描画したときのカメラのビュー行列と射影行列を定数に設定する
		}
		// ピクセルシェーダ―のSlot番目のレジスタに情報をセット(Slot>=4)
		void			SetPixelCameraMatrix(int Slot, const Util::Matrix4x4DX& View, const Util::Matrix4x4DX& Projection) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			auto& BufferHandle = LightCameraMatrixConstantBufferHandle[static_cast<size_t>(Slot - 4)];
			// 設定したカメラのビュー行列と射影行列を取得しておく
			LIGHTCAMERA_MATRIX* LightCameraMatrixConst = (LIGHTCAMERA_MATRIX*)GetBufferShaderConstantBuffer(BufferHandle);
			LightCameraMatrixConst->ViewMatrix = View.get();
			LightCameraMatrixConst->ProjectionMatrix = Projection.get();

			UpdateShaderConstantBuffer(BufferHandle);
			SetShaderConstantBuffer(BufferHandle, DX_SHADERTYPE_PIXEL, Slot);		// 影用深度記録画像を描画したときのカメラのビュー行列と射影行列を定数に設定する
		}
		// ピクセルシェーダ―の2番目のレジスタに画面サイズの情報をセット
		void			SetPixelDispSize(int dispx, int dispy) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			auto& BufferHandle = this->m_PixelShaderSendDispSizeHandle;
			FLOAT2* dispsize = (FLOAT2*)GetBufferShaderConstantBuffer(BufferHandle);	// ピクセルシェーダー用の定数バッファのアドレスを取得
			dispsize->u = static_cast<float>(dispx);
			dispsize->v = static_cast<float>(dispy);
			UpdateShaderConstantBuffer(BufferHandle);									// ピクセルシェーダー用の定数バッファを更新して書き込んだ内容を反映する
			SetShaderConstantBuffer(BufferHandle, DX_SHADERTYPE_PIXEL, 2);				// ピクセルシェーダー用の定数バッファを定数バッファレジスタ2にセット
			this->m_ScreenVertex.SetScreenVertex(dispx, dispy);
		}
		// ピクセルシェーダ―のSlot番目のレジスタに情報をセット(Slot>=3)
		void			SetPixelParam(int Slot, float param1, float param2, float param3, float param4) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			auto& BufferHandle = this->m_PixelShadercbhandle[static_cast<size_t>(Slot - 3)];
			FLOAT4* f4 = (FLOAT4*)GetBufferShaderConstantBuffer(BufferHandle);				// ピクセルシェーダー用の定数バッファのアドレスを取得
			f4->x = param1;
			f4->y = param2;
			f4->z = param3;
			f4->w = param4;
			UpdateShaderConstantBuffer(BufferHandle);											// ピクセルシェーダー用の定数バッファを更新して書き込んだ内容を反映する
			SetShaderConstantBuffer(BufferHandle, DX_SHADERTYPE_PIXEL, Slot);					// ピクセルシェーダー用の定数バッファを定数バッファレジスタ3にセット
		}
		// 3D空間に適用する場合の関数(引数に3D描画のラムダ式を代入)
		void			Draw_lamda(std::function<void()> doing) const noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) {
				doing();
				return;
			}
			SetUseVertexShader(this->m_VertexShaderhandle);											// 使用する頂点シェーダーをセット
			SetUsePixelShader(this->m_PixelShaderhandle);											// 使用するピクセルシェーダーをセット
			SetUseGeometryShader(this->m_GeometryShaderhandle);										// 使用するジオメトリシェーダーをセット
			MV1SetUseOrigShader(TRUE);
			doing();
			MV1SetUseOrigShader(FALSE);
			SetUseVertexShader(InvalidID);
			SetUsePixelShader(InvalidID);
			SetUseGeometryShader(InvalidID);
		}
		// 2D画像に適用する場合の関数
		void			Draw(void) const noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			Draw_lamda([this] {DrawPolygon3DToShader(this->m_ScreenVertex.GetScreenVertex(), 2); });
		}
		// 2D画像に適用する場合の関数
		void			Draw(const ScreenVertex& Screenvertex) const noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			Draw_lamda([&] {DrawPolygon3DToShader(Screenvertex.GetScreenVertex(), 2); });
		}
	};
	// キューブマップ生成
	class RealTimeCubeMap {
	private:
		Draw::GraphHandle dynamicCubeTex;		// 周囲を回る小さいモデルたちを映りこませるための描画対象にできるキューブマップテクスチャ
		Util::Vector3DX lookAt[6]{};	// 映りこむ周囲の環境を描画する際のカメラの注視点
		Util::Vector3DX up[6]{};		// 移りこむ周囲の環境を描画する際のカメラの上方向
		int MIPLEVEL = 2;
		char		padding[4]{};
	public:
		RealTimeCubeMap(void) noexcept {}
		RealTimeCubeMap(const RealTimeCubeMap&) = delete;
		RealTimeCubeMap(RealTimeCubeMap&&) = delete;
		RealTimeCubeMap& operator=(const RealTimeCubeMap&) = delete;
		RealTimeCubeMap& operator=(RealTimeCubeMap&&) = delete;

		~RealTimeCubeMap(void) noexcept {}
	public:
		void Init(void) noexcept {
			// 描画対象にできるキューブマップテクスチャを作成
			SetCreateDrawValidGraphMipLevels(MIPLEVEL);
			SetCubeMapTextureCreateFlag(TRUE);
			dynamicCubeTex.Make(512, 512, true);
			SetCubeMapTextureCreateFlag(FALSE);
			SetCreateDrawValidGraphMipLevels(0);
			// 映りこむ環境を描画する際に使用するカメラの注視点とカメラの上方向を設定
			lookAt[0] = Util::Vector3DX::right();
			lookAt[1] = Util::Vector3DX::left();
			lookAt[2] = Util::Vector3DX::up();
			lookAt[3] = Util::Vector3DX::down();
			lookAt[4] = Util::Vector3DX::forward();
			lookAt[5] = Util::Vector3DX::back();
			up[0] = Util::Vector3DX::up();
			up[1] = Util::Vector3DX::up();
			up[2] = Util::Vector3DX::back();
			up[3] = Util::Vector3DX::forward();
			up[4] = Util::Vector3DX::up();
			up[5] = Util::Vector3DX::up();
		}

		void ReadyDraw(const Util::Vector3DX& Pos, const std::function<void()>& Doing) noexcept {
			for (int loop = 0; loop < 6; ++loop) {		// 映りこむ環境を描画する面の数だけ繰り返し
				for (int loop2 = 0; loop2 < MIPLEVEL; ++loop2) {			// ミップマップの数だけ繰り返し
					dynamicCubeTex.SetRenderTargetToShader(0, loop, loop2);		// 描画先番号０番の描画対象を描画対象にできるキューブマップのloop番目の面に設定
					ClearDrawScreen();										// クリア
					{
						SetupCamera_Perspective(90.0f / 180.0f * DX_PI_F);								// カメラの画角は90度に設定
						SetCameraNearFar(0.5f * Scale3DRate, 1000.0f * Scale3DRate);									// Nearクリップ面とFarクリップ面の距離を設定
						SetCameraPositionAndTargetAndUpVec(Pos.get(), (Pos + lookAt[static_cast<size_t>(loop)]).get(), up[static_cast<size_t>(loop)].get());	// カメラの位置と注視点、カメラの上方向を設定
						Doing();
					}
				}
			}
		}

		void Dispose(void) noexcept {
			dynamicCubeTex.Dispose();
		}

		const auto&		GetCubeMapTex(void) const noexcept { return dynamicCubeTex; }
	};
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	/*スクリーンバッファのつかいまわし																											*/
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	class PostPassScreenBuffer {
		static const int			m_Size = 3;
	public:
		int xSize{};
		int ySize{};
		bool isTrans{};
		bool m_isDepth{};
		char		padding[2]{};
		int m_ZBufferBitDepth{};
		std::array<Draw::GraphHandle, m_Size>	m_Screen{};
		int m_UsedLocal = 0;
		int m_Used = 0;
		int m_UnUse = 0;
		char		padding2[4]{};
	public:
		PostPassScreenBuffer(int xsize, int ysize, bool trans, bool isDepth = false, int ZBufferBitDepth = InvalidID) noexcept {
			this->xSize = xsize;
			this->ySize = ysize;
			this->isTrans = trans;
			this->m_isDepth = isDepth;
			this->m_ZBufferBitDepth = ZBufferBitDepth;
			int PrevZBufferBitDepth{};
			if (this->m_ZBufferBitDepth != InvalidID) {
				PrevZBufferBitDepth = GetCreateDrawValidGraphZBufferBitDepth();
				SetCreateDrawValidGraphZBufferBitDepth(this->m_ZBufferBitDepth);
			}
			for (auto& s : this->m_Screen) {
				if (this->m_isDepth) {
					s.MakeDepth(this->xSize, this->ySize);
				}
				else {
					s.Make(this->xSize, this->ySize, this->isTrans);
				}
			}
			if (this->m_ZBufferBitDepth != InvalidID) {
				SetCreateDrawValidGraphZBufferBitDepth(PrevZBufferBitDepth);
			}
			this->m_Used = 0;
			this->m_UsedLocal = 0;
			this->m_UnUse = 0;
		}
	public:
		PostPassScreenBuffer(const PostPassScreenBuffer& o) = delete;
		PostPassScreenBuffer(PostPassScreenBuffer&& o) = delete;
		PostPassScreenBuffer& operator=(const PostPassScreenBuffer& o) = delete;
		PostPassScreenBuffer& operator=(PostPassScreenBuffer&& o) = delete;
		~PostPassScreenBuffer(void) noexcept {
			for (auto& s : this->m_Screen) {
				s.Dispose();
			}
		}
	public:
		const Draw::GraphHandle* PopBlankScreen(void) noexcept {
			if (this->m_UsedLocal >= this->m_Size) {
				MessageBox(NULL, "None Blank PostPassScreenBuffer", "", MB_OK);
				exit(InvalidID);
			}
			auto* Ret = &this->m_Screen[static_cast<size_t>(this->m_UsedLocal)];
			++this->m_Used;
			++this->m_UsedLocal;
			return Ret;
		}
	};
	class PostPassScreenBufferPool : public Util::SingletonBase<PostPassScreenBufferPool> {
	private:
		friend class Util::SingletonBase<PostPassScreenBufferPool>;
	private:
		std::vector<std::unique_ptr<PostPassScreenBuffer>>	m_ScreenBuffer;		// 描画スクリーン
	private:
		PostPassScreenBufferPool(void) noexcept {}
		PostPassScreenBufferPool(const PostPassScreenBufferPool&) = delete;
		PostPassScreenBufferPool(PostPassScreenBufferPool&&) = delete;
		PostPassScreenBufferPool& operator=(const PostPassScreenBufferPool&) = delete;
		PostPassScreenBufferPool& operator=(PostPassScreenBufferPool&&) = delete;
		virtual ~PostPassScreenBufferPool(void) noexcept {}
	public:
		const Draw::GraphHandle* PopBlankScreen(int xsize, int ysize, bool trans, bool isDepth = false, int ZBufferBitDepth = InvalidID) noexcept {
			auto Find = std::find_if(this->m_ScreenBuffer.begin(), this->m_ScreenBuffer.end(), [&](const std::unique_ptr<PostPassScreenBuffer>& tgt) {
				return  ((tgt->xSize == xsize) && (tgt->ySize == ysize) && (tgt->isTrans == trans) && (tgt->m_isDepth == isDepth) && tgt->m_ZBufferBitDepth == ZBufferBitDepth);
				});
			if (Find != this->m_ScreenBuffer.end()) {
				return (*Find)->PopBlankScreen();
			}
			this->m_ScreenBuffer.emplace_back(std::make_unique<PostPassScreenBuffer>(xsize, ysize, trans, isDepth, ZBufferBitDepth));
			return this->m_ScreenBuffer.back()->PopBlankScreen();
		}
		void ResetUseCount(int xsize, int ysize, bool trans, bool isDepth = false, int ZBufferBitDepth = InvalidID) noexcept {
			for (auto& s : this->m_ScreenBuffer) {
				if ((s->xSize == xsize) && (s->ySize == ysize) && (s->isTrans == trans) && (s->m_isDepth == isDepth) && s->m_ZBufferBitDepth == ZBufferBitDepth) {
					s->m_UsedLocal = 0;
				}
			}
		}
	public:
		void FirstUpdate(void) noexcept {
			for (auto& s : this->m_ScreenBuffer) {
				if (s->m_Used != 0) {
					s->m_UnUse = 0;
				}
				else {
					++s->m_UnUse;
				}
				s->m_UsedLocal = 0;
				s->m_Used = 0;
			}
			//5フレーム間使われていないスクリーンバッファは消す
			for (size_t loop = 0, max = this->m_ScreenBuffer.size(); loop < max; ++loop) {
				auto& s = this->m_ScreenBuffer[loop];
				if (s->m_UnUse > 5) {
					s.reset();
					std::swap(s, this->m_ScreenBuffer.back());
					this->m_ScreenBuffer.pop_back();
					--loop;
					--max;
				}
			}
		}
	};
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	/*ポストプロセス																															*/
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// ベース
	class PostPassBase {
	protected:
		bool	m_PrevActive{ false };
		char		padding[7]{};
	public:
		PostPassBase(void) noexcept {}
		virtual ~PostPassBase(void) noexcept {}
	protected:
		virtual void Load_Sub(void) noexcept {}
		virtual void Dispose_Sub(void) noexcept {}
		virtual bool IsActive_Sub(void) noexcept { return true; }
		virtual void SetEffect_Sub(Draw::GraphHandle*, Draw::GraphHandle*, Draw::GraphHandle*, Draw::GraphHandle*) noexcept {}
	public:
		bool IsActive(void) noexcept { return IsActive_Sub(); }
		void UpdateActive(bool active) noexcept {
			if (this->m_PrevActive != active) {
				this->m_PrevActive = active;
				if (active) {
					Load_Sub();
				}
				else {
					Dispose_Sub();
				}
			}
		}
		void SetEffect(Draw::GraphHandle* TargetGraph, Draw::GraphHandle* ColorGraph, Draw::GraphHandle* NormalPtr, Draw::GraphHandle* DepthPtr) noexcept {
			if (IsActive()) {
				SetEffect_Sub(TargetGraph, ColorGraph, NormalPtr, DepthPtr);
			}
		}
	};
	//
	class ShadowDraw {
		static const int EXTEND = 1;
	private:
		Draw::GraphHandle			BaseShadowHandle;
		Draw::GraphHandle			DepthBaseScreenHandle;
		Draw::GraphHandle			DepthScreenHandle;
		Draw::GraphHandle			DepthFarScreenHandle;
		
		DXLibRef::ShaderController			m_Shader;
		DXLibRef::ShaderController			m_ShaderRigid;
		Util::Vector3DX					m_ShadowVec{ Util::Vector3DX::up() };
		float						m_Scale{ 1.f };
		float						m_ScaleFar{ 1.f };

		bool						m_PrevShadow{ false };
		char		padding[3]{};

		std::array<Util::Matrix4x4DX, 2>	m_CamViewMatrix{};
		std::array<Util::Matrix4x4DX, 2>	m_CamProjectionMatrix{};
	private:
		void SetupCam(Util::Vector3DX Center, float scale) const noexcept;
	public:
		ShadowDraw(void) noexcept {}
		ShadowDraw(const ShadowDraw&) = delete;
		ShadowDraw(ShadowDraw&&) = delete;
		ShadowDraw& operator=(const ShadowDraw&) = delete;
		ShadowDraw& operator=(ShadowDraw&&) = delete;
		~ShadowDraw(void) noexcept { Dispose(); }
	public:
		const auto&		GetCamViewMatrix(bool isFar) const noexcept { return this->m_CamViewMatrix[static_cast<size_t>(isFar ? 1 : 0)]; }
		const auto&		GetCamProjectionMatrix(bool isFar) const noexcept { return this->m_CamProjectionMatrix[static_cast<size_t>(isFar ? 1 : 0)]; }
		const auto&		GetDepthScreen(void) const noexcept { return DepthScreenHandle; }
		const auto&		GetDepthFarScreen(void) const noexcept { return DepthFarScreenHandle; }//未使用
		const auto&		GetShadowDir(void) const noexcept { return this->m_ShadowVec; }
	public:
		void			SetVec(const Util::Vector3DX& Vec) noexcept { this->m_ShadowVec = Vec; }
		void			SetDraw(std::function<void()> doing_rigid, std::function<void()> doing, Draw::Camera3DInfo tmp_cam) noexcept;

		void			Update(std::function<void()> Shadowdoing, Util::Vector3DX Center, float Scale) noexcept;
		void			UpdateFar(std::function<void()> Shadowdoing, Util::Vector3DX Center, float Scale) noexcept;
		void			Draw(void) noexcept;
		void			Dispose(void) noexcept;
	public:
		void			SetActive(void) noexcept;
		bool			UpdateActive(void) noexcept;
	};
	//
	class PostPassEffect : public Util::SingletonBase<PostPassEffect> {
	private:
		friend class Util::SingletonBase<PostPassEffect>;
	private:
		std::array<std::unique_ptr<PostPassBase>, 16>	m_PostPass;
		//
		bool						m_IsActiveGBuffer{ false };
		char		padding[7]{};
		Draw::GraphHandle					m_BufferScreen;	// 描画スクリーン
		Draw::GraphHandle					m_ColorScreen;	// そのまま透過なしにしたスクリーン
		Draw::GraphHandle					m_NormalScreen;	// 法線のGバッファ
		Draw::GraphHandle					m_DepthScreen;	// 深度のGバッファ
		// 
		float						m_near_DoF = 0.f;
		float						m_far_DoF = 0.f;
		float						m_near_DoFMax = 0.f;
		float						m_far_DoFMin = 0.f;
		int							m_InColorPerMin = 20;
		int							m_InColorPerMax = 255;
		float						m_InColorGamma = 1.1f;
		Util::Matrix4x4DX					m_CamViewMat{};
		Util::Matrix4x4DX					m_CamProjectionMat{};
		Draw::Camera3DInfo				m_CamInfo{};

		bool						m_useScope{ false };
		char		padding2[3]{};
		float						m_ScopeXpos{ 0.f };
		float						m_ScopeYpos{ 0.f };
		float						m_ScopeSize{ 0.f };
		float						m_ScopeZoom{ 0.f };
		bool						m_useBlackOut{ false };
		char		padding3[3]{};
		float						m_BlackOutPer{ 1.f };

		float						m_AberrationPower{ 1.f };
		float						m_DistortionPer{ 120.f };
		float						m_GodRayPer{ 0.5f };
		float						m_GodRayPerByPostPass{ 1.f };
		std::unique_ptr<ShadowDraw>	m_ShadowDraw;
		bool						m_IsCubeMap{ false };
		char		padding4[7]{};
		RealTimeCubeMap				m_RealTimeCubeMap;
		ShaderController			m_PBR_Shader;
		float						m_ShadowScale{ 1.f };
	public:
		auto& GetBufferScreen(void) noexcept { return this->m_BufferScreen; }
	public:
		const auto& GetDepthScreen(void) const noexcept { return this->m_DepthScreen; }

		const auto&		GetCamViewMat(void) const noexcept { return this->m_CamViewMat; }
		const auto&		GetCamProjectionMat(void) const noexcept { return this->m_CamProjectionMat; }
		const auto&		GetShadowDraw(void) const noexcept { return this->m_ShadowDraw; }
		const auto&		GetShadowDir(void) const noexcept { return this->m_ShadowDraw->GetShadowDir(); }
		const auto&		GetCubeMapTex(void) const noexcept { return this->m_RealTimeCubeMap.GetCubeMapTex(); }
		const auto&		Get_near_DoF(void) const noexcept { return this->m_near_DoF; }
		const auto&		Get_far_DoF(void) const noexcept { return this->m_far_DoF; }
		const auto&		Get_near_DoFMax(void) const noexcept { return this->m_near_DoFMax; }
		const auto&		Get_far_DoFMin(void) const noexcept { return this->m_far_DoFMin; }
		const auto&		is_lens(void) const noexcept { return this->m_useScope; }
		const auto&		zoom_xpos(void) const noexcept { return this->m_ScopeXpos; }
		const auto&		zoom_ypos(void) const noexcept { return this->m_ScopeYpos; }
		const auto&		zoom_size(void) const noexcept { return this->m_ScopeSize; }
		const auto&		zoom_lens(void) const noexcept { return this->m_ScopeZoom; }
		const auto&		is_Blackout(void) const noexcept { return this->m_useBlackOut; }
		const auto&		GetBlackoutPer(void) const noexcept { return this->m_BlackOutPer; }
		const auto&		GetAberrationPower(void) const noexcept { return this->m_AberrationPower; }
		auto			GetGodRayPerRet(void) const noexcept { return this->m_GodRayPer * this->m_GodRayPerByPostPass; }
		const auto&		GetDistortionPer(void) const noexcept { return this->m_DistortionPer; }
		const auto&		GetShadowScale(void) const noexcept { return this->m_ShadowScale; }
		const auto&		GetGodRayPer(void) const noexcept { return this->m_GodRayPer; }
	public:
		void			Set_is_lens(bool value) noexcept { this->m_useScope = value; }
		void			Set_xp_lens(float value) noexcept { this->m_ScopeXpos = value; }
		void			Set_yp_lens(float value) noexcept { this->m_ScopeYpos = value; }
		void			Set_size_lens(float value) noexcept { this->m_ScopeSize = value; }
		void			Set_zoom_lens(float value) noexcept { this->m_ScopeZoom = value; }
		void			Set_is_Blackout(bool value) noexcept { this->m_useBlackOut = value; }
		void			Set_Per_Blackout(float value) noexcept { this->m_BlackOutPer = value; }
		void			SetAberrationPower(float value) noexcept { this->m_AberrationPower = value; }
		void			SetGodRayPer(float value) noexcept { this->m_GodRayPer = value; }
		void			SetGodRayPerByPostPass(float value) noexcept {
			this->m_GodRayPerByPostPass = Util::Lerp(this->m_GodRayPerByPostPass, value, 1.f - 0.975f);
		}
		void			SetDistortionPer(float value) noexcept { this->m_DistortionPer = value; }
		void			SetShadowScale(float value) noexcept { this->m_ShadowScale = value; }
		// ボケ始める場所を指定(完全にボケるのはニアファーの限度)
		void			Set_DoFNearFar(float near_d, float far_d, float near_m, float far_m) noexcept {
			this->m_near_DoF = near_d;
			this->m_far_DoF = far_d;
			this->m_near_DoFMax = near_m;
			this->m_far_DoFMin = far_m;
		}
		void			SetLevelFilter(int inMin, int inMax, float gamma) noexcept {
			this->m_InColorPerMin = std::clamp(inMin, 0, 255);
			this->m_InColorPerMax = std::clamp(inMax, 0, 255);
			this->m_InColorGamma = std::max(1.f, gamma);
		}
		void			ResetAllParams(void) noexcept {
			SetLevelFilter(0, 255, 1.f);
			SetAberrationPower(1.f);
			Set_is_Blackout(false);
			Set_Per_Blackout(0.f);
			Set_is_lens(false);
			Set_zoom_lens(1.f);
			// 環境光と影の初期化
			SetAmbientLight(Util::Vector3DX::vget(0.25f, -1.f, 0.25f));
		}
	private:
		PostPassEffect(void) noexcept;
		PostPassEffect(const PostPassEffect&) = delete;
		PostPassEffect(PostPassEffect&&) = delete;
		PostPassEffect& operator=(const PostPassEffect&) = delete;
		PostPassEffect& operator=(PostPassEffect&&) = delete;
		virtual ~PostPassEffect(void) noexcept {
			PostPassScreenBufferPool::Release();
			Dispose();

		}
	private:
		void			Dispose(void) noexcept;
	public:
		void			Init(void) noexcept;
		void			UpdateActive(void) noexcept;
		void			SetCamMat(const Draw::Camera3DInfo& camInfo) noexcept;
		void			ResetBuffer(void) noexcept;
		void			DrawGBuffer(float near_len, float far_len, std::function<void()> done) noexcept;
		void			SetDrawShadow(const Draw::Camera3DInfo& camInfo,
			std::function<void()> setshadowdoing_rigid,
			std::function<void()> setshadowdoing) noexcept;
		void			DrawPostProcess(void) noexcept;
		void			ResetAllBuffer(void) noexcept;

		bool			UpdateShadowActive(void) noexcept { return this->m_ShadowDraw->UpdateActive(); }
		void			SetAmbientLight(const Util::Vector3DX& AmbientLightVec) noexcept { this->m_ShadowDraw->SetVec(AmbientLightVec); }
		void			Update_Shadow(std::function<void()> doing, const Util::Vector3DX& CenterPos, bool IsFar) noexcept;
		void			Update_CubeMap(std::function<void()> doing, const Util::Vector3DX& CenterPos) noexcept;

		void			DrawByPBR(std::function<void()> doing) noexcept;
	private:
		void LoadGBuffer(void) noexcept;
		void DisposeGBuffer(void) noexcept;

		void UpdateActiveGBuffer(bool ActiveGBuffer) noexcept {
			if (this->m_IsActiveGBuffer != ActiveGBuffer) {
				this->m_IsActiveGBuffer = ActiveGBuffer;
				if (this->m_IsActiveGBuffer) {
					LoadGBuffer();
				}
				else {
					DisposeGBuffer();
				}
			}
		}

		void UpdateActiveCubeMap(bool ActiveCubeMap) noexcept {
			if (ActiveCubeMap != this->m_IsCubeMap) {
				this->m_IsCubeMap = ActiveCubeMap;
				if (this->m_IsCubeMap) {
					this->m_RealTimeCubeMap.Init();
				}
				else {
					this->m_RealTimeCubeMap.Dispose();
				}
			}
		}
	};
};
