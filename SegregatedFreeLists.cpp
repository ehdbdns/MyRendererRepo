#include"SegregatedFreeLists.h"
SegregatedFreeLists::SegregatedFreeLists() = default;
RT_DS_TextureSegregatedFreeLists::RT_DS_TextureSegregatedFreeLists() = default;
RT_DS_TextureSegregatedFreeLists::RT_DS_TextureSegregatedFreeLists(UINT minsizeinKB, UINT listnum, ID3D12Device4* device) {
    init(minsizeinKB, listnum, device);
}
RT_DS_TextureSegregatedFreeLists::~RT_DS_TextureSegregatedFreeLists() {
    delete[]DefaultHeapState;
}
void RT_DS_TextureSegregatedFreeLists::freeDefaultResource(ResourceID ID) {
    DefaultHeapDeadLists[ID.HeapIndex].push_back(ID.HeapOffset);
    DefaultHeapState[ID.HeapIndex][1]--;
}
void RT_DS_TextureSegregatedFreeLists::init(UINT minsizeinKB, UINT listnum, ID3D12Device4* device) {
    minSizeInKB = minsizeinKB;
    listNum = listnum;
    Device = device;
    DefaultHeaps.resize(listNum);
    DefaultHeapDeadLists = new EST::vector<int>[listnum];
    DefaultHeapState = new EST::vector<int>[listnum];
    for (int i = 0;i < listnum;i++) {
        DefaultHeapState[i].resize(2);
        DefaultHeapState[i].memset(DefaultHeapState[i].Getdata(), 2, 0);
    }
}
ResourceID RT_DS_TextureSegregatedFreeLists::createPlacedResourceInDefaultSFL(ID3D12Resource** Tex, D3D12_RESOURCE_DESC* TextureDesc, D3D12_CLEAR_VALUE* dsclear) {
    UINT currentSize = minSizeInKB * 1024;
    int level = 0;
    ResourceID ID = { -1,-1 };
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT  TexLayouts = {};
    UINT                                nNumRows = {};
    UINT64                              n64RowSizeInBytes = {};
    UINT64                              n64TotalBytes = 0;
    Device->GetCopyableFootprints(TextureDesc, 0, 1, 0, &TexLayouts, &nNumRows, &n64RowSizeInBytes, &n64TotalBytes);
    while (currentSize < n64TotalBytes) {
        currentSize *= 2;
        level++;
    }
    if (level > listNum - 1)
        return ID;
    int NumResource = pow(2, 3 - min(3, level));
    int SizeResource = pow(2, level) * minSizeInKB * 1024;
    if (DefaultHeapState[level][0] == 0)
    {
        DefaultHeapState[level][0] = 1;
        D3D12_HEAP_DESC stDefaultHeapDesc = {  };
        stDefaultHeapDesc.Alignment = 0;
        stDefaultHeapDesc.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;		//上传堆类型
        stDefaultHeapDesc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        stDefaultHeapDesc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        stDefaultHeapDesc.Flags = D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES;
        stDefaultHeapDesc.SizeInBytes = SizeResource * NumResource;
        auto h = (&DefaultHeaps[level]);
        ThrowIfFailed(Device->CreateHeap(&stDefaultHeapDesc, IID_PPV_ARGS(&DefaultHeaps[level])));
    }
    if (DefaultHeapState[level][1] < NumResource)
    {
        int offset;
        if (!DefaultHeapDeadLists[level].empty())
        {
            offset = DefaultHeapDeadLists[level].back();
            DefaultHeapDeadLists[level].pop_back();
        }
        else
            offset = DefaultHeapState[level][1];
        ThrowIfFailed(Device->CreatePlacedResource(
            DefaultHeaps[level].Get()
            , offset * SizeResource
            , TextureDesc
            , D3D12_RESOURCE_STATE_GENERIC_READ
            , dsclear
            , IID_PPV_ARGS(Tex)));
        DefaultHeapState[level][1]++;
        ID.HeapIndex = level;
        ID.HeapOffset = offset;
        return ID;
    }
    return ID;
}
NON_RT_DS_TextureSegregatedFreeLists::NON_RT_DS_TextureSegregatedFreeLists() = default;
NON_RT_DS_TextureSegregatedFreeLists::NON_RT_DS_TextureSegregatedFreeLists(UINT minsizeinKB, UINT listnum, ID3D12Device4* device) {
    init(minsizeinKB, listnum, device);
}
NON_RT_DS_TextureSegregatedFreeLists::~NON_RT_DS_TextureSegregatedFreeLists() {
    delete[]UploadHeapState;
    delete[]DefaultHeapState;
    delete[]UploadHeapDeadLists;
    delete[]DefaultHeapDeadLists;
}
void NON_RT_DS_TextureSegregatedFreeLists::freeUploadResource(ResourceID ID) {
    UploadHeapDeadLists[ID.HeapIndex].push_back(ID.HeapOffset);
    UploadHeapState[ID.HeapIndex][1]--;
}
void NON_RT_DS_TextureSegregatedFreeLists::freeDefaultResource(ResourceID ID) {
    DefaultHeapDeadLists[ID.HeapIndex].push_back(ID.HeapOffset);
    DefaultHeapState[ID.HeapIndex][1]--;
}
void NON_RT_DS_TextureSegregatedFreeLists::init(UINT minsizeinKB, UINT listnum, ID3D12Device4* device) {
    minSizeInKB = minsizeinKB;
    listNum = listnum;
    Device = device;
    UploadHeaps.resize(listNum);
    DefaultHeaps.resize(listNum);
    UploadHeapState = new EST::vector<int>[listnum];
    DefaultHeapState = new EST::vector<int>[listnum];
    UploadHeapDeadLists = new EST::vector<int>[listnum];
    DefaultHeapDeadLists = new EST::vector<int>[listnum];
    for (int i = 0;i < listnum;i++) {
        UploadHeapState[i].resize(2);
        DefaultHeapState[i].resize(2);
        UploadHeapState[i].memset(UploadHeapState[i].Getdata(), 2, 0);
        DefaultHeapState[i].memset(DefaultHeapState[i].Getdata(), 2, 0);
    }
}
ResourceID NON_RT_DS_TextureSegregatedFreeLists::createPlacedResourceInUploadTexSFLHeap(ID3D12Resource** Tex, UINT BufferSize) {
    UINT currentSize = minSizeInKB * 1024;
    int level = 0;
    ResourceID ID = { -1,-1 };
    while (currentSize < BufferSize) {
        currentSize *= 2;
        level++;
    }
    if (level > listNum - 1)
        return ID;
    int NumResource = pow(2, 3 - min(3, level));
    int SizeResource = pow(2, level) * minSizeInKB * 1024;
    if (UploadHeapState[level][0] == 0)
    {
        UploadHeapState[level][0] = 1;
        D3D12_HEAP_DESC stUploadHeapDesc = {  };
        stUploadHeapDesc.Alignment = 0;
        stUploadHeapDesc.Properties.Type = D3D12_HEAP_TYPE_UPLOAD;		//上传堆类型
        stUploadHeapDesc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        stUploadHeapDesc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        stUploadHeapDesc.Flags = D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES;
        stUploadHeapDesc.SizeInBytes = SizeResource * NumResource;
        ThrowIfFailed(Device->CreateHeap(&stUploadHeapDesc, IID_PPV_ARGS(&UploadHeaps[level])));
    }
    if (UploadHeapState[level][1] < NumResource)
    {
        int offset;
        if (!UploadHeapDeadLists[level].empty())
        {
            offset = UploadHeapDeadLists[level].back();
            UploadHeapDeadLists[level].pop_back();
        }
        else
            offset = UploadHeapState[level][1];
        ThrowIfFailed(Device->CreatePlacedResource(
            UploadHeaps[level].Get()
            , offset * SizeResource
            , &CD3DX12_RESOURCE_DESC::Buffer(BufferSize)
            , D3D12_RESOURCE_STATE_GENERIC_READ
            , nullptr
            , IID_PPV_ARGS(Tex)));
        UploadHeapState[level][1]++;//这个Heap存Resource的数量
        ID.HeapIndex = level;
        ID.HeapOffset = offset;
        return ID;
    }
    return ID;
}
ResourceID NON_RT_DS_TextureSegregatedFreeLists::createPlacedResourceInDefaultSFL(ID3D12Resource** Tex, D3D12_RESOURCE_DESC* TextureDesc) {
    UINT currentSize = minSizeInKB * 1024;
    int level = 0;
    ResourceID ID = { -1,-1 };
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT  TexLayouts = {};
    UINT                                nNumRows = {};
    UINT64                              n64RowSizeInBytes = {};
    UINT64                              n64TotalBytes = 0;
    Device->GetCopyableFootprints(TextureDesc, 0, 1, 0, &TexLayouts, &nNumRows, &n64RowSizeInBytes, &n64TotalBytes);
    while (currentSize <= n64TotalBytes) {
        currentSize *= 2;
        level++;
    }
    if (level > listNum - 1)
        return ID;
    int NumResource = pow(2, 3 - min(3, level));
    int SizeResource = pow(2, level) * minSizeInKB * 1024;
    if (DefaultHeapState[level][0] == 0)
    {
        DefaultHeapState[level][0] = 1;
        D3D12_HEAP_DESC stDefaultHeapDesc = {  };
        stDefaultHeapDesc.Alignment = 0;
        stDefaultHeapDesc.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;		//上传堆类型
        stDefaultHeapDesc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        stDefaultHeapDesc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        stDefaultHeapDesc.Flags = D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES;
        stDefaultHeapDesc.SizeInBytes = SizeResource * NumResource;
        ThrowIfFailed(Device->CreateHeap(&stDefaultHeapDesc, IID_PPV_ARGS(&DefaultHeaps[level])));
    }
    if (DefaultHeapState[level][1] < NumResource)
    {
        int offset;
        if (!DefaultHeapDeadLists[level].empty())
        {
            offset = DefaultHeapDeadLists[level].back();
            DefaultHeapDeadLists[level].pop_back();
        }
        else
            offset = DefaultHeapState[level][1];
        ThrowIfFailed(Device->CreatePlacedResource(
            DefaultHeaps[level].Get()
            , offset * SizeResource
            , TextureDesc
            , D3D12_RESOURCE_STATE_GENERIC_READ
            , nullptr
            , IID_PPV_ARGS(Tex)));
        DefaultHeapState[level][1]++;
        ID.HeapIndex = level;
        ID.HeapOffset = offset;
        return ID;
    }
    return ID;
}