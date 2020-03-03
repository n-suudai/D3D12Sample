#include "SampleApp.hpp"


// DXGI & D3D12 のライブラリをリンク
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")


void DebugOutputFormatString(const char* format, ...)
{
    char buff[1024];
    va_list valist;
    va_start(valist, format);
    vsprintf_s(buff, format, valist);
    va_end(valist);

    size_t length = strnlen_s(buff, 1024);

    if (length < 1023)
    {
        buff[length] = '\n';
        buff[length + 1] = '\0';
        OutputDebugStringA(buff);
    }
    else
    {
        OutputDebugStringA(buff);
        OutputDebugStringA("\n");
    }
}


void EnableDebugLayer()
{
    ComPtr<ID3D12Debug> debugLayer;
    ResultUtil result = D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer));
    if (result)
    {
        debugLayer->EnableDebugLayer();
    }
}


SampleApp::SampleApp(IApp* pApp)
    : m_pApp(pApp)
    , m_BufferCount(2)
    , m_BufferFormat(DXGI_FORMAT_R8G8B8A8_UNORM)
    , m_FeatureLevel(D3D_FEATURE_LEVEL_11_1)
{

}

SampleApp::~SampleApp()
{
    Term();
}

// 初期化
bool SampleApp::Init()
{
    ResultUtil result;

#ifdef _DEBUG
    EnableDebugLayer();
#endif

    // ウィンドウハンドルを取得
    HWND hWnd = reinterpret_cast<HWND>(m_pApp->GetWindowHandle());

    // 描画領域のサイズを取得
    const Size2D& clientSize = m_pApp->GetClientSize();

    // ファクトリーを生成
#ifdef _DEBUG
    result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&m_Factory));
#else
    result = CreateDXGIFactory1(IID_PPV_ARGS(&m_Factory));
