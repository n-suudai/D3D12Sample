

#define WINDOW_CLASS_NAME "Sample"


struct KeyCode
{
    WPARAM wParam;
    KEY_CODE keyCode;
};
const KeyCode g_KeyCodeTable[] = {
    { 'A', KEY_CODE_A },
    { 'B', KEY_CODE_B },
    { 'C', KEY_CODE_C },
    { 'D', KEY_CODE_D },
    { 'E', KEY_CODE_E },
    { 'F', KEY_CODE_F },
    { 'G', KEY_CODE_G },
    { 'H', KEY_CODE_H },
    { 'I', KEY_CODE_I },
    { 'J', KEY_CODE_J },
    { 'K', KEY_CODE_K },
    { 'L', KEY_CODE_L },
    { 'M', KEY_CODE_M },
    { 'N', KEY_CODE_N },
    { 'O', KEY_CODE_O },
    { 'P', KEY_CODE_P },
    { 'Q', KEY_CODE_Q },
    { 'R', KEY_CODE_R },
    { 'S', KEY_CODE_S },
    { 'T', KEY_CODE_T },
    { 'U', KEY_CODE_U },
    { 'V', KEY_CODE_V },
    { 'W', KEY_CODE_W },
    { 'X', KEY_CODE_X },
    { 'Y', KEY_CODE_Y },
    { 'Z', KEY_CODE_Z },
    { '1', KEY_CODE_1 },
    { '2', KEY_CODE_2 },
    { '3', KEY_CODE_3 },
    { '4', KEY_CODE_4 },
    { '5', KEY_CODE_5 },
    { '6', KEY_CODE_6 },
    { '7', KEY_CODE_7 },
    { '8', KEY_CODE_8 },
    { '9', KEY_CODE_9 },
    { '0', KEY_CODE_0 },
    { VK_F1, KEY_CODE_F1 },
    { VK_F2, KEY_CODE_F2 },
    { VK_F3, KEY_CODE_F3 },
    { VK_F4, KEY_CODE_F4 },
    { VK_F5, KEY_CODE_F5 },
    { VK_F6, KEY_CODE_F6 },
    { VK_F7, KEY_CODE_F7 },
    { VK_F8, KEY_CODE_F8 },
    { VK_F9, KEY_CODE_F9 },
    { VK_F10, KEY_CODE_F10 },
    { VK_F11, KEY_CODE_F11 },
    { VK_F12, KEY_CODE_F12 },
    { VK_ESCAPE, KEY_CODE_ESCAPE },
    { VK_RETURN, KEY_CODE_RETURN },
    { VK_SPACE, KEY_CODE_SPACE },
    { VK_BACK, KEY_CODE_BACKSPACE },
    { VK_DELETE, KEY_CODE_DELETE },
    { VK_LCONTROL, KEY_CODE_LCONTROL },
    { VK_LSHIFT, KEY_CODE_LSHIFT },
    { VK_RCONTROL, KEY_CODE_RCONTROL },
    { VK_RSHIFT, KEY_CODE_RSHIFT },
    { VK_UP, KEY_CODE_UP },
    { VK_DOWN, KEY_CODE_DOWN },
    { VK_LEFT, KEY_CODE_LEFT },
    { VK_RIGHT, KEY_CODE_RIGHT },
};


KEY_CODE ToKeyCode(WPARAM wParam)
{
    for (auto& keyCode : g_KeyCodeTable)
    {
        if (keyCode.wParam == wParam)
        {
            return keyCode.keyCode;
        }
    }
    return KEY_CODE_UNKNOWN;
}


AppWin::AppWin()
{

}


AppWin::~AppWin()
{
    Term();
}


