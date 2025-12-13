#pragma once

#define NOMINMAX
#include "DxLib.h"

#include <cstdint>
#include <string>
#include <filesystem>

#include "Enum.hpp"

namespace Util {
	/// <summary>
	/// 常駐するシングルトンパターン用の基底クラス
	/// </summary>
	/// <typeparam name="T">対象のクラス</typeparam>
	template <class T>
	class SingletonBase {
	private:
		static const T*	m_Singleton;
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
	private:
		A(){}
		~A(){}
	public:
	}
	//*/

	// --------------------------------------------------------------------------------------------------
	// おまけ
	// --------------------------------------------------------------------------------------------------
	//UTF8をShiftJISに変換する
	static std::string		UTF8toSjis(const std::string& srcUTF8) {
		//Unicodeへ変換後の文字列長を得る
		int lenghtUnicode = static_cast<int>(MultiByteToWideChar(CP_UTF8, 0, srcUTF8.c_str(), static_cast<int>(srcUTF8.size()) + 1, nullptr, 0));

		//必要な分だけUnicode文字列のバッファを確保
		wchar_t* bufUnicode = new wchar_t[static_cast<size_t>(lenghtUnicode)];

		//UTF8からUnicodeへ変換
		MultiByteToWideChar(CP_UTF8, 0, srcUTF8.c_str(), static_cast<int>(srcUTF8.size()) + 1, bufUnicode, lenghtUnicode);

		//ShiftJISへ変換後の文字列長を得る
		int lengthSJis = WideCharToMultiByte(CP_THREAD_ACP, 0, bufUnicode, -1, nullptr, 0, nullptr, nullptr);

		//必要な分だけShiftJIS文字列のバッファを確保
		char* bufShiftJis = new char[static_cast<size_t>(lengthSJis)];

		//UnicodeからShiftJISへ変換
		WideCharToMultiByte(CP_THREAD_ACP, 0, bufUnicode, lenghtUnicode + 1, bufShiftJis, lengthSJis, nullptr, nullptr);

		std::string strSJis(bufShiftJis);

		delete[] bufUnicode;
		delete[] bufShiftJis;

		return strSJis;
	}
	//ShiftJISをUTF8に変換する
	static std::string		SjistoUTF8(const std::string& srcSjis) {
		std::wstring wide;
		{
			int lengthSJis = MultiByteToWideChar(CP_ACP, 0U, srcSjis.c_str(), -1, nullptr, 0U);
			std::vector<wchar_t> dest(static_cast<size_t>(lengthSJis), L'\0');
			MultiByteToWideChar(CP_ACP, 0, srcSjis.c_str(), -1, dest.data(), static_cast<int>(dest.size()));
			dest.resize(std::char_traits<wchar_t>::length(dest.data()));
			dest.shrink_to_fit();
			wide = std::wstring(dest.begin(), dest.end());
		}
		std::string ret;
		{
			int lenghtUnicode = static_cast<int>(WideCharToMultiByte(CP_UTF8, 0U, wide.c_str(), -1, nullptr, 0, nullptr, nullptr));
			std::vector<char> dest(static_cast<size_t>(lenghtUnicode), '\0');
			WideCharToMultiByte(CP_UTF8, 0U, wide.c_str(), -1, dest.data(), static_cast<int>(dest.size()), nullptr, nullptr);
			dest.resize(std::char_traits<char>::length(dest.data()));
			dest.shrink_to_fit();
			ret = std::string(dest.begin(), dest.end());
		}
		return ret;
	}
	// ディレクトリ内のファイルを走査してリスト化
	static void GetFileNamesInDirectory(const char* pPath, std::vector<WIN32_FIND_DATA>* pData) noexcept {
		pData->clear();
		WIN32_FIND_DATA win32fdt;
		HANDLE hFind = FindFirstFile(pPath, &win32fdt);
		if (hFind != INVALID_HANDLE_VALUE) {
			do {
				if (win32fdt.cFileName[0] != '.') {//カレントディレクトリまたは一つ上のディレクトリを指すので省く
					pData->emplace_back(win32fdt);
				}

			} while (FindNextFile(hFind, &win32fdt));
		} // else{ return false; }
		FindClose(hFind);
	}
	// ファイルが存在するかチェック
	static bool IsFileExist(const char* Path) noexcept {
		return std::filesystem::is_regular_file(Path);
		//下記はDXLIB_Initを通ってからでないと動作しない
		/*
		FILEINFO FileInfo;
		return (FileRead_findFirst(Path, &FileInfo) != (DWORD_PTR)InvalidID);
		*/
	}
}

