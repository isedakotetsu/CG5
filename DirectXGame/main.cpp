
#include "KamataEngine.h"

using namespace KamataEngine;


// Windowsアプリでのエントリーポイント
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// エンジン初期化
	Initialize(L"LE3D_02_イセダ_コテツ");
	// DirectX取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	



	// メインループ
	while (true) {

		// エンジン更新（終了チェック）
		if (Update()) {
			break;
		}


		// 描画開始
		dxCommon->PreDraw();


		// 描画終了
		dxCommon->PostDraw();
	}

	

	// エンジン終了
	Finalize();

	return 0;
}