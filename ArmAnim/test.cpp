#define USE_VR (true)

#define NOMINMAX
#include "DxLib.h"

#include <algorithm>
#include <vector>
#include <fstream>
#include <filesystem>

#include "source/Util/Util.hpp"
#include "source/Util/Algorithm.hpp"
#include "source/Draw/MV1.hpp"
#include "source/VR.hpp"

class DX {
	int ShadowMapHandle = -1;
	VECTOR ShadowVec = VGet(0.2f, -0.2f, 0.2f);
public:
	DX() {
		DxLib::SetGraphMode(1280, 1000, 32);
		DxLib_Init();												// ＤＸライブラリ初期化処理
		ChangeWindowMode(TRUE);										// ウィンドウモード
		SetAlwaysRunFlag(TRUE);										// 非アクティブでも動くようにする
		SetMouseDispFlag(TRUE);
		ShadowMapHandle = MakeShadowMap(2048, 2048);				// シャドウマップハンドルの作成
		SetLightDirection(ShadowVec);								// ライトの方向を設定
		SetShadowMapLightDirection(ShadowMapHandle, ShadowVec);		// シャドウマップが想定するライトの方向もセット
	}
	~DX() {
		DeleteShadowMap(ShadowMapHandle);	// シャドウマップの削除
		DxLib_End();				// ＤＸライブラリ使用の終了処理
	}
public:
	VECTOR GetShadowVec() const { return ShadowVec; }
public:
	void SetShadowMapStart(VECTOR CamPos) const {
		SetShadowMapDrawArea(ShadowMapHandle, VAdd(CamPos, VGet(-50.0f, -50.0f, -50.0f)), VAdd(CamPos, VGet(50.0f, 0.0f, 50.0f)));
		ShadowMap_DrawSetup(ShadowMapHandle);		// シャドウマップへの描画の準備
	}
	void SetShadowMapEnd() {
		ShadowMap_DrawEnd();						// シャドウマップへの描画を終了
	}
	void SetUseShadowMapStart() const {
		SetUseShadowMap(0, ShadowMapHandle);	// 描画にシャドウマップを使用する
	}
	void SetUseShadowMapEnd() {
		SetUseShadowMap(0, -1);					// 描画に使用するシャドウマップの設定を解除
	}
};


enum class CharaFrame {
	Center,
	Upper,
	Upper2,
	Head,
	Eye,
	LeftFoot1,
	LeftFoot2,
	LeftFoot,
	RightFoot1,
	RightFoot2,
	RightFoot,
	RightArm,
	RightArm2,
	RightWrist,
	RightHandJoint,
	LeftArm,
	LeftArm2,
	LeftWrist,
	LeftHandJoint,
	Max,
};
static const char* CharaFrameName[static_cast<int>(CharaFrame::Max)] = {
	"センター",
	"上半身",
	"上半身2",
	"頭",
	"両目",
	"左足",
	"左ひざ",
	"左足首",
	"右足",
	"右ひざ",
	"右足首",
	"右腕",
	"右ひじ",
	"右手首",
	"右ダミー",
	"左腕",
	"左ひじ",
	"左手首",
	"左ダミー",
};

struct Frames {
	int first{};
	Util::Matrix4x4 second{};
	Util::Matrix4x4 third{};
};


struct VRAnim {
	int ID{};
	Util::Matrix3x3	m_RightRot;
	Util::Matrix3x3	m_LeftRot;
	Util::VECTOR3D	m_LeftHandPos;
	Util::VECTOR3D	m_RightHandPos;
};


int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	VECTOR MyPos = VGet(0.f, 0.f, 0.f);
	VECTOR MyVec = VGet(0.f, 0.f, 0.f);
	float Xrad = -45.f * DX_PI_F / 180.f;
	float Yrad = 0.f;
	float Speed = 0.f;
	// 初期化
	DX DXParam;

	int MainDrawScreen = MakeScreen(1080, 1920, false);

	DXLibRef::VRControl::Create();
#if USE_VR
	DXLibRef::VRControl::Instance()->Init();
