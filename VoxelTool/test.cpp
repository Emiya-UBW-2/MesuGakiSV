#include "DxLib.h"
#include	"BackGroundSub.hpp"

#include <algorithm>
#include <vector>
#include <fstream>
#include <filesystem>

class DX {
	int ShadowMapHandle = -1;
	VECTOR ShadowVec = VGet(0.1f, -0.3f, -0.1f);
public:
	DX() {
		DxLib::SetGraphMode(1280, 1000, 32);
		DxLib_Init();												// �c�w���C�u��������������
		ChangeWindowMode(TRUE);										// �E�B���h�E���[�h
		SetAlwaysRunFlag(TRUE);										// ��A�N�e�B�u�ł������悤�ɂ���
		SetMouseDispFlag(TRUE);
		ShadowMapHandle = MakeShadowMap(2048, 2048);				// �V���h�E�}�b�v�n���h���̍쐬
		SetLightDirection(ShadowVec);								// ���C�g�̕�����ݒ�
		SetShadowMapLightDirection(ShadowMapHandle, ShadowVec);		// �V���h�E�}�b�v���z�肷�郉�C�g�̕������Z�b�g
	}
	~DX() {
		DeleteShadowMap(ShadowMapHandle);	// �V���h�E�}�b�v�̍폜
		DxLib_End();				// �c�w���C�u�����g�p�̏I������
	}
public:
	VECTOR GetShadowVec() { return ShadowVec; }
public:
	void SetShadowMapStart(VECTOR CamPos) {
		SetShadowMapDrawArea(ShadowMapHandle, VAdd(CamPos, VGet(-50.0f, -50.0f, -50.0f)), VAdd(CamPos, VGet(50.0f, 0.0f, 50.0f)));
		ShadowMap_DrawSetup(ShadowMapHandle);		// �V���h�E�}�b�v�ւ̕`��̏���
	}
	void SetShadowMapEnd() {
		ShadowMap_DrawEnd();						// �V���h�E�}�b�v�ւ̕`����I��
	}
	void SetUseShadowMapStart() {
		SetUseShadowMap(0, ShadowMapHandle);	// �`��ɃV���h�E�}�b�v���g�p����
	}
	void SetUseShadowMapEnd() {
		SetUseShadowMap(0, -1);					// �`��Ɏg�p����V���h�E�}�b�v�̐ݒ������
	}
};

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	BackGround::VoxelControl Voxel;
	VECTOR MyPos = VGet(0.f, 0.f, 0.f);
	VECTOR MyVec = VGet(0.f, 0.f, 0.f);
	float Xrad = -45.f * DX_PI_F / 180.f;
	float Yrad = 0.f;
	float Speed = 0.f;
	// ������
	DX DXParam;

	// �ǂݍ���
	Voxel.Load();												// ���O�ǂݍ���

	Voxel.InitStart();											// �������J�n������
	//Voxel.LoadCellsFile("data/Map.txt");						// �{�N�Z���f�[�^�̓ǂݍ���
	{
		int XSize{}, YSize{};
		int SoftImage1F = LoadSoftImage("data/map1F.bmp");
		int SoftImage1FColor = LoadSoftImage("data/map1FColor.bmp");
		GetSoftImageSize(SoftImage1F, &XSize, &YSize);
		for (int Xvoxel = 0; Xvoxel < Voxel.GetReferenceCells().All; ++Xvoxel) {
			for (int Zvoxel = 0; Zvoxel < Voxel.GetReferenceCells().All; ++Zvoxel) {
				int xp = XSize * Xvoxel / (Voxel.GetReferenceCells().All - 1);
				int yp = YSize - YSize * Zvoxel / (Voxel.GetReferenceCells().All - 1);
				int r = 0;
				GetPixelSoftImage(SoftImage1F, xp, yp, &r, nullptr, nullptr, nullptr);
				int ID = 1;
				{
					int cr = 0;
					int cg = 0;
					int cb = 0;
					GetPixelSoftImage(SoftImage1FColor, xp, yp, &cr, &cg, &cb, nullptr);
					if (cr == 255 && cg == 0 && cb == 0) {
						ID = 4;
					}
					else if (cr == 255 && cg == 255 && cb == 0) {
						ID = 3;
					}
				}

				if (r == 0) {
					for (int Yvoxel = 0; Yvoxel < Voxel.GetReferenceCells().All-52; ++Yvoxel) {
						Voxel.SetBlick(Xvoxel, Yvoxel, Zvoxel, ID, false);
					}
				}
				else {
					for (int Yvoxel = 0; Yvoxel < Voxel.GetReferenceCells().All / 4 + Voxel.GetReferenceCells().All * 3 / 4 * r / 255; ++Yvoxel) {
						Voxel.SetBlick(Xvoxel, Yvoxel, Zvoxel, ID, false);
					}
				}
			}
		}
		DeleteSoftImage(SoftImage1F);
		DeleteSoftImage(SoftImage1FColor);
	}
	{
		int XSize{}, YSize{};
		int SoftImage2F = LoadSoftImage("data/map2F.bmp");
		int SoftImage2FColor = LoadSoftImage("data/map2FColor.bmp");
		GetSoftImageSize(SoftImage2F, &XSize, &YSize);
		for (int Xvoxel = 0; Xvoxel < Voxel.GetReferenceCells().All; ++Xvoxel) {
			for (int Zvoxel = 0; Zvoxel < Voxel.GetReferenceCells().All; ++Zvoxel) {
				int xp = XSize * Xvoxel / (Voxel.GetReferenceCells().All - 1);
				int yp = YSize - YSize * Zvoxel / (Voxel.GetReferenceCells().All - 1);
				int r = 0;
				GetPixelSoftImage(SoftImage2F, xp, yp, &r, nullptr, nullptr, nullptr);
				int ID = 1;
				{
					int cr = 0;
					int cg = 0;
					int cb = 0;
					GetPixelSoftImage(SoftImage2FColor, xp, yp, &cr, &cg, &cb, nullptr);
					if (cr == 255 && cg == 0 && cb == 0) {
						ID = 4;
					}
					else if (cr == 255 && cg == 255 && cb == 0) {
						ID = 3;
					}
				}

				if (r != 0) {
					int Min = Voxel.GetReferenceCells().All / 4 + Voxel.GetReferenceCells().All * 3 / 4 * 160 / 255;
					int Now = Voxel.GetReferenceCells().All / 4 + Voxel.GetReferenceCells().All * 3 / 4 * r / 255;
					for (int Yvoxel = Min - 3; Yvoxel < Now; ++Yvoxel) {
						Voxel.SetBlick(Xvoxel, Yvoxel, Zvoxel, ID, false);
					}
				}
			}
		}
		DeleteSoftImage(SoftImage2F);
		DeleteSoftImage(SoftImage2FColor);
	}

	enum class InfoType : size_t {
		None,
		Entrance1,
		Entrance2,
		Entrance3,
		Exit1,
		Exit2,
		Exit3,
		WayPoint,
		Max,
	};
	static const char* InfoTypeStr[static_cast<int>(InfoType::Max)] = {
		"None",
		"Entrance1",
		"Entrance2",
		"Entrance3",
		"Exit1",
		"Exit2",
		"Exit3",
		"WayPoint",
	};

	struct MapInfo {
		InfoType				m_InfoType{ InfoType::None };
		Algorithm::Vector3Int	m_pos{};
	};

	std::vector<MapInfo> m_MapInfo;
	if(std::filesystem::is_regular_file("data/Event.txt")){
		std::ifstream ifs("data/Event.txt");
		while (true) {
			std::string Buffer;
			std::getline(ifs, Buffer);
			std::string LEFT = Buffer.substr(0, Buffer.find("="));
			std::string RIGHT = Buffer.substr(Buffer.find("=") + 1);
			if (LEFT == "Type") {
				m_MapInfo.emplace_back();
				for (int loop = 0; loop < static_cast<int>(InfoType::Max); ++loop) {
					if (RIGHT == InfoTypeStr[static_cast<size_t>(loop)]) {
						m_MapInfo.back().m_InfoType = static_cast<InfoType>(loop);
						break;
					}
				}
			}
			else if (LEFT == "X") {
				m_MapInfo.back().m_pos.x = std::stoi(RIGHT);
			}
			else if (LEFT == "Y") {
				m_MapInfo.back().m_pos.y = std::stoi(RIGHT);
			}
			else if (LEFT == "Z") {
				m_MapInfo.back().m_pos.z = std::stoi(RIGHT);
			}
			if (ifs.eof()) { break; }
		}
	}
	InfoType				m_InfoType{ InfoType::None };

	Voxel.InitEnd();											// �������I��������
	// �����̍��W�ƌ������w��
	MyPos = VGet(0.f, 0.f, 0.f);

	int Lange = 10;
	int MX{}, MY{};
	int PMX{}, PMY{};
	int DMX{}, DMY{};
	bool PrevPressLeft = false;
	// ���C�����[�v
	while (ProcessMessage() == 0) {
		PMX = MX;
		PMY = MY;
		GetMousePoint(&MX, &MY);
		//
		if ((GetMouseInput() & MOUSE_INPUT_RIGHT) != 0) {
			float X = static_cast<float>(MY - PMY) / 100.f;
			float Y = static_cast<float>(MX - PMX) / 100.f;
			if (CheckHitKey(KEY_INPUT_LSHIFT) != 0) {
				X = 0.f;
			}
			if (CheckHitKey(KEY_INPUT_LCONTROL) != 0) {
				Y = 0.f;
			}
			Xrad -= X;
			Yrad += Y;
		}
		if (CheckHitKey(KEY_INPUT_F1) != 0) {
			Xrad = -89.f * DX_PI_F / 180.f;
			Yrad = -90.f * DX_PI_F / 180.f;
		}
		if (CheckHitKey(KEY_INPUT_F2) != 0) {
			Xrad = -89.f * DX_PI_F / 180.f;
			Yrad = -0.f * DX_PI_F / 180.f;
		}
		if (CheckHitKey(KEY_INPUT_F3) != 0) {
			Xrad = -0.f * DX_PI_F / 180.f;
			Yrad = -0.f * DX_PI_F / 180.f;
		}

		if (CheckHitKey(KEY_INPUT_1) != 0) {
			m_InfoType = InfoType::Entrance1;
		}
		if (CheckHitKey(KEY_INPUT_2) != 0) {
			m_InfoType = InfoType::Entrance2;
		}
		if (CheckHitKey(KEY_INPUT_3) != 0) {
			m_InfoType = InfoType::Entrance3;
		}
		if (CheckHitKey(KEY_INPUT_4) != 0) {
			m_InfoType = InfoType::Exit1;
		}
		if (CheckHitKey(KEY_INPUT_5) != 0) {
			m_InfoType = InfoType::Exit2;
		}
		if (CheckHitKey(KEY_INPUT_6) != 0) {
			m_InfoType = InfoType::Exit3;
		}
		if (CheckHitKey(KEY_INPUT_7) != 0) {
			m_InfoType = InfoType::WayPoint;
		}
		if (CheckHitKey(KEY_INPUT_0) != 0) {
			m_InfoType = InfoType::None;
		}
		bool PressLeft = (GetMouseInput() & MOUSE_INPUT_LEFT) != 0;
		if (PressLeft && !PrevPressLeft) {
			VECTOR Near = ConvScreenPosToWorldPos(VGet(static_cast<float>(MX), static_cast<float>(MY), 0.f));
			VECTOR Far = ConvScreenPosToWorldPos(VGet(static_cast<float>(MX), static_cast<float>(MY), 1.f));

			if (Voxel.CheckLine(Near, &Far)) {
				auto Pos = Voxel.GetReferenceCells().GetVoxelPoint(Far);
				if (m_InfoType != InfoType::None) {
					m_MapInfo.emplace_back();
					m_MapInfo.back().m_pos = Pos;
					m_MapInfo.back().m_InfoType = m_InfoType;
				}
				else {
					for (auto& m : m_MapInfo) {
						if (m.m_pos == Pos) {
							m_MapInfo.erase(m_MapInfo.begin() + (&m - &m_MapInfo.front()));
							break;
						}
					}
				}
			}
		}
		PrevPressLeft = PressLeft;

		Xrad = std::clamp(Xrad, -89.f * DX_PI_F / 180.f, 90.f * DX_PI_F / 180.f);
		MATRIX Mat = MMult(MGetRotAxis(VGet(1.f, 0.f, 0.f), Xrad), MGetRotAxis(VGet(0.f, 1.f, 0.f), Yrad));
		//
		Lange -= GetMouseWheelRotVol();
		Lange = std::clamp(Lange, 1, 20);
		//
		MyVec = VGet(0.f, 0.f, 0.f);
		if ((GetMouseInput() & MOUSE_INPUT_MIDDLE) != 0) {
			float X = static_cast<float>(MY - PMY) / 100.f;
			float Y = static_cast<float>(MX - PMX) / 100.f;
			if (CheckHitKey(KEY_INPUT_LSHIFT) != 0) {
				X = 0.f;
			}
			if (CheckHitKey(KEY_INPUT_LCONTROL) != 0) {
				Y = 0.f;
			}
			MyVec = VAdd(MyVec, VTransform(VGet(Y * static_cast<float>(Lange) / 3.f, X * static_cast<float>(Lange) / 3.f, 0.f), Mat));
		}



		// �����W�𔽉f
		MyPos = VAdd(MyPos, MyVec);
		// �J�������W���w��
		VECTOR CamPos = VSub(MyPos, VTransform(VGet(0.f, 0.f, -50.f), Mat));
		VECTOR CamTarget = MyPos;// ���g�������Ă�������𒍎��_�Ƃ���
		// FPS��\��
		clsDx();
		printfDx("%5.2f fps\n", GetFPS());
		printfDx("InfoType:[%s]\n", InfoTypeStr[static_cast<size_t>(m_InfoType)]);
		// �V���h�E�}�b�v�ɕ`�悷��͈͂�ݒ�
		// �{�N�Z������
		Voxel.SetDrawInfo(VGet(0.f, 0.f,0.f), VNorm(VSub(CamTarget, CamPos)));// �`�悷��ۂ̕`�撆�S���W�ƕ`�悷��������w��
		Voxel.SetShadowDrawInfo(VGet(0.f, 0.f, 0.f), DXParam.GetShadowVec());// �V���h�E�}�b�v�ɕ`�悷��ۂ̕`�撆�S���W�ƕ`�悷��������w��
		Voxel.Update();
		// �V���h�E�}�b�v�ւ̕`��
		DXParam.SetShadowMapStart(CamPos);
		Voxel.DrawShadow();							// �V���h�E�}�b�v�ւ̕`��
		DXParam.SetShadowMapEnd();
		// ����ʂւ̕`��
		SetDrawScreen(DX_SCREEN_BACK);				// �`���𗠉�ʂɕύX
		ClearDrawScreen();											// ��ʂ��N���A
		{
			SetCameraPositionAndTarget_UpVecY(CamPos, CamTarget);// �J�����̈ʒu�ƌ�����ݒ�
			SetCameraNearFar(0.5f, 100.0f);			// �`�悷�鉜�s�������͈̔͂�ݒ�
			SetupCamera_Ortho(2.f* static_cast<float>(Lange));

			DXParam.SetUseShadowMapStart();
			Voxel.Draw();							// �`��
			DXParam.SetUseShadowMapEnd();

			int Axis = 30;
			for (int loop = -Axis; loop <= Axis; ++loop) {
				DrawLine3D(VGet(static_cast<float>(-Axis), 0.f, static_cast<float>(loop)), VGet(static_cast<float>(Axis), 0.f, static_cast<float>(loop)), GetColor(255, 255, 255));
				DrawLine3D(VGet(static_cast<float>(loop), 0.f, static_cast<float>(-Axis)), VGet(static_cast<float>(loop), 0.f, static_cast<float>(Axis)), GetColor(255, 255, 255));
			}

			DrawLine3D(VGet(-100.f, 0.f, 0.f), VGet(0.f, 0.f, 0.f), GetColor(128, 0, 0));
			DrawLine3D(VGet(0.f, -100.f, 0.f), VGet(0.f, 0.f, 0.f), GetColor(0, 128, 0));
			DrawLine3D(VGet(0.f, 0.f, -100.f), VGet(0.f, 0.f, 0.f), GetColor(0, 0, 128));

			DrawLine3D(VGet(0.f, 0.f, 0.f), VGet(100.f, 0.f, 0.f), GetColor(255, 0, 0));
			DrawLine3D(VGet(0.f, 0.f, 0.f), VGet(0.f, 100.f, 0.f), GetColor(0, 255, 0));
			DrawLine3D(VGet(0.f, 0.f, 0.f), VGet(0.f, 0.f, 100.f), GetColor(0, 0, 255));

			{
				for (auto& m: m_MapInfo) {
					unsigned int Color = 0;
					switch (m.m_InfoType) {
					case InfoType::Entrance1:
						Color = GetColor(255, 0, 0);
						break;
					case InfoType::Entrance2:
						Color = GetColor(255, 64, 0);
						break;
					case InfoType::Entrance3:
						Color = GetColor(255, 128, 0);
						break;
					case InfoType::Exit1:
						Color = GetColor(255, 255, 0);
						break;
					case InfoType::Exit2:
						Color = GetColor(255, 255, 64);
						break;
					case InfoType::Exit3:
						Color = GetColor(255, 255, 128);
						break;
					case InfoType::WayPoint:
						Color = GetColor(0, 255, 0);
						break;
					case InfoType::None:
					case InfoType::Max:
					default:
						break;
					}
					DrawSphere3D(Voxel.GetReferenceCells().GetWorldPos(m.m_pos), 0.125f, 8, Color, GetColor(0, 0, 0), TRUE);
				}
				VECTOR Near = ConvScreenPosToWorldPos(VGet(static_cast<float>(MX), static_cast<float>(MY), 0.f));
				VECTOR Far = ConvScreenPosToWorldPos(VGet(static_cast<float>(MX), static_cast<float>(MY), 1.f));
				if (Voxel.CheckLine(Near, &Far)) {
					DrawSphere3D(Voxel.GetReferenceCells().GetWorldPos(Voxel.GetReferenceCells().GetVoxelPoint(Far)), 0.125f, 8, GetColor(0, 255, 0), GetColor(0, 0, 0), FALSE);
				}
			}
		}
		ScreenFlip();								// ����ʂ̓��e��\��ʂɔ��f
	}

	Voxel.SaveCellsFile("data/Map.txt");						// �{�N�Z���f�[�^�̓ǂݍ���

	Voxel.Dispose();
	Voxel.Dispose_Load();
	{
		std::ofstream Ofs("data/Event.txt");
		for (auto& m : m_MapInfo) {
			Ofs << "Type=" << InfoTypeStr[static_cast<size_t>(m.m_InfoType)] << "\n";
			Ofs << "X=" << std::to_string(m.m_pos.x) << "\n";
			Ofs << "Y=" << std::to_string(m.m_pos.y) << "\n";
			Ofs << "Z=" << std::to_string(m.m_pos.z) << "\n";
		}
	}
	return 0;					// �\�t�g�̏I��
}
