#pragma once



class ResultUtil
{
public:
    ResultUtil();

    // HRESULT 受け取り
    ResultUtil(HRESULT hr);

    ~ResultUtil();

    operator bool() const;

    const std::string& GetText() const;

private:
    bool        m_IsSucceeded;
    std::string m_Text;
};

