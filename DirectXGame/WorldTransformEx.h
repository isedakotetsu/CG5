#pragma once
#include "KamataEngine.h"//worldtransformを継承するために必要
class WorldTransformEx :
    public KamataEngine::WorldTransform
{
public:
	//affine変換行列の生成と定数バッファへの転送を行う
	void UpdateMatrix();
	//affine変換行列の生成
	KamataEngine::Matrix4x4 MakeAffineMatrix();
};

