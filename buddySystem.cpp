#include"buddySystem.h"
buddySystem::buddySystem() = default;
buddySystem::buddySystem(UINT totalSizeInKB, UINT hierarchy, ID3D12Device4* device) {
    init(totalSizeInKB, hierarchy, device);
}
 void buddySystem::init(UINT totalSizeInKB, UINT hierarchy, ID3D12Device4* device) {
    totalSize = totalSizeInKB * 1024;
    Hierarchy = hierarchy;
    minSize = totalSize / pow(2, hierarchy - 1);
    stateList = new EST::vector<int>[hierarchy];
    for (int i = 0;i < hierarchy;i++) {
        int num = pow(2, hierarchy - i - 1);
        stateList[i].resize(num);
        stateList[i].memset(stateList[i].Getdata(), num, 0);
    }
}
buddyID buddySystem::createPlacedBufferResourceInBS(ID3D12Resource** BufferResource, ID3D12Device4* device, UINT BufferSize) {
    buddyID ID = allocate(BufferSize);
    if (ID.index != -1) {
        ThrowIfFailed(device->CreatePlacedResource(
            Heap.Get()
            , ID.index * minSize * pow(2, ID.level)
            , &CD3DX12_RESOURCE_DESC::Buffer(BufferSize)
            , D3D12_RESOURCE_STATE_GENERIC_READ
            , nullptr
            , IID_PPV_ARGS(BufferResource)));
    }
    return ID;
}
buddyID buddySystem::allocate(UINT size) {
    buddyID ID = { -1,-1 };
    UINT currentSize = minSize;
    int level = 0;
    while (size > currentSize) {
        level++;
        currentSize *= 2;
        if (level > Hierarchy - 1)
            return ID;
    }
    for (int i = 0;i < stateList[level].size();i++) {
        if (stateList[level][i] == 0) {
            stateList[level][i] = 1;
            for (int j = level + 1;j < Hierarchy - 1;j++)
                stateList[j][i / pow(2, j - level)] = 1;
            for (int j = level - 1;j >= 0;j--) {
                for (int k = 0;k < pow(2, level - j);k++)
                    stateList[j][i * pow(2, level - j) + k] = 1;
            }
            ID.level = level;
            ID.index = i;
            return ID;
        }
    }
    return ID;
}
void buddySystem::free(buddyID ID) {
    stateList[ID.level][ID.index] = 0;
    for (int j = ID.level;j < Hierarchy - 1;j++) {
        int cindex = ID.index / pow(2, j - ID.level);
        stateList[j][cindex] = 0;
        if (ID.index % 2 == 0) {
            if (stateList[j][cindex + 1] == 0)
                continue;
            break;
        }
        else {
            if (stateList[j][cindex - 1] == 0)
                continue;
            break;
        }
    }
    for (int j = ID.level - 1;j >= 0;j--) {
        for (int k = 0;k < pow(2, ID.level - j);k++)
            stateList[j][ID.index * pow(2, ID.level - j) + k] = 0;
    }
}
uploadBuddySystem::uploadBuddySystem() = default;
uploadBuddySystem::uploadBuddySystem(UINT totalSizeInKB, UINT hierarchy, ID3D12Device4* device) {
    init(totalSizeInKB, hierarchy, device);
}
void uploadBuddySystem::init(UINT totalSizeInKB, UINT hierarchy, ID3D12Device4* device) {
    buddySystem::init(totalSizeInKB, hierarchy, device);
    D3D12_HEAP_DESC stDefaultHeapDesc = {  };
    stDefaultHeapDesc.Alignment = 0;
    stDefaultHeapDesc.Properties.Type = D3D12_HEAP_TYPE_UPLOAD;		//上传堆类型
    stDefaultHeapDesc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    stDefaultHeapDesc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    stDefaultHeapDesc.Flags = D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS;
    stDefaultHeapDesc.SizeInBytes = totalSize;
    ThrowIfFailed(device->CreateHeap(&stDefaultHeapDesc, IID_PPV_ARGS(&Heap)));
}
buddyID uploadBuddySystem::createPlacedBufferResourceInBS(ID3D12Resource** BufferResource, ID3D12Device4* device, UINT BufferSize) {
    return  buddySystem::createPlacedBufferResourceInBS(BufferResource, device, BufferSize);
}
buddyID uploadBuddySystem::allocate(UINT size) {
    return  buddySystem::allocate(size);
}
void uploadBuddySystem::free(buddyID ID) {
    buddySystem::free(ID);
}
defaultBuddySystem::defaultBuddySystem() = default;
defaultBuddySystem::defaultBuddySystem(UINT totalSizeInKB, UINT hierarchy, ID3D12Device4* device) {
    init(totalSizeInKB, hierarchy, device);
}
void defaultBuddySystem::init(UINT totalSizeInKB, UINT hierarchy, ID3D12Device4* device) {
    buddySystem::init(totalSizeInKB, hierarchy, device);
    D3D12_HEAP_DESC stDefaultHeapDesc = {  };
    stDefaultHeapDesc.Alignment = 0;
    stDefaultHeapDesc.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;
    stDefaultHeapDesc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    stDefaultHeapDesc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    stDefaultHeapDesc.Flags = D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS;
    stDefaultHeapDesc.SizeInBytes = totalSize;
    ThrowIfFailed(device->CreateHeap(&stDefaultHeapDesc, IID_PPV_ARGS(&Heap)));
}
buddyID defaultBuddySystem::createPlacedBufferResourceInBS(ID3D12Resource** BufferResource, ID3D12Device4* device, UINT BufferSize) {
    return buddySystem::createPlacedBufferResourceInBS(BufferResource, device, BufferSize);
}
buddyID defaultBuddySystem::allocate(UINT size) {
    return buddySystem::allocate(size);
}
void defaultBuddySystem::free(buddyID ID) {
    buddySystem::free(ID);
}