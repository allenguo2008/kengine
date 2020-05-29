#include "PolyVoxShader.hpp"

#include "EntityManager.hpp"

#include "data/InstanceComponent.hpp"
#include "data/TransformComponent.hpp"
#include "data/PolyVoxComponent.hpp"
#include "data/GraphicsComponent.hpp"
#include "data/ModelComponent.hpp"
#include "data/OpenGLModelComponent.hpp"

#include "helpers/cameraHelper.hpp"
#include "helpers/matrixHelper.hpp"
#include "systems/opengl/shaders/ApplyTransparencySrc.hpp"
#include "systems/opengl/shaders/shaderHelper.hpp"

#pragma region GLSL
static inline const char * vert = R"(
#version 330

layout (location = 0) in vec3 position;
layout (location = 2) in vec3 color;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;
uniform vec3 viewPos;

out vec4 WorldPosition;
out vec3 EyeRelativePos;
out vec3 Color;

void main() {
	WorldPosition = model * vec4(position, 1.0);
	EyeRelativePos = WorldPosition.xyz - viewPos;
	Color = color;
	//Color = vec3(1.0); // This is pretty

	gl_Position = proj * view * WorldPosition;
}
		)";

static inline const char * frag = R"(
#version 330

in vec4 WorldPosition;
in vec3 EyeRelativePos;
in vec3 Color;

uniform vec4 color;
uniform float entityID;

layout (location = 0) out vec4 gposition;
layout (location = 1) out vec3 gnormal;
layout (location = 2) out vec4 gdiffuse;
layout (location = 3) out vec4 gspecular;
layout (location = 4) out float gentityID;

void applyTransparency(float a);

void main() {
	applyTransparency(color.a);

    gposition = WorldPosition;
    gnormal = -normalize(cross(dFdy(EyeRelativePos), dFdx(EyeRelativePos)));
    gdiffuse = vec4(Color * color.rgb, 0.0);
    gspecular = vec4(Color * color.rgb, 0.0);
	gentityID = entityID;
}
        )";
#pragma endregion GLSL

namespace kengine {
	static glm::vec3 toVec(const putils::Point3f & p) { return { p.x, p.y, p.z }; }

	PolyVoxShader::PolyVoxShader(EntityManager & em)
		: Program(false, putils_nameof(PolyVoxShader)),
		_em(em)
	{}

	void PolyVoxShader::init(size_t firstTextureID) {
		initWithShaders<PolyVoxShader>(putils::make_vector(
			ShaderDescription{ vert, GL_VERTEX_SHADER },
			ShaderDescription{ frag, GL_FRAGMENT_SHADER },
			ShaderDescription{ Shaders::src::ApplyTransparency::Frag::glsl, GL_FRAGMENT_SHADER }
		));
	}

	void PolyVoxShader::run(const Parameters & params) {
		use();

		_view = params.view;
		_proj = params.proj;
		_viewPos = params.camPos;

		for (const auto &[e, poly, graphics, instance, transform] : _em.getEntities<PolyVoxObjectComponent, GraphicsComponent, InstanceComponent, TransformComponent>()) {
			if (!cameraHelper::entityAppearsInViewport(e, params.viewportID))
				continue;

			const auto model = _em.getEntity(instance.model);
			if (!model.has<OpenGLModelComponent>())
				continue;

			_model = matrixHelper::getModelMatrix(model.get<ModelComponent>(), transform);
			_entityID = (float)e.id;
			_color = graphics.color;

			shaderHelper::drawModel(model.get<OpenGLModelComponent>());
		}
	}
}
