#pragma once
#include"SVGFComponent.h"
void SVGFComponent::createResource(std::unordered_map<std::string, std::unique_ptr< TextureResourceItem>>* textable,//创建资源,在创建资源项时调用
	ID3D12Device4* device, ID3D12DescriptorHeap* srvuavheap, ID3D12DescriptorHeap* rtvheap,
	ID3D12GraphicsCommandList* precmdlist, std::unordered_map<std::string, std::unique_ptr< NON_RT_DS_TextureSegregatedFreeLists>>* NonRtDstable,
	std::unordered_map<std::string, std::unique_ptr< RT_DS_TextureSegregatedFreeLists>>* RtDstable) {
    this->TexTable = textable;
    Device = device;
    SrvUavHeap = srvuavheap;

    D3D12_RESOURCE_DESC filteredTexDesc = {};
    filteredTexDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    filteredTexDesc.Alignment = 0;
    filteredTexDesc.Width = 1024;
    filteredTexDesc.Height = 768;
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

    D3D12_SHADER_RESOURCE_VIEW_DESC SRVdesc = {};
    SRVdesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    SRVdesc.Texture2D.MostDetailedMip = 0;
    SRVdesc.Texture2D.ResourceMinLODClamp = 0.0f;
    SRVdesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    rtvDesc.Texture2D.MipSlice = 0;
    rtvDesc.Texture2D.PlaneSlice = 0;

    //SVFG
   // auto filteredRI = std::make_unique<TextureResourceItem>(device, srvuavheap, nullptr, nullptr, false, NULL);
   // filteredRI->createNON_RT_DS_WritableTex(device, precmdlist, &filteredTexDesc, NonRtDstable);
   //filteredRI->createSRVforResourceItem(&SRVdesc,device, HeapOffsetTable[filteredRI->SRVUAVHeap]);
   //filteredRI->createUAVforResourceItem(&filteredTexUavDesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap],precmdlist);
   //(*TexTable)["DirectFilteredtex"] = std::move(filteredRI);//降噪过程要用的纹理
   //filteredRI = std::make_unique<TextureResourceItem>(device, srvuavheap, nullptr, nullptr, false, NULL);
   //filteredRI->createNON_RT_DS_WritableTex(device, precmdlist, &filteredTexDesc, NonRtDstable);
   //filteredRI->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap]);
   //filteredRI->createUAVforResourceItem(&filteredTexUavDesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap], precmdlist);
   //(*TexTable)["DirectFilteredtex1"] = std::move(filteredRI);//降噪过程要用的纹理
   //filteredRI = std::make_unique<TextureResourceItem>(device, srvuavheap, nullptr, nullptr, false, NULL);
   //filteredRI->createNON_RT_DS_WritableTex(device, precmdlist, &filteredTexDesc, NonRtDstable);
   //filteredRI->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap]);
   //filteredRI->createUAVforResourceItem(&filteredTexUavDesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap], precmdlist);
   //(*TexTable)["IndirectFilteredtex"] = std::move(filteredRI);//降噪过程要用的纹理
   //filteredRI = std::make_unique<TextureResourceItem>(device, srvuavheap, nullptr, nullptr, false, NULL);
   //filteredRI->createNON_RT_DS_WritableTex(device, precmdlist, &filteredTexDesc, NonRtDstable);
   //filteredRI->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap]);
   //filteredRI->createUAVforResourceItem(&filteredTexUavDesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap], precmdlist);
   //(*TexTable)["IndirectFilteredtex1"] = std::move(filteredRI);//降噪过程要用的纹理
   //filteredRI = std::make_unique<TextureResourceItem>(device, srvuavheap, nullptr, nullptr, false, NULL);
   //filteredRI->createNON_RT_DS_WritableTex(device, precmdlist, &filteredTexDesc, NonRtDstable);
   //filteredRI->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap]);
   //filteredRI->createUAVforResourceItem(&filteredTexUavDesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap], precmdlist);
   //(*TexTable)["DirectMoment1tex"] = std::move(filteredRI);//moment1
   //filteredRI = std::make_unique<TextureResourceItem>(device, srvuavheap, nullptr, nullptr, false, NULL);
   //filteredRI->createNON_RT_DS_WritableTex(device, precmdlist, &filteredTexDesc, NonRtDstable);
   //filteredRI->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap]);
   //filteredRI->createUAVforResourceItem(&filteredTexUavDesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap], precmdlist);
   //(*TexTable)["DirectMoment2tex"] = std::move(filteredRI);//moment2
   //filteredRI = std::make_unique<TextureResourceItem>(device, srvuavheap, nullptr, nullptr, false, NULL);
   //filteredRI->createNON_RT_DS_WritableTex(device, precmdlist, &filteredTexDesc, NonRtDstable);
   //filteredRI->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap]);
   //filteredRI->createUAVforResourceItem(&filteredTexUavDesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap], precmdlist);
   //(*TexTable)["IndirectMoment1tex"] = std::move(filteredRI);//moment1
   //filteredRI = std::make_unique<TextureResourceItem>(device, srvuavheap, nullptr, nullptr, false, NULL);
   //filteredRI->createNON_RT_DS_WritableTex(device, precmdlist, &filteredTexDesc, NonRtDstable);
   //filteredRI->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap]);
   //filteredRI->createUAVforResourceItem(&filteredTexUavDesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap], precmdlist);
   //(*TexTable)["IndirectMoment2tex"] = std::move(filteredRI);//moment2
   //filteredRI = std::make_unique<TextureResourceItem>(device, srvuavheap, nullptr, nullptr, false, NULL);
   //filteredRI->createNON_RT_DS_WritableTex(device, precmdlist, &filteredTexDesc, NonRtDstable);
   //filteredRI->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap]);
   //filteredRI->createUAVforResourceItem(&filteredTexUavDesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap], precmdlist);
   //(*TexTable)["DirectVariancetex"] = std::move(filteredRI);//variance
   //filteredRI = std::make_unique<TextureResourceItem>(device, srvuavheap, nullptr, nullptr, false, NULL);
   //filteredRI->createNON_RT_DS_WritableTex(device, precmdlist, &filteredTexDesc, NonRtDstable);
   //filteredRI->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap]);
   //filteredRI->createUAVforResourceItem(&filteredTexUavDesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap], precmdlist);
   //(*TexTable)["IndirectVariancetex"] = std::move(filteredRI);//variance
    filteredTexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    filteredTexDesc.Width = 1024;
    filteredTexDesc.Height = 768;
    filteredTexUavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    //filteredRI = std::make_unique<TextureResourceItem>(device, srvuavheap, nullptr, nullptr, false, NULL);
    //filteredRI->createNON_RT_DS_WritableTex(device, precmdlist, &filteredTexDesc, NonRtDstable);
    //filteredRI->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap]);
    //filteredRI->createUAVforResourceItem(&filteredTexUavDesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap], precmdlist);
    //(*TexTable)["CDF-1tex"] = std::move(filteredRI);//weightMap

    auto DirectLRI = std::make_unique<TextureResourceItem>(device, srvuavheap, rtvheap, nullptr, false, NULL);
    DirectLRI->createNON_RT_DS_WritableTex(device, precmdlist, &filteredTexDesc, NonRtDstable);
    DirectLRI->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[DirectLRI->SRVUAVHeap]);
    DirectLRI->createUAVforResourceItem(&filteredTexUavDesc, device, HeapOffsetTable[DirectLRI->SRVUAVHeap], precmdlist);
    (*TexTable)["DirectL"] = std::move(DirectLRI);//directL图

    //SVGF

   //for (int i = 0;i < 6;i++) {
   //    filteredRI = std::make_unique<TextureResourceItem>(device, srvuavheap, nullptr, nullptr, false, NULL);
   //    filteredRI->createNON_RT_DS_WritableTex(device, precmdlist, &filteredTexDesc, NonRtDstable);
   //    filteredRI->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap]);
   //    filteredRI->createUAVforResourceItem(&filteredTexUavDesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap], precmdlist);
   //    (*TexTable)["DirectHistorytex"+std::to_string(i)] = std::move(filteredRI);//historyTex
   //}
   //for (int i = 0;i < 6;i++) {
   //    filteredRI = std::make_unique<TextureResourceItem>(device, srvuavheap, nullptr, nullptr, false, NULL);
   //    filteredRI->createNON_RT_DS_WritableTex(device, precmdlist, &filteredTexDesc, NonRtDstable);
   //    filteredRI->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap]);
   //    filteredRI->createUAVforResourceItem(&filteredTexUavDesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap], precmdlist);
   //    (*TexTable)["IndirectHistorytex" + std::to_string(i)] = std::move(filteredRI);//historyTex
   //}
   //for (int i = 1;i < 3;i++) {
   //    filteredRI = std::make_unique<TextureResourceItem>(device, srvuavheap, nullptr, nullptr, false, NULL);
   //    filteredRI->createNON_RT_DS_WritableTex(device, precmdlist, &filteredTexDesc, NonRtDstable);
   //    filteredRI->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap]);
   //    filteredRI->createUAVforResourceItem(&filteredTexUavDesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap], precmdlist);
   //    (*TexTable)["DirectLastMoment" + std::to_string(i)] = std::move(filteredRI);//lastmomentTex
   //}
   //for (int i = 1;i < 3;i++) {
   //    filteredRI = std::make_unique<TextureResourceItem>(device, srvuavheap, nullptr, nullptr, false, NULL);
   //    filteredRI->createNON_RT_DS_WritableTex(device, precmdlist, &filteredTexDesc, NonRtDstable);
   //    filteredRI->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap]);
   //    filteredRI->createUAVforResourceItem(&filteredTexUavDesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap], precmdlist);
   //    (*TexTable)["IndirectLastMoment" + std::to_string(i)] = std::move(filteredRI);//lastmomentTex
   //}
    filteredTexDesc.Width = 1024;
    filteredTexDesc.Height = 768;
    filteredTexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    //filteredRI = std::make_unique<TextureResourceItem>(device, srvuavheap, nullptr, nullptr, false, NULL);
    //filteredRI->createNON_RT_DS_WritableTex(device, precmdlist, &filteredTexDesc, NonRtDstable);
    //filteredRI->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap]);
    //(*TexTable)["lastnormal"] = std::move(filteredRI);//lastNormalTex
    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    filteredTexDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
    auto lastFrameBuffer = std::make_unique<TextureResourceItem>(device,srvuavheap, rtvheap, nullptr, false, NULL);
    lastFrameBuffer->createRT_DS_WritableTex(device, precmdlist, &filteredTexDesc,nullptr, (*RtDstable)["RTDS"].get());
    lastFrameBuffer->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[lastFrameBuffer->SRVUAVHeap]);
    lastFrameBuffer->createRTVforResourceItem(&rtvDesc, device, HeapOffsetTable[lastFrameBuffer->RTVHeap], precmdlist);
    (*TexTable)["DirectLastFrameBuffer"] = std::move(lastFrameBuffer);
    lastFrameBuffer = std::make_unique<TextureResourceItem>(device, srvuavheap, rtvheap, nullptr, false, NULL);
    lastFrameBuffer->createRT_DS_WritableTex(device, precmdlist, &filteredTexDesc, nullptr, (*RtDstable)["RTDS"].get());
    lastFrameBuffer->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[lastFrameBuffer->SRVUAVHeap]);
    lastFrameBuffer->createRTVforResourceItem(&rtvDesc, device, HeapOffsetTable[lastFrameBuffer->RTVHeap], precmdlist);
    (*TexTable)["IndirectLastFrameBuffer"] = std::move(lastFrameBuffer);

    //SVFG
    filteredTexDesc.Width = 1024;
    filteredTexDesc.Height = 768;
    filteredTexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;


   auto IndirectLRI = std::make_unique<TextureResourceItem>(device, srvuavheap, rtvheap, nullptr, false, NULL);
   IndirectLRI->createRT_DS_WritableTex(device, precmdlist, &filteredTexDesc, nullptr, (*RtDstable)["RTDS3"].get());
   IndirectLRI->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[srvuavheap]);
   IndirectLRI->createRTVforResourceItem(&rtvDesc, device, HeapOffsetTable[rtvheap], precmdlist);
   (*TexTable)["IndirectL"] = std::move(IndirectLRI);//indirectL图
    IndirectLRI = std::make_unique<TextureResourceItem>(device, srvuavheap, rtvheap, nullptr, false, NULL);
    IndirectLRI->createRT_DS_WritableTex(device, precmdlist, &filteredTexDesc, nullptr, (*RtDstable)["RTDS4"].get());
    IndirectLRI->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[srvuavheap]);
    IndirectLRI->createRTVforResourceItem(&rtvDesc, device, HeapOffsetTable[rtvheap], precmdlist);
    (*TexTable)["modulatetex"] = std::move(IndirectLRI);//modulate图
}
void SVGFComponent::preProcess(std::unordered_map<std::string, std::unique_ptr< SamplerResourceItem>>(*SamplerResourceItemTable),//预处理，case0调用
    std::unordered_map<std::string, std::unique_ptr< RootSignatureItem>>(*RootSignatureItemTable), ID3D12GraphicsCommandList* precmdlist,
    std::unordered_map<std::string, std::unique_ptr< computePSOItem>>* computePSOITable, std::unordered_map<std::string, std::unique_ptr< PSOItem>>(*PSOITable),
    D3D12_VIEWPORT* stViewPort4096, D3D12_RECT* stScissorRect4096, ID3D12DescriptorHeap* pIsamplerHeap, std::unordered_map<std::string, std::unique_ptr< RenderItem>>* NonSVGFRenderItemTable,
    std::unordered_map<std::string, std::unique_ptr< ConstantBufferResourceItem<passconstant>>>* BufferResourceItemTable, std::unique_ptr<StructureBufferResourceItem<float>>* GkernelSBRI) {

                       auto lastDirectRI = (*TexTable)["DirectLastFrameBuffer"].get();
                       auto lastIndirectRI = (*TexTable)["IndirectLastFrameBuffer"].get();

                       ResourceBarrierTrans(lastDirectRI->getResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ, precmdlist);
                       ResourceBarrierTrans(lastIndirectRI->getResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ, precmdlist);


               

}
void SVGFComponent::update(ID3D12GraphicsCommandList* precmdlist) {
    auto DirectLRI = (*TexTable)["DirectL"].get();
    auto IndirectLRI = (*TexTable)["IndirectL"].get();
    //precmdlist->ClearRenderTargetView(DirectLRI->getRTVCPU(), Colors::Black, 0, nullptr);
    //precmdlist->ClearRenderTargetView(IndirectLRI->getRTVCPU(), Colors::Black, 0, nullptr);//这个是prelist
}