#endif
    if (!result)
    {
        ShowErrorMessage(result, "CreateDXGIFactory");
        return false;
    }

    // アダプターを列挙
    {
        std::vector<ComPtr<IDXGIAdapter>> adapters;
        for (UINT i = 0; ; i++)
        {
            ComPtr<IDXGIAdapter> adapter;

            if (m_Factory->EnumAdapters(i, &adapter) == DXGI_ERROR_NOT_FOUND)
            {
                break;
            }

            adapters.push_back(adapter);
        }

        SIZE_T largestVideoMemory = 0;
        for (auto& adapter : adapters)
        {
            DXGI_ADAPTER_DESC adapterDesc = {};

            result = adapter->GetDesc(&adapterDesc);

            if (!result) { continue; }

            // とりあえず一番大きいビデオメモリを持つものを選択
            if (largestVideoMemory > adapterDesc.DedicatedVideoMemory)
            {
                largestVideoMemory = adapterDesc.DedicatedVideoMemory;
                m_Adapter = adapter;
            }
        }
    }

    // D3D12デバイスを生成
    {
        struct {
            D3D_FEATURE_LEVEL featureLevel;
            char featureName[16];
        }
        featureLevels[] = {
            { D3D_FEATURE_LEVEL_12_1, "12.1" },
            { D3D_FEATURE_LEVEL_12_0, "12.0" },
            { D3D_FEATURE_LEVEL_11_1, "11.1" },
            { D3D_FEATURE_LEVEL_11_0, "11.0" },
        };

        for (int i = 0; i < _countof(featureLevels); i++)
        {
            result = D3D12CreateDevice(
                m_Adapter.Get(),
                featureLevels[i].featureLevel,
                IID_PPV_ARGS(&m_Device)
            );
            if (result)
            {
                DebugOutputFormatString("FeatureLevel[%s] is selected.", featureLevels[i].featureName);
                break;
            }
        }

        if (!result)
        {
            ShowErrorMessage(result, "D3D12CreateDevice");
            return false;
        }

//#ifdef _DEBUG
//        ID3D12DebugDevice* debugInterface = nullptr;
//        result = m_Device->QueryInterface(&debugInterface);
//        if (!result)
//        {
//            ShowErrorMessage(result, "ID3D12Device::QueryInterface<ID3D12DebugDevice>");
//            return false;
//        }
//
//        result = debugInterface->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL);
//        if (!result)
//        {
//            ShowErrorMessage(result, "ID3D12DebugDevice::ReportLiveDeviceObjects");
//            return false;
//        }
//        
//        debugInterface->Release();
//#endif
    }

    // コマンドリスト作成
    {
        result = m_Device->CreateCommandAllocator(
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            IID_PPV_ARGS(&m_CommandAllocator)
        );
        if (!result)
        {
            ShowErrorMessage(result, "ID3D12Device::CreateCommandAllocator");
            return false;
        }

        result = m_Device->CreateCommandList(
            0,
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            m_CommandAllocator.Get(),
            nullptr,
            IID_PPV_ARGS(&m_GraphicsCommandList)
        );
        if (!result)
        {
            ShowErrorMessage(result, "ID3D12Device::CreateCommandList");
            return false;
        }
    }

    // コマンドキュー作成
    {
        D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};

        // タイムアウトなし
        commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

        // アダプターを一つしか使わない時は「0」で良い
        commandQueueDesc.NodeMask = 0;

        // プライオリティは特に指定なし
        commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;

        // コマンドリストと合わせる
        commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

        result = m_Device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&m_CommandQueue));
        if (!result)
        {
            ShowErrorMessage(result, "ID3D12Device::CreateCommandQueue");
        }
    }

    // スワップチェインを生成
    {
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};

        swapChainDesc.Width = static_cast<UINT>(clientSize.width);
        swapChainDesc.Height = static_cast<UINT>(clientSize.height);
        swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.Stereo = FALSE;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
        swapChainDesc.BufferCount = m_BufferCount;

        // バックバッファは伸び縮み可能
        swapChainDesc.Scaling = DXGI_SCALING_STRETCH;

        // フリップ後は速やかに破棄
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

        // 特に指定なし
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

        // ウィンドウ ⇔ フルスクリーン切り替え可能
        swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

        ComPtr<IDXGISwapChain1> swapChain;
        result = m_Factory->CreateSwapChainForHwnd(
            m_CommandQueue.Get(),
            hWnd,
            &swapChainDesc,
            nullptr,
            nullptr,
            &swapChain
        );
        if (!result)
        {
            ShowErrorMessage(result, "IDXGIFactory6::CreateSwapChainForHwnd");
            return false;
        }
        swapChain.As(&m_SwapChain);
    }

    // RTV用 ディスクリプタヒープ生成
    {
        D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};

        // アダプターを一つしか使わない時は「0」で良い
        descriptorHeapDesc.NodeMask = 0;

        // 特に指定なし
        descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

        // レンダーターゲットビュー
        descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

        // バックバッファの数分
        descriptorHeapDesc.NumDescriptors = m_BufferCount;

        result = m_Device->CreateDescriptorHeap(
            &descriptorHeapDesc,
            IID_PPV_ARGS(&m_RTVHeaps)
        );
        if (!result)
        {
            ShowErrorMessage(result, "ID3D12Device::CreateDescriptorHeap");
            return false;
        }
    }

    // バックバッファ生成
    {
        m_BackBuffers.resize(m_BufferCount);
        D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle = m_RTVHeaps->GetCPUDescriptorHandleForHeapStart();

        for (UINT i = 0; i < m_BufferCount; i++)
        {
            result = m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_BackBuffers[i]));
            if (!result)
            {
                ShowErrorMessage(result, "IDXGISwapChain4::GetBuffer");
                return false;
            }

            m_Device->CreateRenderTargetView(
                m_BackBuffers[i].Get(),
                nullptr,
                descriptorHandle
            );

            descriptorHandle.ptr += m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        }
    }

    // フェンスの生成
    {
        m_FenceValue = 0;
        result = m_Device->CreateFence(
            m_FenceValue,
            D3D12_FENCE_FLAG_NONE,
            IID_PPV_ARGS(&m_Fence)
        );
        if (!result)
        {
            ShowErrorMessage(result, "ID3D12Device::CreateFence");
            return false;
        }
    }

    return true;
}

// 解放
void SampleApp::Term()
{

}

// 更新処理
void SampleApp::Update()
{

}

