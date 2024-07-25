#pragma once
#include"d3dUtil.h"
#include<cmath>
#include<process.h>
#include <atlcoll.h>
#include<windowsx.h>
#include"SAHtree.h"
#include"ResourceItem.h"
#define GRS_UPPER(A,B) ((UINT)(((A)+((B)-1))&~(B - 1)))

#define PI 3.1415926f

struct GeometryItem {//构造函数传入顶点及其索引，然后调用创建静态or动态顶点，最后获取其vbvibv即可；
public:
    GeometryItem();
    ~GeometryItem();
    template <typename T>
    void createDynamicGeo(ID3D12Device4* device, EST::vector<T>* vertices, EST::vector<std::uint16_t>* indices, std::unordered_map<std::string, std::unique_ptr< uploadBuddySystem>>* upBSTable);
    template <typename T>
    void updateVB(EST::vector<T>* vec);
    template <typename T>
    void createStaticGeo(ID3D12Device4* device, ID3D12GraphicsCommandList* cmdlist, EST::vector<T>* vertices, EST::vector<std::uint16_t>* indices, std::unordered_map<std::string, std::unique_ptr< uploadBuddySystem>>* upBSTable, std::unordered_map<std::string, std::unique_ptr< defaultBuddySystem>>* defBSTable);
    D3D12_VERTEX_BUFFER_VIEW* getVBV();
    D3D12_INDEX_BUFFER_VIEW* getIBV();
    void freeVertexAndIndex();
    UINT indexNum;
private:
    UINT vbsize;
    UINT ibsize;
    UINT8* vmapped = nullptr;
    UINT8* imapped = nullptr;
    //EST::vector<Vertex>* vertices;
    //EST::vector<std::uint16_t>* indices;
    D3D12_VERTEX_BUFFER_VIEW vbv;
    D3D12_INDEX_BUFFER_VIEW ibv;
    ComPtr< ID3D12Resource> VertexBufferDefault;
    ComPtr< ID3D12Resource> VertexBufferUpload;
    ComPtr< ID3D12Resource> IndexBufferDefault;
    ComPtr< ID3D12Resource> IndexBufferUpload;

};
struct RenderItem {
    RenderItem();
    RenderItem(GeometryItem* geo, int instancenum, int basevertex, int startindex, int indexnum, int startinstance, D3D12_PRIMITIVE_TOPOLOGY primitive, objectconstant* objc, ID3D12Device4* device, ID3D12GraphicsCommandList* cmdlist, ID3D12DescriptorHeap* CBVHeap, uploadBuddySystem* upBS, defaultBuddySystem* defBS);
    void init(GeometryItem* geo, int instancenum, int basevertex, int startindex, int indexnum, int startinstance, D3D12_PRIMITIVE_TOPOLOGY primitive, objectconstant* objc, ID3D12Device4* device, ID3D12GraphicsCommandList* cmdlist, ID3D12DescriptorHeap* CBVHeap, uploadBuddySystem* upBS, defaultBuddySystem* defBS);
    std::unique_ptr< ConstantBufferResourceItem<objectconstant>>objconstantRI;
    D3D12_PRIMITIVE_TOPOLOGY Primitive;
    GeometryItem* Geo;
    int InstanceNum;
    int baseVertex;
    int startIndex;
    int indexNum;
    int startInstance;
    int threadNote;
};
struct RootSignatureItem {
public:
    RootSignatureItem();
    RootSignatureItem(ID3D12RootSignature* rs, int descriptorNum, int srvnum, int cbvnum, int uavnum, int samplernum, CD3DX12_DESCRIPTOR_RANGE* dt);
    void init(ID3D12RootSignature* rs, int descriptorNum, CD3DX12_DESCRIPTOR_RANGE* dt);
    int getSRVTableIndex(int srvIndex);
    int getCBVTableIndex(int cbvIndex);
    int getUAVTableIndex(int uavIndex);
    int getSamplerTableIndex(int samplerIndex);
    ID3D12RootSignature* rs = nullptr;
private:
    int SRVNum=0;
    int CBVNum=0;
    int UAVNum=0;
    int SamplerNum=0;
    EST::vector<int>SRVDescriptorTableIndex;
    EST::vector<int>CBVDescriptorTableIndex;
    EST::vector<int>UAVDescriptorTableIndex;
    EST::vector<int>SamplerDescriptorTableIndex;
};
class PSOItem {
public:
    PSOItem();
    PSOItem(D3D12_GRAPHICS_PIPELINE_STATE_DESC* PSOdesc, std::wstring ShaderFileName, ID3D12Device4* device, D3D_SHADER_MACRO* defines, std::string PSName);
    ID3D12PipelineState* PSO;
private:
    ComPtr<ID3DBlob>vsshader = nullptr;
    ComPtr<ID3DBlob>psshader = nullptr;
};
class computePSOItem {
public:
    computePSOItem();
    computePSOItem(D3D12_COMPUTE_PIPELINE_STATE_DESC* computePSOdesc, std::wstring ShaderFileName, ID3D12Device4* device, const D3D_SHADER_MACRO* defines, std::string CSName);
    ID3D12PipelineState* PSO;
private:
    ComPtr<ID3DBlob>csshader = nullptr;
};
//函数声明
void drawRenderItem(RenderItem* ri, ID3D12GraphicsCommandList* cmdlist, int objcPara);
void drawRenderItems(std::unordered_map<std::string, std::unique_ptr< RenderItem>>* RIs, ID3D12GraphicsCommandList* cmdlist, int objcPara, RenderItem* out);
void BuildBoxAndTriangleSBRI(SAHtree* tree, std::unique_ptr<StructureBufferResourceItem<AABBbox>>& boxSBRI, std::unique_ptr<StructureBufferResourceItem<triangle>>& triangleSBRI, ID3D12Device4* device, ID3D12GraphicsCommandList* cmdlist, ID3D12DescriptorHeap* srvheap, uploadBuddySystem* upBS, defaultBuddySystem* defBS);
void GenerateRandomNum(EST::vector<float>& randnums, int num);
void GenerateOffsets(EST::vector<XMINT2>& offsets);
void ResourceBarrierTrans(ID3D12Resource* r, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after, ID3D12GraphicsCommandList* cmdlist);
void AddTrianglesToScene(EST::vector<triangle>& SceneTris, EST::vector<Vertex>& vertices, EST::vector<std::uint16_t>& indices, int texIndex, XMMATRIX world);
void updateLight(GeometryItem* LightGeo, PolygonalLight& l);
void CreateBox(float width, float height, float depth, EST::vector<Vertex>& vertices, EST::vector<std::uint16_t>& indices, XMFLOAT3 color);
meshdata subvide(meshdata& inmesh, int vsize, int isize);
void  createSphere(EST::vector<Vertex>& v, EST::vector<std::uint16_t>& ind, float r, XMFLOAT3 color);
EST::vector<float>CalcGaussWeights(float variance, float scale);



