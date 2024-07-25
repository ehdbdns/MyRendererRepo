#pragma once
#include"d3dUtil.h"
#include<cmath>
#include<process.h>
#include <atlcoll.h>
#include<windowsx.h>
#include"buddySystem.h"
#include"SegregatedFreeLists.h"
class ResourceItem {

};
class SamplerResourceItem :ResourceItem {
public:
    SamplerResourceItem();
    SamplerResourceItem(ID3D12Device4* device, ID3D12DescriptorHeap* samplerheap, D3D12_SAMPLER_DESC samplerDesc, int offsetInHeap);
    void init(ID3D12Device4* device, ID3D12DescriptorHeap* samplerheap, D3D12_SAMPLER_DESC samplerDesc, int offsetInHeap);
    CD3DX12_GPU_DESCRIPTOR_HANDLE getSampler();
    ID3D12DescriptorHeap* samplerHeap = nullptr;
    int samplerOffset;
private:
    UINT SamplerSize;
};
class TextureResourceItem :ResourceItem {//先初始化，然后调用创建资源函数，然后调用创建SRV函数
public:
    TextureResourceItem();
    TextureResourceItem(ID3D12Device4* device, ID3D12DescriptorHeap* srvuavheap, ID3D12DescriptorHeap* rtvheap, ID3D12DescriptorHeap* dsvheap, bool issticker, int texindex);
    void init(ID3D12Device4* device, ID3D12DescriptorHeap* srvuavheap, ID3D12DescriptorHeap* rtvheap, ID3D12DescriptorHeap* dsvheap, bool issticker, int texindex);
    void setTextureToRI(ComPtr<ID3D12Resource> tex);
    ID3D12Resource* getResource();
    void createStickerTex(ID3D12Device4* device, ID3D12GraphicsCommandList* cmdlist, wchar_t* fileName);
    void createNON_RT_DS_WritableTex(ID3D12Device4* device, ID3D12GraphicsCommandList* cmdlist, D3D12_RESOURCE_DESC* TextureDesc, std::unordered_map<std::string, std::unique_ptr< NON_RT_DS_TextureSegregatedFreeLists>>* NONRTDSSFLTable);
    void createRT_DS_WritableTex(ID3D12Device4* device, ID3D12GraphicsCommandList* cmdlist, D3D12_RESOURCE_DESC* TextureDesc, D3D12_CLEAR_VALUE* dsclear, RT_DS_TextureSegregatedFreeLists* sfl);
    void createSRVforResourceItem(D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc, ID3D12Device4* device, UINT SRVOffsetInHeap);
    void createUAVforResourceItem(D3D12_UNORDERED_ACCESS_VIEW_DESC* uavDesc, ID3D12Device4* device, UINT UAVOffsetInHeap, ID3D12GraphicsCommandList* cmdlist);
    void createDSVforResourceItem(D3D12_DEPTH_STENCIL_VIEW_DESC* dsvDesc, ID3D12Device4* device, UINT DSVOffsetInHeap, ID3D12GraphicsCommandList* cmdlist);
    void createRTVforResourceItem(D3D12_RENDER_TARGET_VIEW_DESC* rtvDesc, ID3D12Device4* device, UINT RTVOffsetInHeap, ID3D12GraphicsCommandList* cmdlist);
    CD3DX12_GPU_DESCRIPTOR_HANDLE getSRVGPU(int SRVnote);
    CD3DX12_GPU_DESCRIPTOR_HANDLE getUAVGPU(int UAVnote);
    CD3DX12_CPU_DESCRIPTOR_HANDLE getUAVCPU(int UAVnote);
    CD3DX12_CPU_DESCRIPTOR_HANDLE getRTVCPU();
    CD3DX12_CPU_DESCRIPTOR_HANDLE getDSVCPU();
    ID3D12DescriptorHeap* RTVHeap = nullptr;
    ID3D12DescriptorHeap* DSVHeap = nullptr;
    ID3D12DescriptorHeap* SRVUAVHeap = nullptr;
    NON_RT_DS_TextureSegregatedFreeLists* NON_RT_DS_SFL = nullptr;
    RT_DS_TextureSegregatedFreeLists* RT_DS_SFL = nullptr;
    ResourceID ID;
    int RTVoffset;
    int DSVoffset;
    EST::vector<int>SRVOffsetList;
    EST::vector<int>UAVOffsetList;
    int TextureIndex;
private:
    bool isSticker = true;
    UINT SRVUAVincrementSize;
    UINT DSVincrementSize;
    UINT RTVincrementSize;
    ComPtr< ID3D12Resource> Texture;
    ComPtr< ID3D12Resource> TextureUpload;
};
template<class T>
class ConstantBufferResourceItem :ResourceItem {//管理cbv以及对应结构体以及资源本身,构造函数创建资源，并map，更新函数可以memcpy更新，但创建完资源项得自行创建CBV并赋值偏移量
public:
    ConstantBufferResourceItem();
    ConstantBufferResourceItem(ID3D12Device4* device, ID3D12GraphicsCommandList* cmdlist, T* strP, bool isstatic, ID3D12DescriptorHeap* cbvheap, uploadBuddySystem* upBS, defaultBuddySystem* defBS);
    void init(ID3D12Device4* device, ID3D12GraphicsCommandList* cmdlist, T* strP, bool isstatic, ID3D12DescriptorHeap* cbvheap, uploadBuddySystem* upbs, defaultBuddySystem* defbs);
    void updateCB(T* strP);
    CD3DX12_GPU_DESCRIPTOR_HANDLE getCBVGPU();
    ID3D12DescriptorHeap* CBVHeap = nullptr;

