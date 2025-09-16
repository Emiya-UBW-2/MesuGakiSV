#pragma once
#define NOMINMAX
#pragma warning(disable:4505)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:5039)
#pragma warning(disable:5045)
#pragma warning( push, 3 )
#include "DxLib.h"
#include <cstdint>
#include <vector>
#include <algorithm>
#include <cmath>
#pragma warning( pop )

// --------------------------------------------------------------------------------------------------
// シングルトン
// --------------------------------------------------------------------------------------------------
template <class T>
class SingletonBase {
private:
	static const T* m_Singleton;
public:
	static void Create(void) noexcept {
		m_Singleton = new T();
	}
	static T* Instance(void) noexcept {
#if _DEBUG
		if (m_Singleton == nullptr) {
			MessageBox(NULL, "Failed Instance Create", "", MB_OK);
			exit(-1);
		}
#endif
		// if (m_Singleton == nullptr) { m_Singleton = new T(); }
		return (T*)m_Singleton;
	}
	static void Release(void) noexcept {
		delete m_Singleton;
	}
protected:
	SingletonBase(void) noexcept {}
	virtual ~SingletonBase(void) noexcept {}
private:
	SingletonBase(const SingletonBase&) = delete;
	SingletonBase& operator=(const SingletonBase&) = delete;
	SingletonBase(SingletonBase&&) = delete;
	SingletonBase& operator=(SingletonBase&&) = delete;
};
// 子のサンプル
/*
class A : public SingletonBase<A> {
private:
	friend class SingletonBase<A>;
}
//*/

// --------------------------------------------------------------------------------------------------
// ベクトルデータ型
// --------------------------------------------------------------------------------------------------
struct VECTOR2D
{
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

// --------------------------------------------------------------------------------------------------
// RGBA
// --------------------------------------------------------------------------------------------------
struct ColorRGBA
{
	uint32_t				color{};

public:
	ColorRGBA(void) noexcept {}
	ColorRGBA(const ColorRGBA& o) noexcept {
		this->color = o.color;
	}
	ColorRGBA(ColorRGBA&& o) noexcept {
		this->color = o.color;
	}
	ColorRGBA& operator=(const ColorRGBA& o) noexcept {
		this->color = o.color;
		return *this;
	}
	ColorRGBA& operator=(ColorRGBA&& o) noexcept {
		this->color = o.color;
		return *this;
	}
	virtual ~ColorRGBA(void) noexcept {}
public:
	int GetA(void) const noexcept { return static_cast<int>((color & 0xff000000) >> (8 * 3)); }
	int GetR(void) const noexcept { return static_cast<int>((color & 0x00ff0000) >> (8 * 2)); }
	int GetG(void) const noexcept { return static_cast<int>((color & 0x0000ff00) >> (8 * 1)); }
	int GetB(void) const noexcept { return static_cast<int>((color & 0x000000ff) >> (8 * 0)); }

	uint32_t Get(void) const noexcept { return color; }

	unsigned int GetColor(void) const noexcept { return	DxLib::GetColor(GetR(), GetG(), GetB()); }
public:
	void SetA(int value) noexcept { color = (color & 0x00ffffff) | (static_cast<uint32_t>(std::clamp(value, 0, 255)) << (8 * 3)); }
	void SetR(int value) noexcept { color = (color & 0xff00ffff) | (static_cast<uint32_t>(std::clamp(value, 0, 255)) << (8 * 2)); }
	void SetG(int value) noexcept { color = (color & 0xffff00ff) | (static_cast<uint32_t>(std::clamp(value, 0, 255)) << (8 * 1)); }
	void SetB(int value) noexcept { color = (color & 0xffffff00) | (static_cast<uint32_t>(std::clamp(value, 0, 255)) << (8 * 0)); }

