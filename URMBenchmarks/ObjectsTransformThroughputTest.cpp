#include "ObjectsTransformThroughputTest.h"

void ObjectsTransformThroughputTest::OnInit(URM::Engine::Engine& engine) {
	this->mObjectsParent = engine.GetScene().GetRoot().lock()->AddChild(new URM::Engine::SceneObject());
	this->mLastObject = this->mObjectsParent;

    this->IncreaseCount(1);
}

void ObjectsTransformThroughputTest::OnUpdate(URM::Engine::Engine& engine) {
	this->mObjectsParent->GetTransform().SetRotation({
		0.0f,
		engine.GetTimer().GetElapsedTime() * 45.0f,
		0.0f
	});
}

bool ObjectsTransformThroughputTest::IncreaseCount(size_t amount) {
	for(size_t i = 0; i < amount; i++) {
		auto newChild = this->mLastObject->AddChild(new URM::Engine::SceneObject());
		if (this->mLastObject->GetChildrenCount() >= this->mElementsPerLevel) {
			this->mLastObject = newChild;
		}

		this->mCurrentObjectCount++;
	}

    return true;
}

bool ObjectsTransformThroughputTest::DecreaseCount() {
	if (this->mCurrentObjectCount == 0) {
		return false;
	}

	auto model = this->mObjectsParent->GetChildByIndex(this->mObjectsParent->GetChildrenCount() - 1);
	model->Destroy();
	this->mCurrentObjectCount--;
	return true;
}

unsigned int ObjectsTransformThroughputTest::GetCount() {
    return this->mCurrentObjectCount;
}

// This test is *very* likely to overshoot the target FPS, so we use a lower multiplier.
float ObjectsTransformThroughputTest::GetTargetCurveMultiplier() {
	return 0.1f;
}
