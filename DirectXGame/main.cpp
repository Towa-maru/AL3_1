#include "GaneScene.h"
#include "KamataEngine.h"
#include <Windows.h>

// Windowsアプリでのエントリーポイント(main関数)
using namespace KamataEngine;
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	KamataEngine::Initialize(L"GC2C_05_タシロ_トワ");
	GameScene* gameScene = new GameScene();
	gameScene->Initialize();
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	while (true) {
		if (KamataEngine::Update()) {
			break;
		}
		gameScene->Update();
		// 描画開始
		dxCommon->PreDraw();

		gameScene->Draw();

		dxCommon->PostDraw();
	}

	delete gameScene;

	gameScene = nullptr;

	KamataEngine::Finalize();
	return 0;
}