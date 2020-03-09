#pragma once


struct Vertex_Position
{
    DirectX::XMFLOAT3 position;

    static constexpr D3D12_INPUT_ELEMENT_DESC pInputElementDescs[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    static constexpr UINT NumElements = _countof(pInputElementDescs);
};
