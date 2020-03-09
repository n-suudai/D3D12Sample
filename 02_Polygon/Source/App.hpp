
#pragma once


struct Position2D
{
    s32 x;
    s32 y;
};


struct Size2D
{
    u32 width;
    u32 height;
};


struct AppCallbacks
{
    // リサイズ
    void(*pOnResize)(const Size2D& newSize, void* pUser);
    void* pOnResizeUser;

    // キーダウン
    void(*pOnKeyDown)(KEY_CODE key, void* pUser);
    void* pOnKeyDownUser;

    // キーアップ
    void(*pOnKeyUp)(KEY_CODE key, void* pUser);
    void* pOnKeyUpUser;

    // マウスキーダウン
    void(*pOnMouseKeyDown)(const Position2D& position, MOUSE_BUTTON button, void* pUser);
    void* pOnMouseKeyDownUser;

    // マウスキーアップ
    void(*pOnMouseKeyUp)(const Position2D& position, MOUSE_BUTTON button, void* pUser);
    void* pOnMouseKeyUpUser;

    // マウスホイール
    void(*pOnMouseWheel)(const Position2D& position, s32 delta, void* pUser);
    void* pOnMouseWheelUser;
};


class IApp
{
public:
    virtual ~IApp() = default;

    virtual void SetAppCallbacks(const AppCallbacks& callbacks) = 0;

    virtual bool IsLoop() const = 0;

    virtual void PostQuit() = 0;

    virtual Size2D GetClientSize() const = 0;

    virtual void* GetWindowHandle() const = 0;

    virtual void ShowMessageBox(const std::string& message, const std::string& caption) = 0;
};


bool CreateApp(const Size2D& clientSize, const std::string& title, IApp ** ppOut);

void AppMain();