bool AppWin::Init(const Size2D& clientSize, const std::string& title)
{
    m_hInstance = GetModuleHandleA(nullptr);
    if (!m_hInstance)
    {
        return false;
    }

    m_ClientSize = clientSize;

    // 拡張ウィンドウクラスの設定
    WNDCLASSEXA wc;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = &AppWin::WindowProcedure;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = m_hInstance;
    wc.hIcon = LoadIcon(m_hInstance, IDI_APPLICATION);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = WINDOW_CLASS_NAME;
    wc.hIconSm = LoadIcon(m_hInstance, IDI_APPLICATION);

    if (!RegisterClassExA(&wc))
    {
        return false;
    }

    // 矩形の設定
    RECT rc = {
        0,
        0,
        static_cast<LONG>(m_ClientSize.width),
        static_cast<LONG>(m_ClientSize.height),
    };

    // 指定されたクライアント領域を確保するために必要なウィンドウ座標を計算
    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_THICKFRAME;
    AdjustWindowRect(&rc, style, FALSE);

    m_WindowSize.width = static_cast<u32>(rc.right - rc.left);
    m_WindowSize.height = static_cast<u32>(rc.bottom - rc.top);

    // ウィンドウを生成
    m_hWnd = CreateWindowA(
        WINDOW_CLASS_NAME,
        title.c_str(),
        style,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        static_cast<int>(m_WindowSize.width),
        static_cast<int>(m_WindowSize.height),
        nullptr,
        nullptr,
        m_hInstance,
        this
    );
    if (!m_hWnd)
    {
        return false;
    }

    // ウィンドウを表示
    ShowWindow(m_hWnd, SW_SHOWNORMAL);
    UpdateWindow(m_hWnd);

    // フォーカスを設定します
    SetFocus(m_hWnd);


    SetWindowLongPtrA(m_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    return true;
}


void AppWin::Term()
{
    if (m_hInstance != nullptr)
    {
        UnregisterClassA(WINDOW_CLASS_NAME, m_hInstance);
        m_hInstance = nullptr;
    }
}


void AppWin::SetAppCallbacks(const AppCallbacks& callbacks)
{
    m_Callbacks = callbacks;
}


bool AppWin::IsLoop() const
{
    if (IsWindow(m_hWnd) != TRUE)
    {
        return false;
    }

    MSG msg;
    if (PeekMessageA(&msg, m_hWnd, 0, 0, PM_REMOVE) != 0)
    {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    return msg.message != WM_QUIT;
}


void AppWin::PostQuit()
{
    DestroyWindow(m_hWnd);
}


Size2D AppWin::GetClientSize() const
{
    return m_ClientSize;
}


void* AppWin::GetWindowHandle() const
{
    return m_hWnd;
}


void AppWin::ShowMessageBox(const std::string& message, const std::string& caption)
{
    MessageBoxA(m_hWnd, message.c_str(), caption.c_str(), MB_OK);
}


LRESULT CALLBACK AppWin::WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    AppWin* pInstance = reinterpret_cast<AppWin*>(GetWindowLongPtrA(hWnd, GWLP_USERDATA));

    if (pInstance != nullptr)
    {
        return pInstance->WindowProcedureInstance(
            hWnd,
            uMsg,
            wParam,
            lParam
        );
    }

    return DefWindowProcA(
        hWnd,
        uMsg,
        wParam,
        lParam
    );
}


LRESULT CALLBACK AppWin::WindowProcedureInstance(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        m_hWnd = nullptr;
        break;

    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        if (m_Callbacks.pOnKeyDown != nullptr)
        {
            m_Callbacks.pOnKeyDown(
                ToKeyCode(wParam),
                m_Callbacks.pOnKeyDownUser
            );
        }
        break;

    case WM_KEYUP:
    case WM_SYSKEYUP:
        if (m_Callbacks.pOnKeyUp != nullptr)
        {
            m_Callbacks.pOnKeyUp(
                ToKeyCode(wParam),
                m_Callbacks.pOnKeyUpUser
            );
        }
        break;

    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
        if (m_Callbacks.pOnMouseKeyDown != nullptr)
        {
            Position2D position = {
                static_cast<s32>((int16_t)LOWORD(lParam)),
                static_cast<s32>((int16_t)HIWORD(lParam))
            };

            MOUSE_BUTTON button = MOUSE_BUTTON_LEFT;

            if (uMsg == WM_RBUTTONDOWN)
            {
                button = MOUSE_BUTTON_RIGHT;
            }
            else if (uMsg == WM_MBUTTONDOWN)
            {
                button = MOUSE_BUTTON_MIDDLE;
            }

            m_Callbacks.pOnMouseKeyDown(
                position,
                button,
                m_Callbacks.pOnMouseKeyDownUser
            );
        }
        break;

    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
        if (m_Callbacks.pOnMouseKeyUp != nullptr)
        {
            Position2D position = {
                static_cast<s32>((int16_t)LOWORD(lParam)),
                static_cast<s32>((int16_t)HIWORD(lParam))
            };

            MOUSE_BUTTON button = MOUSE_BUTTON_LEFT;

            if (uMsg == WM_RBUTTONUP)
            {
                button = MOUSE_BUTTON_RIGHT;
            }
            else if (uMsg == WM_MBUTTONUP)
            {
                button = MOUSE_BUTTON_MIDDLE;
            }

            m_Callbacks.pOnMouseKeyUp(
                position,
                button,
                m_Callbacks.pOnMouseKeyUpUser
            );
        }
        break;


    case WM_MOUSEHWHEEL:
        if (m_Callbacks.pOnMouseWheel != nullptr)
        {
            Position2D position = {
                static_cast<s32>((int16_t)LOWORD(lParam)),
                static_cast<s32>((int16_t)HIWORD(lParam))
            };
            s32 wheelDelta = static_cast<s32>((int16_t)wParam);

            m_Callbacks.pOnMouseWheel(
                position,
                wheelDelta,
                m_Callbacks.pOnMouseWheelUser
            );
        }
        break;

    case WM_EXITSIZEMOVE:
        if (m_Callbacks.pOnResize != nullptr)
        {
            RECT rc = { 0 };

            GetClientRect(hWnd, &rc);

            Size2D newSize = {
                static_cast<u32>(rc.right - rc.left),
                static_cast<u32>(rc.bottom - rc.top),
            };

            m_Callbacks.pOnResize(
                newSize,
                m_Callbacks.pOnResizeUser
            );
        }
        break;
    }

    return DefWindowProcA(
        hWnd,
        uMsg,
        wParam,
        lParam
    );
}


bool CreateApp(const Size2D& clientSize, const std::string& title, IApp ** ppOut)
{
    AppWin* pApp = new AppWin();
    if (!pApp->Init(clientSize, title))
    {
        delete pApp;
        *ppOut = nullptr;
        return false;
    }

    *ppOut = pApp;
    return true;
}

