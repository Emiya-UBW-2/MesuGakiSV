#pragma once
#define NOMINMAX
#pragma warning(disable:4505)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:4710)
#pragma warning(disable:5039)
#pragma warning(disable:5045)
#pragma warning( push, 3 )
#include <vector>
#include <cmath>
#include <algorithm>
#pragma warning( pop )

namespace Util {
	// --------------------------------------------------------------------------------------------------
	// ベクトルデータ型
	// --------------------------------------------------------------------------------------------------
	struct VECTOR2D {
		float					x{}, y{};

	public:
		VECTOR2D(float X, float Y) noexcept {
			this->x = X;
			this->y = Y;
		}

		VECTOR2D(void) noexcept {}
		VECTOR2D(const VECTOR2D& o) noexcept {
			this->x = o.x;
			this->y = o.y;
		}
		VECTOR2D(VECTOR2D&& o) noexcept {
			this->x = o.x;
			this->y = o.y;
		}
		VECTOR2D& operator=(const VECTOR2D& o) noexcept {
			this->x = o.x;
			this->y = o.y;
			return *this;
		}
		VECTOR2D& operator=(VECTOR2D&& o) noexcept {
			this->x = o.x;
			this->y = o.y;
			return *this;
		}
		virtual ~VECTOR2D(void) noexcept {}
	public:
		// 比較
		inline bool operator==(const VECTOR2D& obj) const noexcept { return (this->x == obj.x) && (this->y == obj.y); }
		inline bool operator!=(const VECTOR2D& obj) const noexcept { return !(*this == obj); }
		// 加算
		inline VECTOR2D operator+(const VECTOR2D& obj) const noexcept { return VECTOR2D(this->x + obj.x, this->y + obj.y); }
		inline void operator+=(const VECTOR2D& obj) noexcept {
			this->x += obj.x;
			this->y += obj.y;
		}
		// 減算
		inline VECTOR2D operator-(const VECTOR2D& obj) const noexcept { return VECTOR2D(this->x - obj.x, this->y - obj.y); }
		inline void operator-=(const VECTOR2D& obj) noexcept {
			this->x -= obj.x;
			this->y -= obj.y;
		}
		// 乗算
		inline VECTOR2D operator*(float p1) const noexcept { return VECTOR2D(this->x * p1, this->y * p1); }
		inline void operator*=(float p1) noexcept {
			this->x *= p1;
			this->y *= p1;
		}
		// 除算
		inline VECTOR2D operator/(float p1) const noexcept { return *this * (1.f / p1); }
		inline void operator/=(float p1) noexcept { *this *= (1.f / p1); }
	public:
		inline VECTOR2D			Rotate(float Rad) const noexcept {
			VECTOR2D Answer;
			Answer.x = this->x * std::cos(Rad) - this->y * std::sin(Rad);
			Answer.y = this->x * std::sin(Rad) + this->y * std::cos(Rad);
			return Answer;
		}
	public:
		inline static float			Cross(const VECTOR2D& A, const VECTOR2D& B) noexcept { return A.x * B.y - A.y * B.x; }

	public:
		void SetByJson(const std::vector<float>& value) noexcept {
			size_t loop = 0;
			for (const float& dp : value) {
				if (loop == 0) {
					this->x = dp;
				}
				else if (loop == 1) {
					this->y = dp;
				}
				++loop;
			}
		}
	};

	// ---------------------------------------------------------------------------------------------
	// Vector3
	// ---------------------------------------------------------------------------------------------
	class Vector3DX {
	public:
		float x{ 0 };
		float y{ 0 };
		float z{ 0 };
	public:
		Vector3DX(void) noexcept : x(0), y(0), z(0) {}
		Vector3DX(VECTOR value) noexcept { this->Set(value.x, value.y, value.z); }
		inline VECTOR get(void) const noexcept { return DxLib::VGet(this->x, this->y, this->z); }					// 変換
		inline static const Vector3DX vget(float x, float y, float z) noexcept { return DxLib::VGet(x, y, z); }	// 入力
		// 
		inline static const Vector3DX back(void) noexcept { return vget(0.f, 0.f, -1.f); }
		inline static const Vector3DX down(void) noexcept { return vget(0.f, -1.f, 0.f); }
		inline static const Vector3DX forward(void) noexcept { return vget(0.f, 0.f, 1.f); }
		inline static const Vector3DX left(void) noexcept { return vget(-1.f, 0.f, 0.f); }

