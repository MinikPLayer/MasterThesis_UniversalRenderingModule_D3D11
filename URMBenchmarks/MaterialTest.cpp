#include "MaterialTest.h"
#include <URM/Engine/ModelObject.h>
#include <URM/Engine/MeshObject.h>

struct ScreenPositionMaterialData {
	Vector2 screenSize = { 1.0f, 1.0f };
	int calculateLighting = 1;
	int dummy;
};

struct ScreenPositionMaterial : public URM::Core::MaterialWithData<ScreenPositionMaterialData>{
protected:
	bool mCalculateLighting = true;

	const wchar_t* GetShaderFilePath() const override {
		return L"ScreenPositionPixelShader.cso";
	}

	void UploadData(URM::Core::D3DCore& core, bool useAlbedoTexture) override {
		auto size = core.GetWindow().GetSize();
		this->data.screenSize = { static_cast<float>(size.width), static_cast<float>(size.height) };
		this->data.calculateLighting = mCalculateLighting ? 1 : 0;
		this->mConstantBuffer.UpdateWithData(core, &this->data);
	}

public:
	ScreenPositionMaterial(URM::Core::D3DCore& core, bool calculateLighting = true) : MaterialWithData(core) {
		this->mCalculateLighting = calculateLighting;
	}
};

void MaterialTest::OnInit(URM::Engine::Engine& engine) {
	auto root = engine.GetScene().GetRoot().lock();

	auto screenPositionShaderMaterial = std::shared_ptr<ScreenPositionMaterial>(new ScreenPositionMaterial(engine.GetCore(), false));
	auto screenPositionShaderMaterialLight = std::shared_ptr<ScreenPositionMaterial>(new ScreenPositionMaterial(engine.GetCore(), true));
	auto cube = root->AddChild(new URM::Engine::ModelObject("cube.glb", screenPositionShaderMaterial));
	cube->GetTransform().SetLocalScale({ 2.0f, 10.0f, 1.0f });
	cube->GetTransform().SetLocalPosition({ -20.0f, 0.0f, -0.0f });

	auto cubeLight = root->AddChild(new URM::Engine::ModelObject("cube.glb", screenPositionShaderMaterialLight));
	cubeLight->GetTransform().SetLocalScale({ 2.0f, 10.0f, 1.0f });
	cubeLight->GetTransform().SetLocalPosition({ 20.0f, 0.0f, -0.0f });

	const int SPHERE_COUNT = 10;
	const float SPHERE_SEPARATION = 2.5f;

	const int SIMPLE_MIN_ROUGHNESS = 1;
	const int SIMPLE_MAX_ROUGHNESS = 256;

	const float PBR_MIN_METALLIC = 0.001f;
	const float PBR_MAX_METALLIC = 1.0f;
	const float PBR_MIN_ROUGHNESS = 0.001f;
	const float PBR_MAX_ROUGHNESS = 1.0f;

	mLight = std::shared_ptr<URM::Engine::LightObject>(new URM::Engine::LightObject());
	mLight->GetTransform().SetPosition({ 0, 0, SPHERE_COUNT * SPHERE_SEPARATION });
	mLight->attenuationExponent = 0.0f;
	mLight->diffuseIntensity = 0.1f;
	mLight->ambientIntensity = 0.0f;
	root->AddChild(mLight);
	mModelsObject = root->AddChild(new URM::Engine::SceneObject());

	for (int i = 0; i < SPHERE_COUNT; i++) {
		// Simple material
		auto newModel = mModelsObject->AddChild(new URM::Engine::ModelObject("sphere.glb"));
		newModel->GetTransform().SetLocalPosition({ (i - ((SPHERE_COUNT - 1) / 2.0f)) * SPHERE_SEPARATION, (SPHERE_COUNT + 1) / 2.0f * SPHERE_SEPARATION, 0 });
		auto mesh = newModel->GetChildrenByType<URM::Engine::MeshObject>(true)[0];

		auto iValue = (i / ((float)SPHERE_COUNT - 1));
		auto exponentSimple = std::pow(iValue, 2);

		auto simpleMaterial = std::shared_ptr<URM::Core::MaterialSimple>(new URM::Core::MaterialSimple(engine.GetCore()));
		simpleMaterial->data.roughnessPowerExponent = SIMPLE_MIN_ROUGHNESS + exponentSimple * (SIMPLE_MAX_ROUGHNESS - SIMPLE_MIN_ROUGHNESS);
		simpleMaterial->data.albedoColor = Color(0.0f, 1.0f, 0.0f);
		mesh->material = simpleMaterial;

		auto pbrMetallicValue = PBR_MIN_METALLIC + std::pow(iValue, 2) * (PBR_MAX_METALLIC - PBR_MIN_METALLIC);
		for (int j = 0; j < SPHERE_COUNT; j++) {
			// PBR Material
			newModel = mModelsObject->AddChild(new URM::Engine::ModelObject("sphere.glb"));
			newModel->GetTransform().SetLocalPosition({ (i - ((SPHERE_COUNT - 1) / 2.0f)) * SPHERE_SEPARATION, -(j - (SPHERE_COUNT - 1) / 2.0f) * SPHERE_SEPARATION, 0 });
			mesh = newModel->GetChildrenByType<URM::Engine::MeshObject>(true)[0];

			auto iValueRoughness = (j / ((float)SPHERE_COUNT - 1));
			auto pbrRoughnessValue = PBR_MIN_ROUGHNESS + std::pow(iValueRoughness, 2) * (PBR_MAX_ROUGHNESS - PBR_MIN_ROUGHNESS);

			auto pbrMaterial = std::shared_ptr<URM::Core::MaterialPBR>(new URM::Core::MaterialPBR(engine.GetCore()));
			pbrMaterial->data.roughness = pbrRoughnessValue;
			pbrMaterial->data.metallic = pbrMetallicValue;
			pbrMaterial->data.albedo = Color(1.0f, 0.0f, 0.0f);
			mesh->material = pbrMaterial;
		}

	}

	auto camera = engine.GetScene().GetMainCamera().lock();
	camera->GetTransform().SetPosition({ 0, SPHERE_SEPARATION / 2.0f, SPHERE_SEPARATION * SPHERE_COUNT });
	camera->GetTransform().LookAt({ 0, SPHERE_SEPARATION / 2.0f, 0 });
}

void MaterialTest::OnUpdate(URM::Engine::Engine& engine) {
	const float distance = this->mLight->GetTransform().GetPosition().z;
	auto alpha = engine.GetTimer().GetElapsedTime() * 3.14f;

	this->mLight->GetTransform().SetLocalPosition({
		cos(alpha) * distance / 2.0f,
		sin(alpha) * distance / 2.0f,
		distance,
	});
}
