// F = T * R * S

#include "Main.hpp"

float scrWidth = 1200, scrHeight = 700;

using namespace std;

UglyCam cam(glm::vec3(16.0, 7.0, 16.0));
float lastX = scrWidth / 2.0f;
float lastY = scrHeight / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Setup Lights
CGHelpers::DirectionalLight DirLight;
CGHelpers::PointLight TorchLight;
float SunYaw = 240.0f;

bool torchGrabbed = false;

// Coordenadas
float pointCoords[] = { 
	-100.0, 0.0, 0.0,
     100.0, 0.0, 0.0
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	scrWidth = width; scrHeight = height;
	glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
		cam.SwitchMode();
}

void processInput(GLFWwindow* window) {

	// Movement Inputs
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cam.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cam.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cam.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cam.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		cam.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		cam.ProcessKeyboard(DOWN, deltaTime);

	double xpos, ypos, xoffset, yoffset;
	glfwGetCursorPos(window, &xpos, &ypos);

	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	xoffset = xpos - lastX;
	yoffset = ypos - lastY;
	lastX = xpos;
	lastY = ypos;
	cam.ProcessMouseMovement(xoffset, yoffset);
}

void DrawCoords(PShader &shader, glm::mat4 view, glm::mat4 projection) {
	glm::mat4 model = glm::mat4(1.0);
	shader.use();
	shader.setMat4("view", view);
	shader.setMat4("projection", projection);
	shader.setMat4("model", model);
	shader.setVec3("cor", glm::vec3(1.0, 0.0, 0.0));

	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pointCoords), pointCoords, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// X
	glDrawArrays(GL_LINES, 0, 2);

	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
	shader.setMat4("model", model);
	shader.setVec3("cor", glm::vec3(0.0, 0.0, 1.0));
	glDrawArrays(GL_LINES, 0, 2);

	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
	shader.setMat4("model", model);
	shader.setVec3("cor", glm::vec3(0.0, 1.0, 0.0));
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);
}


void PlayerLine(PShader& shader, glm::mat4 view, glm::mat4 projection, UglyCam &cam) {
	// Só precisa multiplicar seu ponto pelo Vetor que aponta a direção que você vai obter uma 
	// movimentação na direção desejada, não somamos o cam.Position pois já estamos aplicando-o na matriz de transformação
	if(!torchGrabbed) {
		float line[] = {
			cam.Front.x, cam.Front.y, cam.Front.z,
		};
		glm::mat4 model = glm::mat4(1.0);
		glm::mat4 translateP = glm::translate(model, cam.Position);
		model = translateP;
		shader.use();
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		shader.setMat4("model", model);
		shader.setVec3("cor", glm::vec3(0.f, 0.f, 0.f));

		unsigned int VAO, VBO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// X
		glPointSize(4.0f);
		glDrawArrays(GL_POINTS, 0, 1);
		glBindVertexArray(0);
	}
}