    UINT strSize;
    int CBVoffset;
    T str;
    bool isStatic = true;
    uploadBuddySystem* upBS = nullptr;
    defaultBuddySystem* defBS = nullptr;
    buddyID upID;
    buddyID defID;
private:
    byte* cbmapped = nullptr;
    UINT SRVCBVUAVincrementSize;
    ComPtr< ID3D12Resource> constantBufferDefault;
    ComPtr< ID3D12Resource> constantBufferUpload;
};
template<class T>
class StructureBufferResourceItem :ResourceItem {//管理cbv以及对应结构体以及资源本身,构造函数创建资源，并map，更新函数可以memcpy更新，但创建完资源项得自行创建CBV并赋值偏移量
public:
    StructureBufferResourceItem();
    StructureBufferResourceItem(ID3D12Device4* device, ID3D12GraphicsCommandList* cmdlist, EST::vector<T>* vec, bool isstatic, ID3D12DescriptorHeap* cbvheap, uploadBuddySystem* upBS, defaultBuddySystem* defBS, UINT elementNum);
    void init(ID3D12Device4* device, ID3D12GraphicsCommandList* cmdlist, EST::vector<T>* vec, bool isstatic, ID3D12DescriptorHeap* srvHeap, uploadBuddySystem* upbs, defaultBuddySystem* defbs, UINT elementNum);
    void updateCB(EST::vector<T>* vec);
    CD3DX12_GPU_DESCRIPTOR_HANDLE getSRVGPU();
    ID3D12DescriptorHeap* SRVHeap = nullptr;

