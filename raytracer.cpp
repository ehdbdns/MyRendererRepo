// d3d12practicelast.cpp : 定义应用程序的入口点。
//
#pragma once
#include"rayTracerUtil.h"
#include"modelReader.h"
#include"OBJreader.h"
#include"TSSSSComponent.h"
#include"SSSSComponent.h"
#define MAX_LOADSTRING 100
#define GRS_WND_CLASS_NAME _T("GRS Game Window Class")
#define GRS_WND_TITLE	_T("GRS DirectX12 Trigger Sample")
UINT samplerindex = 0;
UINT samplercount = 5;
const UINT nFrameBackBufCount = 3u;


#include <D3DCompiler.h>
#define TK_OBJFILE_IMPLEMENTATION
#include "tk_objfile.h"
//#pragma comment(lib, "d3dcompiler.lib")
//#pragma comment(lib, "D3D12.lib")
//#pragma comment(lib, "dxgi.lib")
UINT __stdcall SVGFThreadfunc(void* ppara);
UINT __stdcall NonSVGFThreadfunc(void* ppara);
void testVector() {
    EST::vector<int>ints;
 //   ints.resize(1);
    for (int i = 0;i < 16090;i++)
        ints.push_back(i);
    ints.reserve(30000);//换页时有问题。
    for (int i = 0;i < 10;i++) {
        WriteConsole(g_hOutput,std::to_string(ints[i]).c_str(), 2, NULL, NULL);
    }
    WriteConsole(g_hOutput,std::to_string(ints.back()).c_str(), 2, NULL, NULL);
    ints.pop_back();
    WriteConsole(g_hOutput, std::to_string(ints.back()).c_str(), 2, NULL, NULL);

}

