
/*
	O que eu quero para uma Cena?
	- Quero criar minha cena uma vez antes de entrar no loop
	- Vou alocar meu Shader que criei 
	- Vou alocar meus modelos carregados
	- Vou utilizar meu Mapa para definir a posição dos modelos estáticos da cena
*/

namespace CGHelpers {

	enum ObjectType {TREE, B_TREE, ROCK4, ROCK5, ROCK6, ROCK7, TORCH};

	int map[] = { // 1 - rocks
		1, 1, 1, 1, 1, 1,
		1, 0, 0, 0, 0, 1,
		1, 0, 0, 0, 0, 1,
		1, 0, 0, 0, 0, 1,
		1, 0, 0, 0, 0, 1,
		1, 1, 1, 1, 1, 1
	};

	class Scene {
	public:
		// Recebemos os ponteiros dos objetos criados na Main, as classes não tem um construtor default
		PShader* shader;
		Model* ground;
		std::map<CGHelpers::ObjectType, Model> *objects;
		UglyCam* camera;

		float* delta;

		Scene(PShader& shader, Model& ground, std::map<CGHelpers::ObjectType, Model > &objects, UglyCam &camera, float &delta) { 
			this->shader = &shader;
			this->ground = &ground;
			this->objects = &objects;
			this->camera = &camera;
			this->delta = &delta;
			objects.find(TORCH);
		}

		void Draw() {
			
			DrawGround();


		}

	private:
		void DrawGround() {
			float scaleValue = 15.0;
			glm::mat4 model = glm::mat4(1.0);
			glm::mat4 translateG = glm::translate(model, glm::vec3(5.0 * scaleValue, 0.0, 5.0 * scaleValue));
			glm::mat4 scaleG = glm::scale(model, glm::vec3(scaleValue, 12.0, scaleValue));
			model = translateG * scaleG;
			(*shader).setMat4("model", model);
			(*ground).Draw((*shader));

			(*camera).checkCollisionGround(*ground, *delta, model);
		}

		void DrawObjects() {

		}

	};
}