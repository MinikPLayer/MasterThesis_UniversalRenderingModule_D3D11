#include "Utils.h"
#include "Vector3.h"

using namespace DirectX;

DecomposedMatrix MatrixUtils::DecomposeMatrix(DirectX::XMMATRIX matrix) {
	XMVECTOR scale;
	XMVECTOR translation;
	XMVECTOR rotationQuaternion;

	XMMatrixDecompose(&scale, &rotationQuaternion, &translation, matrix);

	XMFLOAT3 retScale;
	XMFLOAT3 retTranslation;
	XMStoreFloat3(&retScale, scale);
	XMStoreFloat3(&retTranslation, translation);

	return DecomposedMatrix(
		retTranslation,
		retScale,
		rotationQuaternion
	);
}