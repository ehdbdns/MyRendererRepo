#pragma once
#include"ResourceItem.h"
#include"rayTracerUtil.h"
class SSSSComponent {
public:
	void createResource(std::unordered_map<std::string, std::unique_ptr< TextureResourceItem>>* textable,//创建资源,在创建资源项时调用
		ID3D12Device4* device, ID3D12DescriptorHeap* srvuavheap, ID3D12DescriptorHeap* rtvheap,
		ID3D12GraphicsCommandList* precmdlist, std::unordered_map<std::string, std::unique_ptr< NON_RT_DS_TextureSegregatedFreeLists>>* NonRtDstable,
		std::unordered_map<std::string, std::unique_ptr< RT_DS_TextureSegregatedFreeLists>>* RtDstable, std::unordered_map<std::string,
		std::unique_ptr< ConstantBufferResourceItem<passconstant>>>(*BufferResourceItemTable), std::unordered_map<std::string, std::unique_ptr< uploadBuddySystem>>*upBSTable,
	std::unordered_map<std::string, std::unique_ptr< defaultBuddySystem>>*defBSTable);

	void preProcess(std::unordered_map<std::string, std::unique_ptr< SamplerResourceItem>>(*SamplerResourceItemTable),//预处理，case0调用
		std::unordered_map<std::string, std::unique_ptr< RootSignatureItem>>(*RootSignatureItemTable), ID3D12GraphicsCommandList* precmdlist,
		std::unordered_map<std::string, std::unique_ptr< computePSOItem>>* computePSOITable, std::unordered_map<std::string, std::unique_ptr< PSOItem>>(*PSOITable),
		D3D12_VIEWPORT* stViewPort4096, D3D12_RECT* stScissorRect4096, ID3D12DescriptorHeap* pIsamplerHeap, std::unordered_map<std::string, std::unique_ptr< RenderItem>>* NonSVGFRenderItemTable,
		std::unordered_map<std::string, std::unique_ptr< ConstantBufferResourceItem<passconstant>>>* BufferResourceItemTable, std::unique_ptr<StructureBufferResourceItem<float>>* GkernelSBRI);
	void update(std::unordered_map<std::string, std::unique_ptr< SamplerResourceItem>>(*SamplerResourceItemTable),//每一帧要做的事，case1调用
		std::unordered_map<std::string, std::unique_ptr< RootSignatureItem>>(*RootSignatureItemTable), ID3D12GraphicsCommandList* precmdlist,
		std::unordered_map<std::string, std::unique_ptr< computePSOItem>>* computePSOITable, std::unordered_map<std::string, std::unique_ptr< PSOItem>>(*PSOITable),
		D3D12_VIEWPORT* stViewPort4096, D3D12_RECT* stScissorRect4096, ID3D12DescriptorHeap* pIsamplerHeap, std::unordered_map<std::string, std::unique_ptr< RenderItem>>* NonSVGFRenderItemTable,
		std::unordered_map<std::string, std::unique_ptr< ConstantBufferResourceItem<passconstant>>>* BufferResourceItemTable,
		std::unique_ptr<StructureBufferResourceItem<float>>* GkernelSBRI, std::unique_ptr<StructureBufferResourceItem<ParallelLight>>* ParaLightsSBRI, SSSSps* updateSSSS);
	void postProcess(std::unordered_map<std::string, std::unique_ptr< SamplerResourceItem>>(*SamplerResourceItemTable), ID3D12GraphicsCommandList* postcmdlist, int RTnote, std::unordered_map<std::string, std::unique_ptr< RootSignatureItem>>(*RootSignatureItemTable),
		std::unordered_map<std::string, std::unique_ptr< computePSOItem>>* computePSOITable, std::unique_ptr<StructureBufferResourceItem<XMFLOAT4>>* SSSSkernelSBRI, ConstantBufferResourceItem<SSSS>* SSSSRI);



    XMVECTOR gaussian(float variance, float r);


    XMFLOAT3 profile(float r);


	EST::vector<XMFLOAT4> calculateKernel(int nSamples);





private:
	std::unordered_map<std::string, std::unique_ptr< TextureResourceItem>>* TexTable;
	ID3D12Device4* Device;
	ID3D12DescriptorHeap* SrvUavHeap;
	std::unique_ptr < ConstantBufferResourceItem <SSSSps>> ssssRI;
};