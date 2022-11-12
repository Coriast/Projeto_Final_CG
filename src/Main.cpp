
#include "Main.hpp"

float scrWidth = 1200, scrHeight = 700;

using namespace std;

UglyCam cam(glm::vec3(46.0, 10.0, 46.0));
float lastX = scrWidth / 2.0f;
float lastY = scrHeight / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

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


	glm::mat4 model = glm::mat4(1.0);
	model = glm::scale(model, glm::vec3(15.0, 12.0, 15.0));
	model = glm::translate(model, glm::vec3(5.0, 0.0, 5.0));
	shader.setMat4("model", model);

	groundModel.Draw(shader);

	cam.checkCollision(groundModel, deltaTime, model);

	for (int i = 0; i < objects.size(); i++) {
		model = glm::mat4(1.0);
		model = glm::scale(model, glm::vec3(5.0, 5.0, 5.0));
		model = glm::translate(model, glm::vec3(2.0 +  (3.0* i), 1.0, 3.0 + (2.0 * i)));
		shader.setMat4("model", model);

		objects[i].Draw(shader);
	}
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
	PShader ourShader("../../../src/modelLoader.vert", "../../../src/modelLoader.frag");
	
	PShader coordsShader("../../../src/NoLight.vert", "../../../src/NoLight.frag");

	// Model Stuff
	//Model ourModel("../../../objects/backpack/backpack.obj");

	Model groundModel("../../../objects/ground/Ground.obj");
	
	Model treeM("../../../objects/tree/Tree.obj");

	Model BTreeM("../../../objects/Bigger_Tree/Bigger_Tree.obj");

	Model rock4M("../../../objects/Rocks/Rock004.obj");

	Model rock5M("../../../objects/Rocks/Rock005.obj");

	Model rock6M("../../../objects/Rocks/Rock006.obj");

	vector<Model> models = { treeM, BTreeM, rock4M, rock5M, rock6M };

	// render loop
	while (!glfwWindowShouldClose(window)) {
		
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//input 
		processInput(window);

		// rendering
		glClearColor(56.0f/255.0f, 176.0f/255.0f, 222.0f/255.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 view			= cam.GetViewMatrix();
		glm::mat4 projection	= glm::perspective(glm::radians(45.0f), (float)scrWidth / (float)scrHeight, 0.1f, 500.0f);

		ourShader.use();
		ourShader.setVec3("light.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
		ourShader.setVec3("viewPos", cam.Position);

		ourShader.setVec3("light.ambient", glm::vec3(0.3f, 0.3f, 0.3f));
		ourShader.setVec3("light.diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
		ourShader.setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
		ourShader.setFloat("shininess", 12.0f);
		ourShader.setMat4("view", view);
		ourShader.setMat4("projection", projection);
		
		DrawScene(ourShader, groundModel, models);
		
		DrawCoords(coordsShader, view, projection);

		// checa e chama eventos e trocar os buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