namespace ColorPalette {

	// 固定でA8R8G8B8を出す
	constexpr uint32_t GetColorFix(uint8_t Red, uint8_t Green, uint8_t Blue) {
		return 0xff000000 | (((uint32_t)Red) << 16) | (((uint32_t)Green) << 8) | ((uint32_t)Blue);
	}
	// カラーパレット
	static constexpr unsigned int Red{ GetColorFix(255, 0, 0) };
	static constexpr unsigned int Red25{ GetColorFix(192, 0, 0) };
	static constexpr unsigned int Red50{ GetColorFix(128, 0, 0) };
	static constexpr unsigned int Green{ GetColorFix(0, 255, 0) };// GetColorFix(43, 255, 91)
	static constexpr unsigned int DarkGreen{ GetColorFix(0, 64, 0) };// GetColorFix(21, 128, 45)
	static constexpr unsigned int UIGreen{ GetColorFix(0, 192, 0) };
	static constexpr unsigned int UIDarkGreen{ GetColorFix(0, 32, 0) };
	static constexpr unsigned int Blue{ GetColorFix(0, 0, 255) };
	static constexpr unsigned int Yellow{ GetColorFix(255, 255, 0) };

	static constexpr unsigned int White{ GetColorFix(255, 255, 255) };
	static constexpr unsigned int Gray05{ GetColorFix(255 * 95 / 100, 255 * 95 / 100, 255 * 95 / 100) };
	static constexpr unsigned int Gray10{ GetColorFix(255 * 90 / 100, 255 * 90 / 100, 255 * 90 / 100) };
	static constexpr unsigned int Gray15{ GetColorFix(255 * 85 / 100, 255 * 85 / 100, 255 * 85 / 100) };
	static constexpr unsigned int Gray20{ GetColorFix(255 * 80 / 100, 255 * 80 / 100, 255 * 80 / 100) };
	static constexpr unsigned int Gray25{ GetColorFix(255 * 75 / 100, 255 * 75 / 100, 255 * 75 / 100) };
	static constexpr unsigned int Gray30{ GetColorFix(255 * 70 / 100, 255 * 70 / 100, 255 * 70 / 100) };
	static constexpr unsigned int Gray35{ GetColorFix(255 * 65 / 100, 255 * 65 / 100, 255 * 65 / 100) };
	static constexpr unsigned int Gray40{ GetColorFix(255 * 60 / 100, 255 * 60 / 100, 255 * 60 / 100) };
	static constexpr unsigned int Gray45{ GetColorFix(255 * 55 / 100, 255 * 55 / 100, 255 * 55 / 100) };
	static constexpr unsigned int Gray50{ GetColorFix(255 * 50 / 100, 255 * 50 / 100, 255 * 50 / 100) };
	static constexpr unsigned int Gray55{ GetColorFix(255 * 45 / 100, 255 * 45 / 100, 255 * 45 / 100) };
	static constexpr unsigned int Gray60{ GetColorFix(255 * 40 / 100, 255 * 40 / 100, 255 * 40 / 100) };
	static constexpr unsigned int Gray65{ GetColorFix(255 * 35 / 100, 255 * 35 / 100, 255 * 35 / 100) };
	static constexpr unsigned int Gray70{ GetColorFix(255 * 30 / 100, 255 * 30 / 100, 255 * 30 / 100) };
	static constexpr unsigned int Gray75{ GetColorFix(255 * 25 / 100, 255 * 25 / 100, 255 * 25 / 100) };
	static constexpr unsigned int Gray80{ GetColorFix(255 * 20 / 100, 255 * 20 / 100, 255 * 20 / 100) };
	static constexpr unsigned int Gray85{ GetColorFix(255 * 15 / 100, 255 * 15 / 100, 255 * 15 / 100) };
	static constexpr unsigned int Gray90{ GetColorFix(255 * 10 / 100, 255 * 10 / 100, 255 * 10 / 100) };
	static constexpr unsigned int Gray95{ GetColorFix(255 * 5 / 100, 255 * 5 / 100, 255 * 5 / 100) };
	static constexpr unsigned int Black{ GetColorFix(0, 0, 0) };
}