		inline static const Vector3DX one(void) noexcept { return vget(1.f, 1.f, 1.f); }

		inline static const Vector3DX right(void) noexcept { return vget(1.f, 0.f, 0.f); }
		inline static const Vector3DX up(void) noexcept { return vget(0.f, 1.f, 0.f); }
		inline static const Vector3DX zero(void) noexcept { return vget(0.f, 0.f, 0.f); }
		// 
		inline float magnitude(void) const noexcept {
			float Square = sqrMagnitude();
			return Square < 0.0000001f ? 0.0f : std::sqrtf(Square);
		}// サイズ
		inline Vector3DX normalized(void) const noexcept {
			float Size = magnitude();
			if (Size <= 0.0f) {
				return vget(-1.0f, -1.0f, -1.0f);
			}
			return *this / Size;
		}		// 正規化
		inline float sqrMagnitude(void) const noexcept { return this->x * this->x + this->y * this->y + this->z * this->z; }		// サイズ
		// 
		inline bool Equals(const Vector3DX& obj) const noexcept { return *this == obj; }
		inline void Set(float x1, float y1, float z1) noexcept {
			this->x = x1;
			this->y = y1;
			this->z = z1;
		}
		// 
		inline static float			Angle(const Vector3DX& A, const Vector3DX& B) noexcept { return std::acos(Dot(A, B) / (A.magnitude() * B.magnitude())); }
		inline static Vector3DX		ClampMagnitude(const Vector3DX& A, float Limit) noexcept { return A.normalized() * std::clamp(A.magnitude(), 0.f, Limit); }
		inline static Vector3DX		Cross(const Vector3DX& A, const Vector3DX& B) noexcept { return vget(A.y * B.z - A.z * B.y, A.z * B.x - A.x * B.z, A.x * B.y - A.y * B.x); }
		inline static float			Distance(const Vector3DX& A, const Vector3DX& B) noexcept { return (A - B).magnitude(); }
		inline static float			Dot(const Vector3DX& A, const Vector3DX& B) noexcept { return A.x * B.x + A.y * B.y + A.z * B.z; }
		//static Vector3DX			LerpVector3DX(const Vector3DX& A, const Vector3DX& B, float Per) noexcept { return Lerp<Vector3DX>(A, B, Per); }
		//static Vector3DX			LerpUnclampedVector3DX(const Vector3DX& A, const Vector3DX& B, float Per) noexcept { return Lerp<Vector3DX>(A, B, Per); }
		inline static Vector3DX		Max(const Vector3DX& A, const Vector3DX& B) noexcept { return vget(std::max(A.x, B.x), std::max(A.y, B.y), std::max(A.z, B.z)); }
		inline static Vector3DX		Min(const Vector3DX& A, const Vector3DX& B) noexcept { return vget(std::min(A.x, B.x), std::min(A.y, B.y), std::min(A.z, B.z)); }
		// MoveTowards
		inline static Vector3DX		Normalize(const Vector3DX& A) noexcept { return A.normalized(); }
		// OrthoNormalize
		// Project
		// ProjectOnPlane
		inline static Vector3DX		Reflect(const Vector3DX& inDirection, const Vector3DX& inNormal) noexcept { return inDirection + inNormal * (Dot(inNormal, inDirection * -1.f)) * 2.f; }
		// RotateTowards
		inline static Vector3DX		Scale(const Vector3DX& A, const Vector3DX& B) noexcept { return vget((A.x * B.x), (A.y * B.y), (A.z * B.z)); }
		inline static float			SignedAngle(const Vector3DX& A, const Vector3DX& B, const Vector3DX& Axis) noexcept { return Angle(A, B) * (Dot(Cross(Axis, A), B) > 0.f ? 1.f : -1.f); }
		// Slerp
		// SlerpUnclamped
		// SmoothDamp

