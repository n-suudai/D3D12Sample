
#pragma once


class AppWin : public IApp
{
public:
    AppWin();

    ~AppWin();

    bool Init(const Size2D& clientSize, const std::string& title);

    void Term();

    virtual void SetAppCallbacks(const AppCallbacks& callbacks) override;

    virtual bool IsLoop() const override;

    virtual void PostQuit() override;

    virtual Size2D GetClientSize() const override;

    virtual void* GetWindowHandle() const override;

    virtual void ShowMessageBox(const std::string& message, const std::string& caption) override;

private:
    static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

    LRESULT CALLBACK WindowProcedureInstance(HWND, UINT, WPARAM, LPARAM);

private:
    HWND         m_hWnd;
    HINSTANCE    m_hInstance;
    Size2D       m_ClientSize;
    Size2D       m_WindowSize;
    AppCallbacks m_Callbacks;
};


