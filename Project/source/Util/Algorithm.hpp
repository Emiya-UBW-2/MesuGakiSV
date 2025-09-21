#pragma once
#define NOMINMAX
#pragma warning(disable:4505)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:5039)
#pragma warning(disable:5045)
#pragma warning( push, 3 )
#include <vector>
#include <cmath>
#include <algorithm>
#pragma warning( pop )

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
