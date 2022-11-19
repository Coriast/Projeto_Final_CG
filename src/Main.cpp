// L = T * R * S

#include "Main.hpp"

float scrWidth = 1200, scrHeight = 700;

using namespace std;

UglyCam cam(glm::vec3(56.0, 10.0, 76.0));
float lastX = scrWidth / 2.0f;
float lastY = scrHeight / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Setup Lights
CGHelpers::DirectionalLight DirLight;
CGHelpers::PointLight Torch;

glm::vec3 TorchPos = glm::vec3(0.0f, 0.0f, 0.0f);

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

void DrawScene(PShader &shader, Model groundModel, vector<Model> objects) {


	float scaleValue = 15.0;
	glm::mat4 model = glm::mat4(1.0);
	glm::mat4 translateG = glm::translate(model, glm::vec3(5.0 * scaleValue, 0.0, 5.0 * scaleValue));
	glm::mat4 scaleG = glm::scale(model, glm::vec3(scaleValue, 12.0, scaleValue));
	model = translateG * scaleG;
	shader.setMat4("model", model);

	groundModel.Draw(shader);

	cam.checkCollisionGround(groundModel, deltaTime, model);

	#pragma region Paredes de Pedra
	for (int i = 0; i < 5; i++) {
		glm::mat4 objModel = glm::mat4(1.0);
		
		glm::mat4 translateO = glm::translate(objModel, glm::vec3(5.0 * scaleValue, 0.3, -(5.0*scaleValue) + (i * 2.5f) * scaleValue));
		glm::mat4 scaleO = glm::scale(objModel, glm::vec3(0.7, 4.0, 1.0));

		objModel = (translateG * translateO) * (scaleO * scaleG);
		shader.setMat4("model", objModel);
		cam.checkCollision(objects[5], objModel);
		objects[5].Draw(shader);
	}

	for (int i = 0; i < 5; i++) {
		glm::mat4 objModel = glm::mat4(1.0);

		glm::mat4 translateO = glm::translate(objModel, glm::vec3(-(5.0 * scaleValue), 0.8 * scaleValue, -(5.0 * scaleValue) + (i * 2.0f) * scaleValue));
		glm::mat4 rotateO = glm::rotate(objModel, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 rotateY = glm::rotate(objModel, glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 scaleO = glm::scale(objModel, glm::vec3(1.0, 5.0, 1.5));

		objModel = (translateG * translateO) * (rotateY * rotateO) * (scaleO * scaleG);
		shader.setMat4("model", objModel);
		cam.checkCollision(objects[3], objModel);
		objects[3].Draw(shader);
	}

	for (int i = 0; i < 5; i++) {
		glm::mat4 objModel = glm::mat4(1.0);

		glm::mat4 translateO = glm::translate(objModel, glm::vec3(-(5.0 * scaleValue) + (i * 2.5f) * scaleValue, 0.3, 5.0 * scaleValue));
		glm::mat4 rotateO = glm::rotate(objModel, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 scaleO = glm::scale(objModel, glm::vec3(0.7, 4.0, 1.0));

		objModel = (translateG * translateO) * rotateO * (scaleO * scaleG);
		shader.setMat4("model", objModel);
		cam.checkCollision(objects[5], objModel);
		objects[5].Draw(shader);
	}
	#pragma endregion Paredes de Pedra

	for (int i = 0; i < 3; i++) {
		glm::mat4 objModel = glm::mat4(1.0);

		glm::mat4 translateO = glm::translate(objModel, glm::vec3((-3.0 * scaleValue) * (i/2.0), 0.2 * scaleValue, (-2.0 * scaleValue)* i));
		glm::mat4 scaleO = glm::scale(objModel, glm::vec3(0.5, 0.5, 0.5));

		objModel = (translateG * translateO) * (scaleO * scaleG);
		shader.setMat4("model", objModel);
		cam.checkCollision(objects[5], objModel);
		objects[1].Draw(shader);
	}

	for (int i = 0; i < 1; i++) {
		glm::mat4 objModel = glm::mat4(1.0);

		glm::mat4 translateO = glm::translate(objModel, glm::vec3(2.0 * scaleValue, 0.1 * scaleValue, 2.0 * scaleValue));
		glm::mat4 scaleO = glm::scale(objModel, glm::vec3(0.5, 0.5, 0.5));

		objModel = (translateG * translateO) * (scaleO * scaleG);
		shader.setMat4("model", objModel);
		cam.checkCollision(objects[5], objModel);
		objects[0].Draw(shader);
	}

	glm::mat4 objModel = glm::mat4(1.0f);
	glm::mat4 translateO = glm::translate(objModel, glm::vec3(-2.5f * scaleValue, 0.5f * scaleValue, -2.7f * scaleValue));
	glm::mat4 scaleO = glm::scale(objModel, glm::vec3(3.0, 3.0, 3.0));
	objModel = (translateG * translateO) * (scaleG * scaleO);

	for (int i = 0; i < objects[6].meshes.size(); i++) {
		Mesh mesh = objects[6].meshes[i];
		for (int i = 0; i < mesh.vertices.size(); i++) {
			glm::vec3 Vpos = glm::vec3(objModel * glm::vec4(mesh.vertices[i].Position, 1.0f));

			if (Vpos.y > TorchPos.y)
				TorchPos = Vpos;
		}
	}

	shader.setMat4("model", objModel);

	Torch.position = TorchPos;
	Torch.light.ambient		= glm::vec3(0.05f, 0.05f, 0.05f);
	Torch.light.diffuse		= glm::vec3(0.8f, 0.8f, 0.8f);
	Torch.light.specular	= glm::vec3(1.0f, 1.0f, 1.0f);
	Torch.constant	= 1.0f;
	Torch.linear	= 0.007f;
	Torch.quadratic = 0.0002f;
	CGHelpers::SetPointLight(shader, Torch);
	objects[6].Draw(shader);

}

void initLights() {
	// Directional Light
	DirLight.direction = glm::vec3(-0.2f, -1.0f, -0.3f);
	DirLight.light.ambient = glm::vec3(0.1f, 0.1f, 0.1f);
	DirLight.light.diffuse = glm::vec3(254.0f / 255.0f, 249.0f / 255.0f, 167.0f / 255.0f);
	DirLight.light.specular = glm::vec3(1.0f, 1.0f, 1.0f);
	
}

int main()
{
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
	
	PShader coordsShader("../../../src/NoLight.vert", "../../../src/NoLight.frag");

	// Model Stuff
	//Model ourModel("../../../objects/backpack/backpack.obj");


	// Criar Strings com estes caminhos para  ficar mais interativo de fazer
	// inclusive um vetor de string apenas com Rocks o modelo pode ser FindGround, FindTree
	Model groundModel("../../../objects/ground/Ground.obj"); 
	
	Model treeM("../../../objects/tree/Tree.obj");

	Model BTreeM("../../../objects/Bigger_Tree/Bigger_Tree.obj");

	Model rock4M("../../../objects/Rocks/Rock004.obj");

	Model rock5M("../../../objects/Rocks/Rock005.obj");

	Model rock6M("../../../objects/Rocks/Rock006.obj");

	Model rock7M("../../../objects/Rocks/Rock007.obj");

	Model Torch("../../../objects/Torch/Torch.obj");


	vector<Model> models = { treeM, BTreeM, rock4M, rock5M, rock6M, rock7M, Torch};

	initLights();

	// render loop
	while (!glfwWindowShouldClose(window)) {
		
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// rendering
		glClearColor(56.0f/255.0f, 176.0f/255.0f, 222.0f/255.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 view			= cam.GetViewMatrix();
		glm::mat4 projection	= glm::perspective(glm::radians(45.0f), (float)scrWidth / (float)scrHeight, 0.1f, 500.0f);

		shader.use();

		CGHelpers::SetDirectionalLight(shader, DirLight);

		shader.setVec3("viewPos", cam.Position);
		shader.setFloat("material.shininess", 12.0f);
		

		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		DrawScene(shader, groundModel, models);
		
		DrawCoords(coordsShader, view, projection);


		//input 
		processInput(window);

		// checa e chama eventos e trocar os buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
