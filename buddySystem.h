#pragma once
#include"d3dUtil.h"
#include<cmath>
#include<process.h>
#include <atlcoll.h>
#include<windowsx.h>


class buddySystem {
public:
    buddySystem();
    buddySystem(UINT totalSizeInKB, UINT hierarchy, ID3D12Device4* device);
    virtual void init(UINT totalSizeInKB, UINT hierarchy, ID3D12Device4* device);
    buddyID createPlacedBufferResourceInBS(ID3D12Resource** BufferResource, ID3D12Device4* device, UINT BufferSize);
    buddyID allocate(UINT size);
    void free(buddyID ID);
    EST::vector<int>* stateList;
    UINT totalSize;
    UINT minSize;
    UINT Hierarchy;
    ComPtr<ID3D12Heap> Heap;
};
class uploadBuddySystem :buddySystem {
public:
    uploadBuddySystem();
    uploadBuddySystem(UINT totalSizeInKB, UINT hierarchy, ID3D12Device4* device);
    void init(UINT totalSizeInKB, UINT hierarchy, ID3D12Device4* device)override;
    buddyID createPlacedBufferResourceInBS(ID3D12Resource** BufferResource, ID3D12Device4* device, UINT BufferSize);
    buddyID allocate(UINT size);
    void free(buddyID ID);
};
class defaultBuddySystem :buddySystem {
public:
    defaultBuddySystem() ;
    defaultBuddySystem(UINT totalSizeInKB, UINT hierarchy, ID3D12Device4* device);
    void init(UINT totalSizeInKB, UINT hierarchy, ID3D12Device4* device)override;
    buddyID createPlacedBufferResourceInBS(ID3D12Resource** BufferResource, ID3D12Device4* device, UINT BufferSize);
    buddyID allocate(UINT size);
    void free(buddyID ID);
};