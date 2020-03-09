

#include "Sample/SampleApp.hpp"


#define UNUSED(x) (x)



// リサイズ
void OnResize(const Size2D& newSize, void* pUser)
{
    SampleApp* pSample = reinterpret_cast<SampleApp*>(pUser);
    pSample->OnResize(newSize);
}


// キーダウン
void OnKeyDown(KEY_CODE key, void* pUser)
{
    SampleApp* pSample = reinterpret_cast<SampleApp*>(pUser);
    pSample->OnKey(key, true);
}


// キーアップ
void OnKeyUp(KEY_CODE key, void* pUser)
{
    SampleApp* pSample = reinterpret_cast<SampleApp*>(pUser);
    pSample->OnKey(key, false);
}


// マウスキーダウン
void OnMouseKeyDown(const Position2D& position, MOUSE_BUTTON button, void* pUser)
{
    SampleApp* pSample = reinterpret_cast<SampleApp*>(pUser);
    pSample->OnMouse(position, button, true);
}


// マウスキーアップ
void OnMouseKeyUp(const Position2D& position, MOUSE_BUTTON button, void* pUser)
{
    SampleApp* pSample = reinterpret_cast<SampleApp*>(pUser);
    pSample->OnMouse(position, button, false);
}


// マウスホイール
void OnMouseWheel(const Position2D& position, s32 wheelDelta, void* pUser)
{
    SampleApp* pSample = reinterpret_cast<SampleApp*>(pUser);
    pSample->OnMouseWheel(position, wheelDelta);
}



// エントリポイント
void AppMain()
{
    IApp* pApp = nullptr;
    Size2D clientSize = { 640, 480 };

    if (!CreateApp(clientSize, "Sample", &pApp))
    {
        return;
    }

    // サンプルの作成
    SampleApp * pSample = new SampleApp(pApp);

    // コールバックの設定
    {
        AppCallbacks callbacks = { 0 };
        // リサイズ
        callbacks.pOnResize = OnResize;
        callbacks.pOnResizeUser = pSample;
        // キーダウン
        callbacks.pOnKeyDown = OnKeyDown;
        callbacks.pOnKeyDownUser = pSample;
        // キーアップ
        callbacks.pOnKeyUp = OnKeyUp;
        callbacks.pOnKeyUpUser = pSample;
        // マウスキーダウン
        callbacks.pOnMouseKeyDown = OnMouseKeyDown;
        callbacks.pOnMouseKeyDownUser = pSample;
        // マウスキーアップ
        callbacks.pOnMouseKeyUp = OnMouseKeyUp;
        callbacks.pOnMouseKeyUpUser = pSample;
        // マウスホイール
        callbacks.pOnMouseWheel = OnMouseWheel;
        callbacks.pOnMouseWheelUser = pSample;

        pApp->SetAppCallbacks(callbacks);
    }

    // サンプルの初期化
    if (!pSample->Init())
    {
        pApp->ShowMessageBox("サンプルの初期化に失敗しました。", "初期化失敗");
        pApp->PostQuit();
    }

    // メインループ
    while (pApp->IsLoop())
    {
        pSample->Update();
        pSample->Render();
    }

    delete pSample;
    delete pApp;
}