void SVGFComponent::postProcess(std::unordered_map<std::string, std::unique_ptr< SamplerResourceItem>>(*SamplerResourceItemTable), ID3D12GraphicsCommandList* postcmdlist, int RTnote, std::unordered_map<std::string, std::unique_ptr< RootSignatureItem>>(*RootSignatureItemTable),
    std::unordered_map<std::string, std::unique_ptr< computePSOItem>>* computePSOITable,int nFrameIndex,int nFrame, std::unique_ptr < StructureBufferResourceItem<float>> *hSBRI, std::unique_ptr < ConstantBufferResourceItem <lastVPmat>>* lastvpmatRI, std::unique_ptr < StructureBufferResourceItem<XMINT2>>* offsetArray
, std::unordered_map<std::string, std::unique_ptr< ConstantBufferResourceItem<passconstant>>>* BufferResourceItemTable, D3D12_VIEWPORT* stViewPort, D3D12_RECT* stScissorRect, ID3D12DescriptorHeap* pIRTVHeap, std::unordered_map<std::string, std::unique_ptr< PSOItem>>(*PSOITable), std::unordered_map<std::string, std::unique_ptr< RenderItem>>*SVGFRenderItemTable, std::unordered_map<std::string, std::unique_ptr< RenderItem>>* NonSVGFRenderItemTable) {
    //SVGF
    auto csRS = (*RootSignatureItemTable)["cs"].get();
    postcmdlist->SetComputeRootSignature(csRS->rs);
    auto DirectLRI = (*TexTable)["DirectL"].get();
    auto IndirectLRI = (*TexTable)["IndirectL"].get();
        auto currentBackBufferRI = (*TexTable)["backBuffer" + std::to_string(nFrameIndex)].get();
        auto wposTexRI = (*TexTable)["wpos"].get();
        auto normalTexRI = (*TexTable)["normal"].get();
        auto zTexRI = (*TexTable)["depth"].get();
        auto gzTexRI = (*TexTable)["gz"].get();
        auto samplerRI = (*SamplerResourceItemTable)["default"].get();
    auto RSI = (*RootSignatureItemTable)["cs"].get();
    /*   auto DirectFilteredTexRI = (*TexTable)["DirectFilteredtex"].get();
        auto IndirectFilteredTexRI = (*TexTable)["IndirectFilteredtex"].get();
        auto DirectFilteredTex1RI = (*TexTable)["DirectFilteredtex1"].get();
        auto IndirectFilteredTex1RI = (*TexTable)["IndirectFilteredtex1"].get();
        auto DirectLastFrameBufferRI = (*TexTable)["DirectLastFrameBuffer"].get();
        auto IndirectLastFrameBufferRI = (*TexTable)["IndirectLastFrameBuffer"].get();
        auto DirectMoment1Tex = (*TexTable)["DirectMoment1tex"].get();
        auto IndirectMoment1Tex = (*TexTable)["IndirectMoment1tex"].get();
        auto DirectMoment2Tex = (*TexTable)["DirectMoment2tex"].get();
        auto IndirectMoment2Tex = (*TexTable)["IndirectMoment2tex"].get();
        auto DirectVarianceTexRI = (*TexTable)["DirectVariancetex"].get();
        auto IndirectVarianceTexRI = (*TexTable)["IndirectVariancetex"].get();
        auto DirectlastM1 = (*TexTable)["DirectLastMoment1"].get();
        auto DirectlastM2 = (*TexTable)["DirectLastMoment2"].get();
        auto IndirectlastM1 = (*TexTable)["IndirectLastMoment1"].get();
        auto IndirectlastM2 = (*TexTable)["IndirectLastMoment2"].get();
        auto lastnormalRI = (*TexTable)["lastnormal"].get();
        auto weightRI = (*TexTable)["weighttex"].get();*/
        auto RTV = CD3DX12_CPU_DESCRIPTOR_HANDLE(pIRTVHeap->GetCPUDescriptorHandleForHeapStart(), nFrameIndex, Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
        //auto CDF_1texRI = (*TexTable)["CDF-1tex"].get();
        ResourceBarrierTrans(DirectLRI->getResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ, postcmdlist);
        ResourceBarrierTrans(IndirectLRI->getResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ, postcmdlist);
        ID3D12DescriptorHeap* heaps[2] = { wposTexRI->SRVUAVHeap,samplerRI->samplerHeap };
        postcmdlist->SetDescriptorHeaps(2, heaps);
        postcmdlist->SetComputeRootDescriptorTable(RSI->getSRVTableIndex(0), DirectLRI->getSRVGPU(0));
        //postcmdlist->SetComputeRootDescriptorTable(RSI->getSRVTableIndex(1), wposTexRI->getSRVGPU(0));
   /*     postcmdlist->SetComputeRootDescriptorTable(RSI->getSRVTableIndex(2), normalTexRI->getSRVGPU(0));
        postcmdlist->SetComputeRootDescriptorTable(RSI->getSRVTableIndex(5), zTexRI->getSRVGPU(0));
        postcmdlist->SetComputeRootDescriptorTable(RSI->getSRVTableIndex(6), gzTexRI->getSRVGPU(0));
        postcmdlist->SetComputeRootDescriptorTable(RSI->getSRVTableIndex(4), hSBRI->get()->getSRVGPU());
        postcmdlist->SetComputeRootDescriptorTable(RSI->getSRVTableIndex(7), lastnormalRI->getSRVGPU(0));
        postcmdlist->SetComputeRootDescriptorTable(RSI->getSamplerTableIndex(0), samplerRI->getSampler());
        postcmdlist->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(0), DirectFilteredTexRI->getUAVGPU(0));
        postcmdlist->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(1), DirectMoment1Tex->getUAVGPU(0));
        postcmdlist->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(2), DirectMoment2Tex->getUAVGPU(0));
        postcmdlist->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(5), DirectlastM1->getUAVGPU(0));
        postcmdlist->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(6), DirectlastM2->getUAVGPU(0));*/
        postcmdlist->SetComputeRootDescriptorTable(RSI->getCBVTableIndex(0), lastvpmatRI->get()->getCBVGPU());
        //postcmdlist->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(3), (*TexTable)["DirectHistorytex" + std::to_string((nFrame - 1) % 6)].get()->getUAVGPU(0));
        //postcmdlist->SetPipelineState((*computePSOITable)["accumulationPass"].get()->PSO);
        //postcmdlist->Dispatch(64, 48, 1);//累计当前帧到momentTex和historyTex
        //postcmdlist->SetComputeRootDescriptorTable(RSI->getSRVTableIndex(0), IndirectLRI->getSRVGPU(0));
        //postcmdlist->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(1), IndirectMoment1Tex->getUAVGPU(0));
        //postcmdlist->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(2), IndirectMoment2Tex->getUAVGPU(0));
        //postcmdlist->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(5), IndirectlastM1->getUAVGPU(0));
        //postcmdlist->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(6), IndirectlastM2->getUAVGPU(0));
        //postcmdlist->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(3), (*TexTable)["IndirectHistorytex" + std::to_string((nFrame - 1) % 6)].get()->getUAVGPU(0));
        //postcmdlist->Dispatch(64, 48, 1);//累计当前帧到momentTex和historyTex
        //postcmdlist->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(4), IndirectVarianceTexRI->getUAVGPU(0));
        //postcmdlist->SetComputeRootDescriptorTable(RSI->getSRVTableIndex(3), offsetArray[0].get()->getSRVGPU());
        //postcmdlist->SetPipelineState((*computePSOITable)["variancePass"].get()->PSO);
        //postcmdlist->Dispatch(64, 48, 1);//计算当前帧的variance作为filter的输入
        //postcmdlist->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(1), DirectMoment1Tex->getUAVGPU(0));
        //postcmdlist->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(2), DirectMoment2Tex->getUAVGPU(0));
        //postcmdlist->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(4), DirectVarianceTexRI->getUAVGPU(0));
        //postcmdlist->Dispatch(64, 48, 1);//计算当前帧的variance作为filter的输入
        //ResourceBarrierTrans(DirectFilteredTex1RI->getResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ, postcmdlist);
        //postcmdlist->SetComputeRootDescriptorTable(RSI->getSRVTableIndex(0), DirectLRI->getSRVGPU(0));
        ////postcmdlist->SetComputeRootDescriptorTable(RSI->getSRVTableIndex(8), CDF_1texRI->getSRVGPU(0));
        //postcmdlist->SetPipelineState((*computePSOITable)["denoise"].get()->PSO);
        //for (int i = 0;i < 5;i++) {
        //    postcmdlist->SetComputeRootDescriptorTable(RSI->getSRVTableIndex(3), offsetArray[i].get()->getSRVGPU());
        //    postcmdlist->Dispatch(64, 48, 1);//降噪！
        //    if (i == 4)
        //        break;
        //    if (i % 2 == 0) {
        //        ResourceBarrierTrans(DirectFilteredTexRI->getResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ, postcmdlist);
        //        ResourceBarrierTrans(DirectFilteredTex1RI->getResource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, postcmdlist);
        //        postcmdlist->SetComputeRootDescriptorTable(RSI->getSRVTableIndex(0), DirectFilteredTexRI->getSRVGPU(0));
        //        postcmdlist->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(0), DirectFilteredTex1RI->getUAVGPU(0));
        //    }
        //    else {
        //        ResourceBarrierTrans(DirectFilteredTex1RI->getResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ, postcmdlist);
        //        ResourceBarrierTrans(DirectFilteredTexRI->getResource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, postcmdlist);
        //        postcmdlist->SetComputeRootDescriptorTable(RSI->getSRVTableIndex(0), DirectFilteredTex1RI->getSRVGPU(0));
        //        postcmdlist->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(0), DirectFilteredTexRI->getUAVGPU(0));
        //    }
        //}
        //ResourceBarrierTrans(DirectFilteredTex1RI->getResource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, postcmdlist);

        //ResourceBarrierTrans(IndirectFilteredTex1RI->getResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ, postcmdlist);
        //postcmdlist->SetComputeRootDescriptorTable(RSI->getSRVTableIndex(0), IndirectLRI->getSRVGPU(0));
        //postcmdlist->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(4), IndirectVarianceTexRI->getUAVGPU(0));
        //postcmdlist->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(0), IndirectFilteredTexRI->getUAVGPU(0));
        //for (int i = 0;i < 5;i++) {
        //    postcmdlist->SetComputeRootDescriptorTable(RSI->getSRVTableIndex(3), offsetArray[i].get()->getSRVGPU());
        //    postcmdlist->Dispatch(64, 48, 1);//降噪！
        //    if (i == 4)
        //        break;
        //    if (i % 2 == 0) {
        //        ResourceBarrierTrans(IndirectFilteredTexRI->getResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ, postcmdlist);
        //        ResourceBarrierTrans(IndirectFilteredTex1RI->getResource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, postcmdlist);
        //        postcmdlist->SetComputeRootDescriptorTable(RSI->getSRVTableIndex(0), IndirectFilteredTexRI->getSRVGPU(0));
        //        postcmdlist->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(0), IndirectFilteredTex1RI->getUAVGPU(0));
        //    }
        //    else {
        //        ResourceBarrierTrans(IndirectFilteredTex1RI->getResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ, postcmdlist);
        //        ResourceBarrierTrans(IndirectFilteredTexRI->getResource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, postcmdlist);
        //        postcmdlist->SetComputeRootDescriptorTable(RSI->getSRVTableIndex(0), IndirectFilteredTex1RI->getSRVGPU(0));
        //        postcmdlist->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(0), IndirectFilteredTexRI->getUAVGPU(0));
        //    }
        //}
        //ResourceBarrierTrans(IndirectFilteredTex1RI->getResource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, postcmdlist);


        //postcmdlist->SetPipelineState((*computePSOITable)["mixPass"].get()->PSO);
        //ResourceBarrierTrans(DirectFilteredTexRI->getResource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, postcmdlist);
        //ResourceBarrierTrans(IndirectFilteredTexRI->getResource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, postcmdlist);
        //postcmdlist->SetComputeRootDescriptorTable(RSI->getSRVTableIndex(0), DirectLastFrameBufferRI->getSRVGPU(0));
        //postcmdlist->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(0), DirectFilteredTexRI->getUAVGPU(0));
        //postcmdlist->SetComputeRootDescriptorTable(RSI->getCBVTableIndex(0), lastvpmatRI->get()->getCBVGPU());
        //postcmdlist->Dispatch(64, 48, 1);//混合！
        //postcmdlist->SetComputeRootDescriptorTable(RSI->getSRVTableIndex(0), IndirectLastFrameBufferRI->getSRVGPU(0));
        //postcmdlist->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(0), IndirectFilteredTexRI->getUAVGPU(0));
        //postcmdlist->Dispatch(64, 48, 1);//混合！
        //ResourceBarrierTrans(lastnormalRI->getResource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST, postcmdlist);
        //ResourceBarrierTrans(normalTexRI->getResource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_SOURCE, postcmdlist);
        //ResourceBarrierTrans(DirectFilteredTexRI->getResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE, postcmdlist);
        //ResourceBarrierTrans(IndirectFilteredTexRI->getResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE, postcmdlist);
        //ResourceBarrierTrans(DirectLastFrameBufferRI->getResource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST, postcmdlist);
        //ResourceBarrierTrans(IndirectLastFrameBufferRI->getResource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST, postcmdlist);
        //ResourceBarrierTrans(DirectMoment1Tex->getResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE, postcmdlist);
        //ResourceBarrierTrans(DirectMoment2Tex->getResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE, postcmdlist);
        //ResourceBarrierTrans(IndirectMoment1Tex->getResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE, postcmdlist);
        //ResourceBarrierTrans(IndirectMoment2Tex->getResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE, postcmdlist);
        //ResourceBarrierTrans(DirectLRI->getResource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_SOURCE, postcmdlist);
        //ResourceBarrierTrans(IndirectLRI->getResource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_SOURCE, postcmdlist);
        //ResourceBarrierTrans(DirectlastM1->getResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_DEST, postcmdlist);
        //ResourceBarrierTrans(DirectlastM2->getResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_DEST, postcmdlist);
        //ResourceBarrierTrans(IndirectlastM1->getResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_DEST, postcmdlist);
        //ResourceBarrierTrans(IndirectlastM2->getResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_DEST, postcmdlist);
        //ResourceBarrierTrans((*TexTable)["DirectHistorytex" + std::to_string((nFrame - 1) % 6)].get()->getResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_DEST, postcmdlist);
        //ResourceBarrierTrans((*TexTable)["IndirectHistorytex" + std::to_string((nFrame - 1) % 6)].get()->getResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_DEST, postcmdlist);
        //postcmdlist->CopyResource(IndirectLastFrameBufferRI->getResource(), IndirectFilteredTexRI->getResource());
        //postcmdlist->CopyResource(DirectLastFrameBufferRI->getResource(), DirectFilteredTexRI->getResource());
        //postcmdlist->CopyResource(DirectlastM1->getResource(), DirectMoment1Tex->getResource());
        //postcmdlist->CopyResource(DirectlastM2->getResource(), DirectMoment2Tex->getResource());
        //postcmdlist->CopyResource(IndirectlastM1->getResource(), IndirectMoment1Tex->getResource());
        //postcmdlist->CopyResource(IndirectlastM2->getResource(), IndirectMoment2Tex->getResource());
        //postcmdlist->CopyResource(lastnormalRI->getResource(), normalTexRI->getResource());
        //postcmdlist->CopyResource((*TexTable)["DirectHistorytex" + std::to_string((nFrame - 1) % 6)].get()->getResource(), DirectLRI->getResource());

        //待注释，这几行将DirectLcopy到RT上
        ResourceBarrierTrans((*TexTable)["backBuffer" + std::to_string(nFrameIndex)].get()->getResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_DEST, postcmdlist);
        postcmdlist->CopyResource((*TexTable)["backBuffer"+std::to_string(nFrameIndex)].get()->getResource(), DirectLRI->getResource());
        ResourceBarrierTrans((*TexTable)["backBuffer" + std::to_string(nFrameIndex)].get()->getResource(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_RENDER_TARGET, postcmdlist);


 /*       postcmdlist->CopyResource((*TexTable)["IndirectHistorytex" + std::to_string((nFrame - 1) % 6)].get()->getResource(), IndirectLRI->getResource());
        ResourceBarrierTrans(lastnormalRI->getResource(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ, postcmdlist);
        ResourceBarrierTrans(normalTexRI->getResource(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_GENERIC_READ, postcmdlist);
        ResourceBarrierTrans(DirectMoment1Tex->getResource(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, postcmdlist);
        ResourceBarrierTrans(DirectMoment2Tex->getResource(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, postcmdlist);
        ResourceBarrierTrans(IndirectMoment1Tex->getResource(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, postcmdlist);
        ResourceBarrierTrans(IndirectMoment2Tex->getResource(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, postcmdlist);
        ResourceBarrierTrans(DirectLRI->getResource(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_GENERIC_READ, postcmdlist);
        ResourceBarrierTrans(IndirectLRI->getResource(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_GENERIC_READ, postcmdlist);
        ResourceBarrierTrans(DirectlastM1->getResource(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, postcmdlist);
        ResourceBarrierTrans(DirectlastM2->getResource(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, postcmdlist);
        ResourceBarrierTrans(IndirectlastM1->getResource(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, postcmdlist);
        ResourceBarrierTrans(IndirectlastM2->getResource(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, postcmdlist);
        ResourceBarrierTrans((*TexTable)["DirectHistorytex" + std::to_string((nFrame - 1) % 6)].get()->getResource(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, postcmdlist);
        ResourceBarrierTrans((*TexTable)["IndirectHistorytex" + std::to_string((nFrame - 1) % 6)].get()->getResource(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, postcmdlist);

        ResourceBarrierTrans(DirectFilteredTexRI->getResource(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, postcmdlist);
        ResourceBarrierTrans(IndirectFilteredTexRI->getResource(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, postcmdlist);
        ResourceBarrierTrans(DirectLastFrameBufferRI->getResource(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ, postcmdlist);
        ResourceBarrierTrans(IndirectLastFrameBufferRI->getResource(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ, postcmdlist);*/
        RSI = (*RootSignatureItemTable)["default"].get();
        TextureResourceItem* firstTRI = (*TexTable)["grass"].get();
        auto DSV = (*TexTable)["DSForRt"].get()->getDSVCPU();
        auto passRI = (*BufferResourceItemTable)["pass"].get();
      
        postcmdlist->RSSetViewports(1, stViewPort);
        postcmdlist->RSSetScissorRects(1, stScissorRect);
        postcmdlist->SetPipelineState((*PSOITable)["modulatePass"].get()->PSO);
        postcmdlist->SetGraphicsRootSignature(RSI->rs);
        postcmdlist->SetDescriptorHeaps(2, heaps);
        postcmdlist->OMSetRenderTargets(1, &RTV, true, &DSV);
        //postcmdlist->SetGraphicsRootDescriptorTable(RSI->getSRVTableIndex(7), DirectFilteredTexRI->getSRVGPU(0));
        //postcmdlist->SetGraphicsRootDescriptorTable(RSI->getSRVTableIndex(8), IndirectFilteredTexRI->getSRVGPU(0));
        postcmdlist->SetGraphicsRootDescriptorTable(RSI->getSRVTableIndex(0), firstTRI->getSRVGPU(0));
        postcmdlist->SetGraphicsRootDescriptorTable(RSI->getSamplerTableIndex(0), samplerRI->getSampler());
        postcmdlist->SetGraphicsRootDescriptorTable(RSI->getCBVTableIndex(1), passRI->getCBVGPU());


        //drawRenderItems(SVGFRenderItemTable, postcmdlist, RSI->getCBVTableIndex(0), nullptr);
        postcmdlist->SetPipelineState((*PSOITable)["color"].get()->PSO);
        drawRenderItem((*NonSVGFRenderItemTable)["light"].get(), postcmdlist, RSI->getCBVTableIndex(0));

        ResourceBarrierTrans(DirectLRI->getResource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, postcmdlist);
        ResourceBarrierTrans(IndirectLRI->getResource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET, postcmdlist);
 /*       ResourceBarrierTrans(DirectFilteredTexRI->getResource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, postcmdlist);
        ResourceBarrierTrans(IndirectFilteredTexRI->getResource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, postcmdlist);*/
}