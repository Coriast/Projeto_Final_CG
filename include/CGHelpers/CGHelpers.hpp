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
		float raio = 1.0f;
		int sectors = 36;
		int stacks = 18;
		std::vector<int> indices;
		std::vector<float> pontos;
		const float PI = 3.14159265359;

		float deltaPhi = PI / stacks;
		float deltaTheta = 2 * PI / sectors;

		for (int i = 0; i <= stacks; i++) {
			float phi = -PI / 2.0f + i * deltaPhi;
			float temp = raio * cos(phi);
			float y = raio * sin(phi);

			for (int j = 0; j < sectors; j++) {
				float theta = j * deltaTheta;
				float x = temp * sin(theta);
				float z = temp * cos(theta);

				pontos.push_back(x);
				pontos.push_back(y);
				pontos.push_back(z);
			}
		}

		int k1, k2;
		for (int i = 0; i < stacks; i++) {
			k1 = i * (sectors + 1);
			k2 = k1 + sectors + 1;
			for (int j = 0; j < sectors; j++, k1++, k2++) {
				
				indices.push_back(k1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
				
				
				if (i != (stacks - 1)) {
					indices.push_back(k1 + 1);
					indices.push_back(k2);
					indices.push_back(k2 + 1);
				}

				if(j == (sectors -1)) {

					/*
					* Foi mais díficil do que eu esperava, no OpenGL legacy eu acompanhava um curso
					* que mostrou como criar a esfera desta forma, salvando os pontos e depois utilizando 
					* os índices, porém no Legacy os pontos os indices eram armazenados em um formato de Matrix
					* já junto com os pontos, depois apenas utilizavamos a lógica de desenhar o ponto do indice [i][j] atual
					* o ponto do indice [i + 1] [j]. Esta técnica se manteve basicamente a mesma passando para cá, porém a junção do último ponto
					* com o primeiro ponto eu não consegui passar para cá por causa da idéia da matriz [i][0] e [i + 1][0] para ser encontrado em um 
					* array. Carência de matemática, por isso essa junção foi feita na tentativa e erro. Mas ainda assim eu entendi essa costura
					* que estamos fazendo dos pontos finais com os pontos iniciais, se eu tiver tempo posso refatorar depois. 
					*/

					indices.push_back(i * (sectors + 1));
					indices.push_back(k1 + 1);
					indices.push_back((i * (sectors + 1)) + sectors + 1);

					indices.push_back(i * (sectors + 1));						
					indices.push_back(k1 + 1);									
					indices.push_back(((i - 1) * (sectors + 1)) + (sectors));	
				}

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
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);


		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	void PointLightSource(PointLight light, PShader& lightShader, glm::mat4 objectMatrix) {
		lightShader.use();
		lightShader.setMat4("model", objectMatrix);
		lightShader.setVec3("cor", glm::vec3(1.0f));

		DrawSphere();
	}

}

