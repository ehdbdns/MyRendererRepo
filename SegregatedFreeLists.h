#pragma once
#include"d3dUtil.h"
#include<cmath>
#include<process.h>
#include <atlcoll.h>
#include<windowsx.h>
class SegregatedFreeLists {
public:
    SegregatedFreeLists();
};
class RT_DS_TextureSegregatedFreeLists :SegregatedFreeLists {
public:
    RT_DS_TextureSegregatedFreeLists();
    RT_DS_TextureSegregatedFreeLists(UINT minsizeinKB, UINT listnum, ID3D12Device4* device);
    ~RT_DS_TextureSegregatedFreeLists();
    void freeDefaultResource(ResourceID ID);
    void init(UINT minsizeinKB, UINT listnum, ID3D12Device4* device);
    ResourceID createPlacedResourceInDefaultSFL(ID3D12Resource** Tex, D3D12_RESOURCE_DESC* TextureDesc, D3D12_CLEAR_VALUE* dsclear);
private:
    ID3D12Device4* Device;
    EST::vector<ComPtr<ID3D12Heap>>DefaultHeaps;
    UINT minSizeInKB;
    UINT listNum;
    EST::vector<int>(*DefaultHeapDeadLists);
    EST::vector<int>(*DefaultHeapState);
};
class NON_RT_DS_TextureSegregatedFreeLists :SegregatedFreeLists {
public:
    NON_RT_DS_TextureSegregatedFreeLists();
    NON_RT_DS_TextureSegregatedFreeLists(UINT minsizeinKB, UINT listnum, ID3D12Device4* device);
    ~NON_RT_DS_TextureSegregatedFreeLists();
    void freeUploadResource(ResourceID ID);
    void freeDefaultResource(ResourceID ID);
    void init(UINT minsizeinKB, UINT listnum, ID3D12Device4* device);
    ResourceID createPlacedResourceInUploadTexSFLHeap(ID3D12Resource** Tex, UINT BufferSize);
    ResourceID createPlacedResourceInDefaultSFL(ID3D12Resource** Tex, D3D12_RESOURCE_DESC* TextureDesc);
private:
    ID3D12Device4* Device;
    EST::vector<ComPtr<ID3D12Heap>>UploadHeaps;
    EST::vector<ComPtr<ID3D12Heap>>DefaultHeaps;
    EST::vector<int>* UploadHeapDeadLists;
    EST::vector<int>* DefaultHeapDeadLists;
    UINT minSizeInKB;
    UINT listNum;
    EST::vector<int>* UploadHeapState;
    EST::vector<int>* DefaultHeapState;
};