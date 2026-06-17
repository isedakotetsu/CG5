#include "Shader.h"
#include <d3dcompiler.h>   // D3DCompileFromFile
#include <cassert>         // assert
#include "MiscUtility.h"
#include <dxcapi.h>
#pragma comment(lib, "dxcompiler.lib")  
// シェーダーファイルを読み込み、コンパイルする
void Shader::Load(const std::wstring& filePath, const std::wstring& shaderModel)
{

    ID3DBlob* shaderBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;
	std::string mbShaderModel = ConvertString(shaderModel);

    HRESULT hr = D3DCompileFromFile(
        filePath.c_str(),                    // シェーダーファイル名
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,   // インクルード可能にする
        "main", mbShaderModel.c_str(),         // エントリーポイント名、シェーダモデル指定
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
        0,
        &shaderBlob,
        &errorBlob
    );

    if (FAILED(hr))
    {
        if (errorBlob)
        {
            OutputDebugStringA(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
            errorBlob->Release();
        }
    }
	//生成したshederBlobをとっておく
	blob_ = shaderBlob;

}

void Shader::LoadDxc(const std::wstring& filePath, const std::wstring& shaderModel)
{
	static IDxcUtils* dxcUtils = nullptr;
	static IDxcCompiler3* dxcCompiler = nullptr;
	static IDxcIncludeHandler* dxcIncludeHandler = nullptr;

    HRESULT hr;

    // DXC の生成

    if (dxcUtils == nullptr) 
    {
        hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
        assert(SUCCEEDED(hr)); // うまくいかなかったときは起動できない
    }

    if (dxcCompiler == nullptr) 
    {
        hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
        assert(SUCCEEDED(hr)); // うまくいかなかったときは起動できない
    }

    if (dxcIncludeHandler == nullptr) 
    {
        hr = dxcUtils->CreateDefaultIncludeHandler(&dxcIncludeHandler);
        assert(SUCCEEDED(hr)); // うまくいかなかったときは起動できない
    }

    // ファイルを読み込む
    IDxcBlobEncoding* shaderSource = nullptr;
    hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
    assert(SUCCEEDED(hr));

    // 読み込んだファイルの内容をDxcBufferに設定する
    DxcBuffer shaderSourceBuffer{};

    shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
    shaderSourceBuffer.Size = shaderSource->GetBufferSize();
    shaderSourceBuffer.Encoding = DXC_CP_UTF8;

    // Compileに必要なコンパイルオプションの準備
    LPCWSTR arguments[] = {
        filePath.c_str(),      // コンパイル対象のhlslファイル名
        L"-E",
        L"main",               // エントリーポイントの指定。基本的にmain以外にはしない
        L"-T",
        shaderModel.c_str(),   // ShaderProfileの設定
        L"-Zi",
        L"-Qembed_debug",      // デバッグ用の情報を埋め込む
        L"-Od",                // 最適化を外しておく
        L"-Zpr",               // メモリレイアウトは行優先
    };

    // 実際にShaderをコンパイルする
    IDxcResult* shaderResult = nullptr;

    hr = dxcCompiler->Compile(
        &shaderSourceBuffer,       // 読み込んだファイル
        arguments,                 // コンパイルオプション
        _countof(arguments),       // コンパイルオプションの数
        dxcIncludeHandler,            // includeが含まれた諸々
        IID_PPV_ARGS(&shaderResult) // コンパイル結果
    );

    // コンパイルエラーではなくdxcが起動できないなど致命的な状況
    assert(SUCCEEDED(hr));

    // 3. 警告・エラーがでていないか確認する
    IDxcBlobUtf8* shaderError = nullptr;
    IDxcBlobWide* nameBlob = nullptr;

    shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), &nameBlob);

    if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
        OutputDebugStringA(shaderError->GetStringPointer());
        assert(false);
    }

    // 4. Compile結果を受け取る
    IDxcBlob* shaderBlob = nullptr;

    hr = shaderResult->GetOutput(
        DXC_OUT_OBJECT,
        IID_PPV_ARGS(&shaderBlob),
        &nameBlob
    );

	assert(SUCCEEDED(hr));

    // もう使わないリソースを解放
    shaderSource->Release();
    shaderResult->Release();

    // 実行用のバイナリを取っておく
    dxcBlob_ = shaderBlob;

}

ID3DBlob* Shader::GetBlob()
{
    return blob_;
}

IDxcBlob* Shader::GetDxcBlob()
{
	return dxcBlob_;
}

Shader::Shader()
{
}

Shader::~Shader()
{
    if (blob_ != nullptr)
    {
        blob_->Release();
        blob_ = nullptr;
	}

	if (dxcBlob_ != nullptr)
	{
		dxcBlob_->Release();
		dxcBlob_ = nullptr;
	}
}
