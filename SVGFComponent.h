#pragma once
#include"ResourceItem.h"
#include"rayTracerUtil.h"
class SVGFComponent {
public:
	void createResource(std::unordered_map<std::string, std::unique_ptr< TextureResourceItem>>* textable,//创建资源,在创建资源项时调用
		ID3D12Device4* device, ID3D12DescriptorHeap* srvuavheap, ID3D12DescriptorHeap* rtvheap,
		ID3D12GraphicsCommandList* precmdlist, std::unordered_map<std::string, std::unique_ptr< NON_RT_DS_TextureSegregatedFreeLists>>* NonRtDstable,
		std::unordered_map<std::string, std::unique_ptr< RT_DS_TextureSegregatedFreeLists>>* RtDstable);

	void preProcess(std::unordered_map<std::string, std::unique_ptr< SamplerResourceItem>>(*SamplerResourceItemTable),//预处理，case0调用
		std::unordered_map<std::string, std::unique_ptr< RootSignatureItem>>(*RootSignatureItemTable), ID3D12GraphicsCommandList* precmdlist,
		std::unordered_map<std::string, std::unique_ptr< computePSOItem>>* computePSOITable, std::unordered_map<std::string, std::unique_ptr< PSOItem>>(*PSOITable),
		D3D12_VIEWPORT* stViewPort4096, D3D12_RECT* stScissorRect4096, ID3D12DescriptorHeap* pIsamplerHeap, std::unordered_map<std::string, std::unique_ptr< RenderItem>>* NonSVGFRenderItemTable,
		std::unordered_map<std::string, std::unique_ptr< ConstantBufferResourceItem<passconstant>>>* BufferResourceItemTable, std::unique_ptr<StructureBufferResourceItem<float>>* GkernelSBRI);
	void update(ID3D12GraphicsCommandList* precmdlist);
	void postProcess(std::unordered_map<std::string, std::unique_ptr< SamplerResourceItem>>(*SamplerResourceItemTable), ID3D12GraphicsCommandList* postcmdlist, int RTnote, std::unordered_map<std::string, std::unique_ptr< RootSignatureItem>>(*RootSignatureItemTable),
		std::unordered_map<std::string, std::unique_ptr< computePSOItem>>* computePSOITable, int nFrameIndex, int nFrame, std::unique_ptr < StructureBufferResourceItem<float>>* hSBRI, std::unique_ptr < ConstantBufferResourceItem <lastVPmat>>* lastvpmatRI, std::unique_ptr < StructureBufferResourceItem<XMINT2>>* offsetArray,
		std::unordered_map<std::string, std::unique_ptr< ConstantBufferResourceItem<passconstant>>>* BufferResourceItemTable, D3D12_VIEWPORT* stViewPort, D3D12_RECT* stScissorRect, ID3D12DescriptorHeap* pIRTVHeap, std::unordered_map<std::string, std::unique_ptr< PSOItem>>(*PSOITable), std::unordered_map<std::string, std::unique_ptr< RenderItem>>*SVGFRenderItemTable, std::unordered_map<std::string, std::unique_ptr< RenderItem>>* NonSVGFRenderItemTable);
private:
	std::unordered_map<std::string, std::unique_ptr< TextureResourceItem>>* TexTable;
	ID3D12Device4* Device;
	ID3D12DescriptorHeap* SrvUavHeap;
};