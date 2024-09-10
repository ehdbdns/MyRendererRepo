#pragma once
#include"rayTracerUtil.h"
#include"buddySystem.h"
#include"ResourceItem.h"
#include<random>
GeometryItem::GeometryItem() = default;
GeometryItem::~GeometryItem() {
    freeVertexAndIndex();
}

D3D12_VERTEX_BUFFER_VIEW* GeometryItem::getVBV() {
    return &vbv;
}
D3D12_INDEX_BUFFER_VIEW* GeometryItem::getIBV() {
    return &ibv;
}
void GeometryItem::freeVertexAndIndex() {
    //vertices->~vector();
    //indices->~vector();
}

RenderItem::RenderItem() = default;
RenderItem::RenderItem(GeometryItem* geo, int instancenum, int basevertex, int startindex, int indexnum, int startinstance, D3D12_PRIMITIVE_TOPOLOGY primitive, objectconstant* objc, ID3D12Device4* device, ID3D12GraphicsCommandList* cmdlist, ID3D12DescriptorHeap* CBVHeap, uploadBuddySystem* upBS, defaultBuddySystem* defBS) {
    init(geo, instancenum, basevertex, startindex, indexnum, startinstance, primitive, objc, device, cmdlist, CBVHeap, upBS, defBS);
}
void RenderItem::init(GeometryItem* geo, int instancenum, int basevertex, int startindex, int indexnum, int startinstance, D3D12_PRIMITIVE_TOPOLOGY primitive, objectconstant* objc, ID3D12Device4* device, ID3D12GraphicsCommandList* cmdlist, ID3D12DescriptorHeap* CBVHeap, uploadBuddySystem* upBS, defaultBuddySystem* defBS) {
    Geo = geo;
    InstanceNum = instancenum;
    baseVertex = basevertex;
    startIndex = startindex;
    indexNum = indexnum;
    startInstance = startinstance;
    Primitive = primitive;
    auto ptr = std::make_unique< ConstantBufferResourceItem<objectconstant>>(device, cmdlist, objc, true, CBVHeap, upBS, defBS);//让渲染项管理一个资源项
    objconstantRI = std::move(ptr);
}
RootSignatureItem::RootSignatureItem() = default;
RootSignatureItem::RootSignatureItem(ID3D12RootSignature* rs, int descriptorNum, int srvnum, int cbvnum, int uavnum, int samplernum, CD3DX12_DESCRIPTOR_RANGE* dt) {
    init(rs, descriptorNum, dt);
}
void RootSignatureItem::init(ID3D12RootSignature* rs, int descriptorNum, CD3DX12_DESCRIPTOR_RANGE* dt) {
    this->rs = rs;
    for (int i = 0;i < descriptorNum;i++) {
        D3D12_DESCRIPTOR_RANGE_TYPE type = dt[i].RangeType;
        if (type == D3D12_DESCRIPTOR_RANGE_TYPE_SRV) {
            SRVDescriptorTableIndex.push_back(i);
            SRVNum++;
        }
        if (type == D3D12_DESCRIPTOR_RANGE_TYPE_CBV) {
            CBVDescriptorTableIndex.push_back(i);
            CBVNum++;
        }
        if (type == D3D12_DESCRIPTOR_RANGE_TYPE_UAV) {
            UAVDescriptorTableIndex.push_back(i);
            UAVNum++;
        }
        if (type == D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER) {
            SamplerDescriptorTableIndex.push_back(i);
            SamplerNum++;
        }
    }
}
int RootSignatureItem::getSRVTableIndex(int srvIndex) {
    return SRVDescriptorTableIndex[srvIndex];
}
int RootSignatureItem::getCBVTableIndex(int cbvIndex) {
    return CBVDescriptorTableIndex[cbvIndex];
}
int RootSignatureItem::getUAVTableIndex(int uavIndex) {
    return UAVDescriptorTableIndex[uavIndex];
}
int RootSignatureItem::getSamplerTableIndex(int samplerIndex) {
    return SamplerDescriptorTableIndex[samplerIndex];
}
PSOItem::PSOItem() = default;
PSOItem::PSOItem(D3D12_GRAPHICS_PIPELINE_STATE_DESC* PSOdesc, std::wstring ShaderFileName, ID3D12Device4* device, D3D_SHADER_MACRO* defines, std::string PSName) {
    vsshader = d3dUtil::CompileShader(ShaderFileName, defines, "VS", "vs_5_1");
    psshader = d3dUtil::CompileShader(ShaderFileName, defines, PSName, "ps_5_1");
    PSOdesc->VS = { reinterpret_cast<BYTE*>(vsshader->GetBufferPointer()),vsshader->GetBufferSize() };
    PSOdesc->PS = { reinterpret_cast<BYTE*>(psshader->GetBufferPointer()),psshader->GetBufferSize() };
    ThrowIfFailed(device->CreateGraphicsPipelineState(PSOdesc, IID_PPV_ARGS(&PSO)));
}
computePSOItem::computePSOItem() = default;
computePSOItem::computePSOItem(D3D12_COMPUTE_PIPELINE_STATE_DESC* computePSOdesc, std::wstring ShaderFileName, ID3D12Device4* device, const D3D_SHADER_MACRO* defines, std::string CSName) {
    csshader = d3dUtil::CompileShader(ShaderFileName, defines, CSName, "cs_5_1");
    computePSOdesc->CS = { reinterpret_cast<BYTE*>(csshader->GetBufferPointer()),csshader->GetBufferSize() };
    ThrowIfFailed(device->CreateComputePipelineState(computePSOdesc, IID_PPV_ARGS(&PSO)));
}
APP::APP() = default;
bool APP::initDX12(HINSTANCE hInstance, WNDPROC CALLBACK    WndProc, int       nCmdShow) {
    createWindow(hInstance, WndProc, nCmdShow);
    openDebug();
    createFactoryAndDevice();
    return true;
}
bool APP::createWindow(HINSTANCE hInstance, WNDPROC CALLBACK    WndProc, int       nCmdShow) {
    AllocConsole();
    g_hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    {
        WNDCLASSEX wcex = {};
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_GLOBALCLASS;
        wcex.lpfnWndProc = WndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = hInstance;
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);		//防止无聊的背景重绘
        wcex.lpszClassName = GRS_WND_CLASS_NAME;
        RegisterClassEx(&wcex);

        DWORD dwWndStyle = WS_OVERLAPPED | WS_SYSMENU;
        RECT rtWnd = { 0, 0, iWidth, iHeight };
        AdjustWindowRect(&rtWnd, dwWndStyle, FALSE);

        // 计算窗口居中的屏幕坐标
        INT posX = (GetSystemMetrics(SM_CXSCREEN) - rtWnd.right - rtWnd.left) / 2;
        INT posY = (GetSystemMetrics(SM_CYSCREEN) - rtWnd.bottom - rtWnd.top) / 2;

        hWnd = CreateWindowW(GRS_WND_CLASS_NAME
            , GRS_WND_TITLE
            , dwWndStyle
            , posX
            , posY
            , rtWnd.right - rtWnd.left
            , rtWnd.bottom - rtWnd.top
            , nullptr
            , nullptr
            , hInstance
            , nullptr);

        if (!hWnd)
        {
            return FALSE;
        }

        ShowWindow(hWnd, nCmdShow);
        UpdateWindow(hWnd);
    }


}
void APP::openDebug() {
    // 打开显示子系统的调试支持
#if defined(_DEBUG)
    ComPtr<ID3D12Debug> debugController;
    ComPtr<ID3D12Debug1>debugController1;
    ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
    debugController->EnableDebugLayer();
    debugController.As(&debugController1);
    debugController1->SetEnableGPUBasedValidation(true);
    // 打开附加的调试支持
    nDXGIFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;

#endif 
}
void APP::createFactoryAndDevice() {
    //创建factory
    {
        CreateDXGIFactory2(nDXGIFactoryFlags, IID_PPV_ARGS(&pIDXGIFactory5));
        ThrowIfFailed(pIDXGIFactory5->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));
    }
    //创建device
    {
        DXGI_ADAPTER_DESC1 desc = {};
        for (UINT index = 0;DXGI_ERROR_NOT_FOUND != pIDXGIFactory5->EnumAdapters1(index, &pIAdapter);index++) {

            pIAdapter->GetDesc1(&desc);
            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
                continue;
            }
            if (SUCCEEDED(D3D12CreateDevice(pIAdapter.Get(), D3D_FEATURE_LEVEL_12_1, _uuidof(ID3D12Device), nullptr)))
            {

                break;
            }
        }
        ThrowIfFailed(D3D12CreateDevice(pIAdapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&pID3DDevice)));
        TCHAR pszWndTitle[MAX_PATH] = {};
        ThrowIfFailed(pIAdapter->GetDesc1(&desc));
        ::GetWindowText(hWnd, pszWndTitle, MAX_PATH);
        ::SetWindowText(hWnd, pszWndTitle);
    }
}

