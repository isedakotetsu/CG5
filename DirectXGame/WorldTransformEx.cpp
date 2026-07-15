#include "WorldTransformEx.h"

using namespace KamataEngine;
using namespace KamataEngine::MathUtility;//make-matrix4*4同士の類

//scale,rotation,translate 行列からworld行列を計算
//そして定数バッファへの転送も行う
void WorldTransformEx::UpdateMatrix()
{
	//world変換行列を計算しmatworld_に格納する
	matWorld_ = MakeAffineMatrix();
	//定数バッファに転送する
	TransferMatrix();
}

Matrix4x4 WorldTransformEx::MakeAffineMatrix()
{
	//scale matrix
	Matrix4x4 matScale = MakeScaleMatrix(scale_);

	//rotation matrix
	Matrix4x4 matRotX = MakeRotateXMatrix(rotation_.x);
	Matrix4x4 matRotY = MakeRotateYMatrix(rotation_.y);
	Matrix4x4 matRotZ = MakeRotateZMatrix(rotation_.z);
	Matrix4x4 matRot = matRotZ * matRotY * matRotX;

	//translation matrix
	Matrix4x4 matTrans = MakeTranslateMatrix(translation_);

	//world matrix
	Matrix4x4 matWorld = matScale * matRot * matTrans;

	return matWorld;
}