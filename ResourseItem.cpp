#include"ResourceItem.h"
SamplerResourceItem::SamplerResourceItem() = default;
SamplerResourceItem::SamplerResourceItem(ID3D12Device4* device, ID3D12DescriptorHeap* samplerheap, D3D12_SAMPLER_DESC samplerDesc, int offsetInHeap) {
    init(device, samplerheap, samplerDesc, offsetInHeap);
}
void SamplerResourceItem::init(ID3D12Device4* device, ID3D12DescriptorHeap* samplerheap, D3D12_SAMPLER_DESC samplerDesc, int offsetInHeap) {
    samplerHeap = samplerheap;
    samplerOffset = offsetInHeap;
    CD3DX12_CPU_DESCRIPTOR_HANDLE samplerHandel(samplerheap->GetCPUDescriptorHandleForHeapStart());
    device->CreateSampler(&samplerDesc, samplerHandel);
    HeapOffsetTable[samplerHeap]++;
}
CD3DX12_GPU_DESCRIPTOR_HANDLE SamplerResourceItem::getSampler() {
    return CD3DX12_GPU_DESCRIPTOR_HANDLE(samplerHeap->GetGPUDescriptorHandleForHeapStart(), samplerOffset, SamplerSize);
}
TextureResourceItem::TextureResourceItem() = default;
TextureResourceItem::TextureResourceItem(ID3D12Device4* device, ID3D12DescriptorHeap* srvuavheap, ID3D12DescriptorHeap* rtvheap, ID3D12DescriptorHeap* dsvheap, bool issticker, int texindex) {
    init(device, srvuavheap, rtvheap, dsvheap, issticker, texindex);
}
void TextureResourceItem::init(ID3D12Device4* device, ID3D12DescriptorHeap* srvuavheap, ID3D12DescriptorHeap* rtvheap, ID3D12DescriptorHeap* dsvheap, bool issticker, int texindex) {
    SRVUAVincrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    DSVincrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    RTVincrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    RTVHeap = rtvheap;
    SRVUAVHeap = srvuavheap;
    DSVHeap = dsvheap;
    isSticker = issticker;
    TextureIndex = texindex;
}
void TextureResourceItem::setTextureToRI(ComPtr<ID3D12Resource> tex) {
    Texture = tex;
}
ID3D12Resource* TextureResourceItem::getResource() {
    return Texture.Get();
}
void TextureResourceItem::createStickerTex(ID3D12Device4* device, ID3D12GraphicsCommandList* cmdlist, wchar_t* fileName) {
    if (!isSticker)
    {
        return;
    }
    CreateDDSTextureFromFile12(device, cmdlist, fileName, Texture, TextureUpload);//这个库函数是用提交方式创建的，所以我们就让系统自己管理贴图资源,函数过程：loadDDS（）获得上传资源大小和已经创建好的默认堆资源，以及subresource，之后创建上传堆资源，之后updatesubresource
}
void TextureResourceItem::createNON_RT_DS_WritableTex(ID3D12Device4* device, ID3D12GraphicsCommandList* cmdlist, D3D12_RESOURCE_DESC* TextureDesc, std::unordered_map<std::string, std::unique_ptr< NON_RT_DS_TextureSegregatedFreeLists>>* NONRTDSSFLTable) {
    if (isSticker)
        return;
    for (std::unordered_map<std::string, std::unique_ptr< NON_RT_DS_TextureSegregatedFreeLists>>::iterator it = NONRTDSSFLTable->begin();it != NONRTDSSFLTable->end();it++) {
        ID = it->second->createPlacedResourceInDefaultSFL(&Texture, TextureDesc);
        if (ID.HeapIndex != -1)
            break;
    }
}
void TextureResourceItem::createRT_DS_WritableTex(ID3D12Device4* device, ID3D12GraphicsCommandList* cmdlist, D3D12_RESOURCE_DESC* TextureDesc, D3D12_CLEAR_VALUE* dsclear, RT_DS_TextureSegregatedFreeLists* sfl) {
    if (isSticker)
        return;
    RT_DS_SFL = sfl;
    ID = RT_DS_SFL->createPlacedResourceInDefaultSFL(&Texture, TextureDesc, dsclear);
}
void TextureResourceItem::createSRVforResourceItem(D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc, ID3D12Device4* device, UINT SRVOffsetInHeap) {
    srvDesc->Format = Texture->GetDesc().Format;
    if (srvDesc->ViewDimension == D3D12_SRV_DIMENSION_TEXTURE2D) {
        srvDesc->Texture2D.MipLevels = Texture->GetDesc().MipLevels;
    }
    else if (srvDesc->ViewDimension == D3D12_SRV_DIMENSION_TEXTURE3D) {
        srvDesc->Texture3D.MipLevels = Texture->GetDesc().MipLevels;
    }
    else if (srvDesc->ViewDimension == D3D12_SRV_DIMENSION_TEXTURECUBE) {
        srvDesc->TextureCube.MipLevels = Texture->GetDesc().MipLevels;
    }
    CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(SRVUAVHeap->GetCPUDescriptorHandleForHeapStart(), SRVOffsetInHeap, SRVUAVincrementSize);
    device->CreateShaderResourceView(Texture.Get(), srvDesc, cpuHandle);
    SRVOffsetList.push_back(SRVOffsetInHeap);
    HeapOffsetTable[SRVUAVHeap]++;
}
void TextureResourceItem::createUAVforResourceItem(D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc, ID3D12Device4* device, UINT UAVOffsetInHeap, ID3D12GraphicsCommandList* cmdlist) {
    CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(SRVUAVHeap->GetCPUDescriptorHandleForHeapStart(), UAVOffsetInHeap, SRVUAVincrementSize);
    device->CreateUnorderedAccessView(Texture.Get(), nullptr, uavDesc, cpuHandle);
    UAVOffsetList.push_back(UAVOffsetInHeap);
    HeapOffsetTable[SRVUAVHeap]++;
    cmdlist->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition
    (Texture.Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));
}
void TextureResourceItem::createDSVforResourceItem(D3D12_DEPTH_STENCIL_VIEW_DESC* dsvDesc, ID3D12Device4* device, UINT DSVOffsetInHeap, ID3D12GraphicsCommandList* cmdlist) {
    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(DSVHeap->GetCPUDescriptorHandleForHeapStart(), DSVOffsetInHeap, DSVincrementSize);
    device->CreateDepthStencilView(Texture.Get(), dsvDesc, dsvHandle);
    DSVoffset = DSVOffsetInHeap;
    HeapOffsetTable[DSVHeap]++;
    cmdlist->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition
    (Texture.Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE));
}
void TextureResourceItem::createRTVforResourceItem(D3D12_RENDER_TARGET_VIEW_DESC* rtvDesc, ID3D12Device4* device, UINT RTVOffsetInHeap, ID3D12GraphicsCommandList* cmdlist) {
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(RTVHeap->GetCPUDescriptorHandleForHeapStart(), RTVOffsetInHeap, RTVincrementSize);
    device->CreateRenderTargetView(Texture.Get(), rtvDesc, rtvHandle);
    RTVoffset = RTVOffsetInHeap;
    HeapOffsetTable[RTVHeap]++;
    cmdlist->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition
    (Texture.Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET));
}
CD3DX12_GPU_DESCRIPTOR_HANDLE TextureResourceItem::getSRVGPU(int SRVnote) {
    return CD3DX12_GPU_DESCRIPTOR_HANDLE(SRVUAVHeap->GetGPUDescriptorHandleForHeapStart(), SRVOffsetList[SRVnote], SRVUAVincrementSize);
}
CD3DX12_GPU_DESCRIPTOR_HANDLE TextureResourceItem::getUAVGPU(int UAVnote) {
    return CD3DX12_GPU_DESCRIPTOR_HANDLE(SRVUAVHeap->GetGPUDescriptorHandleForHeapStart(), UAVOffsetList[UAVnote], SRVUAVincrementSize);
}
CD3DX12_CPU_DESCRIPTOR_HANDLE TextureResourceItem::getUAVCPU(int UAVnote) {
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(SRVUAVHeap->GetCPUDescriptorHandleForHeapStart(), UAVOffsetList[UAVnote], SRVUAVincrementSize);
}
CD3DX12_CPU_DESCRIPTOR_HANDLE TextureResourceItem::getRTVCPU() {
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(RTVHeap->GetCPUDescriptorHandleForHeapStart(), RTVoffset, RTVincrementSize);
}
CD3DX12_CPU_DESCRIPTOR_HANDLE TextureResourceItem::getDSVCPU() {
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(DSVHeap->GetCPUDescriptorHandleForHeapStart(), DSVoffset, DSVincrementSize);
}
