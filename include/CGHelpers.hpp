#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <Mesh.h>

namespace CGHelpers {

	struct Material {
		Texture diffuse;
		Texture specular;
		float shininess;
	};

	struct Light {
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
	};

	struct DirectionalLight {
		glm::vec3 direction;
		Light light;
	};

	struct PointLight {
		glm::vec3 position;

		float constant;
		float linear;
		float quadratic;

		Light light;
	};

	struct SpotLight {
		glm::vec3 direction;
		float cutOff;
		float outerCutOff;

		glm::vec3 position;
		float constant;
		float linear;
		float quadratic;

		Light light;
	};

	// Em operações que precisamos checar a posição dos vértices no mundo
	// não temos as alterações da matrix MVP, por isso calculamos aqui 
	glm::vec3 MultplyVecByMatrix(glm::mat4 matrix, glm::vec3 vertex) {
		return glm::vec3(matrix * glm::vec4(vertex, 1.0f));
	}

	void SetLight(std::string LightType, PShader& shader, Light light) {
		shader.setVec3(LightType + ".ambient", light.ambient);
		shader.setVec3(LightType + ".diffuse", light.diffuse);
		shader.setVec3(LightType + ".specular", light.specular);

	}

	void SetDirectionalLight(PShader& shader, DirectionalLight DirLight) {

		shader.setVec3("dirLight.direction", DirLight.direction);
		SetLight("dirLight", shader, DirLight.light);
	}

	void SetPointLight(PShader& shader, PointLight pointLight) {
		shader.setVec3("pointLight.position", pointLight.position);
		shader.setFloat("pointLight.constant", pointLight.constant);
		shader.setFloat("pointLight.linear", pointLight.linear);
		shader.setFloat("pointLight.quadratic", pointLight.quadratic);

		SetLight("pointLight", shader, pointLight.light);
	}
}