		// 比較
		inline bool operator==(const Vector3DX& obj) const noexcept { return (*this - obj).magnitude() <= 0.001f; }
		inline bool operator!=(const Vector3DX& obj) const noexcept { return !(*this == obj); }
		// 加算
		inline Vector3DX operator+(const Vector3DX& obj) const noexcept { return vget(this->x + obj.x, this->y + obj.y, this->z + obj.z); }
		inline void operator+=(const Vector3DX& obj) noexcept {
			this->x += obj.x;
			this->y += obj.y;
			this->z += obj.z;
		}
		// 減算
		inline Vector3DX operator-(const Vector3DX& obj) const noexcept { return vget(this->x - obj.x, this->y - obj.y, this->z - obj.z); }
		inline void operator-=(const Vector3DX& obj) noexcept {
			this->x -= obj.x;
			this->y -= obj.y;
			this->z -= obj.z;
		}
		// 乗算
		inline Vector3DX operator*(float p1) const noexcept { return vget(this->x * p1, this->y * p1, this->z * p1); }
		inline void operator*=(float p1) noexcept {
			this->x *= p1;
			this->y *= p1;
			this->z *= p1;
		}
		// 除算
		inline Vector3DX operator/(float p1) const noexcept { return *this * (1.f / p1); }
		inline void operator/=(float p1) noexcept { *this *= (1.f / p1); }

		//
		inline bool IsRangeSmaller(float Lange) const noexcept {
			if (std::abs(this->x) + std::abs(this->y) + std::abs(this->z) > Lange * 3) {
				return false;
			}

			return this->sqrMagnitude() < Lange * Lange;
		}
	};

	// ---------------------------------------------------------------------------------------------
	// Matrix 4x4バージョン
	// ---------------------------------------------------------------------------------------------
	class Matrix4x4DX {
		MATRIX value;
	public:
		Matrix4x4DX(void) noexcept : value(DxLib::MGetIdent()) {}
		Matrix4x4DX(MATRIX value) noexcept { this->value = value; }
		MATRIX get(void) const noexcept { return this->value; }		// 変換
		// 
		static Matrix4x4DX identity(void) noexcept { return DxLib::MGetIdent(); }

		static Matrix4x4DX Axis1(const Vector3DX& yvec, const Vector3DX& zvec, const Vector3DX& pos = Vector3DX::zero()) noexcept { return { DxLib::MGetAxis1(Vector3DX::Cross(yvec, zvec).get(),yvec.get(),zvec.get(),pos.get()) }; }
		static Matrix4x4DX Axis2(const Vector3DX& yvec, const Vector3DX& zvec, const Vector3DX& pos = Vector3DX::zero()) noexcept { return { DxLib::MGetAxis2(Vector3DX::Cross(yvec, zvec).get(),yvec.get(),zvec.get(),pos.get()) }; }
		static Matrix4x4DX RotAxis(const Vector3DX& p1, float p2) noexcept { return DxLib::MGetRotAxis(p1.get(), p2); }
		static Matrix4x4DX RotVec2(const Vector3DX& p1, const Vector3DX& p2) noexcept { return { DxLib::MGetRotVec2(p1.get(), p2.get()) }; }
		static Matrix4x4DX GetScale(float scale) noexcept { return { DxLib::MGetScale(DxLib::VGet(scale,scale,scale)) }; }
		static Matrix4x4DX GetScale(const Vector3DX& scale) noexcept { return { DxLib::MGetScale(scale.get()) }; }
		static Matrix4x4DX Mtrans(const Vector3DX& p1) noexcept { return DxLib::MGetTranslate(p1.get()); }
		static Vector3DX Vtrans(const Vector3DX& p1, const Matrix4x4DX& p2) noexcept { return DxLib::VTransform(p1.get(), p2.get()); }
		// 
		Matrix4x4DX inverse(void) const noexcept { return DxLib::MInverse(this->get()); }		// 逆
		bool isIdentity(void) const noexcept { return *this == DxLib::MGetIdent(); }		// 逆
		// lossyScale
		Vector3DX lossyScale(void) const noexcept { return DxLib::MGetSize(this->get()); }
		Matrix4x4DX rotation(void) const noexcept { return DxLib::MGetRotElem(this->get()); }		// 逆
		Matrix4x4DX transpose(void) const noexcept { return DxLib::MTranspose(this->get()); }		// 逆

