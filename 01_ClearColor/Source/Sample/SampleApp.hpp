#pragma once


class SampleApp
{
public:
    SampleApp(IApp* pApp);

    ~SampleApp();

    // 初期化
    bool Init();

    // 解放
    void Term();

    // 更新処理
    void Update();

    // 描画処理
    void Render();

    // リサイズ
    void OnResize(const Size2D& newSize);

    // キー
    void OnKey(KEY_CODE key, bool isDown);

    // マウスボタン
    void OnMouse(const Position2D& position, MOUSE_BUTTON button, bool isDown);

    // マウスホイール
    void OnMouseWheel(const Position2D& position, s32 wheelDelta);


private:
    // バックバッファを作成
    bool CreateBackBuffer(const Size2D& newSize);

    // エラーメッセージ表示
    void ShowErrorMessage(const ResultUtil& result, const std::string& text);


private:
    IApp * m_pApp;
    UINT m_BufferCount;

    DXGI_FORMAT       m_BufferFormat;
    D3D_FEATURE_LEVEL m_FeatureLevel;

    ComPtr<IDXGIFactory6>   m_Factory;
    ComPtr<IDXGIAdapter>    m_Adapter;
    ComPtr<IDXGISwapChain4> m_SwapChain;

    ComPtr<ID3D12Device>              m_Device;
    ComPtr<ID3D12CommandAllocator>    m_CommandAllocator;
    ComPtr<ID3D12GraphicsCommandList> m_GraphicsCommandList;
    ComPtr<ID3D12CommandQueue>        m_CommandQueue;

    ComPtr<ID3D12DescriptorHeap>         m_RTVHeaps;
    std::vector<ComPtr<ID3D12Resource1>> m_BackBuffers;

    UINT64 m_FenceValue;
    ComPtr<ID3D12Fence1> m_Fence;
};


