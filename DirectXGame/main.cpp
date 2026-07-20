#include "GameScene.h"
#include "KamataEngine.h"
#include "TitleScene.h"
#include <Windows.h>

using namespace KamataEngine;

enum class Scene {
	kUnknown = 0,
	kTitle,
	kGame,
};

Scene scene = Scene::kUnknown;

GameScene* gameScene = nullptr;
TitleScene* titleScene = nullptr;

void ChangeScene() {
	switch (scene) {
	case Scene::kTitle:
		if (titleScene->IsFinished()) {
			/// シーン変更
			scene = Scene::kGame;
			/// 旧シーンの解放
			delete titleScene;
			titleScene = nullptr;
			/// 新シーンの生成と初期化
			gameScene = new GameScene();
			gameScene->Initialize();
		}
		break;
	case Scene::kGame:
		if (gameScene->IsFinished()) {
			/// シーン変更
			scene = Scene::kTitle;
			/// 旧シーンの解放
			delete gameScene;
			gameScene = nullptr;
			/// 新シーンの生成と初期化
			titleScene = new TitleScene();
			titleScene->Initialize();
		}
		break;
	}
}

void UpdateScene() {
	switch (scene) {
	case Scene::kTitle:
		titleScene->Update();
		break;
	case Scene::kGame:
		gameScene->Update();
		break;
	}
}

void DrawScene() {
	switch (scene) {
	case Scene::kTitle:
		titleScene->Draw();
		break;
	case Scene::kGame:
		gameScene->Draw();
		break;
	}
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// エンジンの初期化
	KamataEngine::Initialize(L"GC2C_05_タシロ_トワ_AL3");

	// DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	scene = Scene::kTitle;
	titleScene = new TitleScene();
	titleScene->Initialize();

	while (true) {
		if (KamataEngine::Update()) {
			break;
		}

		/// シーン切り替え
		ChangeScene();
		/// 現在シーン更新
		UpdateScene();

		dxCommon->PreDraw();
		/// 現在シーンの描画
		DrawScene();
		dxCommon->PostDraw();
	}

	delete titleScene;
	delete gameScene;

	// エンジンの終了処理
	KamataEngine::Finalize();

	return 0;
}