		Vector3DX pos(void) const noexcept { return Vtrans(Vector3DX::zero(), this->get()); }
		Vector3DX xvec(void) const noexcept { return Vtrans(Vector3DX::right(), rotation()); }
		Vector3DX yvec(void) const noexcept { return Vtrans(Vector3DX::up(), rotation()); }
		Vector3DX zvec(void) const noexcept { return Vtrans(Vector3DX::forward(), rotation()); }
		Vector3DX zvec2(void) const noexcept { return Vtrans(Vector3DX::back(), rotation()); }//左手座標系で右手座標系のキャラを描画した際の正面
		// 
		void GetRadian(float* angle_x, float* angle_y, float* angle_z) const noexcept {
			constexpr float threshold = 0.001f;
			if (std::abs(value.m[1][2] - 1.0f) < threshold) { // R(2,1) = sin(x) = 1の時
				if (angle_x) {
					*angle_x = DX_PI_F / 2.f;
				}
				if (angle_y) {
					*angle_y = 0.f;
				}
				if (angle_z) {
					*angle_z = std::atan2f(value.m[0][1], value.m[0][0]);
				}
			}
			else if (std::abs(value.m[1][2] + 1.0f) < threshold) { // R(2,1) = sin(x) = -1の時
				if (angle_x) {
					*angle_x = -DX_PI_F / 2.f;
				}
				if (angle_y) {
					*angle_y = 0.f;
				}
				if (angle_z) {
					*angle_z = std::atan2f(value.m[0][1], value.m[0][0]);
				}
			}
			else {
				if (angle_x) {
					*angle_x = std::asinf(value.m[1][2]);
				}
				if (angle_y) {
					*angle_y = std::atan2f(-value.m[0][2], value.m[2][2]);
				}
				if (angle_z) {
					*angle_z = std::atan2f(-value.m[1][0], value.m[1][1]);
				}
			}
		}
		void SetRadian(float x, float y, float z) noexcept {
			value.m[0][0] = std::cos(y) * std::cos(z) - std::sin(x) * std::sin(y) * std::sin(z);
			value.m[1][0] = -std::cos(x) * std::sin(z);
			value.m[2][0] = std::sin(y) * std::cos(z) + std::sin(x) * std::cos(y) * std::sin(z);
			value.m[0][1] = std::cos(y) * std::sin(z) + std::sin(x) * std::sin(y) * std::cos(z);
			value.m[1][1] = std::cos(x) * std::cos(z);
			value.m[2][1] = std::sin(y) * std::sin(z) - std::sin(x) * std::cos(y) * std::cos(z);
			value.m[0][2] = -std::cos(x) * std::sin(y);
			value.m[1][2] = std::sin(x);
			value.m[2][2] = std::cos(x) * std::cos(y);
		}
		// 比較
		bool operator==(const Matrix4x4DX& obj) const noexcept {
			for (int x = 0; x < 4; ++x) {
				for (int y = 0; y < 4; ++y) {
					if (this->get().m[x][y] != obj.get().m[x][y]) {
						return false;
					}
				}
			}
			return true;


		}
		bool operator!=(const Matrix4x4DX& obj) const noexcept { return !(*this == obj); }
		// 乗算
		Matrix4x4DX operator*(const Matrix4x4DX& obj)  const noexcept { return DxLib::MMult(this->get(), obj.get()); }
		void operator*=(const Matrix4x4DX& obj) noexcept { *this = *this * obj; }
	};