    UINT strSize;
    int SRVoffset;
    T* str;
    bool isStatic = true;
    uploadBuddySystem* upBS = nullptr;
    defaultBuddySystem* defBS = nullptr;
    buddyID upID;
    buddyID defID;
private:
    byte* cbmapped = nullptr;
    UINT SRVCBVUAVincrementSize;
    ComPtr< ID3D12Resource> StructureBufferDefault;
    ComPtr< ID3D12Resource> StructureBufferUpload;
};
template<class T>
ConstantBufferResourceItem<T>::ConstantBufferResourceItem() = default;
template<class T>
ConstantBufferResourceItem<T>::ConstantBufferResourceItem(ID3D12Device4* device, ID3D12GraphicsCommandList* cmdlist, T* strP, bool isstatic, ID3D12DescriptorHeap* cbvheap, uploadBuddySystem* upBS, defaultBuddySystem* defBS) {
    init(device, cmdlist, strP, isstatic, cbvheap, upBS, defBS);
}
template<class T>
void ConstantBufferResourceItem<T>::init(ID3D12Device4* device, ID3D12GraphicsCommandList* cmdlist, T* strP, bool isstatic, ID3D12DescriptorHeap* cbvheap, uploadBuddySystem* upbs, defaultBuddySystem* defbs) {
    this->isStatic = isstatic;
    this->CBVHeap = cbvheap;
    this->upBS = upbs;
    this->defBS = defbs;
    CBVoffset = HeapOffsetTable[CBVHeap];
    SRVCBVUAVincrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    this->str = *strP;
    D3D12_RANGE range = { 0,0 };
    strSize = sizeof(T) + 255 & ~255;
    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
    cbvDesc.SizeInBytes = strSize;
    if (isStatic) {
        defID = defBS->createPlacedBufferResourceInBS(&constantBufferDefault, device, strSize);
        upID = upBS->createPlacedBufferResourceInBS(&constantBufferUpload, device, strSize);
        constantBufferUpload->Map(0, &range, reinterpret_cast<void**>(&cbmapped));
        memcpy(cbmapped, &str, strSize);
        cmdlist->CopyBufferRegion(constantBufferDefault.Get(), 0, constantBufferUpload.Get(), 0, strSize);
        //D3D12_SUBRESOURCE_DATA subResourceData;
        //subResourceData.pData = &str;
        //subResourceData.RowPitch = strSize;
        //subResourceData.SlicePitch = subResourceData.RowPitch;
        //UpdateSubresources<1>(cmdlist, constantBufferDefault.Get(), constantBufferUpload.Get(), 0, 0, 1, &subResourceData);//这个函数只需先创建两个资源，然后他会帮你完成Map memcpy、unmap copyregion等操作
        cbvDesc.BufferLocation = constantBufferDefault->GetGPUVirtualAddress();
        //upBS->free(upID);//不要释放上传堆，他还有用
    }
    else {
        upID = upBS->createPlacedBufferResourceInBS(&constantBufferUpload, device, strSize);
        constantBufferUpload->Map(0, &range, reinterpret_cast<void**>(&cbmapped));
        memcpy(cbmapped, &str, strSize);
        cbvDesc.BufferLocation = constantBufferUpload->GetGPUVirtualAddress();
    }
    CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(CBVHeap->GetCPUDescriptorHandleForHeapStart(), CBVoffset, SRVCBVUAVincrementSize);
    device->CreateConstantBufferView(&cbvDesc, cpuHandle);
    HeapOffsetTable[CBVHeap]++;
}
template<class T>
void ConstantBufferResourceItem<T>::updateCB(T* strP) {
    if (isStatic)
        return;
    str = *strP;
    memcpy(cbmapped, &str, strSize);
}
template<class T>
CD3DX12_GPU_DESCRIPTOR_HANDLE ConstantBufferResourceItem<T>::getCBVGPU() {
    return CD3DX12_GPU_DESCRIPTOR_HANDLE(CBVHeap->GetGPUDescriptorHandleForHeapStart(), CBVoffset, SRVCBVUAVincrementSize);
}
template<class T>
StructureBufferResourceItem< T>::StructureBufferResourceItem() = default;

