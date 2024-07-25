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
    auto filteredRI = std::make_unique<TextureResourceItem>(device, srvuavheap, nullptr, nullptr, false, NULL);
    filteredRI->createNON_RT_DS_WritableTex(device, precmdlist, &filteredTexDesc, NonRtDstable);
   filteredRI->createSRVforResourceItem(&SRVdesc,device, HeapOffsetTable[filteredRI->SRVUAVHeap]);
   filteredRI->createUAVforResourceItem(&filteredTexUavDesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap],precmdlist);
   (*TexTable)["DirectFilteredtex"] = std::move(filteredRI);//降噪过程要用的纹理
   filteredRI = std::make_unique<TextureResourceItem>(device, srvuavheap, nullptr, nullptr, false, NULL);
   filteredRI->createNON_RT_DS_WritableTex(device, precmdlist, &filteredTexDesc, NonRtDstable);
   filteredRI->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap]);
   filteredRI->createUAVforResourceItem(&filteredTexUavDesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap], precmdlist);
   (*TexTable)["DirectFilteredtex1"] = std::move(filteredRI);//降噪过程要用的纹理
   filteredRI = std::make_unique<TextureResourceItem>(device, srvuavheap, nullptr, nullptr, false, NULL);
   filteredRI->createNON_RT_DS_WritableTex(device, precmdlist, &filteredTexDesc, NonRtDstable);
   filteredRI->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap]);
   filteredRI->createUAVforResourceItem(&filteredTexUavDesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap], precmdlist);
   (*TexTable)["IndirectFilteredtex"] = std::move(filteredRI);//降噪过程要用的纹理
   filteredRI = std::make_unique<TextureResourceItem>(device, srvuavheap, nullptr, nullptr, false, NULL);
   filteredRI->createNON_RT_DS_WritableTex(device, precmdlist, &filteredTexDesc, NonRtDstable);
   filteredRI->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap]);
   filteredRI->createUAVforResourceItem(&filteredTexUavDesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap], precmdlist);
   (*TexTable)["IndirectFilteredtex1"] = std::move(filteredRI);//降噪过程要用的纹理
   filteredRI = std::make_unique<TextureResourceItem>(device, srvuavheap, nullptr, nullptr, false, NULL);
   filteredRI->createNON_RT_DS_WritableTex(device, precmdlist, &filteredTexDesc, NonRtDstable);
   filteredRI->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap]);
   filteredRI->createUAVforResourceItem(&filteredTexUavDesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap], precmdlist);
   (*TexTable)["DirectMoment1tex"] = std::move(filteredRI);//moment1
   filteredRI = std::make_unique<TextureResourceItem>(device, srvuavheap, nullptr, nullptr, false, NULL);
   filteredRI->createNON_RT_DS_WritableTex(device, precmdlist, &filteredTexDesc, NonRtDstable);
   filteredRI->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap]);
   filteredRI->createUAVforResourceItem(&filteredTexUavDesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap], precmdlist);
   (*TexTable)["DirectMoment2tex"] = std::move(filteredRI);//moment2
   filteredRI = std::make_unique<TextureResourceItem>(device, srvuavheap, nullptr, nullptr, false, NULL);
   filteredRI->createNON_RT_DS_WritableTex(device, precmdlist, &filteredTexDesc, NonRtDstable);
   filteredRI->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap]);
   filteredRI->createUAVforResourceItem(&filteredTexUavDesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap], precmdlist);
   (*TexTable)["IndirectMoment1tex"] = std::move(filteredRI);//moment1
   filteredRI = std::make_unique<TextureResourceItem>(device, srvuavheap, nullptr, nullptr, false, NULL);
   filteredRI->createNON_RT_DS_WritableTex(device, precmdlist, &filteredTexDesc, NonRtDstable);
   filteredRI->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap]);
   filteredRI->createUAVforResourceItem(&filteredTexUavDesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap], precmdlist);
   (*TexTable)["IndirectMoment2tex"] = std::move(filteredRI);//moment2
   filteredRI = std::make_unique<TextureResourceItem>(device, srvuavheap, nullptr, nullptr, false, NULL);
   filteredRI->createNON_RT_DS_WritableTex(device, precmdlist, &filteredTexDesc, NonRtDstable);
   filteredRI->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap]);
   filteredRI->createUAVforResourceItem(&filteredTexUavDesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap], precmdlist);
   (*TexTable)["DirectVariancetex"] = std::move(filteredRI);//variance
   filteredRI = std::make_unique<TextureResourceItem>(device, srvuavheap, nullptr, nullptr, false, NULL);
   filteredRI->createNON_RT_DS_WritableTex(device, precmdlist, &filteredTexDesc, NonRtDstable);
   filteredRI->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap]);
   filteredRI->createUAVforResourceItem(&filteredTexUavDesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap], precmdlist);
   (*TexTable)["IndirectVariancetex"] = std::move(filteredRI);//variance
    filteredTexDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    filteredTexDesc.Width = 1024;
    filteredTexDesc.Height = 768;
    filteredTexUavDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    filteredRI = std::make_unique<TextureResourceItem>(device, srvuavheap, nullptr, nullptr, false, NULL);
    filteredRI->createNON_RT_DS_WritableTex(device, precmdlist, &filteredTexDesc, NonRtDstable);
    filteredRI->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap]);
    filteredRI->createUAVforResourceItem(&filteredTexUavDesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap], precmdlist);
    (*TexTable)["CDF-1tex"] = std::move(filteredRI);//weightMap

    //SVGF

   for (int i = 0;i < 6;i++) {
       filteredRI = std::make_unique<TextureResourceItem>(device, srvuavheap, nullptr, nullptr, false, NULL);
       filteredRI->createNON_RT_DS_WritableTex(device, precmdlist, &filteredTexDesc, NonRtDstable);
       filteredRI->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap]);
       filteredRI->createUAVforResourceItem(&filteredTexUavDesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap], precmdlist);
       (*TexTable)["DirectHistorytex"+std::to_string(i)] = std::move(filteredRI);//historyTex
   }
   for (int i = 0;i < 6;i++) {
       filteredRI = std::make_unique<TextureResourceItem>(device, srvuavheap, nullptr, nullptr, false, NULL);
       filteredRI->createNON_RT_DS_WritableTex(device, precmdlist, &filteredTexDesc, NonRtDstable);
       filteredRI->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap]);
       filteredRI->createUAVforResourceItem(&filteredTexUavDesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap], precmdlist);
       (*TexTable)["IndirectHistorytex" + std::to_string(i)] = std::move(filteredRI);//historyTex
   }
   for (int i = 1;i < 3;i++) {
       filteredRI = std::make_unique<TextureResourceItem>(device, srvuavheap, nullptr, nullptr, false, NULL);
       filteredRI->createNON_RT_DS_WritableTex(device, precmdlist, &filteredTexDesc, NonRtDstable);
       filteredRI->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap]);
       filteredRI->createUAVforResourceItem(&filteredTexUavDesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap], precmdlist);
       (*TexTable)["DirectLastMoment" + std::to_string(i)] = std::move(filteredRI);//lastmomentTex
   }
   for (int i = 1;i < 3;i++) {
       filteredRI = std::make_unique<TextureResourceItem>(device, srvuavheap, nullptr, nullptr, false, NULL);
       filteredRI->createNON_RT_DS_WritableTex(device, precmdlist, &filteredTexDesc, NonRtDstable);
       filteredRI->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap]);
       filteredRI->createUAVforResourceItem(&filteredTexUavDesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap], precmdlist);
       (*TexTable)["IndirectLastMoment" + std::to_string(i)] = std::move(filteredRI);//lastmomentTex
   }
    filteredTexDesc.Width = 2048;
    filteredTexDesc.Height = 1536;
    filteredTexDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    filteredRI = std::make_unique<TextureResourceItem>(device, srvuavheap, nullptr, nullptr, false, NULL);
    filteredRI->createNON_RT_DS_WritableTex(device, precmdlist, &filteredTexDesc, NonRtDstable);
    filteredRI->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[filteredRI->SRVUAVHeap]);
    (*TexTable)["lastnormal"] = std::move(filteredRI);//lastNormalTex

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

   auto DirectLRI = std::make_unique<TextureResourceItem>(device, srvuavheap, rtvheap, nullptr, false, NULL);
   DirectLRI->createRT_DS_WritableTex(device, precmdlist, &filteredTexDesc, nullptr, (*RtDstable)["RTDS3"].get());
   DirectLRI->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[lastFrameBuffer->SRVUAVHeap]);
   DirectLRI->createRTVforResourceItem(&rtvDesc, device, HeapOffsetTable[lastFrameBuffer->RTVHeap], precmdlist);
   (*TexTable)["DirectL"] = std::move(DirectLRI);//directL图

   auto IndirectLRI = std::make_unique<TextureResourceItem>(device, srvuavheap, rtvheap, nullptr, false, NULL);
   IndirectLRI->createRT_DS_WritableTex(device, precmdlist, &filteredTexDesc, nullptr, (*RtDstable)["RTDS3"].get());
   IndirectLRI->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[lastFrameBuffer->SRVUAVHeap]);
   IndirectLRI->createRTVforResourceItem(&rtvDesc, device, HeapOffsetTable[lastFrameBuffer->RTVHeap], precmdlist);
   (*TexTable)["IndirectL"] = std::move(IndirectLRI);//indirectL图
    IndirectLRI = std::make_unique<TextureResourceItem>(device, srvuavheap, rtvheap, nullptr, false, NULL);
    IndirectLRI->createRT_DS_WritableTex(device, precmdlist, &filteredTexDesc, nullptr, (*RtDstable)["RTDS4"].get());
    IndirectLRI->createSRVforResourceItem(&SRVdesc, device, HeapOffsetTable[lastFrameBuffer->SRVUAVHeap]);
    IndirectLRI->createRTVforResourceItem(&rtvDesc, device, HeapOffsetTable[lastFrameBuffer->RTVHeap], precmdlist);
    (*TexTable)["modulatetex"] = std::move(IndirectLRI);//modulate图
}
void SVGFComponent::preProcess(std::unordered_map<std::string, std::unique_ptr< SamplerResourceItem>>(*SamplerResourceItemTable),//预处理，case0调用
    std::unordered_map<std::string, std::unique_ptr< RootSignatureItem>>(*RootSignatureItemTable), ID3D12GraphicsCommandList* precmdlist,
    std::unordered_map<std::string, std::unique_ptr< computePSOItem>>* computePSOITable, std::unordered_map<std::string, std::unique_ptr< PSOItem>>(*PSOITable),
    D3D12_VIEWPORT* stViewPort4096, D3D12_RECT* stScissorRect4096, ID3D12DescriptorHeap* pIsamplerHeap, std::unordered_map<std::string, std::unique_ptr< RenderItem>>* NonSVGFRenderItemTable,
    std::unordered_map<std::string, std::unique_ptr< ConstantBufferResourceItem<passconstant>>>* BufferResourceItemTable, std::unique_ptr<StructureBufferResourceItem<float>>* GkernelSBRI) {

    /*                   auto lastDirectRI = TextureResourceItemTable["DirectLastFrameBuffer"].get();
                       auto lastIndirectRI = TextureResourceItemTable["IndirectLastFrameBuffer"].get();

                       ResourceBarrierTrans(lastDirectRI->getResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ, pIcmdlistpre.Get());
                       ResourceBarrierTrans(lastIndirectRI->getResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ,pIcmdlistpre.Get());


               */

}
void SVGFComponent::update(std::unordered_map<std::string, std::unique_ptr< SamplerResourceItem>>(*SamplerResourceItemTable),//每一帧要做的事，case1调用
    std::unordered_map<std::string, std::unique_ptr< RootSignatureItem>>(*RootSignatureItemTable), ID3D12GraphicsCommandList* precmdlist,
    std::unordered_map<std::string, std::unique_ptr< computePSOItem>>* computePSOITable, std::unordered_map<std::string, std::unique_ptr< PSOItem>>(*PSOITable),
    D3D12_VIEWPORT* stViewPort4096, D3D12_RECT* stScissorRect4096, ID3D12DescriptorHeap* pIsamplerHeap, std::unordered_map<std::string, std::unique_ptr< RenderItem>>* NonSVGFRenderItemTable,
    std::unordered_map<std::string, std::unique_ptr< ConstantBufferResourceItem<passconstant>>>* BufferResourceItemTable,
    std::unique_ptr<StructureBufferResourceItem<float>>* GkernelSBRI, std::unique_ptr<StructureBufferResourceItem<ParallelLight>>* ParaLightsSBRI) {

}

