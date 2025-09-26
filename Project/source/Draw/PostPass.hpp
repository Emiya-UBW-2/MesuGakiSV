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
#include "../Draw/Camera.hpp"


namespace Draw {
	constexpr float		Scale3DRate{ 12.5f };							/*1mに相当する3D空間上の長さ*/
	constexpr int		InvalidID{ -1 };								/*共通の無効値*/
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// シェーダーを使用する際の補助クラス
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	class Shader2DController {
	public:
		// 2Dにシェーダーを適用する際に使用する画面サイズの頂点情報
		class ScreenVertex {
			VERTEX2DSHADER Screen_vertex[6] = {};
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

				Screen_vertex[0].rhw = 1.0f;
				Screen_vertex[1].rhw = 1.0f;
				Screen_vertex[2].rhw = 1.0f;
				Screen_vertex[3].rhw = 1.0f;

				Screen_vertex[0].spc = GetColorU8(0, 0, 0, 0);
				Screen_vertex[1].spc = GetColorU8(0, 0, 0, 0);
				Screen_vertex[2].spc = GetColorU8(0, 0, 0, 0);
				Screen_vertex[3].spc = GetColorU8(0, 0, 0, 0);

				Screen_vertex[0].u = 0.0f; Screen_vertex[0].v = 0.0f;
				Screen_vertex[1].u = 1.0f; Screen_vertex[1].v = 0.0f;
				Screen_vertex[2].u = 0.0f; Screen_vertex[3].v = 1.0f;
				Screen_vertex[3].u = 1.0f; Screen_vertex[2].v = 1.0f;

				Screen_vertex[0].su = 0.0f; Screen_vertex[0].sv = 0.0f;
				Screen_vertex[1].su = 1.0f; Screen_vertex[1].sv = 0.0f;
				Screen_vertex[2].su = 0.0f; Screen_vertex[3].sv = 1.0f;
				Screen_vertex[3].su = 1.0f; Screen_vertex[2].sv = 1.0f;

				Screen_vertex[4] = Screen_vertex[2];
				Screen_vertex[5] = Screen_vertex[1];
			}
		};
		// 影用の深度記録画像を作成した際のカメラのビュー行列と射影行列
		struct LIGHTCAMERA_MATRIX {
			MATRIX ViewMatrix;
			MATRIX ProjectionMatrix;
		};
	private:
		// シェーダーハンドル
		int					m_Shaderhandle{ InvalidID };
		// シェーダーに渡す追加パラメーターを配するハンドル
		std::array<int, 3>	m_LightCameraMatrixHandle{};	// 影用の深度記録画像を作成した際のカメラのビュー行列と射影行列を設定するための定数バッファ
		int					m_ShaderSendDispSizeHandle{ InvalidID };
		std::array<int, 3>	m_Shadercbhandle{};
		ScreenVertex		m_ScreenVertex;					// 頂点データ
	public:
		Shader2DController(void) noexcept {
			// シェーダーハンドル
			this->m_Shaderhandle = InvalidID;
			// シェーダーに渡す追加パラメーターを配するハンドル
			this->m_ShaderSendDispSizeHandle = InvalidID;
			for (auto& h : this->m_Shadercbhandle) {
				h = InvalidID;
			}
		}
		~Shader2DController(void) noexcept {
			Dispose();
		}
	public:
		// 初期化
		void			Init(const char* Shader) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			// ピクセルシェーダ―周り
			this->m_ShaderSendDispSizeHandle = CreateShaderConstantBuffer(sizeof(float) * 4);
			for (auto& h : this->m_Shadercbhandle) {
				h = CreateShaderConstantBuffer(sizeof(float) * 4);
			}
			this->m_Shaderhandle = LoadPixelShader(Shader);
			// 影用の深度記録画像を作成した際のカメラのビュー行列と射影行列を設定するための定数バッファの作成
			for (auto& h : this->m_LightCameraMatrixHandle) {
				h = CreateShaderConstantBuffer(sizeof(LIGHTCAMERA_MATRIX));
			}
		}
		// 後始末
		void			Dispose(void) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			// ピクセルシェーダ―周り
			DeleteShaderConstantBuffer(this->m_ShaderSendDispSizeHandle);
			for (auto& h : this->m_Shadercbhandle) {
				DeleteShaderConstantBuffer(h);
			}
			DeleteShader(this->m_Shaderhandle);
		}
	public:
		// ピクセルシェーダ―のSlot番目のレジスタに情報をセット(Slot>=4)
		void			SetCameraMatrix(int Slot, const Util::Matrix4x4& View, const Util::Matrix4x4& Projection) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			auto& BufferHandle = this->m_LightCameraMatrixHandle[static_cast<size_t>(Slot - 4)];
			// 設定したカメラのビュー行列と射影行列を取得しておく
			LIGHTCAMERA_MATRIX* LightCameraMatrixConst = (LIGHTCAMERA_MATRIX*)GetBufferShaderConstantBuffer(BufferHandle);
			LightCameraMatrixConst->ViewMatrix = View.get();
			LightCameraMatrixConst->ProjectionMatrix = Projection.get();

			UpdateShaderConstantBuffer(BufferHandle);
			SetShaderConstantBuffer(BufferHandle, DX_SHADERTYPE_PIXEL, Slot);		// 影用深度記録画像を描画したときのカメラのビュー行列と射影行列を定数に設定する
		}
		// ピクセルシェーダ―の2番目のレジスタに画面サイズの情報をセット
		void			SetDispSize(int dispx, int dispy) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			auto& BufferHandle = this->m_ShaderSendDispSizeHandle;
			FLOAT2* dispsize = (FLOAT2*)GetBufferShaderConstantBuffer(BufferHandle);	// ピクセルシェーダー用の定数バッファのアドレスを取得
			dispsize->u = static_cast<float>(dispx);
			dispsize->v = static_cast<float>(dispy);
			UpdateShaderConstantBuffer(BufferHandle);									// ピクセルシェーダー用の定数バッファを更新して書き込んだ内容を反映する
			SetShaderConstantBuffer(BufferHandle, DX_SHADERTYPE_PIXEL, 2);				// ピクセルシェーダー用の定数バッファを定数バッファレジスタ2にセット
			this->m_ScreenVertex.SetScreenVertex(dispx, dispy);
		}
		// ピクセルシェーダ―のSlot番目のレジスタに情報をセット(Slot>=3)
		void			SetParam(int Slot, float param1, float param2, float param3, float param4) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			auto& BufferHandle = this->m_Shadercbhandle[static_cast<size_t>(Slot - 3)];
			FLOAT4* f4 = (FLOAT4*)GetBufferShaderConstantBuffer(BufferHandle);				// ピクセルシェーダー用の定数バッファのアドレスを取得
			f4->x = param1;
			f4->y = param2;
			f4->z = param3;
			f4->w = param4;
			UpdateShaderConstantBuffer(BufferHandle);											// ピクセルシェーダー用の定数バッファを更新して書き込んだ内容を反映する
			SetShaderConstantBuffer(BufferHandle, DX_SHADERTYPE_PIXEL, Slot);					// ピクセルシェーダー用の定数バッファを定数バッファレジスタ3にセット
		}
		// 3D空間に適用する場合の関数(引数に3D描画のラムダ式を代入)
		void			Draw(void) const noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			SetUsePixelShader(this->m_Shaderhandle);											// 使用するピクセルシェーダーをセット
			MV1SetUseOrigShader(TRUE);
			DrawPolygon2DToShader(this->m_ScreenVertex.GetScreenVertex(), 2);
			MV1SetUseOrigShader(FALSE);
			SetUsePixelShader(InvalidID);
		}
	};
	class ShaderController {
	public:
		// 影用の深度記録画像を作成した際のカメラのビュー行列と射影行列
		struct LIGHTCAMERA_MATRIX {
			MATRIX ViewMatrix;
			MATRIX ProjectionMatrix;
		};
		// DXLIBから引っ張ってきたシェーダー用の定義
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
		std::array<int, 3>	m_LightCameraMatrixHandle{};	// 影用の深度記録画像を作成した際のカメラのビュー行列と射影行列を設定するための定数バッファ
		std::array<int, 4>	m_VertexShadercbhandle{};
		int					m_GeometryShaderMatcbhandle{ InvalidID };
		int					m_PixelShaderSendDispSizeHandle{ InvalidID };
		std::array<int, 3>	m_PixelShadercbhandle{};
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
			for (auto& h : this->m_LightCameraMatrixHandle) {
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
			for (auto& h : this->m_LightCameraMatrixHandle) {
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
		void			SetVertexCameraMatrix(int Slot, const Util::Matrix4x4& View, const Util::Matrix4x4& Projection) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			auto& BufferHandle = this->m_LightCameraMatrixHandle[static_cast<size_t>(Slot - 4)];
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
		void			SetPixelCameraMatrix(int Slot, const Util::Matrix4x4& View, const Util::Matrix4x4& Projection) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			auto& BufferHandle = this->m_LightCameraMatrixHandle[static_cast<size_t>(Slot - 4)];
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
	};
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	//スクリーンバッファのつかいまわし
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	class PostPassScreenBufferPool : public Util::SingletonBase<PostPassScreenBufferPool> {
	private:
		friend class Util::SingletonBase<PostPassScreenBufferPool>;
	private:
		class PostPassScreenBuffer {
			static const int			m_Size = 3;
		private:
			std::array<Draw::GraphHandle, m_Size>	m_Screen{};
			int										m_xSize{};
			int										m_ySize{};
			int										m_ZBufferBitDepth{};
			int										m_UsedLocal{};
			int										m_Used{};
			int										m_UnUseFrame{};
			bool									m_isTrans{};
			bool									m_isDepth{};
			char		padding[6]{};
		public:
			PostPassScreenBuffer(int xsize, int ysize, bool trans, bool isDepth = false, int ZBufferBitDepth = InvalidID) noexcept {
				this->m_xSize = xsize;
				this->m_ySize = ysize;
				this->m_isTrans = trans;
				this->m_isDepth = isDepth;
				this->m_ZBufferBitDepth = ZBufferBitDepth;
				int PrevZBufferBitDepth{};
				if (this->m_ZBufferBitDepth != InvalidID) {
					PrevZBufferBitDepth = GetCreateDrawValidGraphZBufferBitDepth();
					SetCreateDrawValidGraphZBufferBitDepth(this->m_ZBufferBitDepth);
				}
				for (auto& s : this->m_Screen) {
					if (this->m_isDepth) {
						s.MakeDepth(this->m_xSize, this->m_ySize);
					}
					else {
						s.Make(this->m_xSize, this->m_ySize, this->m_isTrans);
					}
				}
				if (this->m_ZBufferBitDepth != InvalidID) {
					SetCreateDrawValidGraphZBufferBitDepth(PrevZBufferBitDepth);
				}
				this->m_Used = 0;
				this->m_UnUseFrame = 0;
				ResetUseCount();
			}
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
			bool IsActive() const noexcept {
				return this->m_UnUseFrame <= 5;//5フレーム以上使われていない
			}
			bool Equal(int xsize, int ysize, bool trans, bool isDepth, int ZBufferBitDepth) const noexcept {
				return (
					(this->m_xSize == xsize) &&
					(this->m_ySize == ysize) &&
					(this->m_isTrans == trans) &&
					(this->m_isDepth == isDepth) &&
					this->m_ZBufferBitDepth == ZBufferBitDepth
					);
			}
			void ResetUseCount(void) noexcept {
				this->m_UsedLocal = 0;
			}
		public:
			void Update() noexcept {
				if (this->m_Used != 0) {
					this->m_Used = 0;
					this->m_UnUseFrame = 0;
				}
				else {
					++this->m_UnUseFrame;
				}
				ResetUseCount();
			}
		};
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
		const std::unique_ptr<PostPassScreenBuffer>& GetBlankScreen(int xsize, int ysize, bool trans, bool isDepth = false, int ZBufferBitDepth = InvalidID) noexcept {
			auto Find = std::find_if(this->m_ScreenBuffer.begin(), this->m_ScreenBuffer.end(), [&](const std::unique_ptr<PostPassScreenBuffer>& tgt) {
				return  tgt->Equal(xsize, ysize, trans, isDepth, ZBufferBitDepth);
				});
			if (Find != this->m_ScreenBuffer.end()) {
				return (*Find);
			}
			this->m_ScreenBuffer.emplace_back(std::make_unique<PostPassScreenBuffer>(xsize, ysize, trans, isDepth, ZBufferBitDepth));
			return this->m_ScreenBuffer.back();
		}
		void ResetUseCount(int xsize, int ysize, bool trans, bool isDepth = false, int ZBufferBitDepth = InvalidID) noexcept {
			for (auto& s : this->m_ScreenBuffer) {
				if (s->Equal(xsize, ysize, trans, isDepth, ZBufferBitDepth)) {
					s->ResetUseCount();
				}
			}
		}
	public:
		void FirstUpdate(void) noexcept {
			for (auto& s : this->m_ScreenBuffer) {
				s->Update();
			}
			//使われていないスクリーンバッファは消す
			for (size_t loop = 0, max = this->m_ScreenBuffer.size(); loop < max; ++loop) {
				auto& s = this->m_ScreenBuffer[loop];
				if (!s->IsActive()) {
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
	//ポストプロセス
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	class PostPassEffect : public Util::SingletonBase<PostPassEffect> {
	private:
		friend class Util::SingletonBase<PostPassEffect>;
	public:
		class Gbuffer {
		public:
			Gbuffer(void) noexcept {}
			Gbuffer(const Gbuffer&) = delete;
			Gbuffer(Gbuffer&&) = delete;
			Gbuffer& operator=(const Gbuffer&) = delete;
			Gbuffer& operator=(Gbuffer&&) = delete;
			virtual ~Gbuffer(void) noexcept {}
		private:
			Draw::GraphHandle			m_ColorScreen;	// そのまま透過なしにしたスクリーン
			Draw::GraphHandle			m_NormalScreen;	// 法線のGバッファ
			Draw::GraphHandle			m_DepthScreen;	// 深度のGバッファ
		public:
			const auto& GetColorBuffer() const noexcept { return this->m_ColorScreen; }
			const auto& GetNormalBuffer() const noexcept { return this->m_NormalScreen; }
			const auto& GetDepthBuffer() const noexcept { return this->m_DepthScreen; }
		public:
			void		Load(int xsize, int ysize) noexcept {
				auto Prev = GetCreateDrawValidGraphZBufferBitDepth();
				SetCreateDrawValidGraphZBufferBitDepth(24);
				this->m_ColorScreen.Make(xsize, ysize, false);
				this->m_NormalScreen.Make(xsize, ysize, false);
				this->m_DepthScreen.MakeDepth(xsize, ysize);
				SetCreateDrawValidGraphZBufferBitDepth(Prev);
			}
			void		Dispose(void) noexcept {
				this->m_ColorScreen.Dispose();
				this->m_NormalScreen.Dispose();
				this->m_DepthScreen.Dispose();
			}
			void		Reset(void) noexcept {
				this->m_ColorScreen.SetDraw_Screen();
				this->m_NormalScreen.SetDraw_Screen();
				this->m_NormalScreen.FillGraph(128, 128, 255);
				this->m_DepthScreen.SetDraw_Screen();
				this->m_DepthScreen.FillGraph(255, 0, 0);
			}
		};
	public:
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
			virtual void SetEffect_Sub(Draw::GraphHandle*, Gbuffer*) noexcept {}
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
			void SetEffect(Draw::GraphHandle* TargetGraph, Gbuffer* pGbuffer) noexcept {
				if (IsActive()) {
					SetEffect_Sub(TargetGraph, pGbuffer);
				}
			}
		};
	private:
		//
		class ShadowDraw {
			static const int EXTEND = 1;
		private:
			Draw::GraphHandle			m_BaseShadowHandle;
			Draw::GraphHandle			m_DepthBaseScreenHandle;
			Draw::GraphHandle			m_DepthScreenHandle;
			Draw::GraphHandle			m_DepthFarScreenHandle;

			ShaderController			m_Shader;
			ShaderController			m_ShaderRigid;
			Util::VECTOR3D				m_ShadowVec{ Util::VECTOR3D::up() };
			float						m_Scale{ 1.f };

			bool						m_PrevShadow{ false };
			char		padding[7]{};

			std::array<Util::Matrix4x4, 2>	m_CamViewMatrix{};
			std::array<Util::Matrix4x4, 2>	m_CamProjectionMatrix{};
		public:
			ShadowDraw(void) noexcept {
				this->m_PrevShadow = false;
				UpdateActive();
			}
			ShadowDraw(const ShadowDraw&) = delete;
			ShadowDraw(ShadowDraw&&) = delete;
			ShadowDraw& operator=(const ShadowDraw&) = delete;
			ShadowDraw& operator=(ShadowDraw&&) = delete;
			~ShadowDraw(void) noexcept {
				if (this->m_PrevShadow) {
					Dispose();
				}
			}
		private:
			void SetupCam(Util::VECTOR3D Center, float scale) const noexcept {
				ClearDrawScreen();
				SetupCamera_Ortho(30.f * scale);		// カメラのタイプを正射影タイプにセット、描画範囲も指定
				SetCameraNearFar(0.05f * scale, 60.f * scale);		// 描画する奥行き範囲をセット
				// カメラの位置と注視点はステージ全体が見渡せる位置
				auto Vec = this->m_ShadowVec;
				if (this->m_ShadowVec.x == 0.f && this->m_ShadowVec.z == 0.f) {
					Vec.z = (0.1f);
				}
				SetCameraPositionAndTarget_UpVecY((Center - Vec.normalized() * (30.f * scale)).get(), Center.get());
			}
			void			Init(void) noexcept {
				auto* DrawerMngr = Draw::MainDraw::Instance();

				int xsizeEx = DrawerMngr->GetDispWidth() / EXTEND;
				int ysizeEx = DrawerMngr->GetDispHeight() / EXTEND;

				this->m_BaseShadowHandle.Make(xsizeEx, ysizeEx, TRUE);
				int size = 2 << 10;
				this->m_DepthBaseScreenHandle.Make(size, size, FALSE);			// 深度バッファ用の作成
				this->m_DepthScreenHandle.MakeDepth(size, size);					// 深度バッファの作成
				this->m_DepthFarScreenHandle.MakeDepth(size, size);				// 深度バッファの作成
				this->m_Shader.Init("CommonData/shader/VS_SoftShadow.vso", "CommonData/shader/PS_SoftShadow.pso");
				this->m_ShaderRigid.Init("CommonData/shader/VS_SoftShadow_Rigid.vso", "CommonData/shader/PS_SoftShadow.pso");
			}
			void			Dispose(void) noexcept {
				this->m_BaseShadowHandle.Dispose();
				this->m_DepthBaseScreenHandle.Dispose();
				this->m_DepthScreenHandle.Dispose();
				this->m_DepthFarScreenHandle.Dispose();
				this->m_Shader.Dispose();
				this->m_ShaderRigid.Dispose();
			}
		public:
			const auto& GetCamViewMatrix(bool isFar) const noexcept { return this->m_CamViewMatrix[static_cast<size_t>(isFar ? 1 : 0)]; }
			const auto& GetCamProjectionMatrix(bool isFar) const noexcept { return this->m_CamProjectionMatrix[static_cast<size_t>(isFar ? 1 : 0)]; }
			const auto& GetDepthScreen(void) const noexcept { return this->m_DepthScreenHandle; }
			const auto& GetDepthFarScreen(void) const noexcept { return this->m_DepthFarScreenHandle; }//未使用
			const auto& GetShadowDir(void) const noexcept { return this->m_ShadowVec; }
		public:
			void			SetVec(const Util::VECTOR3D& Vec) noexcept { this->m_ShadowVec = Vec; }
			void			SetDraw(std::function<void()> doing_rigid, std::function<void()> doing, Draw::Camera3DInfo tmp_cam) noexcept {
				this->m_BaseShadowHandle.SetUseTextureToShader(0);				// 影用深度記録画像をテクスチャにセット
				this->m_DepthScreenHandle.SetUseTextureToShader(1);
				this->m_DepthFarScreenHandle.SetUseTextureToShader(2);
				// 影の結果を出力
				tmp_cam.SetCamInfo(tmp_cam.GetCamFov(), 0.01f * Scale3DRate, 30.f * Scale3DRate);
				this->m_BaseShadowHandle.SetDraw_Screen();
				tmp_cam.FlipCamInfo();
				{
					auto* pOption = Util::OptionParam::Instance();
					const float ShadowLevel = static_cast<float>(pOption->GetParam(pOption->GetOptionType(Util::OptionType::Shadow))->GetSelect());
					this->m_Shader.SetPixelParam(3, ShadowLevel, this->m_Scale * 180.f, 0.f, 0.f);
					this->m_Shader.SetVertexCameraMatrix(4, GetCamViewMatrix(false), GetCamProjectionMatrix(false));
					this->m_Shader.SetVertexCameraMatrix(5, GetCamViewMatrix(true), GetCamProjectionMatrix(true));
					this->m_Shader.Draw_lamda(doing);
					this->m_ShaderRigid.SetPixelParam(3, ShadowLevel, this->m_Scale * 180.f, 0.f, 0.f);
					this->m_ShaderRigid.SetVertexCameraMatrix(4, GetCamViewMatrix(false), GetCamProjectionMatrix(false));
					this->m_ShaderRigid.SetVertexCameraMatrix(5, GetCamViewMatrix(true), GetCamProjectionMatrix(true));
					this->m_ShaderRigid.Draw_lamda(doing_rigid);
				}
				SetUseTextureToShader(1, InvalidID);				// 使用テクスチャの設定を解除
				SetUseTextureToShader(2, InvalidID);				// 使用テクスチャの設定を解除
				// 後処理
				this->m_BaseShadowHandle.GraphBlend(this->m_DepthBaseScreenHandle, 255, DX_GRAPH_BLEND_RGBA_SELECT_MIX,
					DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_SRC_G, DX_RGBA_SELECT_SRC_R);
			}
			bool			UpdateActive(void) noexcept {
				auto* pOption = Util::OptionParam::Instance();
				bool shadow = pOption->GetParam(pOption->GetOptionType(Util::OptionType::Shadow))->GetSelect() > 0;
				if (this->m_PrevShadow != shadow) {
					this->m_PrevShadow = shadow;
					if (shadow) {
						Init();
						return true;
					}
					else {
						Dispose();
					}
				}
				return false;
			}
		public:
			void			Update(std::function<void()> Shadowdoing, Util::VECTOR3D Center, float Scale) noexcept {
				this->m_Scale = Scale;
				// 影用の深度記録画像の準備を行う
				this->m_DepthBaseScreenHandle.SetRenderTargetToShader(0);
				SetRenderTargetToShader(1, InvalidID);
				this->m_DepthScreenHandle.SetRenderTargetToShader(2);
				{
					SetupCam(Center, this->m_Scale * Scale3DRate);
					this->m_CamViewMatrix[0] = GetCameraViewMatrix();
					this->m_CamProjectionMatrix[0] = GetCameraProjectionMatrix();
					Shadowdoing();
				}
				SetRenderTargetToShader(0, InvalidID);
				SetRenderTargetToShader(1, InvalidID);
				SetRenderTargetToShader(2, InvalidID);
			}
			void			UpdateFar(std::function<void()> Shadowdoing, Util::VECTOR3D Center, float Scale) noexcept {
				// 影用の深度記録画像の準備を行う
				this->m_DepthBaseScreenHandle.SetRenderTargetToShader(0);
				SetRenderTargetToShader(1, InvalidID);
				this->m_DepthFarScreenHandle.SetRenderTargetToShader(2);
				{
					SetupCam(Center, Scale * Scale3DRate);
					this->m_CamViewMatrix[1] = GetCameraViewMatrix();
					this->m_CamProjectionMatrix[1] = GetCameraProjectionMatrix();
					Shadowdoing();
				}
				SetRenderTargetToShader(0, InvalidID);
				SetRenderTargetToShader(1, InvalidID);
				SetRenderTargetToShader(2, InvalidID);
			}
			void			Draw(void) noexcept {
				auto* DrawerMngr = Draw::MainDraw::Instance();
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
				this->m_BaseShadowHandle.DrawExtendGraph(0, 0, DrawerMngr->GetDispWidth(), DrawerMngr->GetDispHeight(), true);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				// DepthScreenHandle.DrawExtendGraph(0, 0,1080,1080, true);
			}
		};
		// キューブマップ生成
		class RealTimeCubeMap {
		private:
			Draw::GraphHandle dynamicCubeTex;		// 周囲を回る小さいモデルたちを映りこませるための描画対象にできるキューブマップテクスチャ
			Util::VECTOR3D lookAt[6]{};	// 映りこむ周囲の環境を描画する際のカメラの注視点
			Util::VECTOR3D up[6]{};		// 移りこむ周囲の環境を描画する際のカメラの上方向
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
				lookAt[0] = Util::VECTOR3D::right();
				lookAt[1] = Util::VECTOR3D::left();
				lookAt[2] = Util::VECTOR3D::up();
				lookAt[3] = Util::VECTOR3D::down();
				lookAt[4] = Util::VECTOR3D::forward();
				lookAt[5] = Util::VECTOR3D::back();
				up[0] = Util::VECTOR3D::up();
				up[1] = Util::VECTOR3D::up();
				up[2] = Util::VECTOR3D::back();
				up[3] = Util::VECTOR3D::forward();
				up[4] = Util::VECTOR3D::up();
				up[5] = Util::VECTOR3D::up();
			}

			void ReadyDraw(const Util::VECTOR3D& Pos, const std::function<void()>& Doing) noexcept {
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

			const auto& GetCubeMapTex(void) const noexcept { return dynamicCubeTex; }
		};
	private:
		std::array<std::unique_ptr<PostPassBase>, 16>	m_PostPass;
		//
		bool						m_IsActiveGBuffer{ false };
		char		padding[7]{};
		Draw::GraphHandle			m_BufferScreen;	// 描画スクリーン
		Gbuffer						m_Gbuffer;
		// 
		float						m_near_DoF = 0.f;
		float						m_far_DoF = 0.f;
		float						m_near_DoFMax = 0.f;
		float						m_far_DoFMin = 0.f;
		int							m_InColorPerMin = 20;
		int							m_InColorPerMax = 255;
		float						m_InColorGamma = 1.1f;
		Util::Matrix4x4			m_CamViewMat{};
		Util::Matrix4x4			m_CamProjectionMat{};
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
		float						m_ShadowScale{ 1.f };
		bool						m_IsCubeMap{ false };
		char		padding4[3]{};
		RealTimeCubeMap				m_RealTimeCubeMap;
	public:
		auto&			GetBufferScreen(void) noexcept { return this->m_BufferScreen; }
		const auto&		GetCamViewMat(void) const noexcept { return this->m_CamViewMat; }
		const auto&		GetCamProjectionMat(void) const noexcept { return this->m_CamProjectionMat; }
		const auto&		GetShadowDraw(void) const noexcept { return this->m_ShadowDraw; }
		const auto&		GetCubeMapHandle(void) const noexcept { return this->m_RealTimeCubeMap.GetCubeMapTex(); }
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
			SetAmbientLight(Util::VECTOR3D::vget(0.25f, -1.f, 0.25f));
		}
	private:
		PostPassEffect(void) noexcept;
		PostPassEffect(const PostPassEffect&) = delete;
		PostPassEffect(PostPassEffect&&) = delete;
		PostPassEffect& operator=(const PostPassEffect&) = delete;
		PostPassEffect& operator=(PostPassEffect&&) = delete;
		virtual ~PostPassEffect(void) noexcept {
			Dispose();
		}
	private:
		void		Init(void) noexcept {
			PostPassScreenBufferPool::Create();
			//
			auto* DrawerMngr = Draw::MainDraw::Instance();
			auto Prev = GetCreateDrawValidGraphZBufferBitDepth();
			SetCreateDrawValidGraphZBufferBitDepth(24);
			this->m_BufferScreen.Make(DrawerMngr->GetDispWidth(), DrawerMngr->GetDispHeight(), true);
			SetCreateDrawValidGraphZBufferBitDepth(Prev);
			this->m_ShadowDraw = std::make_unique<ShadowDraw>();
			// 影生成
			UpdateShadowActive();
		}
		void		Dispose(void) noexcept {
			PostPassScreenBufferPool::Release();
			// ポストエフェクト
			for (auto& P : this->m_PostPass) {
				if (!P) { continue; }
				P.reset();
			}
			this->m_ShadowDraw.reset();
		}
	private:
		void		UpdateActiveGBuffer(bool ActiveGBuffer) noexcept {
			if (this->m_IsActiveGBuffer != ActiveGBuffer) {
				this->m_IsActiveGBuffer = ActiveGBuffer;
				if (this->m_IsActiveGBuffer) {
					auto* DrawerMngr = Draw::MainDraw::Instance();
					this->m_Gbuffer.Load(DrawerMngr->GetDispWidth(), DrawerMngr->GetDispHeight());
				}
				else {
					this->m_Gbuffer.Dispose();
				}
			}
		}
	private:
		void		UpdateActiveCubeMap(bool ActiveCubeMap) noexcept {
			if (this->m_IsCubeMap != ActiveCubeMap) {
				this->m_IsCubeMap = ActiveCubeMap;
				if (this->m_IsCubeMap) {
					this->m_RealTimeCubeMap.Init();
				}
				else {
					this->m_RealTimeCubeMap.Dispose();
				}
			}
		}
	public:
		bool		UpdateShadowActive(void) noexcept { return this->m_ShadowDraw->UpdateActive(); }
		void		SetAmbientLight(const Util::VECTOR3D& AmbientLightVec) noexcept { this->m_ShadowDraw->SetVec(AmbientLightVec); }
		void		UpdateBuffer(void) noexcept {
			auto* pOption = Util::OptionParam::Instance();
			bool ActiveGBuffer = false;
			for (auto& P : this->m_PostPass) {
				if (!P) { continue; }
				if (!P->IsActive()) { continue; }
				ActiveGBuffer = true;
				break;
			}
			UpdateActiveGBuffer(ActiveGBuffer);
			for (auto& P : this->m_PostPass) {
				if (!P) { continue; }
				P->UpdateActive(P->IsActive());
			}
			UpdateActiveCubeMap((pOption->GetParam(pOption->GetOptionType(Util::OptionType::Reflection))->GetSelect() > 0) && false);
		}
		void		Update_CubeMap(std::function<void()> doing, const Util::VECTOR3D& CenterPos) noexcept {
			auto* pOption = Util::OptionParam::Instance();
			if ((pOption->GetParam(pOption->GetOptionType(Util::OptionType::Reflection))->GetSelect() > 0) && false) {
				this->m_RealTimeCubeMap.ReadyDraw(CenterPos, doing);
			}
		}
		void		Update_Shadow(std::function<void()> doing, const Util::VECTOR3D& CenterPos, bool IsFar) noexcept {
			auto* pOption = Util::OptionParam::Instance();
			if (pOption->GetParam(pOption->GetOptionType(Util::OptionType::Shadow))->GetSelect() > 0) {
				// 影用の深度記録画像の準備を行う
				if (!IsFar) {
					this->m_ShadowDraw->Update(doing, CenterPos, this->GetShadowScale());
				}
				else {
					this->m_ShadowDraw->UpdateFar(doing, CenterPos, this->GetShadowScale() * 4.f);
				}
			}
		}
		void		StartDraw(void) noexcept {
			// リセット
			if (this->m_IsActiveGBuffer) {
				this->m_Gbuffer.Reset();
			}
			// カメラ
			auto* CameraParts = Camera::Camera3D::Instance();
			this->m_CamViewMat = CameraParts->GetMainCamera().GetViewMatrix();
			this->m_CamProjectionMat = CameraParts->GetMainCamera().GetProjectionMatrix();
		}
		void		DrawGBuffer(float near_len, float far_len, std::function<void()> done) noexcept {
			auto* CameraParts = Camera::Camera3D::Instance();
			// カラーバッファを描画対象0に、法線バッファを描画対象1に設定
			this->m_BufferScreen.SetRenderTargetToShader(0);
			if (this->m_IsActiveGBuffer) {
				this->m_Gbuffer.GetNormalBuffer().SetRenderTargetToShader(1);
				this->m_Gbuffer.GetDepthBuffer().SetRenderTargetToShader(2);
			}
			ClearDrawScreenZBuffer();
			CameraParts->GetMainCamera().FlipCamInfo();
			SetCameraNearFar(near_len, far_len);
			{
				done();
			}
			SetRenderTargetToShader(0, InvalidID);
			if (this->m_IsActiveGBuffer) {
				SetRenderTargetToShader(1, InvalidID);
				SetRenderTargetToShader(2, InvalidID);
			}
		}
		void		SetDrawShadow(std::function<void()> setshadowdoing_rigid, std::function<void()> setshadowdoing) noexcept {
			auto* CameraParts = Camera::Camera3D::Instance();
			auto* pOption = Util::OptionParam::Instance();
			// 影
			if (pOption->GetParam(pOption->GetOptionType(Util::OptionType::Shadow))->GetSelect() > 0) {
				// 影画像の用意
				this->m_ShadowDraw->SetDraw(setshadowdoing_rigid, setshadowdoing, CameraParts->GetMainCamera());
				// ソフトシャドウ重ね
				this->m_BufferScreen.SetDraw_Screen(false);
				{
					this->m_ShadowDraw->Draw();
				}
			}
		}
		void		DrawPostProcess(void) noexcept {
			this->m_BufferScreen.SetDraw_Screen(false);
			// 色味補正
			this->m_BufferScreen.GraphFilter(DX_GRAPH_FILTER_LEVEL, this->m_InColorPerMin, this->m_InColorPerMax, static_cast<int>(this->m_InColorGamma * 100), 0, 255);
			PostPassScreenBufferPool::Instance()->FirstUpdate();
			// ポストパスエフェクトのbufに描画
			if (this->m_IsActiveGBuffer) {
				for (auto& P : this->m_PostPass) {
					if (!P) { continue; }
					if (!P->IsActive()) { continue; }
					this->m_Gbuffer.GetColorBuffer().GraphFilterBlt(this->m_BufferScreen, DX_GRAPH_FILTER_DOWN_SCALE, 1);
					P->SetEffect(&this->m_BufferScreen, &this->m_Gbuffer);
				}
			}
		}
	public:
		void		Reset(void) noexcept {
			UpdateActiveGBuffer(false);
			for (auto& P : this->m_PostPass) {
				if (!P) { continue; }
				P->UpdateActive(false);
			}
			UpdateActiveCubeMap(false);
			UpdateBuffer();
			ResetAllParams();
		}
	};
};