template<class T>
StructureBufferResourceItem< T>::StructureBufferResourceItem(ID3D12Device4* device, ID3D12GraphicsCommandList* cmdlist, EST::vector<T>* vec, bool isstatic, ID3D12DescriptorHeap* cbvheap, uploadBuddySystem* upBS, defaultBuddySystem* defBS, UINT elementNum) {
    init(device, cmdlist, vec, isstatic, cbvheap, upBS, defBS, elementNum);
}
template<class T>
void StructureBufferResourceItem< T>::init(ID3D12Device4* device, ID3D12GraphicsCommandList* cmdlist, EST::vector<T>* vec, bool isstatic, ID3D12DescriptorHeap* srvHeap, uploadBuddySystem* upbs, defaultBuddySystem* defbs, UINT elementNum) {
    this->isStatic = isstatic;
    this->SRVHeap = srvHeap;
    this->upBS = upbs;
    this->defBS = defbs;
    SRVCBVUAVincrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    this->str = vec->Getdata();
    D3D12_RANGE range = { 0,0 };
    strSize = (sizeof(T) * elementNum) + 255 & ~255;
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Buffer.FirstElement = 0;
    srvDesc.Buffer.NumElements = elementNum;
    srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
    srvDesc.Buffer.StructureByteStride = sizeof(T);
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    if (isStatic) {
        defID = defBS->createPlacedBufferResourceInBS(&StructureBufferDefault, device, strSize);
        upID = upBS->createPlacedBufferResourceInBS(&StructureBufferUpload, device, strSize);
        StructureBufferUpload->Map(0, &range, reinterpret_cast<void**>(&cbmapped));
        vec->memcpyPages(cbmapped, strSize);
        cmdlist->CopyBufferRegion(StructureBufferDefault.Get(), 0, StructureBufferUpload.Get(), 0, strSize);
        /*     D3D12_SUBRESOURCE_DATA subResourceData;
             subResourceData.pData = str;
             subResourceData.RowPitch = strSize;
             subResourceData.SlicePitch = subResourceData.RowPitch;
             UpdateSubresources<1>(cmdlist, StructureBufferDefault.Get(), StructureBufferUpload.Get(), 0, 0, 1, &subResourceData);*///这个函数只需先创建两个资源，然后他会帮你完成Map memcpy、unmap copyregion等操作
        CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(SRVHeap->GetCPUDescriptorHandleForHeapStart(), HeapOffsetTable[SRVHeap], SRVCBVUAVincrementSize);
        device->CreateShaderResourceView(StructureBufferDefault.Get(), &srvDesc, cpuHandle);
        SRVoffset = HeapOffsetTable[SRVHeap];
        HeapOffsetTable[SRVHeap]++;
        //upBS->free(upID);//不要释放上传堆，他还有用
    }
    else {
        upID = upBS->createPlacedBufferResourceInBS(&StructureBufferUpload, device, strSize);
        StructureBufferUpload->Map(0, &range, reinterpret_cast<void**>(&cbmapped));
        vec->memcpyPages(cbmapped, strSize);
        // memcpy(cbmapped, str, strSize);
        CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(SRVHeap->GetCPUDescriptorHandleForHeapStart(), HeapOffsetTable[SRVHeap], SRVCBVUAVincrementSize);
        device->CreateShaderResourceView(StructureBufferUpload.Get(), &srvDesc, cpuHandle);
        SRVoffset = HeapOffsetTable[SRVHeap];
        HeapOffsetTable[SRVHeap]++;
    }
}
template<class T>
void StructureBufferResourceItem< T>::updateCB(EST::vector<T>* vec) {
    if (isStatic)
        return;
    str = vec->Getdata();
    vec->memcpyPages(cbmapped, strSize);
    //  memcpy(cbmapped, str, strSize);
}
template<class T>
CD3DX12_GPU_DESCRIPTOR_HANDLE StructureBufferResourceItem< T>::getSRVGPU() {
    return CD3DX12_GPU_DESCRIPTOR_HANDLE(SRVHeap->GetGPUDescriptorHandleForHeapStart(), SRVoffset, SRVCBVUAVincrementSize);
}