class CGRSCOMException
{
public:
    CGRSCOMException(HRESULT hr) : m_hrError(hr)
    {
    }
    HRESULT Error() const
    {
        return m_hrError;
    }
private:
    const HRESULT m_hrError;
};
UINT64 starttime = 0;
UINT64 currenttime = 0;
XMVECTOR eyepos = XMVectorSet(20.0f, 100.0f, -200.0f, 1.0f);
XMVECTOR target = XMVectorSet(0.0f, .0f, 1.0f, 0.0f);
XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
XMFLOAT2 mousepospre;
float theta = PI /2;
float phi = PI / 2;
float timesecond;
void OnMouseDown(WPARAM wparam, int x, int y) {
    mousepospre.x = x;
    mousepospre.y = y;
}
void OnMouseMove(WPARAM wparam, int x, int y) {
    if ((wparam & MK_LBUTTON) != 0) {
        WriteConsole(g_hOutput, "1", 2, NULL, NULL);
        float dx = x - mousepospre.x;
        float dy = (y - mousepospre.y);
        float dtheta = XMConvertToRadians(0.25f * static_cast<float>(dx));
        float dphi = XMConvertToRadians(0.25f * static_cast<float>(dy));
        theta -= dtheta;
        phi += dphi;
        if (phi >= XM_PI)
            phi -= XM_2PI;
        if (phi <= -XM_PI)
            phi += XM_2PI;
        if (theta >= XM_PI)
            theta -= XM_2PI;
        if (theta <= -XM_2PI)
            theta += XM_2PI;
        mousepospre.x = x;
        mousepospre.y = y;
    }
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_KEYUP:
    {
        if (VK_SPACE == (wParam & 0xFF)) {
            samplerindex++;
            WriteConsole(g_hOutput, L"1", 2, NULL, NULL);
            if (samplerindex == samplercount)
                samplerindex = 0;
        }

    }break;
    case WM_LBUTTONDOWN:

    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN: {
        OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

    }break;
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:


    case WM_MOUSEMOVE: {
        OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
    }break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
void triangleCallBack(TK_TriangleVert a, TK_TriangleVert b, TK_TriangleVert c, void* Vector)
{
    EST::vector<Vertex>* vertices = (EST::vector<Vertex>*)Vector;
    Vertex v1,v2,v3;
    v1.position = XMFLOAT4{a.pos[0],a.pos[1],a.pos[2],1.0f};
    v2.position = XMFLOAT4{b.pos[0],b.pos[1],b.pos[2],1.0f};
    v3.position = XMFLOAT4{c.pos[0],c.pos[1],c.pos[2],1.0f};
    v1.normal = XMFLOAT3{ a.nrm[0],a.nrm[1],a.nrm[2] };
    v2.normal = XMFLOAT3{ b.nrm[0],b.nrm[1],b.nrm[2] };
    v3.normal = XMFLOAT3{ c.nrm[0],c.nrm[1],c.nrm[2] };
    v1.uv = XMFLOAT2{ a.st[0],a.st[1] };
    v2.uv = XMFLOAT2{ b.st[0],b.st[1] };
    v3.uv = XMFLOAT2{ c.st[0],c.st[1] };
    v1.color = XMFLOAT3{ 0.6,0.6,0.6 };
    v2.color = XMFLOAT3{ 0.6,0.6,0.6 };
    v3.color = XMFLOAT3{ 0.6,0.6,0.6 };
    vertices->push_back(v1);
    vertices->push_back(v2);
    vertices->push_back(v3);
}
void matCallBack(const char* mtlName, size_t numTriangles, void* userData)
{
 
}
void* readEntireFile(const char* filename, size_t* out_filesz)
{
    FILE* fp;
    fopen_s(&fp,filename, "r");
    if (!fp) return NULL;

    // Get file size
    fseek(fp, 0L, SEEK_END);
    size_t filesz = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    void* fileData = malloc(filesz);
    if (fileData)
    {
        size_t result = fread(fileData, filesz, 1, fp);

        // result is # of chunks read, we're asking for 1, fread
        // won't return partial reads, so it's all or nothing.
        if (!result)
        {
            free(fileData);
            fileData = NULL;
        }
        else
        {
            // read suceeded, set out filesize
            *out_filesz = filesz;
        }
    }

    return fileData;

}
struct threadparas {
    UINT								nIndex;				//序号
    DWORD								dwThisThreadID;
    HANDLE								hThisThread;
    DWORD								dwMainThreadID;
    HANDLE								hMainThread;
    HANDLE								hRunEvent;
    HANDLE								hEventRenderOver;
    UINT*								nCurrentFrameIndex;//当前渲染后缓冲序号
    ULONGLONG							nStartTime;		   //当前帧开始时间
    ULONGLONG							nCurrentTime;	   //当前时间
    D3D12_VIEWPORT*                     stViewPort;
    D3D12_RECT*							stScissorRect;
    std::unordered_map<std::string,std::unique_ptr< RenderItem>>(*SVGFRenderItemTable);
    std::unordered_map<std::string,std::unique_ptr< RenderItem>>(*NonSVGFRenderItemTable);
    std::unordered_map<std::string,std::unique_ptr< GeometryItem>>(*GeometryItemTable);
    std::unordered_map<std::string,std::unique_ptr< ConstantBufferResourceItem<passconstant>>>(*BufferResourceItemTable);
    std::unordered_map<std::string,std::unique_ptr< TextureResourceItem>>(*TextureResourceItemTable);
    std::unordered_map<std::string,std::unique_ptr< SamplerResourceItem>>(*SamplerResourceItemTable);
    std::unordered_map<std::string,std::unique_ptr< RootSignatureItem>>(*RootSignatureItemTable);
    std::unordered_map<std::string, std::unique_ptr< PSOItem>>(*PSOITable);
    std::unordered_map<std::string, std::unique_ptr< StructureBufferResourceItem<AABBbox>>>(*boxTable);
    std::unordered_map<std::string, std::unique_ptr< StructureBufferResourceItem<triangle>>>(*triangleTable);
    std::unique_ptr<StructureBufferResourceItem<ParallelLight>>*ParaLightsSBRI;
    std::unique_ptr<StructureBufferResourceItem<float>>**GkernelSBRI;
    std::unique_ptr < StructureBufferResourceItem<float>>*randomNumbers;
    std::unique_ptr < StructureBufferResourceItem<PolygonalLight>>*lights;
    std::unique_ptr<StructureBufferResourceItem<material>>*materials;
    std::unique_ptr<StructureBufferResourceItem<XMFLOAT3>>*sumGweights;
    ID3D12Device4* device;
    ID3D12RootSignature* rs;
    ID3D12GraphicsCommandList* cmdlist;
    ID3D12CommandAllocator* cmdalloc;
    ID3D12Resource*               pIARenderTargets[nFrameBackBufCount];
    ID3D12DescriptorHeap* RTVHeap;
    std::string                         RenderItemName;
    std::string                         RootSignatureItemName;
    std::string                         SamplerResourceItemItemName;
    std::string                         TextureResourceItemName;
    std::string                         BufferResourceItemName;
    std::string                         DirectRayTracerPSOName;
    std::string                         IndirectRayTracerPSOName;
};

class FirstAPP:APP {
public:
    FirstAPP() = default;
    void init(HINSTANCE hInstance, int       nCmdShow);
    void startRender();
    void createVideoMemoryManager();
    void createDescriptorHeap();
    void createCommandQueueAndSwapChain();
    void createRootSignature();
    void createCommandList();
    void compileShadersAndCreatePSO();
    void createDepthBufferAndSampler();
    void createGeometryItemAndBVHData();
    void createRenderItem();
    void createResourceItem();
    void startSubThread();
    void startRenderLoop();
    int createNewThreadToRender(std::string RenderItemName, std::string BufferResourceItemName, std::string TextureResourceItemName, std::string RootSignatureItemName, std::string SamplerResourceItemItemName, std::string DirectRayTracerPSOName, std::string IndirectRayTracerPSOName,_beginthreadex_proc_type threadName, int order);
    void startAndWaitNewThread(int newThreadNum);
    void killThread(int ThreadNote);
private:


    D3D12_RECT							stScissorRect = { 0, 0, static_cast<LONG>(iWidth), static_cast<LONG>(iHeight) };
    D3D12_RECT							stScissorRectwpos = { 0, 0, static_cast<LONG>(2048), static_cast<LONG>(1536) };
    D3D12_RECT							stScissorRect4096 = { 0, 0, static_cast<LONG>(4096), static_cast<LONG>(4096) };
    ATOM                MyRegisterClass(HINSTANCE hInstance);
    BOOL                InitInstance(HINSTANCE, int, HWND, UINT, UINT);
    HINSTANCE hInst;                                // 当前实例
    WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
    WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
    TCHAR								pszAppPath[MAX_PATH] = {};
    MSG  msg = {};
    D3D12_VIEWPORT stViewPort = { 0.0f, 0.0f, static_cast<float>(iWidth), static_cast<float>(iHeight), D3D12_MIN_DEPTH, D3D12_MAX_DEPTH };
    D3D12_VIEWPORT stViewPortwpos = { 0.0f, 0.0f, static_cast<float>(2048), static_cast<float>(1536), D3D12_MIN_DEPTH, D3D12_MAX_DEPTH };
    D3D12_VIEWPORT stViewPort4096 = { 0.0f, 0.0f, static_cast<float>(4096), static_cast<float>(4096), D3D12_MIN_DEPTH, D3D12_MAX_DEPTH };
    // 此代码模块中包含的

    UINT nFrameIndex = 0;
    UINT nFrame = 0;
  
    UINT nRTVDescriptorSize = 0U;
    EST::vector<threadparas> paras;
    EST::vector<int> existThreadList;
    EST::vector<int> deadThreadList;

    EST::vector<HANDLE> HandleMessage[4];
    EST::vector<ID3D12CommandList*>cmdlists;
    float fAspectRatio = 3.0f;
    float rotationAngle = 0.0f;
    bool hasSVGF = false;

    UINT64 n64FenceValue = 0ui64;
    HANDLE hFenceEvent = nullptr;
    UINT nSamplerDescriptorSize;


    ComPtr<ID3D12CommandQueue>           pICommandQueue;
    ComPtr<IDXGISwapChain1>              pISwapChain1;
    ComPtr<IDXGISwapChain3>              pISwapChain3;

    ComPtr<ID3D12Resource>               pIARenderTargets[nFrameBackBufCount];
    ComPtr<ID3D12CommandAllocator>       pIcmdallpre;
    ComPtr<ID3D12CommandAllocator>       pIcmdallpost;
    ComPtr<ID3D12RootSignature>          pIRootSignature;
    ComPtr<ID3D12RootSignature>          pICSRootSignature;

    ComPtr<ID3D12GraphicsCommandList>    pIcmdlistpre;
    ComPtr<ID3D12GraphicsCommandList>    pIcmdlistpost;
    ComPtr<ID3D12Fence>                  pIFence;
    ComPtr<ID3D12DescriptorHeap>		 pISRVHeap;
    ComPtr<ID3D12DescriptorHeap>         pISamheap;
    passconstant currpasscb;
    ComPtr<ID3D12DescriptorHeap>                  pIpresrvheap;
    ComPtr<ID3D12DescriptorHeap>                  pIpresamheap;
    ComPtr<ID3D12DescriptorHeap>         pIRTVHeap;
    ComPtr<ID3D12DescriptorHeap>         pIDSVHeap;
    modelReader mreader;

    std::unordered_map<std::string, std::unique_ptr< RenderItem>>SVGFRenderItemTable;
    std::unordered_map<std::string, std::unique_ptr< RenderItem>>NonSVGFRenderItemTable;
    std::unordered_map<std::string, std::unique_ptr< GeometryItem>>GeometryItemTable;
    std::unordered_map<std::string, std::unique_ptr< ConstantBufferResourceItem<passconstant>>>BufferResourceItemTable;
    std::unordered_map<std::string, std::unique_ptr< TextureResourceItem>>TextureResourceItemTable;
    std::unordered_map<std::string, std::unique_ptr< SamplerResourceItem>>SamplerResourceItemTable;
    std::unordered_map<std::string, std::unique_ptr< RootSignatureItem>>RootSignatureItemTable;
    std::unordered_map<std::string, std::unique_ptr< RT_DS_TextureSegregatedFreeLists>>RTDSSFLTable;
    std::unordered_map<std::string, std::unique_ptr< NON_RT_DS_TextureSegregatedFreeLists>>NONRTDSSFLTable;
    std::unordered_map<std::string, std::unique_ptr< uploadBuddySystem>>upBSTable;
    std::unordered_map<std::string, std::unique_ptr< defaultBuddySystem>>defBSTable;
    std::unordered_map<std::string, std::unique_ptr< PSOItem>>PSOITable;
    std::unordered_map<std::string, std::unique_ptr< computePSOItem>>computePSOITable;
    std::unordered_map<std::string, std::unique_ptr< StructureBufferResourceItem<AABBbox>>>(boxTable);
    std::unordered_map<std::string, std::unique_ptr< StructureBufferResourceItem<triangle>>>(triangleTable);
    std::unique_ptr < StructureBufferResourceItem<float>> randomNumbers;
    std::unique_ptr < StructureBufferResourceItem<XMINT2>> offsetArray[5];
    std::unique_ptr < StructureBufferResourceItem<float>> hSBRI;
    std::unique_ptr < StructureBufferResourceItem<float>> gweightSBRI;
    std::unique_ptr < StructureBufferResourceItem<PolygonalLight>> lights;
    std::unique_ptr<StructureBufferResourceItem<material>>materials;
    std::unique_ptr<StructureBufferResourceItem<XMFLOAT3>>guassianWeights;
    std::unique_ptr<StructureBufferResourceItem<ParallelLight>>ParaLightsSBRI;
    std::unique_ptr<StructureBufferResourceItem<float>> *GkernelSBRI;
    std::unique_ptr<StructureBufferResourceItem<XMFLOAT4>> SSSSkernelSBRI;
    std::unique_ptr < ConstantBufferResourceItem <lastVPmat>> lastvpmatRI;
    std::unique_ptr < ConstantBufferResourceItem <SSSS>> ssssRI;
    XMMATRIX last6FramesVP[6];
    SAHtree sahtree;
    EST::vector<triangle>SceneTriangles;
    PolygonalLight l;
    TSSSSComponent tssssComponent;
    SSSSComponent ssssComponent;
    float scaleGkernel=5.0f;
    float SSalpha=11.0f;
    float SSbeta=800.0f;
    float roughness1=0.6f;
    float paraLtheta = PI / 2.0f;
    float paraLphi = PI;
    EST::vector<Vertex>lightV;

};
void FirstAPP::init(HINSTANCE hInstance, int       nCmdShow) {
    initDX12(hInstance, WndProc, nCmdShow);
    testVector();
    createDescriptorHeap();
    createCommandQueueAndSwapChain();
    createRootSignature();
    createCommandList();
    compileShadersAndCreatePSO();
    createVideoMemoryManager();
    createDepthBufferAndSampler();
    createGeometryItemAndBVHData();
    createResourceItem();
    createRenderItem();
    startSubThread();
}
void FirstAPP::startRender() {

    startRenderLoop();
}
void FirstAPP::createDescriptorHeap() {
    //描述符堆及采样器堆
    D3D12_DESCRIPTOR_HEAP_DESC rtvheapdesc = {};
    rtvheapdesc.NumDescriptors = nFrameBackBufCount+10;//之后会加数量
    rtvheapdesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvheapdesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    ThrowIfFailed(pID3DDevice->CreateDescriptorHeap(&rtvheapdesc, IID_PPV_ARGS(&pIRTVHeap)));
    D3D12_DESCRIPTOR_HEAP_DESC presrvheapdesc = {};
    presrvheapdesc.NumDescriptors = 120;//先创建一个静态堆，之后等东西多了再加个动态管理器
    presrvheapdesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    presrvheapdesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    ThrowIfFailed(pID3DDevice->CreateDescriptorHeap(&presrvheapdesc, IID_PPV_ARGS(&pIpresrvheap)));
    presrvheapdesc.NumDescriptors = 1;
    presrvheapdesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    presrvheapdesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
    ThrowIfFailed(pID3DDevice->CreateDescriptorHeap(&presrvheapdesc, IID_PPV_ARGS(&pIpresamheap)));
    presrvheapdesc.NumDescriptors = 5;
    presrvheapdesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    presrvheapdesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    ThrowIfFailed(pID3DDevice->CreateDescriptorHeap(&presrvheapdesc, IID_PPV_ARGS(&pIDSVHeap)));
    //全局堆偏移表，记录目前堆一共绑定了多少描述符
    HeapOffsetTable[pIpresrvheap.Get()] = 0;
    HeapOffsetTable[pIpresamheap.Get()] = 0;
    HeapOffsetTable[pIDSVHeap.Get()] = 0;
    HeapOffsetTable[pIRTVHeap.Get()] = 0;
}
void FirstAPP::createCommandQueueAndSwapChain() {
    //创建命令队列
    {
        D3D12_COMMAND_QUEUE_DESC commandqueuedesc = {};
        commandqueuedesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        ThrowIfFailed(pID3DDevice->CreateCommandQueue(&commandqueuedesc, IID_PPV_ARGS(&pICommandQueue)));
    }
    //创建交换链及其资源
    {
        DXGI_SWAP_CHAIN_DESC1 swapchaindesc = {};
        swapchaindesc.BufferCount = nFrameBackBufCount;
        swapchaindesc.Width = iWidth;
        swapchaindesc.Height = iHeight;
        swapchaindesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapchaindesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapchaindesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapchaindesc.SampleDesc.Count = 1;
        if (hWnd == nullptr)
            WriteConsole(g_hOutput, L"no", 3, NULL, NULL);
        ThrowIfFailed(pIDXGIFactory5->CreateSwapChainForHwnd(pICommandQueue.Get(), hWnd, &swapchaindesc, nullptr, nullptr, &pISwapChain1));
        swapchaindesc.BufferCount = 2;

        ThrowIfFailed(pISwapChain1.As(&pISwapChain3));

        nFrameIndex = pISwapChain3->GetCurrentBackBufferIndex();

        nRTVDescriptorSize = pID3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvcpuhandle(pIRTVHeap->GetCPUDescriptorHandleForHeapStart());
        for (UINT i = 0;i < 3;i++) {
            ThrowIfFailed(pISwapChain3->GetBuffer(i, IID_PPV_ARGS(&pIARenderTargets[i])));
            pID3DDevice->CreateRenderTargetView(pIARenderTargets[i].Get(), nullptr, rtvcpuhandle);
            rtvcpuhandle.Offset(1, nRTVDescriptorSize);
            HeapOffsetTable[pIRTVHeap.Get()]++;
        }
    }
}
void FirstAPP::createRootSignature() {
    {//跟签名
        D3D12_FEATURE_DATA_ROOT_SIGNATURE stFeatureData = {};
        // 检测是否支持V1.1版本的根签名
        stFeatureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
        if (FAILED(pID3DDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &stFeatureData, sizeof(stFeatureData))))
        {
            stFeatureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
        }
#define rsNum 23
        //描述符表
        CD3DX12_DESCRIPTOR_RANGE roottables[rsNum] = {};
        roottables[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
        roottables[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 9,0,0);
        roottables[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);
        roottables[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);
        roottables[4].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0,1);
        roottables[5].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1,1);
        roottables[6].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2,1);
        roottables[7].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3,1);
        roottables[8].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 4,1);
        roottables[9].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 5,1);
        roottables[10].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 6,1);
        roottables[11].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 7,1);
        roottables[12].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 8,1);
        roottables[13].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 9,1);
        roottables[14].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 10,1);
        roottables[15].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 11,1);
        roottables[16].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 6, 0,2);
        roottables[17].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 12,1);
        roottables[18].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0,1);
        roottables[19].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 13,1);
        roottables[20].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 14,1);
        roottables[21].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 15,1);
        roottables[22].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 2);
        //跟参数
        CD3DX12_ROOT_PARAMETER rootparas[rsNum] = {};
        rootparas[0].InitAsDescriptorTable(1, &roottables[0], D3D12_SHADER_VISIBILITY_ALL);
        rootparas[1].InitAsDescriptorTable(1, &roottables[1], D3D12_SHADER_VISIBILITY_PIXEL);
        rootparas[2].InitAsDescriptorTable(1, &roottables[2], D3D12_SHADER_VISIBILITY_ALL);
        rootparas[3].InitAsDescriptorTable(1, &roottables[3], D3D12_SHADER_VISIBILITY_ALL);
        rootparas[4].InitAsDescriptorTable(1, &roottables[4], D3D12_SHADER_VISIBILITY_ALL);
        rootparas[5].InitAsDescriptorTable(1, &roottables[5], D3D12_SHADER_VISIBILITY_ALL);
        rootparas[6].InitAsDescriptorTable(1, &roottables[6], D3D12_SHADER_VISIBILITY_ALL);
        rootparas[7].InitAsDescriptorTable(1, &roottables[7], D3D12_SHADER_VISIBILITY_ALL);
        rootparas[8].InitAsDescriptorTable(1, &roottables[8], D3D12_SHADER_VISIBILITY_ALL);
        rootparas[9].InitAsDescriptorTable(1, &roottables[9], D3D12_SHADER_VISIBILITY_ALL);
        rootparas[10].InitAsDescriptorTable(1, &roottables[10], D3D12_SHADER_VISIBILITY_ALL);
        rootparas[11].InitAsDescriptorTable(1, &roottables[11], D3D12_SHADER_VISIBILITY_ALL);
        rootparas[12].InitAsDescriptorTable(1, &roottables[12], D3D12_SHADER_VISIBILITY_ALL);
        rootparas[13].InitAsDescriptorTable(1, &roottables[13], D3D12_SHADER_VISIBILITY_ALL);
        rootparas[14].InitAsDescriptorTable(1, &roottables[14], D3D12_SHADER_VISIBILITY_ALL);
        rootparas[15].InitAsDescriptorTable(1, &roottables[15], D3D12_SHADER_VISIBILITY_ALL);
        rootparas[16].InitAsDescriptorTable(1, &roottables[16], D3D12_SHADER_VISIBILITY_ALL);
        rootparas[17].InitAsDescriptorTable(1, &roottables[17], D3D12_SHADER_VISIBILITY_ALL);
        rootparas[18].InitAsDescriptorTable(1, &roottables[18], D3D12_SHADER_VISIBILITY_ALL);
        rootparas[19].InitAsDescriptorTable(1, &roottables[19], D3D12_SHADER_VISIBILITY_ALL);
        rootparas[20].InitAsDescriptorTable(1, &roottables[20], D3D12_SHADER_VISIBILITY_ALL);
        rootparas[21].InitAsDescriptorTable(1, &roottables[21], D3D12_SHADER_VISIBILITY_ALL);
        rootparas[22].InitAsDescriptorTable(1, &roottables[22], D3D12_SHADER_VISIBILITY_ALL);



        CD3DX12_ROOT_SIGNATURE_DESC rootdesc(rsNum, rootparas, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
        ComPtr<ID3DBlob>rootsignature = nullptr;
        ComPtr<ID3DBlob>error = nullptr;
        ThrowIfFailed(D3D12SerializeRootSignature(&rootdesc, D3D_ROOT_SIGNATURE_VERSION_1, rootsignature.GetAddressOf(), error.GetAddressOf()));
        assert(rootsignature != nullptr);
        ThrowIfFailed(pID3DDevice->CreateRootSignature(0, rootsignature->GetBufferPointer(), rootsignature->GetBufferSize(), IID_PPV_ARGS(&pIRootSignature)));
        assert(pIRootSignature != nullptr);
#define csrsnum 23
        //CS跟签名
        CD3DX12_DESCRIPTOR_RANGE csroottables[csrsnum] = {};
        csroottables[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);
        csroottables[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
        csroottables[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
        csroottables[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);
        csroottables[4].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3);
        csroottables[5].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 4);
        csroottables[6].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);
        csroottables[7].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
        csroottables[8].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 1);
        csroottables[9].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 2);
        csroottables[10].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 3);
        csroottables[11].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 4);
        csroottables[12].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 5);
        csroottables[13].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 6);
        csroottables[14].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 5);
        csroottables[15].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 6);
        csroottables[16].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 7);
        csroottables[17].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 7);
        csroottables[18].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 8);
        csroottables[19].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 9);
        csroottables[20].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);
        csroottables[21].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 10);
        csroottables[22].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 11);
        //跟参数
        CD3DX12_ROOT_PARAMETER csrootparas[csrsnum] = {};
        csrootparas[0].InitAsDescriptorTable(1, &csroottables[0], D3D12_SHADER_VISIBILITY_ALL);
        csrootparas[1].InitAsDescriptorTable(1, &csroottables[1], D3D12_SHADER_VISIBILITY_ALL);
        csrootparas[2].InitAsDescriptorTable(1, &csroottables[2], D3D12_SHADER_VISIBILITY_ALL);
        csrootparas[3].InitAsDescriptorTable(1, &csroottables[3], D3D12_SHADER_VISIBILITY_ALL);
        csrootparas[4].InitAsDescriptorTable(1, &csroottables[4], D3D12_SHADER_VISIBILITY_ALL);
        csrootparas[5].InitAsDescriptorTable(1, &csroottables[5], D3D12_SHADER_VISIBILITY_ALL);
        csrootparas[6].InitAsDescriptorTable(1, &csroottables[6], D3D12_SHADER_VISIBILITY_ALL);
        csrootparas[7].InitAsDescriptorTable(1, &csroottables[7], D3D12_SHADER_VISIBILITY_ALL);
        csrootparas[8].InitAsDescriptorTable(1, &csroottables[8], D3D12_SHADER_VISIBILITY_ALL);
        csrootparas[9].InitAsDescriptorTable(1, &csroottables[9], D3D12_SHADER_VISIBILITY_ALL);
        csrootparas[10].InitAsDescriptorTable(1, &csroottables[10], D3D12_SHADER_VISIBILITY_ALL);
        csrootparas[11].InitAsDescriptorTable(1, &csroottables[11], D3D12_SHADER_VISIBILITY_ALL);
        csrootparas[12].InitAsDescriptorTable(1, &csroottables[12], D3D12_SHADER_VISIBILITY_ALL);
        csrootparas[13].InitAsDescriptorTable(1, &csroottables[13], D3D12_SHADER_VISIBILITY_ALL);
        csrootparas[14].InitAsDescriptorTable(1, &csroottables[14], D3D12_SHADER_VISIBILITY_ALL);
        csrootparas[15].InitAsDescriptorTable(1, &csroottables[15], D3D12_SHADER_VISIBILITY_ALL);
        csrootparas[16].InitAsDescriptorTable(1, &csroottables[16], D3D12_SHADER_VISIBILITY_ALL);
        csrootparas[17].InitAsDescriptorTable(1, &csroottables[17], D3D12_SHADER_VISIBILITY_ALL);
        csrootparas[18].InitAsDescriptorTable(1, &csroottables[18], D3D12_SHADER_VISIBILITY_ALL);
        csrootparas[19].InitAsDescriptorTable(1, &csroottables[19], D3D12_SHADER_VISIBILITY_ALL);
        csrootparas[20].InitAsDescriptorTable(1, &csroottables[20], D3D12_SHADER_VISIBILITY_ALL);
        csrootparas[21].InitAsDescriptorTable(1, &csroottables[21], D3D12_SHADER_VISIBILITY_ALL);
        csrootparas[22].InitAsDescriptorTable(1, &csroottables[22], D3D12_SHADER_VISIBILITY_ALL);



        CD3DX12_ROOT_SIGNATURE_DESC csrootdesc(csrsnum, csrootparas, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
        ComPtr<ID3DBlob>csrootsignature = nullptr;
        ComPtr<ID3DBlob>cserror = nullptr;
        ThrowIfFailed(D3D12SerializeRootSignature(&csrootdesc, D3D_ROOT_SIGNATURE_VERSION_1, csrootsignature.GetAddressOf(), cserror.GetAddressOf()));
        assert(csrootsignature != nullptr);
        ThrowIfFailed(pID3DDevice->CreateRootSignature(0, csrootsignature->GetBufferPointer(), csrootsignature->GetBufferSize(), IID_PPV_ARGS(&pICSRootSignature)));
        assert(pICSRootSignature != nullptr);
        //创建跟签名项
        auto rsRI = std::make_unique<RootSignatureItem>(pIRootSignature.Get(), rsNum, 3, 0, 1, 1, roottables);
        RootSignatureItemTable["default"] = std::move(rsRI);
        rsRI = std::make_unique<RootSignatureItem>(pICSRootSignature.Get(), csrsnum, 3, 0, 1, 1,csroottables);
        RootSignatureItemTable["cs"] = std::move(rsRI);
    }
}
void FirstAPP::createCommandList() {
    {
        ThrowIfFailed(pID3DDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&pIcmdallpre)));
        ThrowIfFailed(pID3DDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&pIcmdallpost)));
        ThrowIfFailed(pID3DDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, pIcmdallpre.Get(), nullptr, IID_PPV_ARGS(&pIcmdlistpre)));
        ThrowIfFailed(pID3DDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, pIcmdallpost.Get(), nullptr, IID_PPV_ARGS(&pIcmdlistpost)));
    }
}
void FirstAPP::compileShadersAndCreatePSO() {
    {
        BYTE* cbvmapped = nullptr;
#if defined(_DEBUG)
        UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
        UINT compileFlags = 0;
#endif
        compileFlags |= D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
        D3D12_INPUT_ELEMENT_DESC inputdesc[] = {
            {"POSITION",0,DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
            {"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
            {"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
            {"TANGENT",0,DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
            {"AOk",0,DXGI_FORMAT_R32_FLOAT, 0, 48, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
            {"COLOR",0,DXGI_FORMAT_R32G32B32_FLOAT, 0, 52, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
        };
        D3D12_INPUT_ELEMENT_DESC modelinputdesc[] = {
{"POSITION",0,DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
 {"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
 {"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
 {"TANGENT",0,DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
   {"AOk",0,DXGI_FORMAT_R32_FLOAT, 0, 48, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
   {"COLOR",0,DXGI_FORMAT_R32G32B32_FLOAT, 0, 52, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},

 {"WEIGHTS",0,DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 64, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
 {"BONEINDICES",0,DXGI_FORMAT_R8G8B8A8_UINT, 0, 80, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
        };
        //创建PSO
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psodesc = {};
        psodesc.InputLayout = { inputdesc,_countof(inputdesc) };
        psodesc.pRootSignature = pIRootSignature.Get();
        psodesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
        psodesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
        psodesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
        psodesc.BlendState.AlphaToCoverageEnable = FALSE;
        psodesc.BlendState.IndependentBlendEnable = FALSE;
        psodesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
        psodesc.DepthStencilState.DepthEnable = TRUE;
        psodesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
        psodesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
        psodesc.DepthStencilState.StencilEnable = FALSE;
        psodesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
        psodesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psodesc.NumRenderTargets = 1;
        psodesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psodesc.SampleMask = UINT_MAX;
        psodesc.SampleDesc.Count = 1;
        psodesc.SampleDesc.Quality = 0;
        auto PSOI = std::make_unique<PSOItem>(&psodesc, L"Shaders//GlossyDirectLuminance.hlsl", pID3DDevice.Get(),nullptr,"PS");
        PSOITable["glossyRaytraceDirect"] = std::move(PSOI);
        PSOI = std::make_unique<PSOItem>(&psodesc, L"Shaders//GlossyIndirectLightLuminance.hlsl", pID3DDevice.Get(), nullptr, "PS");
        PSOITable["glossyRaytraceIndirect"] = std::move(PSOI);
        PSOI = std::make_unique<PSOItem>(&psodesc, L"Shaders//DiffuseDirectLuminance.hlsl", pID3DDevice.Get(), nullptr, "PS");
        PSOITable["diffuseRaytraceDirect"] = std::move(PSOI);
        PSOI = std::make_unique<PSOItem>(&psodesc, L"Shaders//DiffuseIndirectLuminance.hlsl", pID3DDevice.Get(), nullptr, "PS");
        PSOITable["diffuseRaytraceIndirect"] = std::move(PSOI);
        PSOI = std::make_unique<PSOItem>(&psodesc, L"Shaders//translucentDirectLuminance.hlsl", pID3DDevice.Get(), nullptr, "PS");
        PSOITable["translucentRaytraceDirect"] = std::move(PSOI);
        psodesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
        PSOI= std::make_unique<PSOItem>(&psodesc, L"Shaders//returnColor.hlsl", pID3DDevice.Get(),nullptr, "PS");
        PSOITable["color"] = std::move(PSOI);
        psodesc.InputLayout = { modelinputdesc,_countof(modelinputdesc) };
        psodesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
        PSOI = std::make_unique<PSOItem>(&psodesc, L"Shaders//drawModel.hlsl", pID3DDevice.Get(),nullptr, "PS");
        PSOITable["tex"] = std::move(PSOI);
        psodesc.InputLayout = { inputdesc,_countof(inputdesc) };
        psodesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
        PSOI = std::make_unique<PSOItem>(&psodesc, L"Shaders//drawBox.hlsl", pID3DDevice.Get(),nullptr, "PS");
        PSOITable["box"] = std::move(PSOI);
        psodesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psodesc.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
        PSOI = std::make_unique<PSOItem>(&psodesc, L"Shaders//wposPass.hlsl", pID3DDevice.Get(),nullptr, "PS");
        PSOITable["wpos"] = std::move(PSOI);
        PSOI = std::make_unique<PSOItem>(&psodesc, L"Shaders//normalPass.hlsl", pID3DDevice.Get(),nullptr, "PS");
        PSOITable["normal"] = std::move(PSOI);
        PSOI = std::make_unique<PSOItem>(&psodesc, L"Shaders//zPass.hlsl", pID3DDevice.Get(), nullptr, "PS");
        PSOITable["z"] = std::move(PSOI);
        PSOI = std::make_unique<PSOItem>(&psodesc, L"Shaders//gzPass.hlsl", pID3DDevice.Get(), nullptr, "PS");
        PSOITable["gz"] = std::move(PSOI);
        psodesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        PSOI = std::make_unique<PSOItem>(&psodesc, L"Shaders//firstFramePass.hlsl", pID3DDevice.Get(),nullptr, "PS");
        PSOITable["firstFramePass"] = std::move(PSOI);
        PSOI = std::make_unique<PSOItem>(&psodesc, L"Shaders//modulatePass.hlsl", pID3DDevice.Get(), nullptr, "PS");
        PSOITable["modulatePass"] = std::move(PSOI);
        PSOI = std::make_unique<PSOItem>(&psodesc, L"Shaders//sky.hlsl", pID3DDevice.Get(), nullptr, "PS");
        PSOITable["sky"] = std::move(PSOI);
        //psodesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
        PSOI = std::make_unique<PSOItem>(&psodesc, L"Shaders//skinspecular.hlsl", pID3DDevice.Get(), nullptr, "PS");
        PSOITable["skinspecular"] = std::move(PSOI);
        PSOI = std::make_unique<PSOItem>(&psodesc, L"Shaders//calcIrradiance.hlsl", pID3DDevice.Get(), nullptr, "PS");
        PSOITable["calcIrradiance"] = std::move(PSOI);
        PSOI = std::make_unique<PSOItem>(&psodesc, L"Shaders//shadowMap.hlsl", pID3DDevice.Get(), nullptr, "PS");
        PSOITable["shadowMap"] = std::move(PSOI);
        PSOI = std::make_unique<PSOItem>(&psodesc, L"Shaders//returnIrradianceFromTex.hlsl", pID3DDevice.Get(), nullptr, "PS");
        PSOITable["irradianceFromTex"] = std::move(PSOI);
        PSOI = std::make_unique<PSOItem>(&psodesc, L"Shaders//returnIrradianceOnTheFly.hlsl", pID3DDevice.Get(), nullptr, "PS");
        PSOITable["irradianceOnTheFly"] = std::move(PSOI);
        PSOI = std::make_unique<PSOItem>(&psodesc, L"Shaders//DuallobeSpecular.hlsl", pID3DDevice.Get(), nullptr, "PS");
        PSOITable["dualLobeSpecular"] = std::move(PSOI);
        psodesc.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
        PSOI = std::make_unique<PSOItem>(&psodesc, L"Shaders//stretchMap.hlsl", pID3DDevice.Get(), nullptr, "PS");
        PSOITable["stretch"] = std::move(PSOI);
        psodesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

        //模板写入PSO
        D3D12_DEPTH_STENCIL_DESC ScreenSpaceDepthStencil;
        ScreenSpaceDepthStencil.DepthEnable = true;	//开启深度测试
        ScreenSpaceDepthStencil.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;//禁止深度写入
        ScreenSpaceDepthStencil.DepthFunc = D3D12_COMPARISON_FUNC_LESS;//比较函数“小于”
        ScreenSpaceDepthStencil.StencilEnable = true;//开启模板测试
        ScreenSpaceDepthStencil.StencilReadMask = 0xff;//默认255，不屏蔽模板值
        ScreenSpaceDepthStencil.StencilWriteMask = 0xff;//默认255，不屏蔽模板值
        ScreenSpaceDepthStencil.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;//模板测试失败，保持原模板值
        ScreenSpaceDepthStencil.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;//深度测试失败，保持原模板值
        ScreenSpaceDepthStencil.FrontFace.StencilPassOp = D3D12_STENCIL_OP_REPLACE;//深度模板测试通过，替换Ref模板值
        ScreenSpaceDepthStencil.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;;//比较函数“永远通过测试”
        ScreenSpaceDepthStencil.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;//反面不渲染，随便写
        ScreenSpaceDepthStencil.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;//反面不渲染，随便写
        ScreenSpaceDepthStencil.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;//反面不渲染，随便写
        ScreenSpaceDepthStencil.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;//反面不渲染，随便写
        psodesc.DepthStencilState = ScreenSpaceDepthStencil;
        psodesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
        for (int i = 1;i < 3;i++)
            psodesc.RTVFormats[i] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psodesc.RTVFormats[3] = DXGI_FORMAT_R16G16B16A16_FLOAT;
        psodesc.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
        psodesc.RTVFormats[4] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psodesc.NumRenderTargets = 5;
        PSOI = std::make_unique<PSOItem>(&psodesc, L"Shaders//GBufferPass.hlsl", pID3DDevice.Get(), nullptr, "PS");
        PSOITable["GBufferPass"] = std::move(PSOI);

        D3D12_COMPUTE_PIPELINE_STATE_DESC computePSOdesc = {};
        computePSOdesc.pRootSignature = pICSRootSignature.Get();
        computePSOdesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
        auto computePSOI = std::make_unique<computePSOItem>(&computePSOdesc, L"Shaders//denoise.hlsl", pID3DDevice.Get(),nullptr,"CS");
        computePSOITable["denoise"] = std::move(computePSOI);
        computePSOI = std::make_unique<computePSOItem>(&computePSOdesc, L"Shaders//mixPass.hlsl", pID3DDevice.Get(),nullptr,"CS");
        computePSOITable["mixPass"] = std::move(computePSOI);
        computePSOI = std::make_unique<computePSOItem>(&computePSOdesc, L"Shaders//accumulationPass.hlsl", pID3DDevice.Get(), nullptr, "CS");
        computePSOITable["accumulationPass"] = std::move(computePSOI);
        computePSOI = std::make_unique<computePSOItem>(&computePSOdesc, L"Shaders//variancePass.hlsl", pID3DDevice.Get(), nullptr, "CS");
        computePSOITable["variancePass"] = std::move(computePSOI);
        computePSOI = std::make_unique<computePSOItem>(&computePSOdesc, L"Shaders//tableCDF.hlsl", pID3DDevice.Get(), nullptr, "CS");
        computePSOITable["tableCDF"] = std::move(computePSOI);
        computePSOI = std::make_unique<computePSOItem>(&computePSOdesc, L"Shaders//tablateBeckman.hlsl", pID3DDevice.Get(), nullptr, "CS");
        computePSOITable["tableBeckman"] = std::move(computePSOI);
        computePSOI = std::make_unique<computePSOItem>(&computePSOdesc, L"Shaders//GaussianConvolve.hlsl", pID3DDevice.Get(), nullptr, "CSX");
        computePSOITable["GaussianConvolveX"] = std::move(computePSOI);
        computePSOI = std::make_unique<computePSOItem>(&computePSOdesc, L"Shaders//GaussianConvolve.hlsl", pID3DDevice.Get(), nullptr, "CSY");
        computePSOITable["GaussianConvolveY"] = std::move(computePSOI);
        computePSOI = std::make_unique<computePSOItem>(&computePSOdesc, L"Shaders//convolveStretch.hlsl", pID3DDevice.Get(), nullptr, "CSX");
        computePSOITable["ConvolveStretchX"] = std::move(computePSOI);
        computePSOI = std::make_unique<computePSOItem>(&computePSOdesc, L"Shaders//convolveStretch.hlsl", pID3DDevice.Get(), nullptr, "CSY");
        computePSOITable["ConvolveStretchY"] = std::move(computePSOI);
        computePSOI = std::make_unique<computePSOItem>(&computePSOdesc, L"Shaders//SSSSConvolve.hlsl", pID3DDevice.Get(), nullptr, "CSX");
        computePSOITable["SSSSConvolveX"] = std::move(computePSOI);
        computePSOI = std::make_unique<computePSOItem>(&computePSOdesc, L"Shaders//SSSSConvolve.hlsl", pID3DDevice.Get(), nullptr, "CSY");
        computePSOITable["SSSSConvolveY"] = std::move(computePSOI);
    }
}
void FirstAPP::createVideoMemoryManager() {
  auto RTDSsfl= std::make_unique< RT_DS_TextureSegregatedFreeLists> (GRS_UPPER(800, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT), 6, pID3DDevice.Get());
  RTDSSFLTable["RTDS"] = std::move(RTDSsfl);
  RTDSsfl = std::make_unique< RT_DS_TextureSegregatedFreeLists>(GRS_UPPER(800, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT), 6, pID3DDevice.Get());
  RTDSSFLTable["RTDS1"] = std::move(RTDSsfl);
  RTDSsfl = std::make_unique< RT_DS_TextureSegregatedFreeLists>(GRS_UPPER(800, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT), 6, pID3DDevice.Get());
  RTDSSFLTable["RTDS2"] = std::move(RTDSsfl);
  RTDSsfl = std::make_unique< RT_DS_TextureSegregatedFreeLists>(GRS_UPPER(800, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT), 6, pID3DDevice.Get());
  RTDSSFLTable["RTDS3"] = std::move(RTDSsfl);
  RTDSsfl = std::make_unique< RT_DS_TextureSegregatedFreeLists>(GRS_UPPER(800, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT), 6, pID3DDevice.Get());
  RTDSSFLTable["RTDS4"] = std::move(RTDSsfl);

  auto NONRTDSsfl = std::make_unique< NON_RT_DS_TextureSegregatedFreeLists>(GRS_UPPER(800, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT), 6, pID3DDevice.Get());
  NONRTDSSFLTable["NONRTDS"] = std::move(NONRTDSsfl);
   NONRTDSsfl = std::make_unique< NON_RT_DS_TextureSegregatedFreeLists>(GRS_UPPER(800, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT), 6, pID3DDevice.Get());
  NONRTDSSFLTable["NONRTDS1"] = std::move(NONRTDSsfl);
  NONRTDSsfl = std::make_unique< NON_RT_DS_TextureSegregatedFreeLists>(GRS_UPPER(800, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT), 6, pID3DDevice.Get());
  NONRTDSSFLTable["NONRTDS2"] = std::move(NONRTDSsfl);
  NONRTDSsfl = std::make_unique< NON_RT_DS_TextureSegregatedFreeLists>(GRS_UPPER(800, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT), 6, pID3DDevice.Get());
  NONRTDSSFLTable["NONRTDS3"] = std::move(NONRTDSsfl);
  NONRTDSsfl = std::make_unique< NON_RT_DS_TextureSegregatedFreeLists>(GRS_UPPER(800, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT), 6, pID3DDevice.Get());
  NONRTDSSFLTable["NONRTDS4"] = std::move(NONRTDSsfl);

  auto upBS = std::make_unique<uploadBuddySystem>(1024*8, 8, pID3DDevice.Get());//最小64KB对齐,也就是最精细的buddy最小是64KB及其倍数
  upBSTable["UPBS"] = std::move(upBS);
  auto upBS1 = std::make_unique<uploadBuddySystem>(1024*8, 8, pID3DDevice.Get());//最小64KB对齐,也就是最精细的buddy最小是64KB及其倍数
  upBSTable["UPBS1"] = std::move(upBS1);
 auto upBS2 = std::make_unique<uploadBuddySystem>(1024*8, 8, pID3DDevice.Get());//最小64KB对齐,也就是最精细的buddy最小是64KB及其倍数
  upBSTable["UPBS2"] = std::move(upBS2);
 auto upBS3 = std::make_unique<uploadBuddySystem>(1024 * 8, 8, pID3DDevice.Get());//最小64KB对齐,也就是最精细的buddy最小是64KB及其倍数
  upBSTable["UPBS3"] = std::move(upBS3);

  auto defBS = std::make_unique<defaultBuddySystem>(1024*64, 8, pID3DDevice.Get());
  defBSTable["DEFBS"] = std::move(defBS);

   defBS = std::make_unique<defaultBuddySystem>(1024*8, 8, pID3DDevice.Get());
  defBSTable["DEFBS1"] = std::move(defBS);
  defBS = std::make_unique<defaultBuddySystem>(1024*8, 8, pID3DDevice.Get());
  defBSTable["DEFBS2"] = std::move(defBS);
  defBS = std::make_unique<defaultBuddySystem>(1024 * 8, 8, pID3DDevice.Get());
  defBSTable["DEFBS3"] = std::move(defBS);
}
void FirstAPP::createDepthBufferAndSampler() {
    //创建深度缓冲及其资源项
    D3D12_RESOURCE_DESC dsdesc = {};
    dsdesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    dsdesc.Alignment = 0;
    dsdesc.Width = iWidth;
    dsdesc.Height = iHeight;
    dsdesc.DepthOrArraySize = 1;
    dsdesc.MipLevels = 1;
    dsdesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsdesc.SampleDesc.Count = 1;
    dsdesc.SampleDesc.Quality = 0;
    dsdesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    dsdesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvdesc = {};
    dsvdesc.Flags = D3D12_DSV_FLAG_NONE;
    dsvdesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvdesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvdesc.Texture2D.MipSlice = 0;
    D3D12_CLEAR_VALUE dsclear = {};
    dsclear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsclear.DepthStencil.Depth = 1.0f;
    dsclear.DepthStencil.Stencil = 0;
    D3D12_SHADER_RESOURCE_VIEW_DESC SRVdesc = {};
    SRVdesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    SRVdesc.Texture2D.MostDetailedMip = 0;
    SRVdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    SRVdesc.Texture2D.ResourceMinLODClamp = 0.0f;
    SRVdesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    auto DSRI = std::make_unique<TextureResourceItem>(pID3DDevice.Get(), pIpresrvheap.Get(), nullptr, pIDSVHeap.Get(), false,-1);
    DSRI->createRT_DS_WritableTex(pID3DDevice.Get(), pIcmdlistpre.Get(), &dsdesc, &dsclear,RTDSSFLTable["RTDS3"].get());
    DSRI->createDSVforResourceItem(&dsvdesc, pID3DDevice.Get(), HeapOffsetTable[pIDSVHeap.Get()], pIcmdlistpre.Get());
    TextureResourceItemTable["DS"] = std::move(DSRI);
     DSRI = std::make_unique<TextureResourceItem>(pID3DDevice.Get(), pIpresrvheap.Get(), nullptr, pIDSVHeap.Get(), false, -1);
    DSRI->createRT_DS_WritableTex(pID3DDevice.Get(), pIcmdlistpre.Get(), &dsdesc, &dsclear, RTDSSFLTable["RTDS3"].get());
    DSRI->createDSVforResourceItem(&dsvdesc, pID3DDevice.Get(), HeapOffsetTable[pIDSVHeap.Get()], pIcmdlistpre.Get());
    TextureResourceItemTable["DS1"] = std::move(DSRI);
    DSRI = std::make_unique<TextureResourceItem>(pID3DDevice.Get(), pIpresrvheap.Get(), nullptr, pIDSVHeap.Get(), false, -1);
    DSRI->createRT_DS_WritableTex(pID3DDevice.Get(), pIcmdlistpre.Get(), &dsdesc, &dsclear, RTDSSFLTable["RTDS3"].get());
    DSRI->createDSVforResourceItem(&dsvdesc, pID3DDevice.Get(), HeapOffsetTable[pIDSVHeap.Get()], pIcmdlistpre.Get());
    TextureResourceItemTable["DSForRt"] = std::move(DSRI);
    //DSRI = std::make_unique<TextureResourceItem>(pID3DDevice.Get(), pIpresrvheap.Get(), nullptr, pIDSVHeap.Get(), false, -1);
    //DSRI->createRT_DS_WritableTex(pID3DDevice.Get(), pIcmdlistpre.Get(), &dsdesc, &dsclear, RTDSSFLTable["RTDS"].get());
    //DSRI->createSRVforResourceItem(&SRVdesc, pID3DDevice.Get(), HeapOffsetTable[pIpresrvheap.Get()]);
    //DSRI->createDSVforResourceItem(&dsvdesc, pID3DDevice.Get(), HeapOffsetTable[pIDSVHeap.Get()], pIcmdlistpre.Get());
    //TextureResourceItemTable["shadowMap"] = std::move(DSRI);
    dsdesc.Width = 2048;
    dsdesc.Height = 1536;
    DSRI = std::make_unique<TextureResourceItem>(pID3DDevice.Get(), pIpresrvheap.Get(), nullptr, pIDSVHeap.Get(), false, -1);
    DSRI->createRT_DS_WritableTex(pID3DDevice.Get(), pIcmdlistpre.Get(), &dsdesc, &dsclear, RTDSSFLTable["RTDS3"].get());
    DSRI->createDSVforResourceItem(&dsvdesc, pID3DDevice.Get(), HeapOffsetTable[pIDSVHeap.Get()], pIcmdlistpre.Get());
    TextureResourceItemTable["wposDS"] = std::move(DSRI);
    dsdesc.Width = 4096;
    dsdesc.Height = 4096;
    DSRI = std::make_unique<TextureResourceItem>(pID3DDevice.Get(), pIpresrvheap.Get(), nullptr, pIDSVHeap.Get(), false, -1);
    DSRI->createRT_DS_WritableTex(pID3DDevice.Get(), pIcmdlistpre.Get(), &dsdesc, &dsclear, RTDSSFLTable["RTDS3"].get());
    DSRI->createDSVforResourceItem(&dsvdesc, pID3DDevice.Get(), HeapOffsetTable[pIDSVHeap.Get()], pIcmdlistpre.Get());
    TextureResourceItemTable["4096DS"] = std::move(DSRI);
    //创建一个默认采样器及其资源项
    D3D12_SAMPLER_DESC preSamplerDesc = {};
    preSamplerDesc.Filter = D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
    preSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    preSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    preSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    preSamplerDesc.MipLODBias = 0;
    preSamplerDesc.MaxAnisotropy = 1;
    preSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    preSamplerDesc.MinLOD = 0.0f;
    preSamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
    auto SamplerRI = std::make_unique<SamplerResourceItem>(pID3DDevice.Get(), pIpresamheap.Get(), preSamplerDesc, HeapOffsetTable[pIpresamheap.Get()]);
    SamplerResourceItemTable["default"] = std::move(SamplerRI);
}
void FirstAPP::createGeometryItemAndBVHData() {
    //模型顶点
    mreader.readFileHeader("C:\\mytinyrenderproj\\rayTracer\\rayTracer\\soldier.m3d");
    int istarr[6] = { 0,7230 * 3,11679 * 3,18258 * 3,22065 * 3,10000000000 };
    UINT isizearr[5] = { 7230 * 3 ,4449 * 3 ,6579 * 3 ,3807 * 3 ,442 * 3 };
    int ModelTexIndex = -1;
    int nextIndex = 0;
    //for (int i = 0;i < mreader.mindices.size() / 333;i++) {
    //    triangle tri;
    //    if (i * 3 >= nextIndex) {
    //        ModelTexIndex++;
    //        nextIndex = istarr[ModelTexIndex + 1];
    //    }
    //    tri.texIndex = ModelTexIndex + 3;
    //    modelVertex v1 = mreader.mvertices[mreader.mindices[i * 3]];
    //    modelVertex v2 = mreader.mvertices[mreader.mindices[i * 3 + 1]];
    //    modelVertex v3 = mreader.mvertices[mreader.mindices[i * 3 + 2]];
    //    tri.pos1 = XMFLOAT3{v1.position.x,v1.position.y,v1.position.z};
    //    tri.pos2 = XMFLOAT3{ v2.position.x,v2.position.y,v2.position.z };
    //    tri.pos3 = XMFLOAT3{ v3.position.x,v3.position.y,v3.position.z };
    //    tri.n = mreader.mvertices[mreader.mindices[i * 3]].normal;
    //    tri.uv12 = XMFLOAT4{v1.texuv.x,v1.texuv.y, v2.texuv.x,v2.texuv.y};
    //    tri.uv3 = XMFLOAT2{ v3.texuv.x,v3.texuv.y };
    //    tri.color = v1.color;
    //    SceneTriangles.push_back(tri);
    //}

     // Callbacks for API
    EST::vector<Vertex>LPSheadVers;
    EST::vector<std::uint16_t>LPSheadInds;
    TK_ObjDelegate objDelegate = {};
    objDelegate.userData = &LPSheadVers;
    objDelegate.material = matCallBack;
    objDelegate.triangle = triangleCallBack;
    size_t objFileSize = 0;
    void* objFileData = readEntireFile("LPSHead//head.obj", &objFileSize);

    TK_ParseObj(objFileData, objFileSize, &objDelegate);
    objDelegate.scratchMem = malloc(objDelegate.scratchMemSize);
    TK_ParseObj(objFileData, objFileSize, &objDelegate);
    for (int i = 0;i < LPSheadVers.size();i++)
        LPSheadInds.push_back(i);
    //兔子顶点
    //OBJReader objRD;
    //objRD.readOBJFile("bunny.obj", XMFLOAT3{ 0.6,0.6,0.6 });
   XMMATRIX world = XMMatrixTranslation(40, 0, -50.0f);
    XMMATRIX s = XMMatrixScaling(150, 150, 150);
    world = XMMatrixMultiply(s, world);
    //AddTrianglesToScene(SceneTriangles, objRD.vertices, objRD.indices,-1,(world));




    //房间顶点
    Vertex arrv[] = {
{XMFLOAT4{-60.0f + 6,5,60.0f,1.0f},XMFLOAT2{0,.0f},XMFLOAT3{0,1.0f,0},XMFLOAT3{1.0f,0,0},0},//底
{XMFLOAT4{150.0f,5,60.0f,1.0f},XMFLOAT2{1.0f,.0f},XMFLOAT3{0,1.0f,0},XMFLOAT3{1.0f,0,0},0},
{XMFLOAT4{-60.0f + 6,5,-120.0f,1.0f},XMFLOAT2{0,1.0f},XMFLOAT3{0,1.0f,0},XMFLOAT3{1.0f,0,0},0},
{XMFLOAT4{150.0f,  5 ,-120.0f,1.0f},XMFLOAT2{1.0f,1.0f},XMFLOAT3{0,1.0f,0},XMFLOAT3{1.0f,0,0},0},

{XMFLOAT4{-60.0f + 6, 200.0f ,-120.0f,1.0f},XMFLOAT2{0,0},XMFLOAT3{1.0f,0,0},XMFLOAT3{0,0,-1.0f},0},//左
{XMFLOAT4{-60.0f + 6, 200.0f ,60.0f,1.0f},XMFLOAT2{1.0f,0},XMFLOAT3{1.0f,0,0},XMFLOAT3{0,0,-1.0f},0},
{XMFLOAT4{-60.0f + 6,5,-120.0f,1.0f},XMFLOAT2{0,1.0f},XMFLOAT3{1.0f,0,0},XMFLOAT3{0,0,-1.0f},0},
{XMFLOAT4{-60.0f + 6,5,60.0f,1.0f},XMFLOAT2{1.0f,1.0f},XMFLOAT3{1.0f,0,0},XMFLOAT3{0,0,-1.0f},0},


 {XMFLOAT4{-60.0f + 6,200,60.0f,1.0f},XMFLOAT2{0,0},XMFLOAT3{0,1.0f,0},XMFLOAT3{1.0f,0,0},0},//前
  {XMFLOAT4{150.0f,200,60.0f,1.0f},XMFLOAT2{1.0f,.0f},XMFLOAT3{0,1.0f,0},XMFLOAT3{1.0f,0,0},0},
  {XMFLOAT4{-60.0f + 6,0,60.0f,1.0f},XMFLOAT2{0,1.0f},XMFLOAT3{0,1.0f,0},XMFLOAT3{1.0f,0,0},0},
  {XMFLOAT4{150.0f,0,60.0f,1.0f},XMFLOAT2{1.0f,1.0f},XMFLOAT3{0,1.0f,0},XMFLOAT3{1.0f,0,0},0},

   {XMFLOAT4{150.0f,200,60.0f,1.0f},XMFLOAT2{0,0},XMFLOAT3{0,1.0f,0},XMFLOAT3{1.0f,0,0},0},//右
  {XMFLOAT4{150.0f,200,-120.0f,1.0f},XMFLOAT2{1.0f,.0f},XMFLOAT3{0,1.0f,0},XMFLOAT3{1.0f,0,0},0},
  {XMFLOAT4{150.0f,0,60.0f,1.0f},XMFLOAT2{0,1.0f},XMFLOAT3{0,1.0f,0},XMFLOAT3{1.0f,0,0},0},
  {XMFLOAT4{150.0f,0,-120.0f,1.0f},XMFLOAT2{1.0f,1.0f},XMFLOAT3{0,1.0f,0},XMFLOAT3{1.0f,0,0},0},

     {XMFLOAT4{-60.0f,200,-120.0f,1.0f},XMFLOAT2{0,0},XMFLOAT3{0,1.0f,0},XMFLOAT3{1.0f,0,0},0},//上
  {XMFLOAT4{150.0f,200,-120.0f,1.0f},XMFLOAT2{1.0f,.0f},XMFLOAT3{0,1.0f,0},XMFLOAT3{1.0f,0,0},0},
  {XMFLOAT4{-60.0f,200,60.0f,1.0f},XMFLOAT2{0,1.0f},XMFLOAT3{0,1.0f,0},XMFLOAT3{1.0f,0,0},0},
  {XMFLOAT4{150.0f,200,60.0f,1.0f},XMFLOAT2{1.0f,1.0f},XMFLOAT3{0,1.0f,0},XMFLOAT3{1.0f,0,0},0},
    };
    EST::vector<Vertex>planev(arrv, 20);
    for (int i = 0;i < 4;i++) {
        planev[i].color = XMFLOAT3{ 0.4f,0.4f,0.4f };
        planev[i].normal = XMFLOAT3{ 0,1.0f,0 };
    }
    for (int i = 4;i < 8;i++) {
        planev[i].color = XMFLOAT3{ 0.4f,0,0 };
        planev[i].normal = XMFLOAT3{ 1.0f,0,0 };
    }
    for (int i = 8;i < 12;i++) {
        planev[i].color = XMFLOAT3{ 0.4f,0.4f,0.4f };
        planev[i].normal = XMFLOAT3{ 0,0,-1.0f };
    }
    for (int i = 12;i < 16;i++) {
        planev[i].color = XMFLOAT3{ 0,0.4f,0 };
        planev[i].normal = XMFLOAT3{ -1.0f,0,0 };
    }
    for (int i = 16;i < 20;i++) {
        planev[i].color = XMFLOAT3{ 0.4f,0.4f,0.4f };
        planev[i].normal = XMFLOAT3{ 0,-1.0f,0 };
    }
    std::uint16_t arri[] = {
         0,1,2,2,1,3,
         4,5,6,6,5,7,
         8,9,10,10,9,11,
         12,13,14,14,13,15,
         16,17,18,18,17,19
    };
    EST::vector<std::uint16_t>planei(arri, 30);

    AddTrianglesToScene(SceneTriangles, planev, planei,-1, XMLoadFloat4x4(&MathHelper::Identity4x4()));




    //面光源顶点
    Vertex Lightv[] = {
     {XMFLOAT4{0,199.5,-55,1.0f},XMFLOAT2{0,0},XMFLOAT3{0,-1.0f,0},XMFLOAT3{1.0f,0,0},0,XMFLOAT3{1.0,1.0,1.0}},
  {XMFLOAT4{50,199.5,-55,1.0f},XMFLOAT2{1.0f,.0f},XMFLOAT3{0,-1.0f,0},XMFLOAT3{1.0f,0,0},0,XMFLOAT3{1.0,1.0,1.0}},
  {XMFLOAT4{0,199.5,-5,1.0f},XMFLOAT2{0,1.0f},XMFLOAT3{0,-1.0f,0},XMFLOAT3{1.0f,0,0},0,XMFLOAT3{1.0,1.0,1.0}},
  {XMFLOAT4{50,199.5,-5,1.0f},XMFLOAT2{1.0f,1.0f},XMFLOAT3{0,-1.0f,0},XMFLOAT3{1.0f,0,0},0,XMFLOAT3{1.0,1.0,1.0}}
    };
    std::uint16_t Lighti[] = {
        0,1,2,2,1,3
    };
    EST::vector<Vertex>lightv(Lightv, 4);
    EST::vector<std::uint16_t>lighti(Lighti, 6);
    //康奈尔盒子顶点
    EST::vector<Vertex>cornellBoxv;
    EST::vector<std::uint16_t>cornellBoxi;
    CreateBox(30, 80, 30, cornellBoxv, cornellBoxi, XMFLOAT3{1.0f,1.0f,1.0f});
    world = XMMatrixTranslation(20, 70, 0.0f);
    world = XMMatrixMultiply(world, XMMatrixRotationY(PI / 3));
    AddTrianglesToScene(SceneTriangles, cornellBoxv, cornellBoxi, -1, (world));
    XMMATRIX world1 = XMMatrixTranslation(80, 70, 0.0f);
    world1 = XMMatrixMultiply(world1, XMMatrixRotationY(PI / 6));
    AddTrianglesToScene(SceneTriangles, cornellBoxv, cornellBoxi, -1, (world1));


    EST::vector<Vertex>sphereV;
    EST::vector<std::uint16_t>sphereI;
    createSphere(sphereV, sphereI, 10, XMFLOAT3(0.6f, 0.6f, 0.6f));
    world = XMMatrixTranslation(40, 50, -30.0f);
    world = XMMatrixMultiply(world, XMMatrixRotationY(PI / 3));
    //AddTrianglesToScene(SceneTriangles, sphereV, sphereI, -1, (world));
    EST::vector<Vertex>skySphereV;
    EST::vector<std::uint16_t>skySphereI;
    createSphere(skySphereV, skySphereI, 2000, XMFLOAT3(0.6f, 0.6f, 0.6f));

    sahtree.init(SceneTriangles);
    currpasscb.boxNum = sahtree.boxNum;


    //包围盒顶点
    EST::vector<Vertex>boxesv;
    for (int i = 0;i < sahtree.leafBox.size();i++) {
        XMFLOAT3 c = sahtree.leafBox[i].Center;
        XMFLOAT3 e = sahtree.leafBox[i].Extents;
        Vertex v, v1, v2, v3, v4, v5, v6, v7;
        v.position = XMFLOAT4{ c.x - e.x,c.y - e.y,c.z - e.z,1.0f };
        boxesv.push_back(v);
        v1.position = XMFLOAT4{ c.x - e.x,c.y - e.y,c.z + e.z,1.0f };
        boxesv.push_back(v1);
        v2.position = XMFLOAT4{ c.x + e.x,c.y - e.y,c.z - e.z,1.0f };
        boxesv.push_back(v2);
        v3.position = XMFLOAT4{ c.x + e.x,c.y - e.y,c.z + e.z,1.0f };
        boxesv.push_back(v3);
        v4.position = XMFLOAT4{ c.x - e.x,c.y + e.y,c.z - e.z,1.0f };
        boxesv.push_back(v4);
        v5.position = XMFLOAT4{ c.x - e.x,c.y + e.y,c.z + e.z,1.0f };
        boxesv.push_back(v5);
        v6.position = XMFLOAT4{ c.x + e.x,c.y + e.y,c.z - e.z,1.0f };
        boxesv.push_back(v6);
        v7.position = XMFLOAT4{ c.x + e.x,c.y + e.y,c.z + e.z,1.0f };
        boxesv.push_back(v7);

    }
    EST::vector<std::uint16_t>bvhI;
    for (int i = 0;i < boxesv.size() / 8;i++) {
        bvhI.push_back(i * 8);
        bvhI.push_back(i * 8 + 1);
        bvhI.push_back(i * 8 + 1);
        bvhI.push_back(i * 8 + 3);
        bvhI.push_back(i * 8 + 3);
        bvhI.push_back(i * 8 + 2);
        bvhI.push_back(i * 8 + 2);
        bvhI.push_back(i * 8);
        bvhI.push_back(i * 8 + 4);
        bvhI.push_back(i * 8 + 5);
        bvhI.push_back(i * 8 + 5);
        bvhI.push_back(i * 8 + 7);

        bvhI.push_back(i * 8 + 7);
        bvhI.push_back(i * 8 + 6);
        bvhI.push_back(i * 8 + 6);
        bvhI.push_back(i * 8 + 4);
        bvhI.push_back(i * 8);
        bvhI.push_back(i * 8 + 4);
        bvhI.push_back(i * 8 + 1);
        bvhI.push_back(i * 8 + 5);
        bvhI.push_back(i * 8 + 2);
        bvhI.push_back(i * 8 + 6);
        bvhI.push_back(i * 8 + 3);
        bvhI.push_back(i * 8 + 7);

    }

    //创建几何项以及渲染项
    //几何项
    auto bunnyGeo = std::make_unique<GeometryItem>();
    bunnyGeo->createStaticGeo<Vertex>(pID3DDevice.Get(), pIcmdlistpre.Get(), &LPSheadVers, &LPSheadInds, &upBSTable, &defBSTable);
    GeometryItemTable["LPShead"] = std::move(bunnyGeo);
    auto planeGeo = std::make_unique<GeometryItem>();
    planeGeo->createStaticGeo<Vertex>(pID3DDevice.Get(), pIcmdlistpre.Get(), &planev, &planei, &upBSTable, &defBSTable);
    GeometryItemTable["plane"] = std::move(planeGeo);
    auto modelGeo = std::make_unique<GeometryItem>();
    modelGeo->createStaticGeo<modelVertex>(pID3DDevice.Get(), pIcmdlistpre.Get(), &mreader.mvertices, &mreader.mindices, &upBSTable, &defBSTable);
    GeometryItemTable["model"] = std::move(modelGeo);
    auto boxGeo = std::make_unique<GeometryItem>();
    boxGeo->createStaticGeo<Vertex>(pID3DDevice.Get(), pIcmdlistpre.Get(), &boxesv, &bvhI, &upBSTable, &defBSTable);
    GeometryItemTable["box"] = std::move(boxGeo);
    auto LightGeo = std::make_unique<GeometryItem>();
    LightGeo->createDynamicGeo<Vertex>(pID3DDevice.Get(), &lightv, &lighti, &upBSTable);
    GeometryItemTable["light"] = std::move(LightGeo);
    auto cornellBoxGeo = std::make_unique<GeometryItem>();
    cornellBoxGeo->createStaticGeo<Vertex>(pID3DDevice.Get(), pIcmdlistpre.Get(), &cornellBoxv, &cornellBoxi, &upBSTable, &defBSTable);
    GeometryItemTable["cornellBox"] = std::move(cornellBoxGeo);
    auto sphereGeo = std::make_unique<GeometryItem>();
    sphereGeo->createStaticGeo<Vertex>(pID3DDevice.Get(), pIcmdlistpre.Get(), &sphereV, &sphereI, &upBSTable, &defBSTable);
    GeometryItemTable["sphere"] = std::move(sphereGeo);
    auto skySphereGeo = std::make_unique<GeometryItem>();
    skySphereGeo->createStaticGeo<Vertex>(pID3DDevice.Get(), pIcmdlistpre.Get(), &skySphereV, &skySphereI, &upBSTable, &defBSTable);
    GeometryItemTable["skysphere"] = std::move(skySphereGeo);
    //auto head1Geo = std::make_unique<GeometryItem>();
    //head1Geo->createStaticGeo<Vertex>(pID3DDevice.Get(), pIcmdlistpre.Get(), &objRD1.vertices, &objRD1.indices, upBS2, defBS2);
    //GeometryItemTable["head1"] = std::move(head1Geo);
}
void FirstAPP::createResourceItem() {
    //创建纹理资源以及缓冲资源
     //创建资源项
    auto upBS = upBSTable["UPBS1"].get();
    auto defBS = defBSTable["DEFBS1"].get();

    passconstant passc;
    auto passBRI = std::make_unique<ConstantBufferResourceItem<passconstant>>(pID3DDevice.Get(), pIcmdlistpre.Get(), &passc, false, pIpresrvheap.Get(), upBS, defBS);
    BufferResourceItemTable["pass"] = std::move(passBRI);
    SSSS ssss;
    auto ssssri= std::make_unique<ConstantBufferResourceItem<SSSS>>(pID3DDevice.Get(), pIcmdlistpre.Get(), &ssss, false, pIpresrvheap.Get(), upBS, defBS);
    ssssRI = std::move(ssssri);
    lastVPmat lvp;
    lastvpmatRI= std::make_unique<ConstantBufferResourceItem<lastVPmat>>(pID3DDevice.Get(), pIcmdlistpre.Get(), &lvp, false, pIpresrvheap.Get(), upBS, defBS);
    std::unique_ptr<StructureBufferResourceItem<AABBbox>>boxRI;
    std::unique_ptr<StructureBufferResourceItem<triangle>>triangleRI;
    BuildBoxAndTriangleSBRI(&sahtree, boxRI, triangleRI, pID3DDevice.Get(), pIcmdlistpre.Get(), pIpresrvheap.Get(), upBS, defBS);
    boxTable["model"] = std::move(boxRI);
    triangleTable["model"] = std::move(triangleRI);
    std::unique_ptr<StructureBufferResourceItem<float>>randomNumberRI;
    EST::vector<float>randnums;
    GenerateRandomNum(randnums, 10000);
    randomNumberRI = std::make_unique<StructureBufferResourceItem<float>>(pID3DDevice.Get(), pIcmdlistpre.Get(), &randnums, true, pIpresrvheap.Get(), upBS, defBS, randnums.size());
    randomNumbers = std::move(randomNumberRI);
    std::unique_ptr<StructureBufferResourceItem<XMINT2>>offsetRI;
    EST::vector<XMINT2>offsets;
    GenerateOffsets(offsets);
    for (int i = 0;i < 5;i++) {
        EST::vector<XMINT2>off(offsets.Getdata() + i * 25, 25);
        offsetRI = std::make_unique<StructureBufferResourceItem<XMINT2>>(pID3DDevice.Get(), pIcmdlistpre.Get(), &off, true, pIpresrvheap.Get(), upBS, defBS, 25);
        offsetArray[i] = std::move(offsetRI);
    }
    std::unique_ptr<StructureBufferResourceItem<float>>hRI;
    EST::vector<float>h;
    h.push_back(1.0f/16.0f);
    h.push_back(1.0f/4.0f);
    h.push_back(3.0f/8.0f);
    h.push_back(1.0f/4.0f);
    h.push_back(1.0f/16.0f);
    hRI = std::make_unique<StructureBufferResourceItem<float>>(pID3DDevice.Get(), pIcmdlistpre.Get(), &h, true, pIpresrvheap.Get(), upBS, defBS,h.size() );
    hSBRI = std::move(hRI);

    std::unique_ptr<StructureBufferResourceItem<float>>gweightRI;
    EST::vector<float>gweights;//{0.006, 0.061, 0.242, 0.383, 0.242, 0.061, 0.006}.
    gweights.push_back(0.006f);
    gweights.push_back(0.061f);
    gweights.push_back(0.242f);
    gweights.push_back(0.383f);
    gweights.push_back(0.242f);
    gweights.push_back(0.061f);
    gweights.push_back(0.006f);
    gweightRI = std::make_unique<StructureBufferResourceItem<float>>(pID3DDevice.Get(), pIcmdlistpre.Get(), &gweights, true, pIpresrvheap.Get(), upBS, defBS, gweights.size());
    gweightSBRI = std::move(gweightRI);
    std::unique_ptr<StructureBufferResourceItem<PolygonalLight>>lightRI;
    l.area = 2500.0f;
    l.color = XMFLOAT3{ 1.0,1.0,1.0 };
    l.Xstart = 0;l.Xend = 50;
    l.Zstart = -55;l.Zend = -5;
    l.normal = XMFLOAT3{ 0,-1.0f,0 };
    EST::vector<PolygonalLight>ls(&l, 1);
    lightRI = std::make_unique<StructureBufferResourceItem<PolygonalLight>>(pID3DDevice.Get(), pIcmdlistpre.Get(), &ls, false, pIpresrvheap.Get(), upBS, defBS, 1);
    lights = std::move(lightRI);

    EST::vector<material>mats;
    material mat1;
    mat1.albedo = XMFLOAT3{ 0.6f,0,0 };
    mat1.F0 = XMFLOAT3{ 1.0,1.0,1.0 };
    mat1.roughness = 0.4f;
    mats.push_back(mat1);
    auto matRI = std::make_unique<StructureBufferResourceItem<material>>(pID3DDevice.Get(), pIcmdlistpre.Get(), &mats, false, pIpresrvheap.Get(), upBS, defBS, mats.size());
    materials = std::move(matRI);

    XMFLOAT3 gw[] = { {0.233,0.455,0.649},
        {0.1,0.336,0.344},
        {0.118,0.198,0},
        {0.113,0.007,0.007},
        {0.358,0.004,0},
        {0.078,0,0} };

    EST::vector<XMFLOAT3>Gweights;
    Gweights.addData(&gw[0], 6);
    auto GWsSBRI = std::make_unique<StructureBufferResourceItem<XMFLOAT3>>(pID3DDevice.Get(), pIcmdlistpre.Get(), &Gweights, false, pIpresrvheap.Get(), upBS, defBS, Gweights.size());
    guassianWeights = std::move(GWsSBRI);
    ParallelLight paraLights[] = {
        {XMFLOAT3{0, 0,1},XMFLOAT3{0.8,0.8,0.8}},
        {XMFLOAT3{-0.8, 0, 0.2},XMFLOAT3{0,0,0}},
        {XMFLOAT3{0, 0, -1},XMFLOAT3{0.5,0.5,0.5}},
        {XMFLOAT3{-0.8,0.8,0.5},XMFLOAT3{0.5,0.5,0.5}}
    };
    EST::vector<ParallelLight>paraLightsVec(paraLights, 4);
    auto ParaLightssbri = std::make_unique<StructureBufferResourceItem<ParallelLight>>(pID3DDevice.Get(), pIcmdlistpre.Get(), &paraLightsVec, false, pIpresrvheap.Get(), upBS, defBS, paraLightsVec.size());
    ParaLightsSBRI = std::move(ParaLightssbri);
    float ff[7] = { 0, 0.0064,0.0484,0.187,0.567,1.99,7.41 };
    GkernelSBRI = new  std::unique_ptr<StructureBufferResourceItem<float>>[6];
    for (int i = 0;i < 6;i++) {
        EST::vector<float>Gkernel = CalcGaussWeights((ff[i+1]-ff[i]),200.0f);
        auto Gkernelsbri = std::make_unique<StructureBufferResourceItem<float>>(pID3DDevice.Get(), pIcmdlistpre.Get(), &Gkernel, false, pIpresrvheap.Get(), upBS, defBS, Gkernel.size());
        GkernelSBRI[i] = std::move(Gkernelsbri);
    }
    EST::vector<XMFLOAT4>sssskernel = ssssComponent.calculateKernel(128);
    auto sssskernelsbri= std::make_unique<StructureBufferResourceItem<XMFLOAT4>>(pID3DDevice.Get(), pIcmdlistpre.Get(), &sssskernel, false, pIpresrvheap.Get(), upBS, defBS, sssskernel.size());
    SSSSkernelSBRI = std::move(sssskernelsbri);

    auto grassTRI = std::make_unique<TextureResourceItem>(pID3DDevice.Get(), pIpresrvheap.Get(), nullptr, nullptr, true,0);
    grassTRI->createStickerTex(pID3DDevice.Get(), pIcmdlistpre.Get(), L"Textures\\grass.dds");
    D3D12_SHADER_RESOURCE_VIEW_DESC SRVdesc = {};
    SRVdesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    SRVdesc.Texture2D.MostDetailedMip = 0;
    SRVdesc.Texture2D.ResourceMinLODClamp = 0.0f;
    SRVdesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    grassTRI->createSRVforResourceItem(&SRVdesc, pID3DDevice.Get(), HeapOffsetTable[grassTRI->SRVUAVHeap]);
    TextureResourceItemTable["grass"] = std::move(grassTRI);//众多纹理图

    auto brickTRI = std::make_unique<TextureResourceItem>(pID3DDevice.Get(), pIpresrvheap.Get(), nullptr, nullptr, true,1);
    brickTRI->createStickerTex(pID3DDevice.Get(), pIcmdlistpre.Get(), L"Textures\\bricks3.dds");
    brickTRI->createSRVforResourceItem(&SRVdesc, pID3DDevice.Get(), HeapOffsetTable[brickTRI->SRVUAVHeap]);
    TextureResourceItemTable["brick"] = std::move(brickTRI);

    auto cbTRI = std::make_unique<TextureResourceItem>(pID3DDevice.Get(), pIpresrvheap.Get(), nullptr, nullptr, true,2);
    cbTRI->createStickerTex(pID3DDevice.Get(), pIcmdlistpre.Get(), L"Textures\\checkboard.dds");
    cbTRI->createSRVforResourceItem(&SRVdesc, pID3DDevice.Get(), HeapOffsetTable[cbTRI->SRVUAVHeap]);
    TextureResourceItemTable["checkboard"] = std::move(cbTRI);

    auto modelRI = std::make_unique<TextureResourceItem>(pID3DDevice.Get(), pIpresrvheap.Get(), nullptr, nullptr, true,3);
    modelRI->createStickerTex(pID3DDevice.Get(), pIcmdlistpre.Get(), L"Textures\\head_diff.dds");
    modelRI->createSRVforResourceItem(&SRVdesc, pID3DDevice.Get(), HeapOffsetTable[modelRI->SRVUAVHeap]);
    TextureResourceItemTable["model0"] = std::move(modelRI);

    modelRI = std::make_unique<TextureResourceItem>(pID3DDevice.Get(), pIpresrvheap.Get(), nullptr, nullptr, true,4);
    modelRI->createStickerTex(pID3DDevice.Get(), pIcmdlistpre.Get(), L"Textures\\jacket_diff.dds");
    modelRI->createSRVforResourceItem(&SRVdesc, pID3DDevice.Get(), HeapOffsetTable[modelRI->SRVUAVHeap]);
    TextureResourceItemTable["model1"] = std::move(modelRI);

    modelRI = std::make_unique<TextureResourceItem>(pID3DDevice.Get(), pIpresrvheap.Get(), nullptr, nullptr, true,5);
    modelRI->createStickerTex(pID3DDevice.Get(), pIcmdlistpre.Get(), L"Textures\\pants_diff.dds");
    modelRI->createSRVforResourceItem(&SRVdesc, pID3DDevice.Get(), HeapOffsetTable[modelRI->SRVUAVHeap]);
    TextureResourceItemTable["model2"] = std::move(modelRI);

    modelRI = std::make_unique<TextureResourceItem>(pID3DDevice.Get(), pIpresrvheap.Get(), nullptr, nullptr, true,6);
    modelRI->createStickerTex(pID3DDevice.Get(), pIcmdlistpre.Get(), L"Textures\\upBody_diff.dds");
    modelRI->createSRVforResourceItem(&SRVdesc, pID3DDevice.Get(), HeapOffsetTable[modelRI->SRVUAVHeap]);
    TextureResourceItemTable["model3"] = std::move(modelRI);

    modelRI = std::make_unique<TextureResourceItem>(pID3DDevice.Get(), pIpresrvheap.Get(), nullptr, nullptr, true, 7);
    modelRI->createStickerTex(pID3DDevice.Get(), pIcmdlistpre.Get(), L"Textures\\head_diff.dds");
    modelRI->createSRVforResourceItem(&SRVdesc, pID3DDevice.Get(), HeapOffsetTable[modelRI->SRVUAVHeap]);
    TextureResourceItemTable["model4"] = std::move(modelRI);

    SRVdesc.Format = DXGI_FORMAT_BC1_UNORM;
    modelRI = std::make_unique<TextureResourceItem>(pID3DDevice.Get(), pIpresrvheap.Get(), nullptr, nullptr, true, 8);
    modelRI->createStickerTex(pID3DDevice.Get(), pIcmdlistpre.Get(), L"LPSHead\\lambertian.dds");
    modelRI->createSRVforResourceItem(&SRVdesc, pID3DDevice.Get(), HeapOffsetTable[modelRI->SRVUAVHeap]);
    TextureResourceItemTable["LPSHeadDiff"] = std::move(modelRI);

    SRVdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    SRVdesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
    SRVdesc.TextureCube.MostDetailedMip = 0;
    SRVdesc.TextureCube.ResourceMinLODClamp = 0.0f;
    auto skyTexRI = std::make_unique<TextureResourceItem>(pID3DDevice.Get(), pIpresrvheap.Get(), nullptr, nullptr, true, -1);
    skyTexRI->createStickerTex(pID3DDevice.Get(), pIcmdlistpre.Get(), L"Textures\\snowcube1024.dds");
    skyTexRI->createSRVforResourceItem(&SRVdesc, pID3DDevice.Get(), HeapOffsetTable[skyTexRI->SRVUAVHeap]);
    TextureResourceItemTable["snowcube"] = std::move(skyTexRI);
    SRVdesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    SRVdesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    auto headNormTexRI = std::make_unique<TextureResourceItem>(pID3DDevice.Get(), pIpresrvheap.Get(), nullptr, nullptr, true, -1);
    headNormTexRI->createStickerTex(pID3DDevice.Get(), pIcmdlistpre.Get(), L"Textures\\head_norm.dds");
    headNormTexRI->createSRVforResourceItem(&SRVdesc, pID3DDevice.Get(), HeapOffsetTable[headNormTexRI->SRVUAVHeap]);
    TextureResourceItemTable["headnorm"] = std::move(headNormTexRI);
    SRVdesc.Format = DXGI_FORMAT_BC1_UNORM;
    headNormTexRI = std::make_unique<TextureResourceItem>(pID3DDevice.Get(), pIpresrvheap.Get(), nullptr, nullptr, true, -1);
    headNormTexRI->createStickerTex(pID3DDevice.Get(), pIcmdlistpre.Get(), L"LPSHead\\normal.dds");
    headNormTexRI->createSRVforResourceItem(&SRVdesc, pID3DDevice.Get(), HeapOffsetTable[headNormTexRI->SRVUAVHeap]);
    TextureResourceItemTable["LPSHeadNorm"] = std::move(headNormTexRI);
    SRVdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    auto filteredRI=std::make_unique<TextureResourceItem>(pID3DDevice.Get(), pIpresrvheap.Get(), nullptr, nullptr,false, NULL);
    D3D12_RESOURCE_DESC filteredTexDesc = {};
    filteredTexDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    filteredTexDesc.Alignment = 0;
    filteredTexDesc.Width = iWidth;
    filteredTexDesc.Height = iHeight;
    filteredTexDesc.DepthOrArraySize = 1;
    filteredTexDesc.MipLevels = 1;
    filteredTexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    filteredTexDesc.SampleDesc.Count = 1;
    filteredTexDesc.SampleDesc.Quality = 0;
    filteredTexDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    filteredTexDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

    D3D12_UNORDERED_ACCESS_VIEW_DESC filteredTexUavDesc = {};
    filteredTexUavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
    filteredTexUavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    filteredTexUavDesc.Texture2D.MipSlice = 0;
    filteredTexUavDesc.Texture2D.PlaneSlice = 0;


    //TSSSS创建纹理的过程被封装到这里了
   // tssssComponent.createResource(&TextureResourceItemTable, pID3DDevice.Get(), pIpresrvheap.Get(),pIRTVHeap.Get(), pIcmdlistpre.Get(), &NONRTDSSFLTable,&RTDSSFLTable);

    //SSSS创建纹理的过程被封装到这里了
    ssssComponent.createResource(&TextureResourceItemTable, pID3DDevice.Get(), pIpresrvheap.Get(), pIRTVHeap.Get(), pIcmdlistpre.Get(), &NONRTDSSFLTable, &RTDSSFLTable,&BufferResourceItemTable,&upBSTable,&defBSTable);

    //天空球纹理
    //filteredTexDesc.DepthOrArraySize = 6;
    //filteredTexDesc.Width = 1024;
    //filteredTexDesc.Height = 1024;
    //filteredTexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    //SRVdesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
    //SRVdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    //filteredRI = std::make_unique<TextureResourceItem>(pID3DDevice.Get(), pIpresrvheap.Get(), nullptr, nullptr, false, NULL);
    //filteredRI->createNON_RT_DS_WritableTex(pID3DDevice.Get(), pIcmdlistpre.Get(), &filteredTexDesc, &NONRTDSSFLTable);
    //filteredRI->createSRVforResourceItem(&SRVdesc, pID3DDevice.Get(), HeapOffsetTable[filteredRI->SRVUAVHeap]);
    //TextureResourceItemTable["skycube"] = std::move(filteredRI);//skycube
    filteredTexDesc.Width = 1024;
    filteredTexDesc.Height = 768;
    filteredTexDesc.DepthOrArraySize = 1;
    SRVdesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    for (int i = 0;i < nFrameBackBufCount;i++) {
        auto backBufferRI = std::make_unique<TextureResourceItem>(pID3DDevice.Get(), pIpresrvheap.Get(),nullptr, nullptr, false, NULL);
        backBufferRI->setTextureToRI(pIARenderTargets[i]);
        backBufferRI->createSRVforResourceItem(&SRVdesc, pID3DDevice.Get(), HeapOffsetTable[backBufferRI->SRVUAVHeap]);
        TextureResourceItemTable["backBuffer" + std::to_string(i)] = std::move(backBufferRI);//为每个backBuffer创建SRV
    }

    //Gbuffer
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    rtvDesc.Texture2D.MipSlice = 0;
    rtvDesc.Texture2D.PlaneSlice = 0;
    filteredTexDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
    filteredTexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    SRVdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    filteredTexDesc.Width = 1024;
    filteredTexDesc.Height = 768;
    filteredTexDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    rtvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    SRVdesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    auto wposTexRI= std::make_unique<TextureResourceItem>(pID3DDevice.Get(), pIpresrvheap.Get(),pIRTVHeap.Get(), nullptr, false, NULL);
    wposTexRI->createRT_DS_WritableTex(pID3DDevice.Get(), pIcmdlistpre.Get(), &filteredTexDesc, nullptr, RTDSSFLTable["RTDS"].get());
    wposTexRI->createRTVforResourceItem(&rtvDesc, pID3DDevice.Get(), HeapOffsetTable[pIRTVHeap.Get()], pIcmdlistpre.Get());
    wposTexRI->createSRVforResourceItem(&SRVdesc, pID3DDevice.Get(), HeapOffsetTable[pIpresrvheap.Get()]);
    TextureResourceItemTable["depth"] = std::move(wposTexRI);//z图
    filteredTexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    SRVdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    auto normalTexRI = std::make_unique<TextureResourceItem>(pID3DDevice.Get(), pIpresrvheap.Get(), pIRTVHeap.Get(), nullptr, false, NULL);
    normalTexRI->createRT_DS_WritableTex(pID3DDevice.Get(), pIcmdlistpre.Get(), &filteredTexDesc, nullptr, RTDSSFLTable["RTDS1"].get());
    normalTexRI->createRTVforResourceItem(&rtvDesc, pID3DDevice.Get(), HeapOffsetTable[pIRTVHeap.Get()], pIcmdlistpre.Get());
    normalTexRI->createSRVforResourceItem(&SRVdesc, pID3DDevice.Get(), HeapOffsetTable[pIpresrvheap.Get()]);
    TextureResourceItemTable["normal"] = std::move(normalTexRI);//normal图

    wposTexRI = std::make_unique<TextureResourceItem>(pID3DDevice.Get(), pIpresrvheap.Get(), pIRTVHeap.Get(), nullptr, false, NULL);
    wposTexRI->createRT_DS_WritableTex(pID3DDevice.Get(), pIcmdlistpre.Get(), &filteredTexDesc, nullptr, RTDSSFLTable["RTDS"].get());
    wposTexRI->createRTVforResourceItem(&rtvDesc, pID3DDevice.Get(), HeapOffsetTable[pIRTVHeap.Get()], pIcmdlistpre.Get());
    wposTexRI->createSRVforResourceItem(&SRVdesc, pID3DDevice.Get(), HeapOffsetTable[pIpresrvheap.Get()]);
    TextureResourceItemTable["matte"] = std::move(wposTexRI);//matte图

    filteredTexDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    rtvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    SRVdesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
     normalTexRI = std::make_unique<TextureResourceItem>(pID3DDevice.Get(), pIpresrvheap.Get(), pIRTVHeap.Get(), nullptr, false, NULL);
    normalTexRI->createRT_DS_WritableTex(pID3DDevice.Get(), pIcmdlistpre.Get(), &filteredTexDesc, nullptr, RTDSSFLTable["RTDS2"].get());
    normalTexRI->createRTVforResourceItem(&rtvDesc, pID3DDevice.Get(), HeapOffsetTable[pIRTVHeap.Get()], pIcmdlistpre.Get());
    normalTexRI->createSRVforResourceItem(&SRVdesc, pID3DDevice.Get(), HeapOffsetTable[pIpresrvheap.Get()]);
    TextureResourceItemTable["wpos"] = std::move(normalTexRI);//wpos图
    filteredTexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    SRVdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    normalTexRI = std::make_unique<TextureResourceItem>(pID3DDevice.Get(), pIpresrvheap.Get(), pIRTVHeap.Get(), nullptr, false, NULL);
    normalTexRI->createRT_DS_WritableTex(pID3DDevice.Get(), pIcmdlistpre.Get(), &filteredTexDesc, nullptr, RTDSSFLTable["RTDS"].get());
    normalTexRI->createRTVforResourceItem(&rtvDesc, pID3DDevice.Get(), HeapOffsetTable[pIRTVHeap.Get()], pIcmdlistpre.Get());
    normalTexRI->createSRVforResourceItem(&SRVdesc, pID3DDevice.Get(), HeapOffsetTable[pIpresrvheap.Get()]);
    TextureResourceItemTable["gz"] = std::move(normalTexRI);//gz图

   
}
void FirstAPP::createRenderItem() {
    auto upBS = upBSTable["UPBS3"].get();
    auto defBS = defBSTable["DEFBS3"].get();
    //渲染项
    objectconstant objc;
    XMMATRIX world = XMMatrixTranslation(0, 0, 0);
    XMStoreFloat4x4(&objc.world, XMMatrixTranspose(world));
    XMStoreFloat4x4(&objc.invTworld, (XMMatrixInverse( nullptr,world)));
    objc.texIndex = -1;
    auto Geo = GeometryItemTable["plane"].get();
    //auto planeRi = std::make_unique<RenderItem>(Geo, 1, 0, 0, Geo->indexNum, 0, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, &objc, pID3DDevice.Get(), pIcmdlistpre.Get(), pIpresrvheap.Get(), upBS, defBS);
    //NonSVGFRenderItemTable["plane0"] = std::move(planeRi);
    //world = XMMatrixTranslation(20, 70, 0.0f);
    //world = XMMatrixMultiply(world, XMMatrixRotationY(PI/3));
    //XMStoreFloat4x4(&objc.world, XMMatrixTranspose(world));
    //XMStoreFloat4x4(&objc.invTworld, (XMMatrixInverse(nullptr, world)));
    //Geo = GeometryItemTable["cornellBox"].get();
    //auto cornellBoxRi = std::make_unique<RenderItem>(Geo, 1, 0, 0, Geo->indexNum, 0, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, &objc, pID3DDevice.Get(), pIcmdlistpre.Get(), pIpresrvheap.Get(), upBS, defBS);
    //NonSVGFRenderItemTable["cornellBox"] = std::move(cornellBoxRi);
    //world = XMMatrixTranslation(80, 70, 0.0f);
    //world = XMMatrixMultiply(world, XMMatrixRotationY(PI / 6));
    //XMStoreFloat4x4(&objc.world, XMMatrixTranspose(world));
    //XMStoreFloat4x4(&objc.invTworld, (XMMatrixInverse(nullptr, world)));
    //Geo = GeometryItemTable["cornellBox"].get();
    //cornellBoxRi = std::make_unique<RenderItem>(Geo, 1, 0, 0, Geo->indexNum, 0, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, &objc, pID3DDevice.Get(), pIcmdlistpre.Get(), pIpresrvheap.Get(), upBS, defBS);
    //NonSVGFRenderItemTable["cornellBox1"] = std::move(cornellBoxRi);
    //XMStoreFloat4x4(&objc.world, XMMatrixTranspose(XMMatrixTranslation(114, 0, 0)));
    //objc.texIndex = TextureResourceItemTable["brick"].get()->TextureIndex;
    //planeRi = std::make_unique<RenderItem>(Geo, 1, 0, 0, Geo->indexNum, 0, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, &objc, pID3DDevice.Get(), pIcmdlistpre.Get(), pIpresrvheap.Get(), upBS, defBS);
    //RenderItemTable["plane1"] = std::move(planeRi);
    //objc.texIndex = TextureResourceItemTable["checkboard"].get()->TextureIndex;
    //XMStoreFloat4x4(&objc.world, XMMatrixTranspose(XMMatrixTranslation(228, 0, 0)));
    //planeRi = std::make_unique<RenderItem>(Geo, 1, 0, 0, Geo->indexNum, 0, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, &objc, pID3DDevice.Get(), pIcmdlistpre.Get(), pIpresrvheap.Get(), upBS, defBS);
    //RenderItemTable["plane2"] = std::move(planeRi);
    //objc.texIndex = TextureResourceItemTable["grass"].get()->TextureIndex;
    //XMStoreFloat4x4(&objc.world, XMMatrixTranspose(XMMatrixTranslation(342, 0, 0)));
    //planeRi = std::make_unique<RenderItem>(Geo, 1, 0, 0, Geo->indexNum, 0, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, &objc, pID3DDevice.Get(), pIcmdlistpre.Get(), pIpresrvheap.Get(), upBS, defBS);
    //RenderItemTable["plane3"] = std::move(planeRi);

  
    //int istarr[5] = { 0,7230 * 3,11679 * 3,18258 * 3,22065 * 3 };
    //UINT isizearr[5] = { 7230 * 3 ,4449 * 3 ,6579 * 3 ,3807 * 3 ,442 * 3 };
    //Geo = GeometryItemTable["model"].get();
    //for (int i = 0;i < 5;i++) {
    //    XMMATRIX w = XMMatrixTranslation(0, -67, 0);
    //    world = XMMatrixTranslation(0+80*i, 0, 0.0f);
    //    XMMATRIX s= XMMatrixScaling( 10, 10, 10);
    //    XMMATRIX m = XMMatrixMultiply(w, s);
    //    m = XMMatrixMultiply(m, world);
    //    XMStoreFloat4x4(&objc.world, XMMatrixTranspose(m));
    //    XMStoreFloat4x4(&objc.invTworld, (XMMatrixInverse(nullptr, m)));
    //    std::string name = "model";
    //    objc.texIndex= TextureResourceItemTable[name+std::to_string(0)].get()->TextureIndex;
    //    auto modelRi = std::make_unique<RenderItem>(Geo, 1, 0, istarr[0], isizearr[0], 0, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, &objc, pID3DDevice.Get(), pIcmdlistpre.Get(), pIpresrvheap.Get(), upBS, defBS);
    //    NonSVGFRenderItemTable[name+std::to_string(i)] = std::move(modelRi);
    //}
    //XMMATRIX w = XMMatrixTranslation(0, -67, 0);
    //world = XMMatrixTranslation(0 + 80 * 5, 0, 0.0f);
    //XMMATRIX s = XMMatrixScaling(10, 10, 10);
    //XMMATRIX m = XMMatrixMultiply(w, s);
    //m = XMMatrixMultiply(m, world);
    //XMStoreFloat4x4(&objc.world, XMMatrixTranspose(m));
    //XMStoreFloat4x4(&objc.invTworld, (XMMatrixInverse(nullptr, m)));
    //objc.texIndex = -1;
    //auto head1RI = std::make_unique<RenderItem>(Geo, 1, 0,0,Geo->indexNum, 0, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, &objc, pID3DDevice.Get(), pIcmdlistpre.Get(), pIpresrvheap.Get(), upBS, defBS, nullptr);
    //NonSVGFRenderItemTable["head1"] = std::move(head1RI);

    //world = XMMatrixTranslation(0, 0, 0.0f);
    //XMStoreFloat4x4(&objc.world, XMMatrixTranspose(world));
    //XMStoreFloat4x4(&objc.invTworld, (XMMatrixInverse(nullptr, world)));
    //Geo = GeometryItemTable["box"].get();
    //auto boxRi = std::make_unique<RenderItem>(Geo, 1, 0, 0, Geo->indexNum, 0, D3D_PRIMITIVE_TOPOLOGY_LINELIST,&objc, pID3DDevice.Get(), pIcmdlistpre.Get(), pIpresrvheap.Get(), upBS, defBS);
    //RenderItemTable["box"] = std::move(boxRi);
    //world = XMMatrixTranslation(0, 0, 0.0f);
    //XMStoreFloat4x4(&objc.world, XMMatrixTranspose(world));
    //XMStoreFloat4x4(&objc.invTworld, (XMMatrixInverse(nullptr, world)));
    //Geo = GeometryItemTable["light"].get();
    //auto lightRI = std::make_unique<RenderItem>(Geo, 1, 0, 0, Geo->indexNum, 0, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, &objc, pID3DDevice.Get(), pIcmdlistpre.Get(), pIpresrvheap.Get(), upBS, defBS);
    //NonSVGFRenderItemTable["light"] = std::move(lightRI);

    //world = XMMatrixTranslation(50, 40, -10.0f);
    //XMStoreFloat4x4(&objc.world, XMMatrixTranspose(world));
    //XMStoreFloat4x4(&objc.invTworld, (XMMatrixInverse(nullptr, world)));
    //Geo = GeometryItemTable["sphere"].get();
    //auto sphereRI = std::make_unique<RenderItem>(Geo, 1, 0, 0, Geo->indexNum, 0, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, &objc, pID3DDevice.Get(), pIcmdlistpre.Get(), pIpresrvheap.Get(), upBS, defBS);
    //NonSVGFRenderItemTable["sphere"] = std::move(sphereRI);

    world = XMMatrixTranslation(40, 0, -50.0f);
    XMMATRIX s= XMMatrixScaling( 5, 5, 5);
    world = XMMatrixMultiply(s, world);
    XMStoreFloat4x4(&objc.world, XMMatrixTranspose(world));
    XMStoreFloat4x4(&objc.invTworld, (XMMatrixInverse(nullptr, world)));
    objc.texIndex = TextureResourceItemTable["LPSHeadDiff"].get()->TextureIndex;
    Geo = GeometryItemTable["LPShead"].get();
    auto bunnyRI = std::make_unique<RenderItem>(Geo, 1, 0, 0, Geo->indexNum, 0, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, &objc, pID3DDevice.Get(), pIcmdlistpre.Get(), pIpresrvheap.Get(), upBS, defBS);
    NonSVGFRenderItemTable["LPShead"] = std::move(bunnyRI);

    world = XMMatrixTranslation(130, 0, -50.0f);
     s = XMMatrixScaling(5, 5, 5);
    world = XMMatrixMultiply(s, world);
    XMStoreFloat4x4(&objc.world, XMMatrixTranspose(world));
    XMStoreFloat4x4(&objc.invTworld, (XMMatrixInverse(nullptr, world)));
    objc.texIndex = TextureResourceItemTable["LPSHeadDiff"].get()->TextureIndex;
    Geo = GeometryItemTable["LPShead"].get();
     bunnyRI = std::make_unique<RenderItem>(Geo, 1, 0, 0, Geo->indexNum, 0, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, &objc, pID3DDevice.Get(), pIcmdlistpre.Get(), pIpresrvheap.Get(), upBS, defBS);
    NonSVGFRenderItemTable["LPShead1"] = std::move(bunnyRI);

    //world = XMMatrixTranslation(0, 0, 0);
    //XMStoreFloat4x4(&objc.world, XMMatrixTranspose(world));
    //XMStoreFloat4x4(&objc.invTworld, (XMMatrixInverse(nullptr, world)));
    //Geo = GeometryItemTable["skysphere"].get();
    //auto skyRI = std::make_unique<RenderItem>(Geo, 1, 0, 0, Geo->indexNum, 0, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, &objc, pID3DDevice.Get(), pIcmdlistpre.Get(), pIpresrvheap.Get(), upBS, defBS);
    //NonSVGFRenderItemTable["sky"] = std::move(skyRI);
}

int FirstAPP::createNewThreadToRender(std::string RenderItemName, std::string BufferResourceItemName, std::string TextureResourceItemName, std::string RootSignatureItemName, std::string SamplerResourceItemItemName, std::string DirectRayTracerPSOName,std::string IndirectRayTracerPSOName, _beginthreadex_proc_type threadName, int order) {
    //order为当前创建线程在本次创建的多个线程中的序号（从0开始）
    int i;
    int dtlsize = deadThreadList.size();
    if(order+1>dtlsize)
        i = ThreadNum;
    else {
        i = deadThreadList[dtlsize - 1 - order];
    }
    if (threadName == SVGFThreadfunc) {
        SVGFRenderItemTable[RenderItemName].get()->threadNote = i;
        hasSVGF = true;
    }//为渲染项赋值它的线程标记，方便删除对应线程
    else
        NonSVGFRenderItemTable[RenderItemName].get()->threadNote = i;
    HandleMessage->clear();
    //reserve会改变内存地址，resize过程可能会调用reserve，所以
    paras[i].RenderItemName = RenderItemName;
    paras[i].BufferResourceItemName = BufferResourceItemName;
    paras[i].TextureResourceItemName = TextureResourceItemName;
    paras[i].RootSignatureItemName = RootSignatureItemName;
    paras[i].SamplerResourceItemItemName = SamplerResourceItemItemName;
    paras[i].DirectRayTracerPSOName = DirectRayTracerPSOName;
    paras[i].IndirectRayTracerPSOName = IndirectRayTracerPSOName;
    paras[i].stViewPort = &stViewPort;
    paras[i].stScissorRect = &stScissorRect;
    paras[i].rs = pIRootSignature.Get();
    paras[i].device = pID3DDevice.Get();
    paras[i].dwMainThreadID = ::GetCurrentThreadId();
    paras[i].hMainThread = ::GetCurrentThread();
    paras[i].hEventRenderOver = ::CreateEvent(NULL, FALSE, NULL, FALSE);
    paras[i].hRunEvent = ::CreateEvent(NULL, FALSE, NULL, FALSE);
    ThrowIfFailed(pID3DDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&paras[i].cmdalloc)));
    ThrowIfFailed(pID3DDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, paras[i].cmdalloc, nullptr, IID_PPV_ARGS(&paras[i].cmdlist)));
    paras[i].cmdlist->Close();
    paras[i].nIndex = i;
    paras[i].SVGFRenderItemTable = &SVGFRenderItemTable;
    paras[i].NonSVGFRenderItemTable = &NonSVGFRenderItemTable;
    paras[i].GeometryItemTable = &GeometryItemTable;
    paras[i].TextureResourceItemTable = &TextureResourceItemTable;
    paras[i].BufferResourceItemTable = &BufferResourceItemTable;
    paras[i].SamplerResourceItemTable = &SamplerResourceItemTable;
    paras[i].RootSignatureItemTable = &RootSignatureItemTable;
    paras[i].ParaLightsSBRI = &ParaLightsSBRI;
    paras[i].materials = &materials;
    paras[i].sumGweights = &guassianWeights;
    paras[i].boxTable = &boxTable;
    paras[i].triangleTable = &triangleTable;
    paras[i].randomNumbers = &randomNumbers;
    paras[i].GkernelSBRI = &GkernelSBRI;
    paras[i].lights = &lights;
    paras[i].PSOITable = &PSOITable;
    paras[i].nCurrentFrameIndex = &nFrameIndex;
    paras[i].RTVHeap = pIRTVHeap.Get();
    for (int ii = 0;ii < nFrameBackBufCount;ii++)
        paras[i].pIARenderTargets[ii] = pIARenderTargets[ii].Get();
    paras[i].hThisThread = (HANDLE)_beginthreadex(nullptr, 0, threadName, (void*)&paras[i], CREATE_SUSPENDED, (UINT*)&(paras[i].dwThisThreadID));
    HandleMessage->push_back(paras[i].hEventRenderOver);
    existThreadList.push_back(i);
    ThreadNum++;
    return i;
}
void FirstAPP::startAndWaitNewThread(int newThreadNum) {
    int deadNum = 0;
    int bornNum = 0;
    for (int i =0;i <newThreadNum;i++) {
        int threadNoteToStart;
        if (!deadThreadList.empty()) {//死亡列表非空所以复活死亡列表中线程
            deadNum++;
            threadNoteToStart = deadThreadList.back();
            deadThreadList.pop_back();
        }
        else {//没有死亡线程就创建新的线程
            threadNoteToStart = ThreadNum - newThreadNum + deadNum + bornNum;
            bornNum++;
        }
        ::ResumeThread(paras[threadNoteToStart].hThisThread);//线程~~启动！！
    }
    //等待线程初始化完成
    ::MsgWaitForMultipleObjects(HandleMessage->size(), HandleMessage->Getdata(), TRUE, INFINITE, QS_ALLINPUT);
}
void FirstAPP::killThread(int threadNote) {
    existThreadList.erase(existThreadList.getbegin(), existThreadList.getend(),threadNote);
    deadThreadList.push_back(threadNote);
    ::TerminateThread(
        paras[threadNote].hThisThread,NULL
    );
    ThreadNum--;
    //置零paras[threadNote]
}
void FirstAPP::startSubThread() {
    //线程初始化传参并启动
    HandleMessage->clear();
    paras.resize(20);
    createNewThreadToRender("LPShead", "pass", "model3", "default", "default", "irradianceOnTheFly", "glossyRaytraceIndirect", NonSVGFThreadfunc, 0);
    createNewThreadToRender("LPShead1", "pass", "model3", "default", "default", "irradianceOnTheFly", "glossyRaytraceIndirect", NonSVGFThreadfunc, 1);
    //createNewThreadToRender("model4", "pass", "model3", "default", "default", "skinspecular", "glossyRaytraceIndirect", NonSVGFThreadfunc, 4);
    //createNewThreadToRender("bunny", "pass", "model4", "default", "default", "skinspecular", "glossyRaytraceIndirect", NonSVGFThreadfunc, 3);
    //createNewThreadToRender("model4", "pass", "grass", "default", "default", "raytrace",3);
    //createNewThreadToRender("light", "pass", "grass", "default", "default", "color", "diffuseRaytraceIndirect", NonSVGFThreadfunc, 0);
    //createNewThreadToRender("cornellBox", "pass", "grass", "default", "default", "translucentRaytraceDirect", "glossyRaytraceIndirect", NonSVGFThreadfunc,1);
    //createNewThreadToRender("cornellBox1", "pass", "grass", "default", "default", "translucentRaytraceDirect", "glossyRaytraceIndirect", NonSVGFThreadfunc,2);
    //createNewThreadToRender("sphere", "pass", "grass", "default", "default", "translucentRaytraceDirect", "glossyRaytraceIndirect", NonSVGFThreadfunc,3);
    //createNewThreadToRender("box", "pass", "grass", "default", "default", "diffuseRaytraceDirect", "diffuseRaytraceIndirect", 4);
    //createNewThreadToRender("sphere", "pass", "grass", "default", "default", "glossyRaytraceDirect", "glossyRaytraceIndirect",4);
    //createNewThreadToRender("sky", "pass", "grass", "default", "default", "sky", "glossyRaytraceIndirect", NonSVGFThreadfunc,2);
    //createNewThreadToRender("head1", "pass", "grass", "default", "default", "color", "glossyRaytraceIndirect", NonSVGFThreadfunc,5);
    //createNewThreadToRender("plane0", "pass", "checkboard", "default", "default", "diffuseRaytraceDirect", "diffuseRaytraceIndirect", NonSVGFThreadfunc, 1);
    startAndWaitNewThread(2);
}
void FirstAPP::startRenderLoop() {
    try {

        {
            ThrowIfFailed(pID3DDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pIFence)));
            n64FenceValue = 1;
            hFenceEvent = CreateEvent(nullptr, false, false, nullptr);
            if (hFenceEvent == nullptr) {
                ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
            }
        }
        DWORD dwRet = 0;
        BOOL bExit = FALSE;
        UINT nStates = 0; //初始状态为0
        UINT n64fence = 0;
        ThrowIfFailed(pIcmdlistpre->Close());
        //处理上传纹理以及buffer的指令
        cmdlists.push_back(pIcmdlistpre.Get());
        pICommandQueue->ExecuteCommandLists(cmdlists.size(), cmdlists.Getdata());
        n64fence = n64FenceValue;
        n64FenceValue++;
        pICommandQueue->Signal(pIFence.Get(), n64fence);
        pIFence->SetEventOnCompletion(n64fence, hFenceEvent);
        HandleMessage->clear();
        HandleMessage->push_back(hFenceEvent);
        SetTimer(hWnd, WM_USER + 100, 1, nullptr);//waitfor需要消息来停止阻塞
        //主渲染循环
        while (!bExit)
        {
            if (nStates == 2) {//渲染完一帧再处理消息,不然一个state处理一次就会卡顿
                while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))//用if会卡顿
                {
                    if (WM_QUIT != msg.message)
                    {
                        ::TranslateMessage(&msg);
                        ::DispatchMessage(&msg);
                    }
                    else
                    {
                        bExit = TRUE;
                    }
                }
            }
            dwRet = ::MsgWaitForMultipleObjects(HandleMessage->size(), HandleMessage->Getdata(), TRUE, INFINITE, QS_ALLINPUT);//返回接受到事件在事件数组中索引,否则阻塞
            UINT a = (UINT)(dwRet - WAIT_OBJECT_0);
            if ((a) == 0)
            {
                switch (nStates) {
                case 0:
                {
                    float x = cos(theta) * sin(phi);
                    float z = sin(theta) * sin(phi);
                    float y = cos(phi);
                    XMMATRIX Viewmat = XMMatrixLookAtLH(eyepos, eyepos + XMVectorSet(x, y, z, 0), up);
                    XMMATRIX projmat = XMMatrixPerspectiveFovLH(XM_PIDIV2, (FLOAT)1 / (FLOAT)1, 0.1f, 1000.0f);
                    XMMATRIX vpmat = XMMatrixMultiply(Viewmat, projmat);
                    XMMATRIX invP = XMMatrixInverse(nullptr, projmat);
                    XMStoreFloat3(&currpasscb.eyepos, eyepos);
                    XMStoreFloat4x4(&currpasscb.V, XMMatrixTranspose(Viewmat));
                    XMStoreFloat4x4(&currpasscb.VP, XMMatrixTranspose(vpmat));
                    XMStoreFloat4x4(&currpasscb.P, XMMatrixTranspose(projmat));
                    BufferResourceItemTable["pass"]->updateCB(&currpasscb);
                    lastVPmat lvp;
                    lvp.lastVP =XMMatrixTranspose( vpmat);
                    lvp.nframe = nFrame;
                    lastvpmatRI->updateCB(&lvp);
                    ThrowIfFailed(pIcmdlistpre->Reset(pIcmdallpre.Get(), nullptr));

                    //TSSSS的预处理被封装到这个函数里了
      /*              tssssComponent.preProcess(&SamplerResourceItemTable, &RootSignatureItemTable, pIcmdlistpre.Get(), &computePSOITable, &PSOITable,
                        &stViewPort4096, &stScissorRect4096, pIpresamheap.Get(), &NonSVGFRenderItemTable, &BufferResourceItemTable, GkernelSBRI);*/

                    //预处理pass渲染（整个程序只需要渲染一次）
      

                    //处理指令
                    pIcmdlistpre->Close();
                    nStates = 1;
                    cmdlists.clear();
                    cmdlists.push_back(pIcmdlistpre.Get());
                    pICommandQueue->ExecuteCommandLists(cmdlists.size(), cmdlists.Getdata());
                    n64fence = n64FenceValue;
                    n64FenceValue++;
                    pICommandQueue->Signal(pIFence.Get(), n64fence);
                    pIFence->SetEventOnCompletion(n64fence, hFenceEvent);
                    nStates = 1;
                    HandleMessage->clear();
                    HandleMessage->push_back(hFenceEvent);

                }
                break;

                case 1://子线程和gpu都加载完了，现在就剩下让子线程开始绘制
                {

                    nFrame++;
                    float x = cos(theta) * sin(phi);
                    float z = sin(theta) * sin(phi);
                    float y = cos(phi);
                    nFrameIndex = pISwapChain3->GetCurrentBackBufferIndex();
                    currenttime = ::GetTickCount();
                    currpasscb.curTime = (UINT)currenttime;
                    rotationAngle += (currenttime - starttime) * 0.003f;
                    float k = (currenttime - starttime) * 0.001;
                    timesecond += k;
                    if (timesecond >= 1.25f)
                        timesecond = 0;

                    starttime = currenttime;
                    XMVECTOR right = XMVector3Cross(up, XMVectorSet(x, y, z, 0)) * 10.0f;
                    XMVECTOR front;///////////////////////////

                    if (GetAsyncKeyState('W') & 0x8000)
                    {
                        WriteConsole(g_hOutput, "w", 2, NULL, NULL);
                        eyepos = eyepos + XMVectorSet(x, y, z, 0) * 5;

                    }
                    if (GetAsyncKeyState('A') & 0x8000)
                    {
                        WriteConsole(g_hOutput, "a", 2, NULL, NULL);
                        eyepos = eyepos - right * k*7;

                    }
                    if (GetAsyncKeyState('S') & 0x8000)
                    {
                        WriteConsole(g_hOutput, "s", 2, NULL, NULL);
                        eyepos = eyepos - XMVectorSet(x, y, z, 0) * 5;
                    }
                    if (GetAsyncKeyState('D') & 0x8000)
                    {
                        WriteConsole(g_hOutput, "d", 2, NULL, NULL);
                        eyepos = eyepos + right * k*7;

                    }
                    if (GetAsyncKeyState(VK_LEFT) & 0x8000)
                    {
                        WriteConsole(g_hOutput, "d", 2, NULL, NULL);
                        l.Xstart -= 3.0f;
                        l.Xend -= 3.0f;
                    }
                    if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
                    {
                        WriteConsole(g_hOutput, "d", 2, NULL, NULL);
                        l.Xstart += 3.0f;
                        l.Xend += 3.0f;
                    }
                    if (GetAsyncKeyState(VK_UP) & 0x8000)
                    {
                        WriteConsole(g_hOutput, "d", 2, NULL, NULL);
                        l.Zstart += 3.0f;
                        l.Zend += 3.0f;
                    }
                    if (GetAsyncKeyState(VK_DOWN) & 0x8000)
                    {
                        WriteConsole(g_hOutput, "d", 2, NULL, NULL);
                        l.Zstart -= 3.0f;
                        l.Zend -= 3.0f;
                    }
                    if (GetAsyncKeyState('Z') & 0x8000)
                    {
                        WriteConsole(g_hOutput, "z", 2, NULL, NULL);
                        scaleGkernel += 1.0f;
                    }
                    if (GetAsyncKeyState('X') & 0x8000)
                    {
                        WriteConsole(g_hOutput, "x", 2, NULL, NULL);
                        scaleGkernel -= 1.0f;
                        scaleGkernel = max(1.0, scaleGkernel);
                    }
                    if (GetAsyncKeyState('1') & 0x8000)
                    {
                        WriteConsole(g_hOutput, "1", 2, NULL, NULL);
                        SSalpha += 1.0f;
                    }
                    if (GetAsyncKeyState('2') & 0x8000)
                    {
                        WriteConsole(g_hOutput, "2", 2, NULL, NULL);
                        SSalpha -= 1.0f;
                        SSalpha = max(1.0, SSalpha);
                    }
                    if (GetAsyncKeyState('3') & 0x8000)
                    {
                        WriteConsole(g_hOutput, "3", 2, NULL, NULL);
                        SSbeta += 10.0f;
                    }
                    if (GetAsyncKeyState('4') & 0x8000)
                    {
                        WriteConsole(g_hOutput, "4", 2, NULL, NULL);
                        SSbeta -= 10.0f;
                        SSbeta = max(1.0, SSbeta);
                    }
                    if (GetAsyncKeyState('L') & 0x8000)
                    {
                        WriteConsole(g_hOutput, "i", 2, NULL, NULL);
                        paraLtheta += 0.2f;
                        paraLtheta = min(2 * PI, paraLtheta);
                    }
                    if (GetAsyncKeyState('I') & 0x8000)
                    {
                        WriteConsole(g_hOutput, "j", 2, NULL, NULL);
                        paraLphi += 0.1f;
                        paraLphi = min(PI, paraLphi);
                    }
                    if (GetAsyncKeyState('J') & 0x8000)
                    {
                        WriteConsole(g_hOutput, "k", 2, NULL, NULL);
                        paraLtheta -= 0.2f;
                        paraLtheta = max(0, paraLtheta);
                    }
                    if (GetAsyncKeyState('K') & 0x8000)
                    {
                        WriteConsole(g_hOutput, "l", 2, NULL, NULL);
                        paraLphi -= 0.1f;
                        paraLphi = max(0, paraLphi);
                    }
                    if (GetAsyncKeyState('5') & 0x8000)
                    {
                        WriteConsole(g_hOutput, "5", 2, NULL, NULL);
                        roughness1 -= 0.005f;
                        roughness1 = max(0, roughness1);
                    }
                    if (GetAsyncKeyState('6') & 0x8000)
                    {
                        WriteConsole(g_hOutput, "6", 2, NULL, NULL);
                        roughness1 += 0.005f;
                        roughness1 = min(1.0f, roughness1);
                    }
                    if (rotationAngle >= XM_2PI)
                        rotationAngle = fmod(rotationAngle, XM_2PI);
                    XMMATRIX Viewmat = XMMatrixLookAtLH(eyepos, eyepos + XMVectorSet(x, y, z, 0), up);
                    XMMATRIX projmat = XMMatrixPerspectiveFovLH(XM_PIDIV2, (FLOAT)1 / (FLOAT)1, 0.1f, 1000.0f);
                    XMMATRIX vpmat = XMMatrixMultiply(Viewmat, projmat);
                    lastvpmat.lastVP = XMMatrixTranspose( vpmat);
                    lastvpmat.last6VP = XMMatrixTranspose(last6FramesVP[(nFrame) % 6]);
                    last6FramesVP[(nFrame - 1) % 6] = vpmat;
                    lastvpmat.nframe = nFrame;
                    XMMATRIX invP = XMMatrixInverse(nullptr, projmat);
                    XMStoreFloat3(&currpasscb.eyepos, eyepos);
                    XMStoreFloat4x4(&currpasscb.V, XMMatrixTranspose(Viewmat));
                    XMStoreFloat4x4(&currpasscb.VP, XMMatrixTranspose(vpmat));
                    XMStoreFloat4x4(&currpasscb.P, XMMatrixTranspose(projmat));
                    currpasscb.lightdir = XMFLOAT3{ 1.0f,1.0f,.0f };
                    currpasscb.AL = XMFLOAT3{ 50.5f,50.5f,50.5f };
                    currpasscb.BL = XMFLOAT3{ 1.0f,1.0f,1.0f };
                    currpasscb.lightdir = XMFLOAT3(1.0f, 1.0f, 0);
                    currpasscb.nFrame = nFrame;
                    //每帧都变化的CBmemcpy

                    BufferResourceItemTable["pass"]->updateCB(&currpasscb);
                    SSSS ssss;
                    ssss.scale = scaleGkernel;
                    ssss.alpha = SSalpha;
                    ssss.beta = SSbeta;
                    ssssRI->updateCB(&ssss);
                    EST::vector<PolygonalLight>lightsVec(&l, 1);
                    lights->updateCB(&lightsVec);
                    updateLight(GeometryItemTable["light"].get(), l);

                    float ff[7] = { 0, 0.0064,0.0484,0.187,0.567,1.99,7.41 };
                    for (int i = 0;i < 6;i++) {
                        EST::vector<float>Gkernel = CalcGaussWeights((ff[i + 1] - ff[i]),scaleGkernel);
                        
                        GkernelSBRI[i]->updateCB(&Gkernel);
                    }
                    ParallelLight paraLights[] = {
    {XMFLOAT3{cos(paraLtheta) * sin(paraLphi),  sin(paraLtheta) * sin(paraLphi),cos(paraLphi)},XMFLOAT3{1,1,1}},
    {XMFLOAT3{-0.8, 0, 0.2},XMFLOAT3{0,0,0}},
    {XMFLOAT3{0, 0, 1},XMFLOAT3{0,0,0}},
    {XMFLOAT3{-0.8,0.8,0.5},XMFLOAT3{0,0,0}}
                    };
                    EST::vector<ParallelLight>paraLightsVec(paraLights, 4);

                    ParaLightsSBRI->updateCB(&paraLightsVec);
                    //每帧都变化的点
                    // ·······
                    // ·······
                    // ·······
                    //这一帧是否增加新的渲染线程或删除线程
              
                    ThrowIfFailed(pIcmdlistpre->Reset(pIcmdallpre.Get(), nullptr));
                    //每帧都要渲染的预处理pass(变化的纹理,比如Gbuffer)
                    auto DSV = TextureResourceItemTable["DS"].get()->getDSVCPU();
                    auto DSVrt = TextureResourceItemTable["DSForRt"].get()->getDSVCPU();
                    auto DS4096 = TextureResourceItemTable["4096DS"].get()->getDSVCPU();
                    auto RSI = RootSignatureItemTable["default"].get();
                    auto wposRI = TextureResourceItemTable["wpos"].get();
                    auto normalRI = TextureResourceItemTable["normal"].get();
                    auto matteRI = TextureResourceItemTable["matte"].get();
                    auto SRI = SamplerResourceItemTable["default"].get();
                    auto passRI = BufferResourceItemTable["pass"].get();
                    auto depthRI = TextureResourceItemTable["depth"].get();
                    auto gzRI = TextureResourceItemTable["gz"].get();

                    
                    //渲染每一帧的Gbuffer
                    //GBuffer
                    ResourceBarrierTrans(wposRI->getResource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET, pIcmdlistpre.Get());
                    ResourceBarrierTrans(normalRI->getResource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET, pIcmdlistpre.Get());
                    ResourceBarrierTrans(depthRI->getResource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET, pIcmdlistpre.Get());
                    ResourceBarrierTrans(matteRI->getResource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET, pIcmdlistpre.Get());
                    ResourceBarrierTrans(gzRI->getResource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET, pIcmdlistpre.Get());
                    pIcmdlistpre->OMSetStencilRef(1);
                    pIcmdlistpre->ClearRenderTargetView(depthRI->getRTVCPU(), Colors::Black, 0, nullptr);
                    pIcmdlistpre->ClearRenderTargetView(wposRI->getRTVCPU(), Colors::Black, 0, nullptr);
                    pIcmdlistpre->ClearRenderTargetView(normalRI->getRTVCPU(), Colors::Black, 0, nullptr);
                    pIcmdlistpre->ClearRenderTargetView(matteRI->getRTVCPU(), Colors::Black, 0, nullptr);
                    pIcmdlistpre->ClearRenderTargetView(gzRI->getRTVCPU(), Colors::Black, 0, nullptr);
                    pIcmdlistpre->ClearDepthStencilView(DSV, D3D12_CLEAR_FLAG_DEPTH|D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
                    pIcmdlistpre->ClearDepthStencilView(DSVrt, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
                    pIcmdlistpre->OMSetRenderTargets(5,&depthRI->getRTVCPU(), TRUE, &DSV);
                    pIcmdlistpre->RSSetViewports(1, &stViewPort);
                    pIcmdlistpre->RSSetScissorRects(1, &stScissorRect);
                    pIcmdlistpre->SetGraphicsRootSignature(RSI->rs);
                    pIcmdlistpre->SetPipelineState(PSOITable["GBufferPass"].get()->PSO);
                    ID3D12DescriptorHeap* heaps[2] = { wposRI->SRVUAVHeap,SRI->samplerHeap };
                    pIcmdlistpre->SetDescriptorHeaps(2, heaps);
                    pIcmdlistpre->SetGraphicsRootDescriptorTable(RSI->getCBVTableIndex(1), passRI->getCBVGPU());
                    pIcmdlistpre->SetGraphicsRootDescriptorTable(RSI->getSamplerTableIndex(0), SRI->getSampler());
                    drawRenderItems(&SVGFRenderItemTable, pIcmdlistpre.Get(), RSI->getCBVTableIndex(0),nullptr);
                    drawRenderItems(&NonSVGFRenderItemTable, pIcmdlistpre.Get(), RSI->getCBVTableIndex(0),nullptr);
                    ResourceBarrierTrans(wposRI->getResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ, pIcmdlistpre.Get());
                    ResourceBarrierTrans(normalRI->getResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ, pIcmdlistpre.Get());
                    ResourceBarrierTrans(depthRI->getResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ, pIcmdlistpre.Get());
                    ResourceBarrierTrans(matteRI->getResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ, pIcmdlistpre.Get());
                    ResourceBarrierTrans(gzRI->getResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ, pIcmdlistpre.Get());
 


                    //TSSSS每一帧要做的事被封装到了这个函数
   /*                 tssssComponent.update(&SamplerResourceItemTable, &RootSignatureItemTable, pIcmdlistpre.Get(), &computePSOITable, &PSOITable,
                        &stViewPort4096, &stScissorRect4096, pIpresamheap.Get(), &NonSVGFRenderItemTable, &BufferResourceItemTable, GkernelSBRI, &ParaLightsSBRI);*/

                    //SSSS每一帧要做的事被封装到了这个函数
                    SSSSps ssssps;
                    ssssps.roughness1 = roughness1;
                    ssssComponent.update(&SamplerResourceItemTable, &RootSignatureItemTable, pIcmdlistpre.Get(), &computePSOITable, &PSOITable,
                        &stViewPort, &stScissorRect, pIpresamheap.Get(), &NonSVGFRenderItemTable, &BufferResourceItemTable, GkernelSBRI, &ParaLightsSBRI,&ssssps);

                    //cmdlistpre传入命令队列进行处理
                    pIcmdlistpre->Close();
                    nStates = 2;
                    cmdlists.clear();
                    cmdlists.push_back(pIcmdlistpre.Get());
                    pICommandQueue->ExecuteCommandLists(cmdlists.size(), cmdlists.Getdata());
                    n64fence = n64FenceValue;
                    n64FenceValue++;
                    pICommandQueue->Signal(pIFence.Get(), n64fence);
                    pIFence->SetEventOnCompletion(n64fence, hFenceEvent);
                    ::MsgWaitForMultipleObjects(1, &hFenceEvent, TRUE, INFINITE, QS_ALLINPUT);
                    HandleMessage->clear();
                    for (int i = 0;i < ThreadNum;i++)
                        HandleMessage->push_back(paras[existThreadList[i]].hEventRenderOver);
                    for (int i = 0;i < ThreadNum;i++)
                        SetEvent(paras[existThreadList[i]].hRunEvent);//通知子线程开始将渲染命令计入cmdlist

                }
                break;
                case 2: {
                    //线程渲染结束，进行后处理
                    ThrowIfFailed(pIcmdallpre->Reset());
                    ThrowIfFailed(pIcmdlistpre->Reset(pIcmdallpre.Get(), nullptr));
                    if (nFrame != 1) {
                        ThrowIfFailed(pIcmdallpost->Reset());
                        ThrowIfFailed(pIcmdlistpost->Reset(pIcmdallpost.Get(), nullptr));
                    }
                    auto DirectLRI = TextureResourceItemTable["DirectL"].get();
                    auto IndirectLRI = TextureResourceItemTable["IndirectL"].get();
                    CD3DX12_CPU_DESCRIPTOR_HANDLE stRTVHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(pIRTVHeap->GetCPUDescriptorHandleForHeapStart(), nFrameIndex, pID3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
                    ResourceBarrierTrans(pIARenderTargets[nFrameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET, pIcmdlistpre.Get());//交换链资源在创建完后可以直接由present转换到rendertarget，我猜应该是common状态
                    const FLOAT clearColor[4] = { 0.94117653f,0.9725491f,1.0f,0 }; 
                    pIcmdlistpre->ClearRenderTargetView(stRTVHandle, clearColor, 0, nullptr);
                    pIcmdlistpre->ClearDepthStencilView(TextureResourceItemTable["DS"].get()->getDSVCPU(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
                    pIcmdlistpre->ClearDepthStencilView(TextureResourceItemTable["DS1"].get()->getDSVCPU(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

                    pIcmdlistpre->Close();
                    //后处理pass
                          //tssssComponent.postProcess(pIcmdlistpost.Get());
                    ssssComponent.postProcess(&SamplerResourceItemTable,pIcmdlistpost.Get(), nFrameIndex,&RootSignatureItemTable,&computePSOITable,&SSSSkernelSBRI,ssssRI.get());
                    ResourceBarrierTrans(pIARenderTargets[nFrameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT, pIcmdlistpost.Get());

                    pIcmdlistpost->Close();
                    cmdlists.clear();
                    cmdlists.push_back(pIcmdlistpre.Get());
                    for (int i = 0;i < ThreadNum;i++)
                        cmdlists.push_back(paras[existThreadList[i]].cmdlist);
                    cmdlists.push_back(pIcmdlistpost.Get());
                    pICommandQueue->ExecuteCommandLists(cmdlists.size(), cmdlists.Getdata());
                    ThrowIfFailed(pISwapChain3->Present(1, 0));
                    n64fence = n64FenceValue;
                    n64FenceValue++;
                    pICommandQueue->Signal(pIFence.Get(), n64fence);
                    pIFence->SetEventOnCompletion(n64fence, hFenceEvent);
                    nStates = 1;
                    HandleMessage->clear();
                    HandleMessage->push_back(hFenceEvent);
                    lastvpmatRI->updateCB(&lastvpmat);

                } break;
                default: {

                }
                       break;
                }
            }
        }
    }
    catch (DxException& e)
    {//发生了COM异常

        WriteConsole(g_hOutput, e.ToString().c_str(), e.ToString().size(), NULL, NULL);
        while (1);
    }
}
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{

    FirstAPP myapp;
    myapp.init(hInstance, nCmdShow);
    myapp.startRender();

return 0;
}

UINT __stdcall SVGFThreadfunc(void* ppara) {
    threadparas* para = reinterpret_cast<threadparas*>(ppara);
    //线程的预处理操作
    //·········
    //·········
    //·········
    //while (1);
    SetEvent(para->hEventRenderOver);
    DWORD dwRet = 0;
    BOOL  bQuit = FALSE;
    MSG   msg = {};
    while (!bQuit) {
        dwRet = ::MsgWaitForMultipleObjects(1, &para->hRunEvent, FALSE, INFINITE, QS_ALLPOSTMESSAGE);
        switch (dwRet - WAIT_OBJECT_0) {
        case 0:
        {
            RootSignatureItem* RSI = (*para->RootSignatureItemTable)[para->RootSignatureItemName].get();
            RenderItem* RI = (*para->SVGFRenderItemTable)[para->RenderItemName].get();
            auto BRI = (*para->BufferResourceItemTable)[para->BufferResourceItemName].get();
            SamplerResourceItem* SRI = (*para->SamplerResourceItemTable)[para->SamplerResourceItemItemName].get();
            TextureResourceItem* firstTRI = (*(para->TextureResourceItemTable))["grass"].get();//将连续纹理SRV中的第一个set到跟参数上，跟签名会自动将所有纹理都绑定上去
            StructureBufferResourceItem<AABBbox>* boxSBRI = (*para->boxTable)["model"].get();
            StructureBufferResourceItem<triangle>* triangleSBRI = (*para->triangleTable)["model"].get();
            StructureBufferResourceItem<material>* materialSBRI = para->materials->get();
            auto DirectLRI= (*para->TextureResourceItemTable)["DirectL"].get();
            auto IndirectLRI= (*para->TextureResourceItemTable)["IndirectL"].get();
            auto DirectLPSO = (*para->PSOITable)[para->DirectRayTracerPSOName].get()->PSO;
            auto IndirectLPSO = (*para->PSOITable)[para->IndirectRayTracerPSOName].get()->PSO;
            auto cdf_1texRI = (*para->TextureResourceItemTable)["CDF-1tex"].get();
            auto DSV = &(*para->TextureResourceItemTable)["DS"]->getDSVCPU();
            auto DSV1 = &(*para->TextureResourceItemTable)["DS1"]->getDSVCPU();
            para->cmdalloc->Reset();
            para->cmdlist->Reset(para->cmdalloc,DirectLPSO);
            //必须每个子线程都setRT，视口以及裁剪矩形
            para->cmdlist->OMSetRenderTargets(1, &DirectLRI->getRTVCPU(), true, DSV);
            para->cmdlist->RSSetViewports(1, para->stViewPort);
            para->cmdlist->RSSetScissorRects(1, para->stScissorRect);
            para->cmdlist->SetGraphicsRootSignature(RSI->rs);
            para->cmdlist->SetPipelineState(DirectLPSO);
            ID3D12DescriptorHeap* heaps[2] = {BRI->CBVHeap,SRI->samplerHeap};
            para->cmdlist->SetDescriptorHeaps(2, heaps);
            para->cmdlist->SetGraphicsRootDescriptorTable(RSI->getSRVTableIndex(0), firstTRI->getSRVGPU(0));
            para->cmdlist->SetGraphicsRootDescriptorTable(RSI->getSamplerTableIndex(0), SRI->getSampler());
            para->cmdlist->SetGraphicsRootDescriptorTable(RSI->getCBVTableIndex(1), BRI->getCBVGPU());
            para->cmdlist->SetGraphicsRootDescriptorTable(RSI->getSRVTableIndex(1), boxSBRI->getSRVGPU());
            para->cmdlist->SetGraphicsRootDescriptorTable(RSI->getSRVTableIndex(2), triangleSBRI->getSRVGPU());
            para->cmdlist->SetGraphicsRootDescriptorTable(RSI->getSRVTableIndex(3), materialSBRI->getSRVGPU());
            para->cmdlist->SetGraphicsRootDescriptorTable(RSI->getSRVTableIndex(4), para->randomNumbers->get()->getSRVGPU());
            para->cmdlist->SetGraphicsRootDescriptorTable(RSI->getSRVTableIndex(5), para->lights->get()->getSRVGPU());
            para->cmdlist->SetGraphicsRootDescriptorTable(RSI->getSRVTableIndex(9), cdf_1texRI->getSRVGPU(0));

            drawRenderItem(RI, para->cmdlist, RSI->getCBVTableIndex(0));//渲染项中包含objectconstant有纹理索引，可以在shader中找到资源项对应的纹理索引。
            para->cmdlist->OMSetRenderTargets(1, &IndirectLRI->getRTVCPU(), true, DSV1);
            para->cmdlist->SetPipelineState(IndirectLPSO);
            drawRenderItem(RI, para->cmdlist, RSI->getCBVTableIndex(0));
            para->cmdlist->Close();
            SetEvent(para->hEventRenderOver);
        }
        break;
        }
    }
    return 0;
}

UINT __stdcall NonSVGFThreadfunc(void* ppara) {
    threadparas* para = reinterpret_cast<threadparas*>(ppara);
    //线程的预处理操作
    //·········
    //·········
    //·········
    //while (1);
    SetEvent(para->hEventRenderOver);
    DWORD dwRet = 0;
    BOOL  bQuit = FALSE;
    MSG   msg = {};
    while (!bQuit) {
        dwRet = ::MsgWaitForMultipleObjects(1, &para->hRunEvent, FALSE, INFINITE, QS_ALLPOSTMESSAGE);
        switch (dwRet - WAIT_OBJECT_0) {
        case 0:
        {
            RootSignatureItem* RSI = (*para->RootSignatureItemTable)[para->RootSignatureItemName].get();
            RenderItem* RI = (*para->NonSVGFRenderItemTable)[para->RenderItemName].get();
            auto BRI = (*para->BufferResourceItemTable)[para->BufferResourceItemName].get();
            SamplerResourceItem* SRI = (*para->SamplerResourceItemTable)[para->SamplerResourceItemItemName].get();
            StructureBufferResourceItem<AABBbox>* boxSBRI = (*para->boxTable)["model"].get();
            StructureBufferResourceItem<triangle>* triangleSBRI = (*para->triangleTable)["model"].get();
            StructureBufferResourceItem<material>* materialSBRI = para->materials->get();
           // auto cdf_1texRI = (*para->TextureResourceItemTable)["CDF-1tex"].get();
            TextureResourceItem* firstTRI = (*(para->TextureResourceItemTable))["grass"].get();//将连续纹理SRV中的第一个set到跟参数上，跟签名会自动将所有纹理都绑定上去
            auto PSO = (*para->PSOITable)[para->DirectRayTracerPSOName].get()->PSO;
            auto DSV = &(*para->TextureResourceItemTable)["DSForRt"]->getDSVCPU();
          //  auto skyRI = (*para->TextureResourceItemTable)["snowcube"].get();
            auto beckmanTexRI = (*para->TextureResourceItemTable)["beckmantex"].get();
            auto headNormRI = (*para->TextureResourceItemTable)["LPSHeadNorm"].get();
            auto convolveTex = (*para->TextureResourceItemTable)["convolveTex1"].get();
            auto irradianceTex = (*para->TextureResourceItemTable)["irradianceTex"].get();
            auto stretchMap = (*para->TextureResourceItemTable)["stretchMap"].get();
            auto sumGw = (para->sumGweights);
            auto RTV = CD3DX12_CPU_DESCRIPTOR_HANDLE(para->RTVHeap->GetCPUDescriptorHandleForHeapStart(), *para->nCurrentFrameIndex,para->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
            para->cmdalloc->Reset();
            para->cmdlist->Reset(para->cmdalloc, PSO);
            //必须每个子线程都setRT，视口以及裁剪矩形
            para->cmdlist->OMSetRenderTargets(1, &RTV, true, DSV);
            para->cmdlist->RSSetViewports(1, para->stViewPort);
            para->cmdlist->RSSetScissorRects(1, para->stScissorRect);
            para->cmdlist->SetGraphicsRootSignature(RSI->rs);
            para->cmdlist->SetPipelineState(PSO);
            ID3D12DescriptorHeap* heaps[2] = { BRI->CBVHeap,SRI->samplerHeap };
            para->cmdlist->SetDescriptorHeaps(2, heaps);
            para->cmdlist->SetGraphicsRootDescriptorTable(RSI->getSRVTableIndex(0), firstTRI->getSRVGPU(0));
            para->cmdlist->SetGraphicsRootDescriptorTable(RSI->getSRVTableIndex(1), boxSBRI->getSRVGPU());
            para->cmdlist->SetGraphicsRootDescriptorTable(RSI->getSRVTableIndex(2), triangleSBRI->getSRVGPU());
            para->cmdlist->SetGraphicsRootDescriptorTable(RSI->getSRVTableIndex(3), materialSBRI->getSRVGPU());
            para->cmdlist->SetGraphicsRootDescriptorTable(RSI->getSamplerTableIndex(0), SRI->getSampler());
            para->cmdlist->SetGraphicsRootDescriptorTable(RSI->getCBVTableIndex(1), BRI->getCBVGPU());
            para->cmdlist->SetGraphicsRootDescriptorTable(RSI->getSRVTableIndex(4), para->randomNumbers->get()->getSRVGPU());
            para->cmdlist->SetGraphicsRootDescriptorTable(RSI->getSRVTableIndex(5), para->lights->get()->getSRVGPU());
         //   para->cmdlist->SetGraphicsRootDescriptorTable(RSI->getSRVTableIndex(9), cdf_1texRI->getSRVGPU(0));
          //  para->cmdlist->SetGraphicsRootDescriptorTable(RSI->getSRVTableIndex(10), skyRI->getSRVGPU(0));
            para->cmdlist->SetGraphicsRootDescriptorTable(RSI->getSRVTableIndex(11), headNormRI->getSRVGPU(0));

            //TSSSS
        //    para->cmdlist->SetGraphicsRootDescriptorTable(RSI->getSRVTableIndex(12), beckmanTexRI->getSRVGPU(0));
        //    para->cmdlist->SetGraphicsRootDescriptorTable(RSI->getSRVTableIndex(13), convolveTex->getSRVGPU(0));
        //    para->cmdlist->SetGraphicsRootDescriptorTable(RSI->getSRVTableIndex(14), sumGw->get()->getSRVGPU());
        //    para->cmdlist->SetGraphicsRootDescriptorTable(RSI->getUAVTableIndex(0), irradianceTex->getUAVGPU(0));
        //    para->cmdlist->SetGraphicsRootDescriptorTable(RSI->getSRVTableIndex(15), stretchMap->getSRVGPU(0));
            //

            para->cmdlist->SetGraphicsRootDescriptorTable(RSI->getSRVTableIndex(16), para->ParaLightsSBRI->get()->getSRVGPU());
            drawRenderItem(RI, para->cmdlist, RSI->getCBVTableIndex(0));//渲染项中包含objectconstant有纹理索引，可以在shader中找到资源项对应的纹理索引。
            para->cmdlist->Close();
            SetEvent(para->hEventRenderOver);
        }
        break;
        }
    }
    return 0;
}
