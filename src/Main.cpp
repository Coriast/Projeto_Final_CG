/*
	Coordenada Local - para manipular o objeto no seu ponto de origem
	Model Matrix - Manipular os objetos em relação ao mundo, coordenadas globais
	View Matrix - Manipular em relação ao ponto de vista da câmera
	Projection Matrix - Setar o range do mundo para algo diferente de -1.0 to 1.0 para depois converter para este range

	V_clip = M_proj * M_view * M_model * M_local

	 "../../../src/basicLight.vert", "../../../src/basicLight.frag"
*/

#include "Main.hpp"
float scrWidth = 1200, scrHeight = 700;

using namespace std;

UglyCam cam(glm::vec3(0.0, 0.0, 5.0));
float lastX = scrWidth / 2.0f;
float lastY = scrHeight / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	scrWidth = width; scrHeight = height;
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
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

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(scrWidth, scrHeight, "The Witness", NULL, NULL);
	glfwMakeContextCurrent(window);
	// Callback para Resize de tela
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	stbi_set_flip_vertically_on_load(true);

	glEnable(GL_DEPTH_TEST);

	// SHADER STUFF 
	PShader ourShader("../../../src/modelLoader.vert", "../../../src/modelLoader.frag");

	// Model Stuff
	//Model ourModel("../../../objects/backpack/backpack.obj");
	Model ourModel("../../../objects/ground/Ground.obj");

	//Model treeModel("../../../objects/tree/Tree.obj");

	//Model BiggerTreeModel("../../../objects/Bigger_Tree/Bigger_Tree.obj");

	Model rockModel("../../../objects/Rocks/Rock006.obj");

	// render loop
	while (!glfwWindowShouldClose(window)) {
		
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//input 
		processInput(window);

		// rendering
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ourShader.use();
		ourShader.setVec3("light.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
		ourShader.setVec3("viewPos", cam.Position);

		ourShader.setVec3("light.ambient", glm::vec3(0.3f, 0.3f, 0.3f));
		ourShader.setVec3("light.diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
		ourShader.setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
		ourShader.setFloat("shininess", 12.0f);

		glm::mat4 view			= cam.GetViewMatrix();
		glm::mat4 projection	= glm::perspective(glm::radians(45.0f), (float)scrWidth / (float)scrHeight, 0.1f, 100.0f);

		ourShader.setMat4("view", view);
		ourShader.setMat4("projection", projection);
		

		glm::mat4 model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 6.0f));
		//model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		ourShader.setMat4("model", model);
		
		ourModel.Draw(ourShader);
		
		rockModel.Draw(ourShader);
		//treeModel.Draw(ourShader);

		//BiggerTreeModel.Draw(ourShader);
		
		// checa e chama eventos e trocar os buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
