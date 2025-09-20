#pragma once
#define NOMINMAX
#pragma warning(disable:4505)
#pragma warning(disable:4514)
#pragma warning(disable:4668)
#pragma warning(disable:5039)
#pragma warning(disable:5045)
#pragma warning( push, 3 )
#include "DxLib.h"
#pragma warning( pop )

#include "Algorithm.hpp"

// --------------------------------------------------------------------------------------------------
// ÉVÉìÉOÉãÉgÉì
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
// éqÇÃÉTÉìÉvÉã
/*
class A : public SingletonBase<A> {
private:
	friend class SingletonBase<A>;
}
//*/

// --------------------------------------------------------------------------------------------------
// RGBA
// --------------------------------------------------------------------------------------------------
struct ColorRGBA {
	uint32_t				color{};
	char		padding[4]{};
public:
	ColorRGBA(int R, int G, int B, int A) noexcept {
		Set(R, G, B, A);
	}

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
		SetR(R);
		SetG(G);
		SetB(B);
		SetA(A);
	}
public:
	ColorRGBA GetMult(const ColorRGBA& Target) const noexcept {
		ColorRGBA	Color{};
		Color.SetR(static_cast<int>(255.f * (static_cast<float>(this->GetR()) / 255.f * static_cast<float>(Target.GetR()) / 255.f)));
		Color.SetG(static_cast<int>(255.f * (static_cast<float>(this->GetG()) / 255.f * static_cast<float>(Target.GetG()) / 255.f)));
		Color.SetB(static_cast<int>(255.f * (static_cast<float>(this->GetB()) / 255.f * static_cast<float>(Target.GetB()) / 255.f)));
		Color.SetA(static_cast<int>(255.f * (static_cast<float>(this->GetA()) / 255.f * static_cast<float>(Target.GetA()) / 255.f)));
		return Color;
	}
public:
	// î‰är
	inline bool operator==(const ColorRGBA& obj) const noexcept { return (this->color == obj.color); }
	inline bool operator!=(const ColorRGBA& obj) const noexcept { return !(*this == obj); }
	// â¡éZ
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
	// å∏éZ
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
	// èÊéZ
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
	// èúéZ
	inline ColorRGBA operator/(float p1) const noexcept { return *this * (1.f / p1); }
	inline void operator/=(float p1) noexcept { *this *= (1.f / p1); }
public:
	void SetByJson(const std::vector<int>& value) noexcept {
		this->SetA(255);
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

inline ColorRGBA Lerp(const ColorRGBA& A, const ColorRGBA& B, float Per) noexcept {
	ColorRGBA Answer;
	Answer.SetR(static_cast<int>(Lerp(static_cast<float>(A.GetR()), static_cast<float>(B.GetR()), Per)));
	Answer.SetG(static_cast<int>(Lerp(static_cast<float>(A.GetG()), static_cast<float>(B.GetG()), Per)));
	Answer.SetB(static_cast<int>(Lerp(static_cast<float>(A.GetB()), static_cast<float>(B.GetB()), Per)));
	Answer.SetA(static_cast<int>(Lerp(static_cast<float>(A.GetA()), static_cast<float>(B.GetA()), Per)));
	return Answer;
}
