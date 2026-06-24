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
}