void grabTorch(CGHelpers::Scene &cena, Model& object, CGHelpers::PointLight& pointLight, PShader& lightShader) {
	glm::mat4 objectMatrix(1.0f);
	glm::mat4 translateT = glm::translate(objectMatrix, glm::vec3(cam.Position.x, cam.Position.y - 4.0f, cam.Position.z));
	glm::mat4 translateP = glm::translate(objectMatrix, glm::vec3(1.0f, 0.0f, 1.0f));
	glm::mat4 rotateT = glm::rotate(objectMatrix, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 rotateP = glm::rotate(objectMatrix, glm::radians(cam.Yaw - 90.f), glm::vec3(0.0f, -1.0f, 0.0f));
	glm::mat4 scaleT = glm::scale(objectMatrix, glm::vec3(cena.sceneScaled));
	// Como eu não quero que meu objeto Translade em volta de sí mesmo e sim em volta do Player foi necessário fazer o seguinte
	// Após a Escala e a Rotação de 45 graus pra a tocha ficar inclinada nós movemos ela 1.0f em X e Z para que a próxima rotação
	// não aconteça em volta do próprio e sim para a esquerda da visão do jogador. 
	// (Não sei se isso quebra as "Regras" da ordem de multiplicação de Matrizes mas o calculo funciona de forma estável e acredito 
	// que aprendi a manipular para alcançar o resultado desejado)
	objectMatrix = translateT * (rotateP * translateP * rotateT) * scaleT; 
	cena.DrawTorch(objectMatrix, object, pointLight, lightShader);
}

void initLights() {
	// Directional Light
	DirLight.direction = glm::vec3(-0.2f, -1.0f, 0.0f);
	DirLight.light.ambient = glm::vec3(0.05f, 0.05f, 0.05f);
	DirLight.light.diffuse = glm::vec3(254.0f / 255.0f, 249.0f / 255.0f, 167.0f / 255.0f);
	DirLight.light.specular = glm::vec3(1.0f, 1.0f, 1.0f);
	
	// Torch Light
	TorchLight.light.ambient = glm::vec3(0.05f, 0.05f, 0.05f);
	TorchLight.light.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	TorchLight.light.specular = glm::vec3(0.5f, 0.5f, 0.5f);
	TorchLight.constant = 1.0f;
	TorchLight.linear = 0.045f;
	TorchLight.quadratic = 0.0075f;
}

int main() {
	HWND consoleW = GetConsoleWindow();
	SetWindowPos(consoleW, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	
	
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(scrWidth, scrHeight, "The Witness", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetWindowPos(window, 300, 200);
	// Callback para Resize de tela
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	stbi_set_flip_vertically_on_load(true);

	glEnable(GL_DEPTH_TEST);

	// SHADER STUFF 
	PShader shader("../../../src/modelLoader.vert", "../../../src/modelLoader.frag");
	
	PShader shaderSource("../../../src/NoLight.vert", "../../../src/NoLight.frag");

	// Criar Strings com estes caminhos para  ficar mais interativo de fazer
	// inclusive um vetor de string apenas com Rocks o modelo pode ser FindGround, FindTree
	Model groundModel("../../../objects/ground/Ground.obj"); 
	
	Model tree("../../../objects/tree/Tree.obj");

	Model BTree("../../../objects/Bigger_Tree/Bigger_Tree.obj");

	Model rock4("../../../objects/Rocks/Rock004.obj");

	Model rock5("../../../objects/Rocks/Rock005.obj");

	Model rock6("../../../objects/Rocks/Rock006.obj");

	Model rock7("../../../objects/Rocks/Rock007.obj");

	Model Torch("../../../objects/Torch/Torch.obj");


	map<CGHelpers::ObjectType, Model> models;

	models.insert({ CGHelpers::ObjectType::TREE, tree });
	models.insert({ CGHelpers::ObjectType::B_TREE, BTree });
	models.insert({ CGHelpers::ObjectType::ROCK4, rock4 });
	models.insert({ CGHelpers::ObjectType::ROCK5, rock5 });
	models.insert({ CGHelpers::ObjectType::ROCK6, rock6 });
	models.insert({ CGHelpers::ObjectType::ROCK7, rock7 });
	models.insert({ CGHelpers::ObjectType::TORCH, Torch });


	CGHelpers::Scene cena(shader, groundModel, models, cam, deltaTime);
	vector<Vertex> vertices = models.find(CGHelpers::TORCH)->second.meshes[0].vertices;
	initLights();
	bool isPressed = false;
	float second = 0.0f;
	// render loop
	while (!glfwWindowShouldClose(window)) {

		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		second = second + deltaTime;

		if (second > 1) {
			cout << cam.Front.x << " " << cam.Front.y << " " << cam.Front.z << endl;

			cout << " Sun:" << SunYaw << endl;
			second = 0.0f;
		}

		// rendering
		glClearColor(56.0f/255.0f, 176.0f/255.0f, 222.0f/255.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		shader.use();
		glm::mat4 view = cam.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)scrWidth / (float)scrHeight, 0.1f, 500.0f);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		shader.setVec3("viewPos", cam.Position);
		shader.setFloat("material.shininess", 12.0f);

		shaderSource.use();
		shaderSource.setMat4("view", view);
		shaderSource.setMat4("projection", projection);

		{ // Definindo nosso Ciclo de Dia e Noite
			DirLight.direction.z = 0.0f;
			DirLight.direction.x = cos(glm::radians(SunYaw));
			DirLight.direction.y = sin(glm::radians(SunYaw));
			DirLight.direction = glm::normalize(DirLight.direction);

			if (SunYaw < 360.0f) {
				SunYaw += 200.0f * (deltaTime / 30);
			}
			else {
				SunYaw = 0.0f;
			}
		}

		CGHelpers::SetDirectionalLight(shader, DirLight);
		
		cena.Draw(TorchLight, shaderSource, torchGrabbed);

		DrawCoords(shaderSource, view, projection);
		PlayerLine(shaderSource, view, projection, cam);
			

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && isPressed == false && torchGrabbed == false) {
			isPressed = true;
			vector<glm::vec3> verts;
			for (int i = 0; i < vertices.size(); i++)
			{
				verts.push_back(CGHelpers::MultplyVecByMatrix(cena.TorchMatrix, vertices[i].Position));
			}
			glm::mat4 cameraMatrix(1.0f);
			cameraMatrix = glm::translate(cameraMatrix, cam.Position);
			glm::vec3 camFront = CGHelpers::MultplyVecByMatrix(cameraMatrix, cam.Front);
			glm::vec3 front, back;
			float top, bottom;
			front = back = verts[0];
			top = bottom = verts[0].y;

			for (int i = 0; i < verts.size(); i++) {
				if (verts[i].x > front.x)
					front.x = verts[i].x;
				if (verts[i].x < back.x)
					back.x = verts[i].x;

				if (verts[i].z > front.z)
					front.z = verts[i].z;
				if (verts[i].z < back.z)
					back.z = verts[i].z;

				if (verts[i].y > top)
					top = verts[i].y;
				if (verts[i].y < bottom)
					bottom = verts[i].y;
			}

			for (float i = 0.f; i < 50.0f; i += 1.0f) {
				glm::vec3 point(camFront.x + (i *cam.Front.x), camFront.y +  (i * cam.Front.y), camFront.z +  (i * cam.Front.z));
				if (point.x > back.x && point.x < front.x) {
					if (point.z > back.z && point.z < front.z) {
						if (point.y < top && point.y > bottom) {
							torchGrabbed = true;
						}
					}
				}
			}

		} else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE){
			isPressed = false;
		}

		if (torchGrabbed) {
			grabTorch(cena, Torch, TorchLight, shaderSource);
		}

		//input 
		processInput(window);

		// checa e chama eventos e trocar os buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