	void Set(int R, int G, int B, int A) noexcept {
		SetA(R);
		SetR(G);
		SetG(B);
		SetB(A);
	}
public:
	// 比較
	inline bool operator==(const ColorRGBA& obj) const noexcept { return (this->color == obj.color); }
	inline bool operator!=(const ColorRGBA& obj) const noexcept { return !(*this == obj); }
	// 加算
	inline ColorRGBA operator+(const ColorRGBA& obj) const noexcept {
		ColorRGBA Answer;
		Answer.SetR(this->GetR() + obj.GetR());
		Answer.SetG(this->GetG() + obj.GetG());
		Answer.SetB(this->GetB() + obj.GetB());
		Answer.SetA(this->GetA() + obj.GetA());
		return Answer;
	}
	inline void operator+=(const ColorRGBA& obj) noexcept {
		(*this) = (*this) + obj;
	}
	// 減算
	inline ColorRGBA operator-(const ColorRGBA& obj) const noexcept {
		ColorRGBA Answer;
		Answer.SetR(this->GetR() - obj.GetR());
		Answer.SetG(this->GetG() - obj.GetG());
		Answer.SetB(this->GetB() - obj.GetB());
		Answer.SetA(this->GetA() - obj.GetA());
		return Answer;
	}
	inline void operator-=(const ColorRGBA& obj) noexcept {
		(*this) = (*this) - obj;
	}
	// 乗算
	inline ColorRGBA operator*(float p1) const noexcept {
		ColorRGBA Answer;
		Answer.SetR(static_cast<int>(static_cast<float>(this->GetR()) * p1));
		Answer.SetG(static_cast<int>(static_cast<float>(this->GetG()) * p1));
		Answer.SetB(static_cast<int>(static_cast<float>(this->GetB()) * p1));
		Answer.SetA(static_cast<int>(static_cast<float>(this->GetA()) * p1));
		return Answer;
	}
	inline void operator*=(float p1) noexcept {
		(*this) = (*this) * p1;
	}
	// 除算
	inline ColorRGBA operator/(float p1) const noexcept { return *this * (1.f / p1); }
	inline void operator/=(float p1) noexcept { *this *= (1.f / p1); }
public:
	void SetByJson(const std::vector<int>& value) noexcept {
		size_t loop = 0;
		for (const int& dp : value) {
			if (loop == 0) {
				this->SetR(dp);
			}
			else if (loop == 1) {
				this->SetG(dp);
			}
			else if (loop == 2) {
				this->SetB(dp);
			}
			else if (loop == 3) {
				this->SetA(dp);
			}
			++loop;
		}
	}
};


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

	float CenterX = o1.x + xs * Center.x;
	float CenterY = o1.y + ys * Center.y;

	auto SetPoint = [&](float xper, float yper, int xc, int yc) {
		Vertex.resize(Vertex.size() + 1);
		Vertex.back().pos = VGet(
			o1.x + xs * xper - CenterX,
			o1.y + ys * yper - CenterY,
			0.f);

		Vertex.back().pos = VGet(
			CenterX + Vertex.back().pos.x * std::cos(Angle) - Vertex.back().pos.y * std::sin(Angle),
			CenterY + Vertex.back().pos.x * std::sin(Angle) + Vertex.back().pos.y * std::cos(Angle),
			0.f);

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

inline ColorRGBA Lerp(const ColorRGBA& A, const ColorRGBA& B, float Per) noexcept {
	ColorRGBA Answer;
	Answer.SetR(static_cast<int>(Lerp(static_cast<float>(A.GetR()), static_cast<float>(B.GetR()), Per)));
	Answer.SetG(static_cast<int>(Lerp(static_cast<float>(A.GetG()), static_cast<float>(B.GetG()), Per)));
	Answer.SetB(static_cast<int>(Lerp(static_cast<float>(A.GetB()), static_cast<float>(B.GetB()), Per)));
	Answer.SetA(static_cast<int>(Lerp(static_cast<float>(A.GetA()), static_cast<float>(B.GetA()), Per)));
	return Answer;
}
