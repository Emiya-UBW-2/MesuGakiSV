#pragma once
#pragma warning(disable:4514)
#pragma warning( push, 3 )
#include <fstream>
#include <string>
#pragma warning( pop )

#define USE_DXLIB (false)

#if USE_DXLIB
#define NOMINMAX
#pragma warning( push, 3 )
#include "DxLib.h"
#pragma warning( pop )
#endif

//�t�@�C���ǂݍ���
class InputFileStream {
#if USE_DXLIB
	int mdata = -1;
#else
	std::ifstream stream{};
#endif
public:
	//�R���X�g���N�^
	InputFileStream(void) noexcept {}
	//�錾���Ƀt�@�C���I�[�v����
	InputFileStream(const char* FilePath) noexcept { Open(FilePath); }

	InputFileStream(const InputFileStream&) = delete;
	InputFileStream(InputFileStream&&) = delete;
	InputFileStream& operator=(const InputFileStream&) = delete;
	InputFileStream& operator=(InputFileStream&&) = delete;

	//�f�X�g���N�^
	~InputFileStream(void) noexcept { Close(); }
public:
	//�t�@�C�����J���A�T���|�C���g���n�_�Ɉړ�
	void Open(const char* FilePath) noexcept {
#if USE_DXLIB
		mdata = DxLib::FileRead_open(FilePath, FALSE);
#else
		stream.open(FilePath);
#endif
	}
	// 1�s���̂܂܎擾���A���̍s�ɒT���|�C���g���ڂ�
	std::string SeekLineAndGetStr(void) noexcept {
#if USE_DXLIB
		const int charLength = 512;
		char mstr[charLength] = "";
		DxLib::FileRead_gets(mstr, charLength, mdata);
		return std::string(mstr);
#else
		std::string Buffer;
		std::getline(stream, Buffer);
		return Buffer;
#endif
	}
	// �T���|�C���g���I�[(EOF)�ŏI���
	bool ComeEof(void) const noexcept {
#if USE_DXLIB
		return DxLib::FileRead_eof(mdata) != 0;
#else
		return stream.eof();
#endif
	}
	//�@����
	void Close(void) noexcept {
#if USE_DXLIB
		if (mdata != -1) {
			DxLib::FileRead_close(mdata);
			mdata = -1;
		}
#else
		stream.close();
#endif
	}
public:
	// �����񂩂�=���E�̒l�擾
	static std::string getleft(std::string tempname, const char* DivWord) noexcept { return tempname.substr(0, tempname.find(DivWord)); }
	// �����񂩂�=���E�̒l�擾
	static std::string getright(std::string tempname, const char* DivWord) noexcept { return tempname.substr(tempname.find(DivWord) + 1); }
};

//�t�@�C���o��
class OutputFileStream {
	std::ofstream stream{};
public:
	//�R���X�g���N�^
	OutputFileStream(void) noexcept {}
	//�錾���Ƀt�@�C���I�[�v����
	OutputFileStream(const char* FilePath) noexcept { Open(FilePath); }

	OutputFileStream(const OutputFileStream&) = delete;
	OutputFileStream(OutputFileStream&&) = delete;
	OutputFileStream& operator=(const OutputFileStream&) = delete;
	OutputFileStream& operator=(OutputFileStream&&) = delete;

	//�f�X�g���N�^
	~OutputFileStream(void) noexcept { Close(); }
public:
	//�t�@�C�����J���A�T���|�C���g���n�_�Ɉړ�
	void Open(const char* FilePath) noexcept {
		stream.open(FilePath);
	}
	// 1�s��������
	void AddLine(std::string Str) noexcept {
		stream << Str + "\n";
	}
	//�@����
	void Close(void) noexcept {
		stream.close();
	}
};
