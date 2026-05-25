#include <Windows.h>
#include"KamataEngine.h"
#include"GaneScene.h"

using namespace KamataEngine;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	KamataEngine::Initialize(L"GC1C_05_タシロ_トワ");

	GameScene* gameScene = new GameScene();
	gameScene->Initialize();

	while(true) {
		if (KamataEngine::Update()) {
			break;
		}

		gameScene->Update();

		dxCommon->PreDraw();

		gameScene->Draw();

		dxCommon->PostDraw();

	}

	KamataEngine::Finalize();

	delete gameScene;
	gameScene = nullptr;

	return 0;
}