// 描画処理
void SampleApp::Render()
{
    ResultUtil result;

    UINT backBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();

    // リソースバリア
    {
        D3D12_RESOURCE_BARRIER resourceBarrier = {};
        resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; // 状態遷移
        resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE; // 特別なことはしないので特に指定しない
        resourceBarrier.Transition.pResource = m_BackBuffers[backBufferIndex].Get();
        resourceBarrier.Transition.Subresource = 0;
        resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT; // 直前は Present 状態
        resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // 今から RenderTarget 状態
        m_GraphicsCommandList->ResourceBarrier(1, &resourceBarrier);
    }

    
    // レンダーターゲットの設定
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_RTVHeaps->GetCPUDescriptorHandleForHeapStart();
    rtvHandle.ptr += backBufferIndex * m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    m_GraphicsCommandList->OMSetRenderTargets(1, &rtvHandle, TRUE, nullptr);

    // 画面クリア
    {
        FLOAT clearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red, green, blue, alpha
        m_GraphicsCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
    }

    // リソースバリア
    {
        D3D12_RESOURCE_BARRIER resourceBarrier = {};
        resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION; // 状態遷移
        resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE; // 特別なことはしないので特に指定しない
        resourceBarrier.Transition.pResource = m_BackBuffers[backBufferIndex].Get();
        resourceBarrier.Transition.Subresource = 0;
        resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET; // 直前は RenderTarget 状態
        resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT; // 今から Present 状態
        m_GraphicsCommandList->ResourceBarrier(1, &resourceBarrier);
    }

    // 命令のクローズ
    result = m_GraphicsCommandList->Close();
    if (!result)
    {
        ShowErrorMessage(result, "ID3D12GraphicsCommandList::Close");
        return;
    }

    // コマンドリストの実行
    {
        ID3D12CommandList* commandLists[] = { m_GraphicsCommandList.Get() };
        m_CommandQueue->ExecuteCommandLists(
            _countof(commandLists),
            commandLists
        );
    }

    // 待ち
    m_CommandQueue->Signal(m_Fence.Get(), ++m_FenceValue);

    // 画面フリップ
    result = m_SwapChain->Present(1, 0);
    if (!result)
    {
        ShowErrorMessage(result, "IDXGISwapChain4::Present");
        return;
    }

    if (m_Fence->GetCompletedValue() != m_FenceValue)
    {
        HANDLE eventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);

        result = m_Fence->SetEventOnCompletion(m_FenceValue, eventHandle);
        if (!result)
        {
            ShowErrorMessage(result, "ID3D12Fence::SetEventOnCompletion");
            return;
        }

        WaitForSingleObject(eventHandle, INFINITE);

        CloseHandle(eventHandle);
    }

    // コマンドリストクリア
    {
        //キューをクリア
        result = m_CommandAllocator->Reset();
        if (!result)
        {
            ShowErrorMessage(result, "ID3D12CommandAllocator::Reset");
            return;
        }

        //再びコマンドリストをためる準備
        result = m_GraphicsCommandList->Reset(m_CommandAllocator.Get(), nullptr);
        if (!result)
        {
            ShowErrorMessage(result, "ID3D12GraphicsCommandList::Reset");
            return;
        }
    }
}

// リサイズ
void SampleApp::OnResize(const Size2D& newSize)
{
    // バックバッファを再生成
    CreateBackBuffer(newSize);
}

// キー
void SampleApp::OnKey(KEY_CODE key, bool isDown)
{
    if (key == KEY_CODE_ESCAPE && isDown)
    {
        m_pApp->PostQuit();
    }
}

// マウスボタン
void SampleApp::OnMouse(const Position2D& position, MOUSE_BUTTON button, bool isDown)
{
    position; button; isDown;
}

// マウスホイール
void SampleApp::OnMouseWheel(const Position2D& position, s32 wheelDelta)
{
    position; wheelDelta;
}


// バックバッファを作成
bool SampleApp::CreateBackBuffer(const Size2D& newSize)
{
    newSize;
    return true;
}


// エラーメッセージ表示
void SampleApp::ShowErrorMessage(const ResultUtil& result, const std::string& text)
{
    m_pApp->ShowMessageBox(
        result.GetText() + "\n\n" + text,
        "エラー"
    );
}