class APP {
public:
    APP();
    bool initDX12(HINSTANCE hInstance, WNDPROC CALLBACK    WndProc, int       nCmdShow);
    int iWidth = 1024;
    int iHeight = 768;
    UINT nDXGIFactoryFlags = 0U;
    HWND hWnd = nullptr;
    UINT ThreadNum = 0;
    lastVPmat lastvpmat;
    ComPtr<IDXGIFactory5>                pIDXGIFactory5;
    ComPtr<IDXGIAdapter1>                pIAdapter;
    ComPtr<ID3D12Device4>                pID3DDevice;
private:

    bool createWindow(HINSTANCE hInstance, WNDPROC CALLBACK    WndProc, int       nCmdShow);
    void openDebug();
    void createFactoryAndDevice();

};
template <typename T>
void GeometryItem::createDynamicGeo(ID3D12Device4* device, EST::vector<T>* vertices, EST::vector<std::uint16_t>* indices, std::unordered_map<std::string, std::unique_ptr< uploadBuddySystem>>* upBSTable) {
    //this->vertices = vertices;
//this->indices = indices;
    indexNum = indices->size();
    vbsize = (UINT)sizeof(T) * (UINT)vertices->size();
    ibsize = (UINT)sizeof(std::uint16_t) * (UINT)indices->size();
    //顶点缓冲及索引缓冲初始状态为common效率最佳
    for (std::unordered_map<std::string, std::unique_ptr< uploadBuddySystem>>::iterator it = upBSTable->begin();it != upBSTable->end();it++) {
        it->second->createPlacedBufferResourceInBS(&VertexBufferUpload, device, vbsize);
        if (VertexBufferUpload.Get() != nullptr)
            break;
    }
    for (std::unordered_map<std::string, std::unique_ptr< uploadBuddySystem>>::iterator it = upBSTable->begin();it != upBSTable->end();it++) {
        it->second->createPlacedBufferResourceInBS(&IndexBufferUpload, device, vbsize);
        if (IndexBufferUpload.Get() != nullptr)
            break;
    }
    D3D12_RANGE range = { 0,0 };
    VertexBufferUpload->Map(0, &range, reinterpret_cast<void**>(&vmapped));
    IndexBufferUpload->Map(0, &range, reinterpret_cast<void**>(&imapped));

    vertices->memcpyPages(vmapped, vbsize);
    indices->memcpyPages(imapped, ibsize);
    // memcpy(vmapped, vertices->Getdata(), vbsize);
    // memcpy(imapped, indices->Getdata(), ibsize);
    VertexBufferUpload->Unmap(0, nullptr);
    IndexBufferUpload->Unmap(0, nullptr);
    vbv.BufferLocation = VertexBufferUpload->GetGPUVirtualAddress();
    vbv.SizeInBytes = vbsize;
    vbv.StrideInBytes = sizeof(T);
    ibv.Format = DXGI_FORMAT_R16_UINT;
    ibv.SizeInBytes = ibsize;
    ibv.BufferLocation = IndexBufferUpload->GetGPUVirtualAddress();
}
template <typename T>
void GeometryItem::updateVB(EST::vector<T>* vec) {
    vec->memcpyPages(vmapped, vbsize);
    //memcpy(vmapped, vp, vbsize);
}
template <typename T>
void GeometryItem::createStaticGeo(ID3D12Device4* device, ID3D12GraphicsCommandList* cmdlist, EST::vector<T>* vertices, EST::vector<std::uint16_t>* indices, std::unordered_map<std::string, std::unique_ptr< uploadBuddySystem>>* upBSTable, std::unordered_map<std::string, std::unique_ptr< defaultBuddySystem>>* defBSTable) {
    createDynamicGeo<T>(device, vertices, indices, upBSTable);
    for (std::unordered_map<std::string, std::unique_ptr< defaultBuddySystem>>::iterator it = defBSTable->begin();it != defBSTable->end();it++) {
        it->second->createPlacedBufferResourceInBS(&VertexBufferDefault, device, vbsize);
        if (VertexBufferDefault.Get() != nullptr)
            break;
    }
    for (std::unordered_map<std::string, std::unique_ptr< defaultBuddySystem>>::iterator it = defBSTable->begin();it != defBSTable->end();it++) {
        it->second->createPlacedBufferResourceInBS(&IndexBufferDefault, device, vbsize);
        if (IndexBufferDefault.Get() != nullptr)
            break;
    }

    cmdlist->CopyBufferRegion(VertexBufferDefault.Get(), 0, VertexBufferUpload.Get(), 0, vbsize);
    cmdlist->CopyBufferRegion(IndexBufferDefault.Get(), 0, IndexBufferUpload.Get(), 0, ibsize);
    /*     D3D12_SUBRESOURCE_DATA subResourceData;
         subResourceData.pData = vertices->Getdata();
         subResourceData.RowPitch = vbsize;
         subResourceData.SlicePitch = subResourceData.RowPitch;
         UpdateSubresources<1>(cmdlist, VertexBufferDefault.Get(), VertexBufferUpload.Get(), 0, 0, 1, &subResourceData);
         subResourceData.pData = indices->Getdata();
         subResourceData.RowPitch = ibsize;
         subResourceData.SlicePitch = subResourceData.RowPitch;
         UpdateSubresources<1>(cmdlist, IndexBufferDefault.Get(), IndexBufferUpload.Get(), 0, 0, 1, &subResourceData);*/



    vbv.BufferLocation = VertexBufferDefault->GetGPUVirtualAddress();
    vbv.SizeInBytes = vbsize;
    vbv.StrideInBytes = sizeof(T);
    ibv.Format = DXGI_FORMAT_R16_UINT;
    ibv.SizeInBytes = ibsize;
    ibv.BufferLocation = IndexBufferDefault->GetGPUVirtualAddress();
}