
/*
	O que eu quero para uma Cena?
	- Quero criar minha cena uma vez antes de entrar no loop
	- Vou alocar meu Shader que criei 
	- Vou alocar meus modelos carregados
	- Vou utilizar meu Mapa para definir a posição dos modelos estáticos da cena
*/

namespace CGHelpers {

	enum ObjectType {GROUND, TREE, B_TREE, ROCK4, ROCK5, ROCK6, ROCK7, TORCH};

	// O objeto do Ground tem tamanho 10, e após transladar ele para a posição 0,0 ele fica da posição 0 até a posição 10.
	// Com essa informação podemos montar o mapa da posição 0 até 10 e posicionamos nossos objetos no mapa de forma mais fácil, mesmo escalando o mapa.
	int map[11][11] = { // 3 - rocks
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
		3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
		3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
		3, 0, 0, 0, 0, 2, 2, 2, 2, 0, 3,
		3, 0, 0, 0, 2, 0, 0, 0, 0, 0, 3,
		3, 0, 2, 0, 2, 0, 2, 0, 2, 0, 3,
		3, 0, 7, 0, 0, 0, 0, 0, 0, 0, 3,
		3, 0, 2, 0, 0, 0, 0, 0, 0, 0, 3,
		3, 0, 0, 0, 2, 0, 0, 0, 2, 0, 3,
		3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3
	};

	class Scene {
	public:
		// Recebemos os ponteiros dos objetos criados na Main, as classes não tem um construtor default
		PShader* shader;
		Model* ground;
		std::map<ObjectType, Model> *objects;
		UglyCam* camera;

		glm::mat4 TorchMatrix;

		float sceneScaled = 20.0f;

		float* delta;

		Scene(PShader& shader, Model& ground, std::map<ObjectType, Model > &objects, UglyCam &camera, float &delta) { 
			this->shader = &shader;
			this->ground = &ground;
			this->objects = &objects;
			this->camera = &camera;
			this->delta = &delta;
			//objects.find(TORCH);
		}

		void Draw(PointLight pointLight, PShader &lightShader) {
			
			DrawGround();

			DrawObjects(pointLight, lightShader);
		}

	private:

		void DrawGround() {
			glm::mat4 groundMatrix = glm::mat4(1.0);
			glm::mat4 translateG = glm::translate(groundMatrix, glm::vec3(5.0 * sceneScaled, 0.0, 5.0 * sceneScaled));
			glm::mat4 scaleG = glm::scale(groundMatrix, glm::vec3(sceneScaled, sceneScaled / 2, sceneScaled));
			groundMatrix = translateG * scaleG;
			(*shader).setMat4("model", groundMatrix);
			(*ground).Draw(*shader);
			(*camera).checkCollisionGround(*ground, *delta, groundMatrix);
		}

		void DrawObjects(PointLight pointLight, PShader &lightShader) {
			// Tecnicamente complexidade de O(n^4) mas apenas assintoticamente, nossos valores não são algo acima de 3 digitos.
			for (int i = 0; i < 11; i++) {
				for (int j = 0; j < 11; j++) {
					if (map[i][j] != GROUND) {
						glm::mat4 objectMatrix = glm::mat4(1.0f);
						glm::mat4 translateO;
						glm::mat4 scaleO;
						Model object = objects->find(static_cast<ObjectType>(map[i][j]))->second;

						if (map[i][j] >= ROCK4 && map[i][j] <= ROCK7) {
							glm::mat4 rotateO = glm::mat4(1.0f);
							// Rotaciona as pedras dos cantos superior e inferior
							if (i == 0 || i == 10) {
								rotateO = glm::rotate(objectMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
							}
							// A escala das rochas é desigual por causa do chão que tem morros
							translateO = glm::translate(objectMatrix, glm::vec3(i * sceneScaled, 0.4 * sceneScaled, j * sceneScaled));
							scaleO = glm::scale(objectMatrix, glm::vec3(sceneScaled / 1.5, sceneScaled, sceneScaled / 1.5));
							objectMatrix = translateO * rotateO * scaleO;
						}
						else if (map[i][j] == TORCH) {
							translateO = glm::translate(objectMatrix, glm::vec3(i * sceneScaled, 0.1 * sceneScaled, j * sceneScaled));
							scaleO = glm::scale(objectMatrix, glm::vec3(sceneScaled, sceneScaled, sceneScaled));
							objectMatrix = translateO * scaleO;
							TorchMatrix = objectMatrix;
							for (int i = 0; i < object.meshes.size(); i++) {
								Mesh mesh = object.meshes[i];
								for (int i = 0; i < mesh.vertices.size(); i++) {
									glm::vec3 Vpos = CGHelpers::MultplyVecByMatrix(objectMatrix, mesh.vertices[i].Position);

									if (Vpos.y > pointLight.position.y)
										pointLight.position = Vpos;
								}
							}

							glm::mat4 lightMatrix = glm::mat4(1.0f);
							glm::mat4 translateL = glm::translate(lightMatrix, glm::vec3(pointLight.position.x, pointLight.position.y, pointLight.position.z));
							glm::mat4 scaleL = glm::scale(lightMatrix, glm::vec3(0.5f, 0.5f, 0.5f));
							lightMatrix = translateL * scaleL;
							CGHelpers::SetPointLight(*shader, pointLight);
							CGHelpers::PointLightSource(pointLight, lightShader, lightMatrix);
						}
						else {
							translateO = glm::translate(objectMatrix, glm::vec3(i * sceneScaled, 0.1  * sceneScaled, j * sceneScaled));
							scaleO = glm::scale(objectMatrix, glm::vec3(sceneScaled / 4, sceneScaled / 3, sceneScaled / 4));
							objectMatrix = translateO * scaleO;
						}
						(*shader).use();
						(*shader).setMat4("model", objectMatrix);
						object.Draw(*shader);
						(*camera).checkCollision(object, objectMatrix);
					}

				}
			}
		}

	};
}