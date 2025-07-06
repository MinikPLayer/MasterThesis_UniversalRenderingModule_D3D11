#include "PixelThroughputTest.h"
#include <URM/Engine/ModelObject.h>

void PixelThroughputTest::OnInit(URM::Engine::Engine& engine) {
	auto newModel = engine.GetScene().GetRoot().lock()->AddChild(new URM::Engine::ModelObject("cube_textured_external.glb"));
	newModel->GetTransform().SetLocalScale({ 2.0f, 1.0f, 1.0f });
	newModel->GetTransform().SetLocalPosition({ 0, 0.0f, 0.0f });

	auto mainCamera = engine.GetScene().GetMainCamera().lock();
	mainCamera->GetTransform().SetPosition({ 0.0f, 0.0f, 2.0f });
	mainCamera->GetTransform().LookAt({ 0.0f, 0.0f, 0.0f });

	auto light = new URM::Engine::LightObject();
	light->GetTransform().SetPosition({ 0, 3, 5 });
	engine.GetScene().GetRoot().lock()->AddChild(light);
}

void PixelThroughputTest::OnUpdate(URM::Engine::Engine& engine) {
	if (this->mResolutionChanged) {
		auto oldSize = engine.GetCore().GetWindow().GetSize();
		auto retValue = engine.GetCore().GetWindow().SetSize(mCurrentWindowSize);
		auto newSize = engine.GetCore().GetWindow().GetSize();	
		if (retValue == false || oldSize == newSize) {
			spdlog::error("[PixelThroughputTest] Failed to set window size to {}x{}", mCurrentWindowSize.width, mCurrentWindowSize.height);
			return;
		}

		this->mResolutionChanged = false;
	}
}

bool PixelThroughputTest::IncreaseCount(size_t amount) {
	auto currentPixelCount = mCurrentWindowSize.width * mCurrentWindowSize.height;
	auto newPixelCount = currentPixelCount + amount;

	// Use 1:1 aspect ratio for maximum pixel count.
	auto newSize = static_cast<int>(ceil(sqrt(newPixelCount)));

	// D3D11 limit.
	if (newSize >= 32768) {
		spdlog::warn("[PixelThroughputTest] Maximum resolution reached, cannot increase further.");
		return false;
	}

	this->mCurrentWindowSize = { newSize, newSize };
	this->mResolutionChanged = true;

	spdlog::trace("[PixelThroughputTest] Increasing resolution to {}x{}", mCurrentWindowSize.width, mCurrentWindowSize.height);

	return true;
}

bool PixelThroughputTest::DecreaseCount() {
	return false;
}

unsigned int PixelThroughputTest::GetCount() {
	return mCurrentWindowSize.width;
}

unsigned int PixelThroughputTest::GetScore() {
	return mCurrentWindowSize.width * mCurrentWindowSize.height;
}
