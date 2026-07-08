#include "RootSignature.h"
#include "KamataEngine.h"

using namespace KamataEngine;

void RootSignature::Create()
{
	//すでにインスタンスがあるなら解放する　//createメンバ関数が　２度実行されたときの対処
	if (rootSignature_)
	{
		rootSignature_->Release();
		rootSignature_ = nullptr;
	}

	//クラス内で取得するために追加
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// RootSignature作成
    // 構造体にデータを用意する
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//でスクリプタレンジ
	D3D12_DESCRIPTOR_RANGE srvDescRange[1]{};
	//t0 レジスタを利用可能にする
	srvDescRange[0].BaseShaderRegister = 0;//0から始まる
	srvDescRange[0].NumDescriptors = 1;//数は１つ
	srvDescRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//srv
	srvDescRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//rootParameterの設定
	D3D12_ROOT_PARAMETER rootParameters[1]{};

	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//ディスクリプタテーブル
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//ピクセルシェーダーから見える
	rootParameters[0].DescriptorTable.pDescriptorRanges = srvDescRange;//ディスクリプタレンジのアドレス
	rootParameters[0].DescriptorTable.NumDescriptorRanges = _countof(srvDescRange);//ディスクリプタレンジの数

	descriptionRootSignature.pParameters = rootParameters;//ルートパラメータのアドレス
	descriptionRootSignature.NumParameters = _countof(rootParameters);//ルートパラメータの数

	//samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSampler[1]{};
	staticSampler[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//線形補間
	staticSampler[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//u方向は繰り返し
	staticSampler[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//v方向は繰り返し
	staticSampler[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//w方向は繰り返し
	staticSampler[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//比較しない
	staticSampler[0].MaxLOD = D3D12_FLOAT32_MAX;//最大値
	staticSampler[0].ShaderRegister = 0;//シェーダーレジスタ０
	staticSampler[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//ピクセルシェーダーから見える

	descriptionRootSignature.pStaticSamplers = staticSampler;//スタティックサンプラーのアドレス
	descriptionRootSignature.NumStaticSamplers = _countof(staticSampler);//スタティックサンプラーの数


	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	HRESULT hr = D3D12SerializeRootSignature(
		&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&signatureBlob,
		&errorBlob);

	if (FAILED(hr)) 
	{
		DebugText::GetInstance()->ConsolePrintf(
			reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}

	// ハンドルをもとに生成
	ID3D12RootSignature* rootSignature = nullptr;
	hr = dxCommon->GetDevice()->CreateRootSignature(
		0,
		signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature)
	);
	assert(SUCCEEDED(hr));

	// signatureBlob は RootSignatureの生成後解放してもいい
	signatureBlob->Release();

	// 生成した RootSignature をとっておく
	rootSignature_ = rootSignature;

}

// 生成した RootSignatureを返す
ID3D12RootSignature* RootSignature::Get() 
{
	return rootSignature_;
}

// コンストラクタ
RootSignature::RootSignature() 
{
}

// デストラクタ
RootSignature::~RootSignature()
{
	if (rootSignature_) {
		rootSignature_->Release();
		rootSignature_ = nullptr;
	}
}