void SVGFComponent::postProcess() {
    //SVGF
//                   pIcmdlistpre->ClearRenderTargetView(DirectLRI->getRTVCPU(), Colors::Black, 0, nullptr);
 //             pIcmdlistpre->ClearRenderTargetView(IndirectLRI->getRTVCPU(), Colors::Black, 0, nullptr);//这个是prelist


    //if (hasSVGF) {
    //    pIcmdlistpost->SetComputeRootSignature(pICSRootSignature.Get());
    //    auto currentBackBufferRI = TextureResourceItemTable["backBuffer" + std::to_string(nFrameIndex)].get();
    //    auto wposTexRI = TextureResourceItemTable["wpos"].get();
    //    auto normalTexRI = TextureResourceItemTable["normal"].get();
    //    auto zTexRI = TextureResourceItemTable["z"].get();
    //    auto gzTexRI = TextureResourceItemTable["gz"].get();
    //    auto samplerRI = SamplerResourceItemTable["default"].get();
    //    auto RSI = RootSignatureItemTable["cs"].get();
    //    auto DirectFilteredTexRI = TextureResourceItemTable["DirectFilteredtex"].get();
    //    auto IndirectFilteredTexRI = TextureResourceItemTable["IndirectFilteredtex"].get();
    //    auto DirectFilteredTex1RI = TextureResourceItemTable["DirectFilteredtex1"].get();
    //    auto IndirectFilteredTex1RI = TextureResourceItemTable["IndirectFilteredtex1"].get();
    //    auto DirectLastFrameBufferRI = TextureResourceItemTable["DirectLastFrameBuffer"].get();
    //    auto IndirectLastFrameBufferRI = TextureResourceItemTable["IndirectLastFrameBuffer"].get();
    //    auto DirectMoment1Tex = TextureResourceItemTable["DirectMoment1tex"].get();
    //    auto IndirectMoment1Tex = TextureResourceItemTable["IndirectMoment1tex"].get();
    //    auto DirectMoment2Tex = TextureResourceItemTable["DirectMoment2tex"].get();
    //    auto IndirectMoment2Tex = TextureResourceItemTable["IndirectMoment2tex"].get();
    //    auto DirectVarianceTexRI = TextureResourceItemTable["DirectVariancetex"].get();
    //    auto IndirectVarianceTexRI = TextureResourceItemTable["IndirectVariancetex"].get();
    //    auto DirectlastM1 = TextureResourceItemTable["DirectLastMoment1"].get();
    //    auto DirectlastM2 = TextureResourceItemTable["DirectLastMoment2"].get();
    //    auto IndirectlastM1 = TextureResourceItemTable["IndirectLastMoment1"].get();
    //    auto IndirectlastM2 = TextureResourceItemTable["IndirectLastMoment2"].get();
    //    auto lastnormalRI = TextureResourceItemTable["lastnormal"].get();
    //    auto weightRI = TextureResourceItemTable["weighttex"].get();
    //    auto CDF_1texRI = TextureResourceItemTable["CDF-1tex"].get();
    //    ResourceBarrierTrans(DirectLRI->getResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ, pIcmdlistpost.Get());
    //    ResourceBarrierTrans(IndirectLRI->getResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ, pIcmdlistpost.Get());
    //    ID3D12DescriptorHeap* heaps[2] = { wposTexRI->SRVUAVHeap,samplerRI->samplerHeap };
    //    pIcmdlistpost->SetDescriptorHeaps(2, heaps);
    //    pIcmdlistpost->SetComputeRootDescriptorTable(RSI->getSRVTableIndex(0), DirectLRI->getSRVGPU(0));
    //    pIcmdlistpost->SetComputeRootDescriptorTable(RSI->getSRVTableIndex(1), wposTexRI->getSRVGPU(0));
    //    pIcmdlistpost->SetComputeRootDescriptorTable(RSI->getSRVTableIndex(2), normalTexRI->getSRVGPU(0));
    //    pIcmdlistpost->SetComputeRootDescriptorTable(RSI->getSRVTableIndex(5), zTexRI->getSRVGPU(0));
    //    pIcmdlistpost->SetComputeRootDescriptorTable(RSI->getSRVTableIndex(6), gzTexRI->getSRVGPU(0));
    //    pIcmdlistpost->SetComputeRootDescriptorTable(RSI->getSRVTableIndex(4), hSBRI.get()->getSRVGPU());
    //    pIcmdlistpost->SetComputeRootDescriptorTable(RSI->getSRVTableIndex(7), lastnormalRI->getSRVGPU(0));
    //    pIcmdlistpost->SetComputeRootDescriptorTable(RSI->getSamplerTableIndex(0), samplerRI->getSampler());
    //    pIcmdlistpost->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(0), DirectFilteredTexRI->getUAVGPU(0));
    //    pIcmdlistpost->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(1), DirectMoment1Tex->getUAVGPU(0));
    //    pIcmdlistpost->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(2), DirectMoment2Tex->getUAVGPU(0));
    //    pIcmdlistpost->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(5), DirectlastM1->getUAVGPU(0));
    //    pIcmdlistpost->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(6), DirectlastM2->getUAVGPU(0));
    //    pIcmdlistpost->SetComputeRootDescriptorTable(RSI->getCBVTableIndex(0), lastvpmatRI->getCBVGPU());
    //    pIcmdlistpost->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(3), TextureResourceItemTable["DirectHistorytex" + std::to_string((nFrame - 1) % 6)].get()->getUAVGPU(0));
    //    pIcmdlistpost->SetPipelineState(computePSOITable["accumulationPass"].get()->PSO);
    //    pIcmdlistpost->Dispatch(64, 48, 1);//累计当前帧到momentTex和historyTex
    //    pIcmdlistpost->SetComputeRootDescriptorTable(RSI->getSRVTableIndex(0), IndirectLRI->getSRVGPU(0));
    //    pIcmdlistpost->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(1), IndirectMoment1Tex->getUAVGPU(0));
    //    pIcmdlistpost->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(2), IndirectMoment2Tex->getUAVGPU(0));
    //    pIcmdlistpost->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(5), IndirectlastM1->getUAVGPU(0));
    //    pIcmdlistpost->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(6), IndirectlastM2->getUAVGPU(0));
    //    pIcmdlistpost->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(3), TextureResourceItemTable["IndirectHistorytex" + std::to_string((nFrame - 1) % 6)].get()->getUAVGPU(0));
    //    pIcmdlistpost->Dispatch(64, 48, 1);//累计当前帧到momentTex和historyTex
    //    pIcmdlistpost->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(4), IndirectVarianceTexRI->getUAVGPU(0));
    //    pIcmdlistpost->SetComputeRootDescriptorTable(RSI->getSRVTableIndex(3), offsetArray[0].get()->getSRVGPU());
    //    pIcmdlistpost->SetPipelineState(computePSOITable["variancePass"].get()->PSO);
    //    pIcmdlistpost->Dispatch(64, 48, 1);//计算当前帧的variance作为filter的输入
    //    pIcmdlistpost->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(1), DirectMoment1Tex->getUAVGPU(0));
    //    pIcmdlistpost->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(2), DirectMoment2Tex->getUAVGPU(0));
    //    pIcmdlistpost->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(4), DirectVarianceTexRI->getUAVGPU(0));
    //    pIcmdlistpost->Dispatch(64, 48, 1);//计算当前帧的variance作为filter的输入
    //    ResourceBarrierTrans(DirectFilteredTex1RI->getResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ, pIcmdlistpost.Get());
    //    pIcmdlistpost->SetComputeRootDescriptorTable(RSI->getSRVTableIndex(0), DirectLRI->getSRVGPU(0));
    //    pIcmdlistpost->SetComputeRootDescriptorTable(RSI->getSRVTableIndex(8), CDF_1texRI->getSRVGPU(0));
    //    pIcmdlistpost->SetPipelineState(computePSOITable["denoise"].get()->PSO);
    //    for (int i = 0;i < 5;i++) {
    //        pIcmdlistpost->SetComputeRootDescriptorTable(RSI->getSRVTableIndex(3), offsetArray[i].get()->getSRVGPU());
    //        pIcmdlistpost->Dispatch(64, 48, 1);//降噪！
    //        if (i == 4)
    //            break;
    //        if (i % 2 == 0) {
    //            ResourceBarrierTrans(DirectFilteredTexRI->getResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ, pIcmdlistpost.Get());
    //            ResourceBarrierTrans(DirectFilteredTex1RI->getResource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, pIcmdlistpost.Get());
    //            pIcmdlistpost->SetComputeRootDescriptorTable(RSI->getSRVTableIndex(0), DirectFilteredTexRI->getSRVGPU(0));
    //            pIcmdlistpost->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(0), DirectFilteredTex1RI->getUAVGPU(0));
    //        }
    //        else {
    //            ResourceBarrierTrans(DirectFilteredTex1RI->getResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ, pIcmdlistpost.Get());
    //            ResourceBarrierTrans(DirectFilteredTexRI->getResource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, pIcmdlistpost.Get());
    //            pIcmdlistpost->SetComputeRootDescriptorTable(RSI->getSRVTableIndex(0), DirectFilteredTex1RI->getSRVGPU(0));
    //            pIcmdlistpost->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(0), DirectFilteredTexRI->getUAVGPU(0));
    //        }
    //    }
    //    ResourceBarrierTrans(DirectFilteredTex1RI->getResource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, pIcmdlistpost.Get());

    //    ResourceBarrierTrans(IndirectFilteredTex1RI->getResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ, pIcmdlistpost.Get());
    //    pIcmdlistpost->SetComputeRootDescriptorTable(RSI->getSRVTableIndex(0), IndirectLRI->getSRVGPU(0));
    //    pIcmdlistpost->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(4), IndirectVarianceTexRI->getUAVGPU(0));
    //    pIcmdlistpost->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(0), IndirectFilteredTexRI->getUAVGPU(0));
    //    for (int i = 0;i < 5;i++) {
    //        pIcmdlistpost->SetComputeRootDescriptorTable(RSI->getSRVTableIndex(3), offsetArray[i].get()->getSRVGPU());
    //        pIcmdlistpost->Dispatch(64, 48, 1);//降噪！
    //        if (i == 4)
    //            break;
    //        if (i % 2 == 0) {
    //            ResourceBarrierTrans(IndirectFilteredTexRI->getResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ, pIcmdlistpost.Get());
    //            ResourceBarrierTrans(IndirectFilteredTex1RI->getResource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, pIcmdlistpost.Get());
    //            pIcmdlistpost->SetComputeRootDescriptorTable(RSI->getSRVTableIndex(0), IndirectFilteredTexRI->getSRVGPU(0));
    //            pIcmdlistpost->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(0), IndirectFilteredTex1RI->getUAVGPU(0));
    //        }
    //        else {
    //            ResourceBarrierTrans(IndirectFilteredTex1RI->getResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ, pIcmdlistpost.Get());
    //            ResourceBarrierTrans(IndirectFilteredTexRI->getResource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, pIcmdlistpost.Get());
    //            pIcmdlistpost->SetComputeRootDescriptorTable(RSI->getSRVTableIndex(0), IndirectFilteredTex1RI->getSRVGPU(0));
    //            pIcmdlistpost->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(0), IndirectFilteredTexRI->getUAVGPU(0));
    //        }
    //    }
    //    ResourceBarrierTrans(IndirectFilteredTex1RI->getResource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, pIcmdlistpost.Get());


    //    pIcmdlistpost->SetPipelineState(computePSOITable["mixPass"].get()->PSO);
    //    ResourceBarrierTrans(DirectFilteredTexRI->getResource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, pIcmdlistpost.Get());
    //    ResourceBarrierTrans(IndirectFilteredTexRI->getResource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, pIcmdlistpost.Get());
    //    pIcmdlistpost->SetComputeRootDescriptorTable(RSI->getSRVTableIndex(0), DirectLastFrameBufferRI->getSRVGPU(0));
    //    pIcmdlistpost->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(0), DirectFilteredTexRI->getUAVGPU(0));
    //    pIcmdlistpost->SetComputeRootDescriptorTable(RSI->getCBVTableIndex(0), lastvpmatRI->getCBVGPU());
    //    pIcmdlistpost->Dispatch(64, 48, 1);//混合！
    //    pIcmdlistpost->SetComputeRootDescriptorTable(RSI->getSRVTableIndex(0), IndirectLastFrameBufferRI->getSRVGPU(0));
    //    pIcmdlistpost->SetComputeRootDescriptorTable(RSI->getUAVTableIndex(0), IndirectFilteredTexRI->getUAVGPU(0));
    //    pIcmdlistpost->Dispatch(64, 48, 1);//混合！
    //    ResourceBarrierTrans(lastnormalRI->getResource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST, pIcmdlistpost.Get());
    //    ResourceBarrierTrans(normalTexRI->getResource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_SOURCE, pIcmdlistpost.Get());
    //    ResourceBarrierTrans(DirectFilteredTexRI->getResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE, pIcmdlistpost.Get());
    //    ResourceBarrierTrans(IndirectFilteredTexRI->getResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE, pIcmdlistpost.Get());
    //    ResourceBarrierTrans(DirectLastFrameBufferRI->getResource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST, pIcmdlistpost.Get());
    //    ResourceBarrierTrans(IndirectLastFrameBufferRI->getResource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST, pIcmdlistpost.Get());
    //    ResourceBarrierTrans(DirectMoment1Tex->getResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE, pIcmdlistpost.Get());
    //    ResourceBarrierTrans(DirectMoment2Tex->getResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE, pIcmdlistpost.Get());
    //    ResourceBarrierTrans(IndirectMoment1Tex->getResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE, pIcmdlistpost.Get());
    //    ResourceBarrierTrans(IndirectMoment2Tex->getResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE, pIcmdlistpost.Get());
    //    ResourceBarrierTrans(DirectLRI->getResource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_SOURCE, pIcmdlistpost.Get());
    //    ResourceBarrierTrans(IndirectLRI->getResource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_SOURCE, pIcmdlistpost.Get());
    //    ResourceBarrierTrans(DirectlastM1->getResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_DEST, pIcmdlistpost.Get());
    //    ResourceBarrierTrans(DirectlastM2->getResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_DEST, pIcmdlistpost.Get());
    //    ResourceBarrierTrans(IndirectlastM1->getResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_DEST, pIcmdlistpost.Get());
    //    ResourceBarrierTrans(IndirectlastM2->getResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_DEST, pIcmdlistpost.Get());
    //    ResourceBarrierTrans(TextureResourceItemTable["DirectHistorytex" + std::to_string((nFrame - 1) % 6)].get()->getResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_DEST, pIcmdlistpost.Get());
    //    ResourceBarrierTrans(TextureResourceItemTable["IndirectHistorytex" + std::to_string((nFrame - 1) % 6)].get()->getResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_DEST, pIcmdlistpost.Get());
    //    pIcmdlistpost->CopyResource(IndirectLastFrameBufferRI->getResource(), IndirectFilteredTexRI->getResource());
    //    pIcmdlistpost->CopyResource(DirectLastFrameBufferRI->getResource(), DirectFilteredTexRI->getResource());
    //    pIcmdlistpost->CopyResource(DirectlastM1->getResource(), DirectMoment1Tex->getResource());
    //    pIcmdlistpost->CopyResource(DirectlastM2->getResource(), DirectMoment2Tex->getResource());
    //    pIcmdlistpost->CopyResource(IndirectlastM1->getResource(), IndirectMoment1Tex->getResource());
    //    pIcmdlistpost->CopyResource(IndirectlastM2->getResource(), IndirectMoment2Tex->getResource());
    //    pIcmdlistpost->CopyResource(lastnormalRI->getResource(), normalTexRI->getResource());
    //    pIcmdlistpost->CopyResource(TextureResourceItemTable["DirectHistorytex" + std::to_string((nFrame - 1) % 6)].get()->getResource(), DirectLRI->getResource());
    //    pIcmdlistpost->CopyResource(TextureResourceItemTable["IndirectHistorytex" + std::to_string((nFrame - 1) % 6)].get()->getResource(), IndirectLRI->getResource());
    //    ResourceBarrierTrans(lastnormalRI->getResource(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ, pIcmdlistpost.Get());
    //    ResourceBarrierTrans(normalTexRI->getResource(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_GENERIC_READ, pIcmdlistpost.Get());
    //    ResourceBarrierTrans(DirectMoment1Tex->getResource(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, pIcmdlistpost.Get());
    //    ResourceBarrierTrans(DirectMoment2Tex->getResource(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, pIcmdlistpost.Get());
    //    ResourceBarrierTrans(IndirectMoment1Tex->getResource(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, pIcmdlistpost.Get());
    //    ResourceBarrierTrans(IndirectMoment2Tex->getResource(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, pIcmdlistpost.Get());
    //    ResourceBarrierTrans(DirectLRI->getResource(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_GENERIC_READ, pIcmdlistpost.Get());
    //    ResourceBarrierTrans(IndirectLRI->getResource(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_GENERIC_READ, pIcmdlistpost.Get());
    //    ResourceBarrierTrans(DirectlastM1->getResource(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, pIcmdlistpost.Get());
    //    ResourceBarrierTrans(DirectlastM2->getResource(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, pIcmdlistpost.Get());
    //    ResourceBarrierTrans(IndirectlastM1->getResource(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, pIcmdlistpost.Get());
    //    ResourceBarrierTrans(IndirectlastM2->getResource(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, pIcmdlistpost.Get());
    //    ResourceBarrierTrans(TextureResourceItemTable["DirectHistorytex" + std::to_string((nFrame - 1) % 6)].get()->getResource(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, pIcmdlistpost.Get());
    //    ResourceBarrierTrans(TextureResourceItemTable["IndirectHistorytex" + std::to_string((nFrame - 1) % 6)].get()->getResource(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, pIcmdlistpost.Get());

    //    ResourceBarrierTrans(DirectFilteredTexRI->getResource(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, pIcmdlistpost.Get());
    //    ResourceBarrierTrans(IndirectFilteredTexRI->getResource(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, pIcmdlistpost.Get());
    //    ResourceBarrierTrans(DirectLastFrameBufferRI->getResource(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ, pIcmdlistpost.Get());
    //    ResourceBarrierTrans(IndirectLastFrameBufferRI->getResource(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ, pIcmdlistpost.Get());
    //    RSI = RootSignatureItemTable["default"].get();
    //    TextureResourceItem* firstTRI = TextureResourceItemTable["grass"].get();
    //    auto DSV = TextureResourceItemTable["DSForRt"].get()->getDSVCPU();
    //    auto passRI = BufferResourceItemTable["pass"].get();
    //    auto RTV = CD3DX12_CPU_DESCRIPTOR_HANDLE(pIRTVHeap->GetCPUDescriptorHandleForHeapStart(), nFrameIndex, pID3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
    //    pIcmdlistpost->RSSetViewports(1, &stViewPort);
    //    pIcmdlistpost->RSSetScissorRects(1, &stScissorRect);
    //    pIcmdlistpost->SetPipelineState(PSOITable["modulatePass"].get()->PSO);
    //    pIcmdlistpost->SetGraphicsRootSignature(pIRootSignature.Get());
    //    pIcmdlistpost->SetDescriptorHeaps(2, heaps);
    //    pIcmdlistpost->OMSetRenderTargets(1, &RTV, true, &DSV);
    //    pIcmdlistpost->SetGraphicsRootDescriptorTable(RSI->getSRVTableIndex(7), DirectFilteredTexRI->getSRVGPU(0));
    //    pIcmdlistpost->SetGraphicsRootDescriptorTable(RSI->getSRVTableIndex(8), IndirectFilteredTexRI->getSRVGPU(0));
    //    pIcmdlistpost->SetGraphicsRootDescriptorTable(RSI->getSRVTableIndex(0), firstTRI->getSRVGPU(0));
    //    pIcmdlistpost->SetGraphicsRootDescriptorTable(RSI->getSamplerTableIndex(0), samplerRI->getSampler());
    //    pIcmdlistpost->SetGraphicsRootDescriptorTable(RSI->getCBVTableIndex(1), passRI->getCBVGPU());
    //    drawRenderItems(&SVGFRenderItemTable, pIcmdlistpost.Get(), RSI->getCBVTableIndex(0), nullptr);
    //    ResourceBarrierTrans(DirectLRI->getResource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET, pIcmdlistpost.Get());
    //    ResourceBarrierTrans(IndirectLRI->getResource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET, pIcmdlistpost.Get());
    //    ResourceBarrierTrans(DirectFilteredTexRI->getResource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, pIcmdlistpost.Get());
    //    ResourceBarrierTrans(IndirectFilteredTexRI->getResource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, pIcmdlistpost.Get());
    //}
}