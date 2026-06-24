#include "IndexBuffer.h"
#include "KamataEngine.h"

using namespace KamataEngine;

void IndexBuffer::Create(const UINT size, const UINT stride)
{
	//strideの値によって、１つのインデックスのフォーマットを決める
	assert(stride == 2 || stride == 4); //2byte or 4byde のみ受けつける
	DXGI_FORMAT format = (stride == 2) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;

	//クラス内でdxcommonを利用するために追加
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// インデックスリソースの生成
    // インデックスリソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD; // CPUから書き込むヒープ

	// インデックスリソースの設定
	D3D12_RESOURCE_DESC indexResourceDesc{};
	indexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER; // バッファ
	indexResourceDesc.Width = size;                               // リソースのサイズ。引数sizeで受け取った値

	// バッファの場合はこれらにする決まり
	indexResourceDesc.Height = 1;
	indexResourceDesc.DepthOrArraySize = 1;
	indexResourceDesc.MipLevels = 1;
	indexResourceDesc.SampleDesc.Count = 1;

	// バッファの場合はこれにする決まり
	indexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// 生成したインデックスリソースを取得する
	ID3D12Resource* indexResource = nullptr;

	HRESULT hr = dxCommon->GetDevice()->CreateCommittedResource(
		&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &indexResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&indexResource));
	assert(SUCCEEDED(hr)); // うまくいかなかったときは起動できない

	// 生成したインデックスリソースをとっておく
	indexBuffer_ = indexResource;


	// IndexBufferViewを作成する
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};
	// リソースの説明アドレスから使う

	indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
	// 使用するインデックスフェータの全サイズ
	indexBufferView.SizeInBytes = size;

	// インデックスのフォーマット
	indexBufferView.Format = format; // インデックス1つ分のサイズ

	// IndexBufferView をとっておく
	indexBufferView_ = indexBufferView;
}

ID3D12Resource* IndexBuffer::Get()
{
	return indexBuffer_;
}

D3D12_INDEX_BUFFER_VIEW* IndexBuffer::GetView()
{
	return &indexBufferView_;
}

IndexBuffer::IndexBuffer()
{
}

IndexBuffer::~IndexBuffer()
{
	// インデックスバッファの解放
	if (indexBuffer_) {
		indexBuffer_->Release();
		indexBuffer_ = nullptr;
	}
}