void drawRenderItem(RenderItem* ri, ID3D12GraphicsCommandList* cmdlist, int objcPara) {
    CD3DX12_GPU_DESCRIPTOR_HANDLE objcHandle = ri->objconstantRI->getCBVGPU();
    cmdlist->SetGraphicsRootDescriptorTable(objcPara, objcHandle);
    cmdlist->IASetPrimitiveTopology(ri->Primitive);
    cmdlist->IASetVertexBuffers(0, 1, ri->Geo->getVBV());
    cmdlist->IASetIndexBuffer(ri->Geo->getIBV());
    cmdlist->DrawIndexedInstanced(ri->indexNum, ri->InstanceNum, ri->startIndex, ri->baseVertex, ri->startInstance);
}

void drawRenderItems(std::unordered_map<std::string, std::unique_ptr< RenderItem>>* RIs, ID3D12GraphicsCommandList* cmdlist, int objcPara, RenderItem* out) {
    for (std::unordered_map<std::string, std::unique_ptr< RenderItem>>::iterator it = RIs->begin();it != RIs->end();it++) {
        if (it->second.get() == out)
            continue;
        drawRenderItem(it->second.get(), cmdlist, objcPara);
    }
}
void BuildBoxAndTriangleSBRI(SAHtree* tree, std::unique_ptr<StructureBufferResourceItem<AABBbox>>& boxSBRI, std::unique_ptr<StructureBufferResourceItem<triangle>>& triangleSBRI, ID3D12Device4* device, ID3D12GraphicsCommandList* cmdlist, ID3D12DescriptorHeap* srvheap, uploadBuddySystem* upBS, defaultBuddySystem* defBS) {
    boxSBRI = std::make_unique<StructureBufferResourceItem<AABBbox>>(device, cmdlist, &tree->SortedBoxes, true, srvheap, upBS, defBS, tree->SortedBoxes.size());
    triangleSBRI = std::make_unique<StructureBufferResourceItem<triangle>>(device, cmdlist, &tree->Triangles, true, srvheap, upBS, defBS, tree->Triangles.size());
}
void GenerateRandomNum(EST::vector<float>& randnums, int num) {
    srand((unsigned)time(NULL));
    std::mt19937 gen((unsigned int)time(nullptr)); // 定义随机数生成器对象gen，使用time(nullptr)作为随机数生成器的种子
    std::uniform_real_distribution<double> dis(0.0, 1.0); // 定义随机数分布器对象dis，为在[-1.0,1.0]区间内的均匀分布
 
    for (int i = 0;i < num;i++)
        randnums.push_back(dis(gen));
}
void GenerateOffsets(EST::vector<XMINT2>& offsets) {
    for (int i = 0;i < 5;i++) {
        for (int j = -2;j < 3;j++) {
            for (int k = -2;k < 3;k++) {
                XMINT2 int2 = XMINT2{ j * (int)pow(2,i),k * (int)pow(2,i) };
                offsets.push_back(int2);
            }
        }
    }
}
void ResourceBarrierTrans(ID3D12Resource* r, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after, ID3D12GraphicsCommandList* cmdlist) {
    cmdlist->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition
    (r, before, after));
}
void AddTrianglesToScene(EST::vector<triangle>& SceneTris, EST::vector<Vertex>& vertices, EST::vector<std::uint16_t>& indices, int texIndex, XMMATRIX world) {
    for (int i = 0;i < indices.size() / 3;i++) {
        triangle tri;
        tri.texIndex = texIndex;
        Vertex v1 = vertices[indices[i * 3]];
        Vertex v2 = vertices[indices[i * 3 + 1]];
        Vertex v3 = vertices[indices[i * 3 + 2]];
        XMStoreFloat3(&tri.pos1, (XMVector3TransformCoord(XMLoadFloat4(&XMFLOAT4{ v1.position.x,v1.position.y,v1.position.z,1.0f }), world)));
        XMStoreFloat3(&tri.pos2, (XMVector3TransformCoord(XMLoadFloat4(&XMFLOAT4{ v2.position.x,v2.position.y,v2.position.z,1.0f }), world)));
        XMStoreFloat3(&tri.pos3, (XMVector3TransformCoord(XMLoadFloat4(&XMFLOAT4{ v3.position.x,v3.position.y,v3.position.z,1.0f }), world)));
        XMVECTOR trin = XMVector3Normalize(XMVector3Cross(XMLoadFloat4(&v1.position) - XMLoadFloat4(&v2.position), XMLoadFloat4(&v3.position) - XMLoadFloat4(&v2.position)));
        trin = (XMVectorGetX(XMVector3Dot(XMLoadFloat3(&v1.normal), trin)) > 0) ? trin : -trin;
        XMStoreFloat3(&tri.n, XMVector4Normalize(XMVector3TransformNormal(trin, world)));
        tri.uv12 = XMFLOAT4{ v1.uv.x,v1.uv.y, v2.uv.x,v2.uv.y };
        tri.uv3 = XMFLOAT2{ v3.uv.x,v3.uv.y };
        tri.color = v2.color;
        SceneTris.push_back(tri);
    }
}
//void updateLight(GeometryItem* LightGeo, PolygonalLight& l) {
//    Vertex v[] = {
//{XMFLOAT4{l.Xstart ,199.5,l.Zstart,1.0f},XMFLOAT2{0,0},XMFLOAT3{0,-1.0f,0},XMFLOAT3{1.0f,0,0},0,XMFLOAT3{1.0,1.0,1.0}},
//{XMFLOAT4{l.Xend,199.5,l.Zstart,1.0f},XMFLOAT2{1.0f,.0f},XMFLOAT3{0,-1.0f,0},XMFLOAT3{1.0f,0,0},0,XMFLOAT3{1.0,1.0,1.0}},
//{XMFLOAT4{l.Xstart,199.5,l.Zend,1.0f},XMFLOAT2{0,1.0f},XMFLOAT3{0,-1.0f,0},XMFLOAT3{1.0f,0,0},0,XMFLOAT3{1.0,1.0,1.0}},
//{XMFLOAT4{l.Xend,199.5,l.Zend,1.0f},XMFLOAT2{1.0f,1.0f},XMFLOAT3{0,-1.0f,0},XMFLOAT3{1.0f,0,0},0,XMFLOAT3{1.0,1.0,1.0}}
//    };
//    EST::vector< Vertex> Lightv(v, 4);
//
//    LightGeo->updateVB(&Lightv);
//}
void CreateBox(float width, float height, float depth, EST::vector<Vertex>& vertices, EST::vector<std::uint16_t>& indices, XMFLOAT3 color)
{
    float w2 = 0.5f * width;
    float h2 = 0.5f * height;
    float d2 = 0.5f * depth;
    Vertex v[24] = {
        // Fill in the front face vertex data.
        {XMFLOAT4{ -w2, -h2, -d2,1.0f},XMFLOAT2{ 0.0f, 1.0f},XMFLOAT3{ 0.0f, 0.0f, -1.0f},XMFLOAT3{ 1.0f, 0.0f, 0.0f},0,color},
        {XMFLOAT4{ -w2, +h2, -d2,1.0f},XMFLOAT2{ 0.0f, 0.0f},XMFLOAT3{  0.0f, 0.0f, -1.0f},XMFLOAT3{ 1.0f, 0.0f, 0.0f},0,color},
        {XMFLOAT4{ +w2, +h2, -d2,1.0f},XMFLOAT2{ 1.0f, 0.0f},XMFLOAT3{  0.0f, 0.0f, -1.0f},XMFLOAT3{1.0f, 0.0f, 0.0f},0,color},
        {XMFLOAT4{ +w2, -h2, -d2,1.0f},XMFLOAT2{ 1.0f, 1.0f},XMFLOAT3{  0.0f, 0.0f, -1.0f},XMFLOAT3{ 1.0f, 0.0f, 0.0f},0,color},
        // Fill in the back face vertex data.
        {XMFLOAT4{ -w2, -h2, +d2,1.0f},XMFLOAT2{ 1.0f, 1.0f},XMFLOAT3{ 0.0f, 0.0f, 1.0f},XMFLOAT3{ -1.0f, 0.0f, 0.0f},0,color},
        {XMFLOAT4{ +w2, -h2, +d2,1.0f},XMFLOAT2{ 0.0f, 1.0f},XMFLOAT3{  0.0f, 0.0f, 1.0f},XMFLOAT3{ -1.0f, 0.0f, 0.0f},0,color},
        {XMFLOAT4{ +w2, +h2, +d2,1.0f},XMFLOAT2{ 0.0f, 0.0f},XMFLOAT3{  0.0f, 0.0f, 1.0f},XMFLOAT3{-1.0f, 0.0f, 0.0f},0,color},
        {XMFLOAT4{ -w2, +h2, +d2,1.0f},XMFLOAT2{ 1.0f, 0.0f},XMFLOAT3{  0.0f, 0.0f, 1.0f},XMFLOAT3{ -1.0f, 0.0f, 0.0f},0,color},
        // Fill in the top face vertex data.
        {XMFLOAT4{ -w2, +h2, -d2,1.0f},XMFLOAT2{ 0.0f, 1.0f},XMFLOAT3{ 0.0f, 1.0f, 0.0f},XMFLOAT3{ 1.0f, 0.0f, 0.0f},0,color},
        {XMFLOAT4{ -w2, +h2, +d2,1.0f},XMFLOAT2{ 0.0f, 0.0f},XMFLOAT3{  0.0f, 1.0f, 0.0f},XMFLOAT3{ 1.0f, 0.0f, 0.0f},0,color},
        {XMFLOAT4{ +w2, +h2, +d2,1.0f},XMFLOAT2{ 1.0f, 0.0f},XMFLOAT3{  0.0f, 1.0f, 0.0f},XMFLOAT3{1.0f, 0.0f, 0.0f},0,color},
        {XMFLOAT4{ +w2, +h2, -d2,1.0f},XMFLOAT2{ 1.0f, 1.0f},XMFLOAT3{  0.0f, 1.0f, 0.0f},XMFLOAT3{ 1.0f, 0.0f, 0.0f},0,color},
        // Fill in the bottom face vertex data.
        {XMFLOAT4{ -w2, -h2, -d2,1.0f},XMFLOAT2{ 1.0f, 1.0f},XMFLOAT3{ 0.0f, -1.0f, 0.0f},XMFLOAT3{ 1.0f, 0.0f, 0.0f},0,color},
        {XMFLOAT4{ +w2, -h2, -d2,1.0f},XMFLOAT2{ 0.0f, 1.0f},XMFLOAT3{  0.0f, -1.0f, 0.0f},XMFLOAT3{ 1.0f, 0.0f, 0.0f},0,color},
        {XMFLOAT4{ +w2, -h2, +d2,1.0f},XMFLOAT2{ 0.0f, 0.0f},XMFLOAT3{  0.0f, -1.0f, 0.0f},XMFLOAT3{1.0f, 0.0f, 0.0f},0,color},
        {XMFLOAT4{ -w2, -h2, +d2,1.0f},XMFLOAT2{ 1.0f, 0.0f},XMFLOAT3{  0.0f, -1.0f, 0.0f},XMFLOAT3{ 1.0f, 0.0f, 0.0f},0,color},
        // Fill in the left face vertex data.
        {XMFLOAT4{ -w2, -h2, +d2,1.0f},XMFLOAT2{ 0.0f, 1.0f},XMFLOAT3{ -1.0f, 0.0f, 0.0f},XMFLOAT3{ 0.0f, 0.0f,-1.0f},0,color},
        {XMFLOAT4{ -w2, +h2, +d2,1.0f},XMFLOAT2{ 0.0f, 0.0f},XMFLOAT3{  -1.0f, 0.0f, 0.0f},XMFLOAT3{ 0.0f, 0.0f, -1.0f},0,color},
        {XMFLOAT4{ -w2, +h2, -d2,1.0f},XMFLOAT2{ 1.0f, 0.0f},XMFLOAT3{  -1.0f, 0.0f, 0.0f},XMFLOAT3{0.0f, 0.0f, -1.0f},0,color},
        {XMFLOAT4{ -w2, -h2, -d2,1.0f},XMFLOAT2{ 1.0f, 1.0f},XMFLOAT3{  -1.0f, 0.0f, 0.0f},XMFLOAT3{ 0.0f, 0.0f, -1.0f},0,color},

        {XMFLOAT4{ +w2, -h2, -d2,1.0f},XMFLOAT2{ 0.0f, 1.0f},XMFLOAT3{ 1.0f, 0.0f, 0.0f},XMFLOAT3{ 0.0f, 0.0f,1.0f},0,color},
        {XMFLOAT4{ +w2, +h2, -d2,1.0f},XMFLOAT2{ 0.0f, 0.0f},XMFLOAT3{  1.0f, 0.0f, 0.0f},XMFLOAT3{ 0.0f, 0.0f, 1.0f},0,color},
        {XMFLOAT4{ +w2, +h2, +d2,1.0f},XMFLOAT2{ 1.0f, 0.0f},XMFLOAT3{  1.0f, 0.0f, 0.0f},XMFLOAT3{0.0f, 0.0f, 1.0f},0,color},
        {XMFLOAT4{ +w2, -h2, +d2,1.0f},XMFLOAT2{ 1.0f, 1.0f},XMFLOAT3{  1.0f, 0.0f, 0.0f},XMFLOAT3{ 0.0f, 0.0f, 1.0f},0,color}
        // Fill in the right face vertex data.
    };
    vertices.assign(v, 24);
    std::uint16_t i[36];
    // Fill in the front face index data
    i[0] = 0; i[1] = 1; i[2] = 2;
    i[3] = 0; i[4] = 2; i[5] = 3;
    // Fill in the back face index data
    i[6] = 4; i[7] = 5; i[8] = 6;
    i[9] = 4; i[10] = 6; i[11] = 7;
    // Fill in the top face index data
    i[12] = 8; i[13] = 9; i[14] = 10;
    i[15] = 8; i[16] = 10; i[17] = 11;
    // Fill in the bottom face index data
    i[18] = 12; i[19] = 13; i[20] = 14;
    i[21] = 12; i[22] = 14; i[23] = 15;
    // Fill in the left face index data
    i[24] = 16; i[25] = 17; i[26] = 18;
    i[27] = 16; i[28] = 18; i[29] = 19;
    // Fill in the right face index data
    i[30] = 20; i[31] = 21; i[32] = 22;
    i[33] = 20; i[34] = 22; i[35] = 23;
    indices.assign(i, 36);
}
meshdata subvide(meshdata& inmesh, int vsize, int isize) {
    meshdata mesh;
    for (int meshindex = 1;meshindex <= (isize / 3);meshindex++) {
        int a = inmesh.indices[(meshindex - 1) * 3];//每个面源的点索引
        int b = inmesh.indices[(meshindex - 1) * 3 + 1];
        int c = inmesh.indices[(meshindex - 1) * 3 + 2];
        XMFLOAT3 v1 = XMFLOAT3{ (inmesh.vertices[a].x + inmesh.vertices[b].x) / 2,(inmesh.vertices[a].y + inmesh.vertices[b].y) / 2,(inmesh.vertices[a].z + inmesh.vertices[b].z) / 2 };
        XMFLOAT3 v2 = XMFLOAT3{ (inmesh.vertices[b].x + inmesh.vertices[c].x) / 2,(inmesh.vertices[b].y + inmesh.vertices[c].y) / 2,(inmesh.vertices[b].z + inmesh.vertices[c].z) / 2 };
        XMFLOAT3 v3 = XMFLOAT3{ (inmesh.vertices[a].x + inmesh.vertices[c].x) / 2,(inmesh.vertices[a].y + inmesh.vertices[c].y) / 2,(inmesh.vertices[a].z + inmesh.vertices[c].z) / 2 };
        mesh.vertices.push_back(inmesh.vertices[a]);//0
        mesh.vertices.push_back(inmesh.vertices[b]);//1
        mesh.vertices.push_back(inmesh.vertices[c]);//2
        mesh.vertices.push_back(v1);//3
        mesh.vertices.push_back(v2);//4
        mesh.vertices.push_back(v3);//5
        int k = (meshindex - 1) * 6;
        mesh.indices.push_back(3 + k);
        mesh.indices.push_back(1 + k);
        mesh.indices.push_back(4 + k);
        mesh.indices.push_back(0 + k);
        mesh.indices.push_back(3 + k);
        mesh.indices.push_back(5 + k);
        mesh.indices.push_back(3 + k);
        mesh.indices.push_back(4 + k);
        mesh.indices.push_back(5 + k);
        mesh.indices.push_back(5 + k);
        mesh.indices.push_back(4 + k);
        mesh.indices.push_back(2 + k);
    }
    return mesh;
}
void  createSphere(EST::vector<Vertex>& v, EST::vector<std::uint16_t>& ind, float r, XMFLOAT3 color) {
    const float X = 0.5257f;
    const float Z = 0.8506f;
    XMFLOAT3 sphereVertices[] = {
      XMFLOAT3(-X, 0.0f, Z),  XMFLOAT3(X, 0.0f, Z),
    XMFLOAT3(-X, 0.0f, -Z), XMFLOAT3(X, 0.0f, -Z),
    XMFLOAT3(0.0f, Z, X),   XMFLOAT3(0.0f, Z, -X),
    XMFLOAT3(0.0f, -Z, X),  XMFLOAT3(0.0f, -Z, -X),
    XMFLOAT3(Z, X, 0.0f),   XMFLOAT3(-Z, X, 0.0f),
    XMFLOAT3(Z, -X, 0.0f),  XMFLOAT3(-Z, -X, 0.0f)
    };
    std::uint16_t sphereIndices[] = {
    1,4,0,  4,9,0,  4,5,9,  8,5,4,  1,8,4,
    1,10,8, 10,3,8, 8,3,5,  3,2,5,  3,7,2,
    3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
    10,1,6, 11,0,9, 2,11,9, 5,2,9,  11,2,7
    };
    meshdata mesh12;
    mesh12.indices.addData(sphereIndices, 60);
    mesh12.vertices.addData(sphereVertices, 12);

    meshdata m1 = subvide(mesh12, mesh12.vertices.size(), mesh12.indices.size());
    meshdata m2 = subvide(m1, m1.vertices.size(), m1.indices.size());
    meshdata m3 = subvide(m2, m2.vertices.size(), m2.indices.size());

    for (int i = 0;i < m3.vertices.size();i++) {
        XMVECTOR vec = { m3.vertices[i].x, m3.vertices[i].y, m3.vertices[i].z };
        vec = XMVector3Normalize(vec);
        float theta = std::atanf(XMVectorGetZ(vec) / XMVectorGetX(vec));
        if (theta < 0)
            theta += XM_2PI;
        float thetak = theta / XM_2PI;
        float phi = std::acosf(XMVectorGetY(vec) / 1.0f);
        float phik = phi / XM_PI;
        Vertex ver;
        ver.position = XMFLOAT4{ XMVectorGetX(vec) * r ,XMVectorGetY(vec) * r,XMVectorGetZ(vec) * r,1.0f };
        ver.uv = { thetak ,phik };
        ver.TangentU = XMFLOAT3{ r * sin(phi) * sin(theta),0,r * sin(phi) * cos(theta) };
        ver.color = color;
        XMStoreFloat3(&ver.normal, vec);
        v.push_back(ver);
    }
    for (int i = 0;i < m3.indices.size();i++) {
        ind.push_back(m3.indices[i]);
    }
}
EST::vector<float>CalcGaussWeights(float variance, float scale)
{
    int MaxBlurRadius = 100;
    float sigma = std::sqrt(variance);
    float twoSigma2 = 2.0f * sigma * sigma;
    int blurRadius = (int)ceil(2.0f * sigma * scale);
    blurRadius = min(90, blurRadius);
    // assert(blurRadius <= MaxBlurRadius);

    EST::vector<float> weights;
    weights.resize(2 * blurRadius + 2);
    weights[0] = float(2 * blurRadius + 1);
    float weightSum = 0.0f;

    for (int i = -blurRadius; i <= blurRadius; ++i)
    {
        float x = (float(i) / scale);

        weights[i + blurRadius + 1] = expf(-x * x / twoSigma2);

        weightSum += weights[i + blurRadius + 1];
    }

    // Divide by the sum so all the weights add up to 1.0.
    for (int i = 0; i < weights.size() - 1; ++i)
    {
        weights[i + 1] /= weightSum;
    }

    return weights;
}
void GenerateLights(EST::vector<Vertex>& Lv, EST::vector<std::uint16_t>& Li,int numLights) {
        //
        float Z=-120;
        float Y=0;
        for (int j = 0;j < 10;j++) {
            for (int k = 0;k < 10;k++) {
                Vertex v1 = { XMFLOAT4{-60+6,Y+k*20,Z+j*18,1.0f},XMFLOAT2{0,0},XMFLOAT3{1.0f,0,0},XMFLOAT3{0,0,-1.0f},0 };
                Vertex v2 = { XMFLOAT4{-60+6,Y+20+k*20,Z+j*18,1.0f},XMFLOAT2{0,1},XMFLOAT3{1.0f,0,0},XMFLOAT3{0,0,-1.0f},0 };
                Vertex v3 = { XMFLOAT4{-60+6,Y+k*20,Z+18+j*18,1.0f},XMFLOAT2{1,0},XMFLOAT3{1.0f,0,0},XMFLOAT3{0,0,-1.0f},0 };
                Vertex v4 = { XMFLOAT4{-60+6,Y+20+k*20,Z+18+j*18,1.0f},XMFLOAT2{1,1},XMFLOAT3{1.0f,0,0},XMFLOAT3{0,0,-1.0f},0 };
                int index = k + 1 + j * 10;
                switch (index % 3) {
                case 0:
                    v1.color = XMFLOAT3{ 1.0,0,0 };
                    v2.color = XMFLOAT3{ 1.0,0,0 };
                    v3.color = XMFLOAT3{ 1.0,0,0 };
                    v4.color = XMFLOAT3{ 1.0,0,0 };
                    break;
                case 1:
                    v1.color = XMFLOAT3{ 0,1.0,0 };
                    v2.color = XMFLOAT3{ 0,1.0,0 };
                    v3.color = XMFLOAT3{ 0,1.0,0 };
                    v4.color = XMFLOAT3{ 0,1.0,0 };
                    break;
                case 2:
                    v1.color = XMFLOAT3{ 0,0,1.0 };
                    v2.color = XMFLOAT3{ 0,0,1.0 };
                    v3.color = XMFLOAT3{ 0,0,1.0 };
                    v4.color = XMFLOAT3{ 0,0,1.0 };
                    break;
                }

                Lv.push_back(v1);
                Lv.push_back(v2);
                Lv.push_back(v3);
                Lv.push_back(v4);
                Li.push_back(0 + j * 10 * 4 + k * 4);
                Li.push_back(1 + j * 10 * 4 + k * 4);
                Li.push_back(2 + j * 10 * 4 + k * 4);
                Li.push_back(2 + j * 10 * 4 + k * 4);
                Li.push_back(1 + j * 10 * 4 + k * 4);
                Li.push_back(3 + j * 10 * 4 + k * 4);
            }
        }
        Z = -120;
        Y = 0;
        for (int j = 0;j < 10;j++) {
            for (int k = 0;k < 10;k++) {
                Vertex v1 = { XMFLOAT4{149.5 + 6,Y + k * 20,Z + j * 18,1.0f},XMFLOAT2{0,0},XMFLOAT3{-1.0f,0,0},XMFLOAT3{0,0,-1.0f},0 };
                Vertex v2 = { XMFLOAT4{149.5 + 6,Y + 20 + k * 20,Z + j * 18,1.0f},XMFLOAT2{0,1},XMFLOAT3{-1.0f,0,0},XMFLOAT3{0,0,-1.0f},0 };
                Vertex v3 = { XMFLOAT4{149.5 + 6,Y + k * 20,Z + 18 + j * 18,1.0f},XMFLOAT2{1,0},XMFLOAT3{-1.0f,0,0},XMFLOAT3{0,0,-1.0f},0 };
                Vertex v4 = { XMFLOAT4{149.5 + 6,Y + 20 + k * 20,Z + 18 + j * 18,1.0f},XMFLOAT2{1,1},XMFLOAT3{-1.0f,0,0},XMFLOAT3{0,0,-1.0f},0 };
                int index = k + 1 + j * 10;
                switch (index % 3) {
                case 0:
                    v1.color = XMFLOAT3{ 1.0,0,0 };
                    v2.color = XMFLOAT3{ 1.0,0,0 };
                    v3.color = XMFLOAT3{ 1.0,0,0 };
                    v4.color = XMFLOAT3{ 1.0,0,0 };
                    break;
                case 1:
                    v1.color = XMFLOAT3{ 0,1.0,0 };
                    v2.color = XMFLOAT3{ 0,1.0,0 };
                    v3.color = XMFLOAT3{ 0,1.0,0 };
                    v4.color = XMFLOAT3{ 0,1.0,0 };
                    break;
                case 2:
                    v1.color = XMFLOAT3{ 0,0,1.0 };
                    v2.color = XMFLOAT3{ 0,0,1.0 };
                    v3.color = XMFLOAT3{ 0,0,1.0 };
                    v4.color = XMFLOAT3{ 0,0,1.0 };
                    break;
                }

                Lv.push_back(v1);
                Lv.push_back(v2);
                Lv.push_back(v3);
                Lv.push_back(v4);
                Li.push_back(400 + j * 10 * 4 + k * 4);
                Li.push_back(401 + j * 10 * 4 + k * 4);
                Li.push_back(402 + j * 10 * 4 + k * 4);
                Li.push_back(402 + j * 10 * 4 + k * 4);
                Li.push_back(401 + j * 10 * 4 + k * 4);
                Li.push_back(403 + j * 10 * 4 + k * 4);
            }
        }
        float lightStrengh = 2.5f;
        Vertex v1 = { XMFLOAT4{30 ,200,-45,1.0f},XMFLOAT2{0,0},XMFLOAT3{0,-1,0},XMFLOAT3{0,0,-1.0f},0,XMFLOAT3{lightStrengh,lightStrengh,lightStrengh} };
        Vertex v2 = { XMFLOAT4{60,200,-45,1.0f},XMFLOAT2{0,1},XMFLOAT3{0,-1.0f,0},XMFLOAT3{0,0,-1.0f},0,XMFLOAT3{lightStrengh,lightStrengh,lightStrengh} };
        Vertex v3 = { XMFLOAT4{30 ,200,-5,1.0f},XMFLOAT2{1,0},XMFLOAT3{0,-1.0f,0},XMFLOAT3{0,0,-1.0f},0,XMFLOAT3{lightStrengh,lightStrengh,lightStrengh} };
        Vertex v4 = { XMFLOAT4{60,200,-5,1.0f},XMFLOAT2{1,1},XMFLOAT3{0,-1.0f,0},XMFLOAT3{0,0,-1.0f},0 ,XMFLOAT3{lightStrengh,lightStrengh,lightStrengh} };
        Lv.push_back(v1);
        Lv.push_back(v2);
        Lv.push_back(v3);
        Lv.push_back(v4);
        Li.push_back(800);
        Li.push_back(801);
        Li.push_back(802);
        Li.push_back(802);
        Li.push_back(801);
        Li.push_back(803);
}
void createLightsRI(EST::vector<PolygonalLight>&lights, EST::vector<Vertex>*LightV) {
    for (int i = 0;i < LightV->size()/4;i++) {
        PolygonalLight l;
        l.v1 = convertF4toF3( (*LightV)[i * 4].position);
        l.v2 =convertF4toF3( (*LightV)[i * 4+1].position);
        l.v3 = convertF4toF3((*LightV)[i * 4+2].position);
        l.v4 = convertF4toF3((*LightV)[i * 4+3].position);
        l.normal = (*LightV)[i * 4].normal;
        l.color = (*LightV)[i * 4].color;
        l.area = length(F3miF3( l.v1 , l.v2)) * length(F3miF3( l.v1 , l.v3));
        lights.push_back(l);
    }
}