#include "KamataEngine.h"
#include "Shader.h"
#include "RootSignature.h"
#include "PipelineState.h"
#include "VertexBuffer.h"

using namespace KamataEngine;

void SetupPipelineState(PipelineState& pipelineState, RootSignature& rootSignature, Shader& vs, Shader& ps)
{
	

	 //InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[1] = {};

	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	//BlendState
	D3D12_BLEND_DESC blendDesc{};
	//全ての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//RasterizerState
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面（反時計回り）をカリングする
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//塗りつぶしモードをソリッドにする（ワイヤーフレームならD3D12_FILL_MODE_WIREFRAME）
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;


	// PSO(PipelineStateObject)の生成 ----------
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature.Get();   // RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;    // InputLayout
	graphicsPipelineStateDesc.VS = { vs.GetDxcBlob()->GetBufferPointer(), vs.GetDxcBlob()->GetBufferSize() }; // VertexShader
	graphicsPipelineStateDesc.PS = { ps.GetDxcBlob()->GetBufferPointer(), ps.GetDxcBlob()->GetBufferSize() }; // PixelShader

	graphicsPipelineStateDesc.BlendState = blendDesc;           // BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc; // RasterizerState

	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1; // 1つのRTVに書き込む ※2つ同時にしようと思えばできる
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	// 利用するトポロジ(形状)のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// どのように画面に色を打ち込むかの設定(今は気にしなくていい)
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	pipelineState.Create(graphicsPipelineStateDesc);

}

//関数プロント宣言
ID3D10Blob* CompileShader(const std::wstring& filePath, const std::string& shaderModel);
// Windowsアプリでのエントリーポイント
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// エンジン初期化
	Initialize(L"LE3D_02_イセダ_コテツ");
	// DirectX取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	int32_t w = dxCommon->GetBackBufferWidth();
	int32_t h = dxCommon->GetBackBufferHeight();
	DebugText::GetInstance()->ConsolePrintf(std::format("width: {}, height: {}\n", w, h).c_str());

	//DirectXCommonクラスが管理している、コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon->GetCommandList();

	// RootSignatureの生成 ----------
	RootSignature rootSignature;
	rootSignature.Create();

	//頂点シェーダーの読み込みとコンパイル
	Shader vs;
	vs.LoadDxc(L"TestVS.hlsl", L"vs_6_0");
	assert(vs.GetDxcBlob() != nullptr);
	//ピクセルシェーダーの読み込みとコンパイル
	Shader ps;
	ps.LoadDxc(L"TestPS.hlsl", L"ps_6_0");
	assert(ps.GetDxcBlob() != nullptr);

	//PipelineStateクラスを使ってPSOを生成する
	PipelineState pipelineState;
	SetupPipelineState(pipelineState, rootSignature, vs, ps);

	//VertexBufferクラスを使って頂点バッファを生成する
	VertexBuffer vertexBuffer;
	vertexBuffer.Create(sizeof(Vector4) * 3, sizeof(Vector4));

	//頂点resourceにデータを書き込む
	Vector4* vertexData = nullptr;

	vertexBuffer.Get()->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	vertexData[0] = Vector4(-0.5f, -0.5f, 0.0f, 1.0f);//左下
	vertexData[1] = Vector4(0.0f, 0.5f, 0.0f, 1.0f);//上
	vertexData[2] = Vector4(0.5f, -0.5f, 0.0f, 1.0f);//右下
	//頂点リソースのマップを解除する
	vertexBuffer.Get()->Unmap(0, nullptr);

	// メインループ
	while (true) {

		// エンジン更新（終了チェック）
		if (Update()) {
			break;
		}


		// 描画開始
		dxCommon->PreDraw();
		//コマンドを積む
		commandList->SetGraphicsRootSignature(rootSignature.Get());//rootsignatureの設定
		commandList->SetPipelineState(pipelineState.Get());//PSOの設定
		commandList->IASetVertexBuffers(0, 1, vertexBuffer.GetView());//VBVの設定
		//トポロジの設定
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//頂点数、インスタンス数、インデックスの開始位置、インデックスのオフセット（三角形）
		commandList->DrawInstanced(3, 1, 0, 0);

		// 描画終了
		dxCommon->PostDraw();
	}

	
	
	
	
	

	// エンジン終了
	Finalize();

	return 0;
}