#endif

	VRAnim				m_VRAnim{};

	std::vector<VRAnim>	m_VRAnims{};
	m_VRAnims.resize(60 * 30);
	float m_AminTime = 0.f;
	int m_AminTimePrev = 0;

	float m_Time = 0.f;
	bool m_IsRecord = false;

	std::vector<Frames>	m_Frames;
	Draw::MV1			Model;
	Util::Matrix4x4		m_RightPos;
	Util::Matrix4x4		m_LeftPos;

	auto GetFrameNum = [&](void) { return static_cast<int>(CharaFrame::Max); };
	auto GetFrameStr = [&](int id) { return CharaFrameName[id]; };
	auto GetFrame = [&](int frame) { return m_Frames[static_cast<size_t>(frame)].first; };
	auto GetFrameBaseLocalMat = [&](int frame) { return m_Frames[static_cast<size_t>(frame)].second; };
	auto GetFrameBaseLocalWorldMat = [&](int frame) { return m_Frames[static_cast<size_t>(frame)].third; };

	auto Save = [&](void) {
		std::ofstream File("data/anim.txt");
		int loop = 0;
		for (auto& V : m_VRAnims) {
			File << "Frame=" + std::to_string(loop) + "\n";

			File << "RightRot0=" + std::to_string(V.m_RightRot.get().m[0][0]) + "," + std::to_string(V.m_RightRot.get().m[0][1]) + "," + std::to_string(V.m_RightRot.get().m[0][2]) + "\n";
			File << "RightRot1=" + std::to_string(V.m_RightRot.get().m[1][0]) + "," + std::to_string(V.m_RightRot.get().m[1][1]) + "," + std::to_string(V.m_RightRot.get().m[1][2]) + "\n";
			File << "RightRot2=" + std::to_string(V.m_RightRot.get().m[2][0]) + "," + std::to_string(V.m_RightRot.get().m[2][1]) + "," + std::to_string(V.m_RightRot.get().m[2][2]) + "\n";
			File << "RightPos=" + std::to_string(V.m_RightHandPos.x) + "," + std::to_string(V.m_RightHandPos.y) + "," + std::to_string(V.m_RightHandPos.z) + "\n";

			File << "LeftRot0=" + std::to_string(V.m_LeftRot.get().m[0][0]) + "," + std::to_string(V.m_LeftRot.get().m[0][1]) + "," + std::to_string(V.m_LeftRot.get().m[0][2]) + "\n";
			File << "LeftRot1=" + std::to_string(V.m_LeftRot.get().m[1][0]) + "," + std::to_string(V.m_LeftRot.get().m[1][1]) + "," + std::to_string(V.m_LeftRot.get().m[1][2]) + "\n";
			File << "LeftRot2=" + std::to_string(V.m_LeftRot.get().m[2][0]) + "," + std::to_string(V.m_LeftRot.get().m[2][1]) + "," + std::to_string(V.m_LeftRot.get().m[2][2]) + "\n";
			File << "LeftPos=" + std::to_string(V.m_LeftHandPos.x) + "," + std::to_string(V.m_LeftHandPos.y) + "," + std::to_string(V.m_LeftHandPos.z) + "\n";
			++loop;
		}
	};


	Draw::MV1::Load("data/Soldier/model.mv1",&Model);
	// フレーム
	{
		m_Frames.clear();
		if (GetFrameNum() > 0) {
			m_Frames.resize(static_cast<size_t>(GetFrameNum()));
			for (auto& f : m_Frames) {
				f.first = InvalidID;
			}
			size_t count = 0;
			for (int frameNum = 0, Max = Model.GetFrameNum(); frameNum < Max; ++frameNum) {
				if (Model.GetFrameName(frameNum) == GetFrameStr(static_cast<int>(count))) {
					// そのフレームを登録
					m_Frames[count].first = frameNum;
					m_Frames[count].second = Util::Matrix4x4::Mtrans(Model.GetFrameLocalMatrix(m_Frames[count].first).pos());
					m_Frames[count].third = Model.GetFrameLocalWorldMatrix(m_Frames[count].first);
				}
				else if (frameNum < Max - 1) {
					continue;// 飛ばす
				}
				++count;
				frameNum = 0;
				if (count >= m_Frames.size()) {
					break;
				}
			}
		}
	}

	// 自分の座標と向きを指定
	MyPos = VGet(0.f, 0.f, 0.f);

	int Lange = 10;
	int MX{}, MY{};
	int PMX{}, PMY{};
	int DMX{}, DMY{};
	bool PrevPressLeft = false;
	// メインループ
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

		bool PressLeft = (GetMouseInput() & MOUSE_INPUT_LEFT) != 0;
		if (PressLeft && !PrevPressLeft) {
			VECTOR Near = ConvScreenPosToWorldPos(VGet(static_cast<float>(MX), static_cast<float>(MY), 0.f));
			VECTOR Far = ConvScreenPosToWorldPos(VGet(static_cast<float>(MX), static_cast<float>(MY), 1.f));
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

		if (DXLibRef::VRControl::Instance()->Get_VR_Hand1Device() && DXLibRef::VRControl::Instance()->Get_VR_Hand1Device()->IsActive()) {
			if (DXLibRef::VRControl::Instance()->Get_VR_Hand1Device()->PadTouch(DXLibRef::VR_PAD::TRIGGER)) {
				if (m_AminTime == 0.f) {
					m_AminTime = 60.f * 30.f;
				}
			}
		}
		if (DXLibRef::VRControl::Instance()->Get_VR_Hand2Device() && DXLibRef::VRControl::Instance()->Get_VR_Hand2Device()->IsActive()) {
			if (DXLibRef::VRControl::Instance()->Get_VR_Hand2Device()->PadTouch(DXLibRef::VR_PAD::TRIGGER)) {
				if (m_AminTime == 0.f) {
					m_AminTime = 60.f * 30.f;
				}
			}
		}
		if (CheckHitKey(KEY_INPUT_SPACE) != 0) {
			if (m_Time == 0.f) {
				m_Time = 60.f * 30.f;
				m_IsRecord = true;
			}
		}

		{
			Util::Matrix4x4 HandBaseMat = Model.GetFrameLocalWorldMatrix(GetFrame(static_cast<int>(CharaFrame::Head)));
			HandBaseMat =
				Util::Matrix4x4::Mtrans(Util::VECTOR3D::vget(0.f, 0.f, -0.15f) * Scale3DRate) *
				HandBaseMat.rotation() * Util::Matrix4x4::Mtrans(HandBaseMat.pos());

			if (m_IsRecord) {
				m_VRAnim = m_VRAnims.at(std::clamp(60 * 30 - static_cast<int>(m_Time), 0, static_cast<int>(m_VRAnims.size() - 1)));
				m_Time = std::max(m_Time - 30.f / GetFPS(), 0.f);
				if (m_Time == 0.f) {
					m_IsRecord = false;
				}
			}
			else {
				Util::Matrix4x4 BaseMat = GetFrameBaseLocalWorldMat(static_cast<int>(CharaFrame::Head));
				if (DXLibRef::VRControl::Instance()->Get_VR_Hand1Device() && DXLibRef::VRControl::Instance()->Get_VR_Hand1Device()->IsActive()) {
					DXLibRef::VRControl::Instance()->GetHand1Position(&m_VRAnim.m_LeftHandPos, &m_VRAnim.m_LeftRot);
					m_VRAnim.m_LeftRot *= Util::Matrix3x3::RotAxis(Util::VECTOR3D::up(), Util::deg2rad(-90));
					m_VRAnim.m_LeftRot *= Util::Matrix3x3::RotAxis(Util::VECTOR3D::right(), Util::deg2rad(-45));
					m_VRAnim.m_LeftHandPos = Util::Matrix3x3::Vtrans(m_VRAnim.m_LeftHandPos * Scale3DRate - BaseMat.pos(), Util::Matrix3x3::RotAxis(Util::VECTOR3D::up(), Util::deg2rad(-90)));
				}
				if (DXLibRef::VRControl::Instance()->Get_VR_Hand2Device() && DXLibRef::VRControl::Instance()->Get_VR_Hand2Device()->IsActive()) {
					DXLibRef::VRControl::Instance()->GetHand2Position(&m_VRAnim.m_RightHandPos, &m_VRAnim.m_RightRot);
					m_VRAnim.m_RightRot *= Util::Matrix3x3::RotAxis(Util::VECTOR3D::up(), Util::deg2rad(-90));
					m_VRAnim.m_RightRot *= Util::Matrix3x3::RotAxis(Util::VECTOR3D::right(), Util::deg2rad(-45));
					m_VRAnim.m_RightHandPos = Util::Matrix3x3::Vtrans(m_VRAnim.m_RightHandPos * Scale3DRate - BaseMat.pos(), Util::Matrix3x3::RotAxis(Util::VECTOR3D::up(), Util::deg2rad(-90)));
				}

				if (m_AminTimePrev != static_cast<int>(m_AminTime)) {
					m_AminTimePrev = static_cast<int>(m_AminTime);
					m_VRAnims.at(std::clamp(60 * 30 - static_cast<int>(m_AminTime), 0, static_cast<int>(m_VRAnims.size() - 1))) = m_VRAnim;
				}
				m_AminTime = std::max(m_AminTime - 30.f / GetFPS(), 0.f);
			}

			{
				Draw::IK_RightArm(
					&Model,
					GetFrame(static_cast<int>(CharaFrame::RightArm)),
					GetFrameBaseLocalMat(static_cast<int>(CharaFrame::RightArm)),
					GetFrame(static_cast<int>(CharaFrame::RightArm2)),
					GetFrameBaseLocalMat(static_cast<int>(CharaFrame::RightArm2)),
					GetFrame(static_cast<int>(CharaFrame::RightWrist)),
					GetFrameBaseLocalMat(static_cast<int>(CharaFrame::RightWrist)),
					m_VRAnim.m_RightRot.Get44DX() * HandBaseMat.rotation() *
					Util::Matrix4x4::Mtrans(Util::Matrix4x4::Vtrans(m_VRAnim.m_RightHandPos, HandBaseMat.rotation()) + HandBaseMat.pos())
				);
			}
			{
				Draw::IK_LeftArm(
					&Model,
					GetFrame(static_cast<int>(CharaFrame::LeftArm)),
					GetFrameBaseLocalMat(static_cast<int>(CharaFrame::LeftArm)),
					GetFrame(static_cast<int>(CharaFrame::LeftArm2)),
					GetFrameBaseLocalMat(static_cast<int>(CharaFrame::LeftArm2)),
					GetFrame(static_cast<int>(CharaFrame::LeftWrist)),
					GetFrameBaseLocalMat(static_cast<int>(CharaFrame::LeftWrist)),
					m_VRAnim.m_LeftRot.Get44DX() * HandBaseMat.rotation() *
					Util::Matrix4x4::Mtrans(Util::Matrix4x4::Vtrans(m_VRAnim.m_LeftHandPos, HandBaseMat.rotation()) + HandBaseMat.pos())
				);
			}
		}


		// 仮座標を反映
		MyPos = VAdd(MyPos, MyVec);
		// カメラ座標を指定
		VECTOR CamPos = VSub(MyPos, VTransform(VGet(0.f, 0.f, -50.f), Mat));
		VECTOR CamTarget = MyPos;// 自身が向いている方向を注視点とする
		
		if(false){
			Util::VECTOR3D pos;
			Util::Matrix3x3 mat;
			DXLibRef::VRControl::Instance()->GetHMDPosition(&pos, &mat);
			pos = pos * Scale3DRate;

			CamPos = pos.get();
			CamTarget = (pos + mat.zvec() * -1.f).get();
		}

		DXLibRef::VRControl::Instance()->Update();
		// FPSを表示
		clsDx();
		printfDx("%5.2f fps\n", GetFPS());
		printfDx("再生 %5.2f time\n", m_Time / 30.f);
		printfDx("記録 %5.2f time\n", m_AminTime / 30.f);
		// シャドウマップへの描画
		DXParam.SetShadowMapStart(CamPos);
		{
			Model.DrawModel();
		}
		DXParam.SetShadowMapEnd();
		// 裏画面への描画
#if USE_VR
		SetDrawScreen(MainDrawScreen);				// 描画先を裏画面に変更
#else
		SetDrawScreen(DX_SCREEN_BACK);				// 描画先を裏画面に変更
#endif
		ClearDrawScreen();											// 画面をクリア
		{
			SetCameraPositionAndTarget_UpVecY(CamPos, CamTarget);// カメラの位置と向きを設定
			SetCameraNearFar(0.5f, 100.0f);			// 描画する奥行き方向の範囲を設定
			SetupCamera_Ortho(2.f* static_cast<float>(Lange));

			SetUseZBufferFlag(TRUE);
			SetWriteZBuffer3D(TRUE);


			DXParam.SetUseShadowMapStart();
			{
				Model.DrawModel();
			}
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
		}
		DXLibRef::VRControl::Instance()->SubmitDraw(0, MainDrawScreen);
		DXLibRef::VRControl::Instance()->SubmitDraw(1, MainDrawScreen);
		DXLibRef::VRControl::Instance()->WaitSync();
		ScreenFlip();								// 裏画面の内容を表画面に反映
	}
	DXLibRef::VRControl::Instance()->Dispose();
	Save();
	return 0;					// ソフトの終了
}
