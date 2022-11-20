#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>

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

	void SetLight(std::string LightType, PShader& shader, Light light) {
		shader.setVec3(LightType + ".ambient", light.ambient);
		shader.setVec3(LightType + ".diffuse", light.diffuse);
		shader.setVec3(LightType + ".specular", light.specular);

	}

	void SetDirectionalLight(PShader& shader, DirectionalLight DirLight) {
		shader.use();
		shader.setVec3("dirLight.direction", DirLight.direction);
		SetLight("dirLight", shader, DirLight.light);
	}

	void SetPointLight(PShader& shader, PointLight pointLight) {
		shader.use();
		shader.setVec3("pointLight.position", pointLight.position);
		shader.setFloat("pointLight.constant", pointLight.constant);
		shader.setFloat("pointLight.linear", pointLight.linear);
		shader.setFloat("pointLight.quadratic", pointLight.quadratic);

		SetLight("pointLight", shader, pointLight.light);
	}



	// Em operações que precisamos checar a posição dos vértices no mundo
	// não temos as alterações da matrix MVP, por isso calculamos aqui 
	glm::vec3 MultplyVecByMatrix(glm::mat4 matrix, glm::vec3 vertex) {
		return glm::vec3(matrix * glm::vec4(vertex, 1.0f));
	}

	void DrawSphere() {
		std::vector<unsigned int> indices;
		std::vector<float> pontos;
		const float PI = 3.14159265359;

		float deltaPhi = PI / 8;
		float deltaTheta = 2 * PI / 8;
		float raio = 1.0f;

		for (int i = 0; i <= 8; i++) {
			float phi = -PI / 2.0f + i * deltaPhi;
			float temp = raio * cos(phi);
			float y = raio * sin(phi);

			for (int j = 0; j < 8; j++) {
				float theta = j * deltaTheta;
				float x = temp * sin(theta);
				float z = temp * cos(theta);

				pontos.push_back(x);
				pontos.push_back(y);
				pontos.push_back(z);
				int index = pontos.size() - 1;
				indices.push_back(index);
			}
		}

		unsigned int VAO, VBO, EBO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*pontos.size(), &pontos[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glPointSize(40.0f);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
	}

	void PointLightSource(PointLight light, PShader& lightShader, glm::mat4 objectMatrix) {
		lightShader.use();
		lightShader.setMat4("model", objectMatrix);
		lightShader.setVec3("cor", glm::vec3(1.0f));

		DrawSphere();
	}

}