	// ---------------------------------------------------------------------------------------------
	// Vector3
	// ---------------------------------------------------------------------------------------------

	// 点と矩形との2D判定
	static bool HitPointToRectangle(int xp, int yp, int x1, int y1, int x2, int y2) noexcept { return (xp >= x1 && xp <= x2 && yp >= y1 && yp <= y2); }
	// 矩形と矩形との2D判定
	static bool HitRectangleToRectangle(int xp1, int yp1, int xp2, int yp2, int x1, int y1, int x2, int y2) noexcept { return (xp1 < x2 && x1 < xp2) && (yp1 < y2 && y1 < yp2); }
	// 点と四角形との2D判定
	static bool HitPointToSquare(VECTOR2D point, VECTOR2D c1, VECTOR2D c2, VECTOR2D c3, VECTOR2D c4) noexcept {
		if (0 > VECTOR2D::Cross(c2 - c1, point - c1)) { return false; }
		if (0 > VECTOR2D::Cross(c3 - c2, point - c2)) { return false; }
		if (0 > VECTOR2D::Cross(c4 - c3, point - c3)) { return false; }
		if (0 > VECTOR2D::Cross(c1 - c4, point - c4)) { return false; }
		return true;
	}

	//	o1 Angleが0の場合の左上座標
	//	o2 Angleが0の場合の右下座標
	//	minp 左上角からの固定長さ
	//	maxp 右下角からの固定長さ
	//	Center　　: 画像を回転描画する画像上の中心座標(左上を(0.0f,0.0f)、右下を(1.0f,1.0f)とした割合)
	//	Angle　　　: 描画角度（ラジアン指定）
	//	GrHandle　 : 描画するグラフィックの識別番号（グラフィックハンドル）
	//	TransFlag　: 画像の透明度を有効にするかどうか( TRUE：有効にする　FALSE：無効にする )
	//	TilingFlag : 角以外の部分をタイリングするか拡縮させるか( TRUE：タイリング　FALSE：拡縮 )
	static void Draw9SliceGraph(
		VECTOR2D o1, VECTOR2D o2,
		VECTOR2D minp, VECTOR2D maxp,
		VECTOR2D Center, float Angle,
		int GrHandle, bool TransFlag, bool TilingFlag) noexcept {
		//最低限のサイズを指定
		if (o2.x < o1.x + minp.x + maxp.x) { o2.x = o1.x + minp.x + maxp.x; }
		if (o2.y < o1.y + minp.y + maxp.y) { o2.y = o1.y + minp.y + maxp.y; }
		//用意する頂点情報
		std::vector<VERTEX2D> Vertex;
		std::vector<unsigned short> Index;

		float xs = (o2.x - o1.x);
		float ys = (o2.y - o1.y);

		VECTOR2D center;
		center.x = o1.x + xs * Center.x;
		center.y = o1.y + ys * Center.y;

		auto SetPoint = [&](float xper, float yper, int xc, int yc) {
			Vertex.resize(Vertex.size() + 1);
			VECTOR2D ofs;
			ofs.x = o1.x + xs * xper;
			ofs.y = o1.y + ys * yper;
			VECTOR2D pos = center + (ofs - center).Rotate(Angle);
			Vertex.back().pos = VGet(pos.x, pos.y, 0.f);

			Vertex.back().rhw = 1.0f;
			Vertex.back().dif = GetColorU8(255, 255, 255, 255);
			Vertex.back().u = static_cast<float>(xc) / 3.f;
			Vertex.back().v = static_cast<float>(yc) / 3.f;
			return (unsigned short)(Vertex.size() - 1);
			};
		auto SetBox = [&](float xmin, float ymin, float xmax, float ymax, int xc, int yc) {
			Index.emplace_back(SetPoint(xmin, ymin, xc, yc));// 左上の頂点の情報をセット
			auto RU = SetPoint(xmax, ymin, xc + 1, yc);
			auto LD = SetPoint(xmin, ymax, xc, yc + 1);
			Index.emplace_back(RU);// 右上の頂点の情報をセット
			Index.emplace_back(LD);// 左下の頂点の情報をセット
			Index.emplace_back(SetPoint(xmax, ymax, xc + 1, yc + 1));// 右下の頂点の情報をセット
			Index.emplace_back(LD);// 左下の頂点の情報をセット
			Index.emplace_back(RU);// 右上の頂点の情報をセット
			};

		float xminpt = minp.x / xs;
		float xmaxpt = maxp.x / xs;
		float xmaxt = 1.f - xmaxpt;
		float xmidt = xmaxt - xminpt;

		float yminpt = minp.y / ys;
		float ymaxpt = maxp.y / ys;
		float ymaxt = 1.f - ymaxpt;
		float ymidt = ymaxt - yminpt;

		int xtile = 1;
		int ytile = 1;
		//タイリング
		if (TilingFlag) {
			xtile = (int)(xmidt / ((xminpt + xmaxpt) / 2.f)) + 1;
			if (xtile <= 0) { xtile = 1; }
			ytile = (int)(ymidt / ((yminpt + ymaxpt) / 2.f)) + 1;
			if (ytile <= 0) { ytile = 1; }
		}

		Vertex.reserve((size_t)(3 * 2 * ((xtile + 2) * (ytile + 2))));
		float xmin = 0.f;
		float xmax = xminpt;
		int xc = 0;
		for (int x = 0; x < xtile + 2; x++) {
			float ymin = 0.f;
			float ymax = yminpt;
			int yc = 0;
			for (int y = 0; y < ytile + 2; y++) {
				SetBox(xmin, ymin, xmax, ymax, xc, yc);
				//次
				ymin = ymax;
				ymax = TilingFlag ? (ymin + ymidt / static_cast<float>(ytile)) : ymaxt;
				if (y == 0) {
					yc = 1;
				}
				if (y == ytile) {
					ymax = 1.f;
					yc = 2;
				}
			}
			//次
			xmin = xmax;
			xmax = TilingFlag ? (xmin + xmidt / static_cast<float>(xtile)) : xmaxt;
			if (x == 0) {
				xc = 1;
			}
			if (x == xtile) {
				xmax = 1.f;
				xc = 2;
			}
		}
		DrawPolygonIndexed2D(Vertex.data(), (int)Vertex.size(), Index.data(), (int)Index.size() / 3, GrHandle, TransFlag ? TRUE : FALSE);
	}

	// 線形補完
	template <class T>
	inline T Lerp(const T& A, const T& B, float Per) noexcept {
		if (Per == 0.f) {
			return A;
		}
		else if (Per == 1.f) {
			return B;
		}
		else {
			return A + (T)((B - A) * Per);
		}
	}

	// --------------------------------------------------------------------------------------------------
	// 角度変換
	// --------------------------------------------------------------------------------------------------
	// 角度からラジアンに
	extern void* enabler;// ダミー変数
	template <class T, typename std::enable_if<std::is_arithmetic<T>::value>::type*& = enabler>
	constexpr float deg2rad(T p1) noexcept { return static_cast<float>(p1) * DX_PI_F / 180.f; }
	// ラジアンから角度に
	template <class T, typename std::enable_if<std::is_arithmetic<T>::value>::type*& = enabler>
	constexpr float rad2deg(T p1) noexcept { return static_cast<float>(p1) * 180.f / DX_PI_F; }
}