

ResultUtil::ResultUtil()
{
    m_Text = "";
    m_IsSucceeded = false;
    m_HRESULT = E_FAIL;
}

// HRESULT 受け取り
ResultUtil::ResultUtil(HRESULT hr)
    : ResultUtil()
{
    m_HRESULT = hr;
    m_IsSucceeded = SUCCEEDED(hr);

    LPVOID lpMsgBuf;

    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        hr,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // 既定の言語
        (LPSTR)&lpMsgBuf,
        0,
        NULL
    );

    // 文字列をコピーする。
    if (lpMsgBuf != nullptr)
    {
        m_Text = (LPCSTR)lpMsgBuf;
    }

    // バッファを解放する。
    LocalFree(lpMsgBuf);
}

ResultUtil::~ResultUtil()
{

}

ResultUtil::operator bool() const
{
    return m_IsSucceeded;
}

const std::string& ResultUtil::GetText() const
{
    return m_Text;
}

HRESULT ResultUtil::GetHRESULT() const
{
    return m_HRESULT;
}

