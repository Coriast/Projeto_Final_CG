/*
	Coordenada Local - para manipular o objeto no seu ponto de origem
	Model Matrix - Manipular os objetos em relação ao mundo, coordenadas globais
	View Matrix - Manipular em relação ao ponto de vista da câmera
	Projection Matrix - Setar o range do mundo para algo diferente de -1.0 to 1.0 para depois converter para este range

	V_clip = M_proj * M_view * M_model * M_local
*/

#include "Main.hpp"
float scrWidth = 1200, scrHeight = 800;

using namespace std;

float vertices[] = {
	// positions          // normals           // texture coords
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
};

glm::vec3 cubePos[] = {
	glm::vec3(0.0f,  0.0f,  0.0f),
	glm::vec3(2.0f,  5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f,  3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f,  2.0f, -2.5f),
	glm::vec3(1.5f,  0.2f, -1.5f),
	glm::vec3(-1.3f,  1.0f, -1.5f)
};

glm::vec3 lightPos[] = {
	glm::vec3(0.7f,  0.2f,  2.0f),
	glm::vec3(2.3f, -3.3f, -4.0f),
	glm::vec3(-4.0f,  2.0f, -12.0f),
	glm::vec3(0.0f,  0.0f, -3.0f)
};

UglyCam cam(glm::vec3(0.0, 0.0, 3.0));
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

unsigned int loadTexture(std::string path) {
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
	if (data) {
		GLenum format;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);
	} else {
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}
	return texture;
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

	glEnable(GL_DEPTH_TEST);

	// SHADER STUFF 
	PShader lightingShader("../../../src/basicLight.vert", "../../../src/basicLight.frag");
	PShader lightSourceShader("../../../src/lightSource.vert", "../../../src/lightSource.frag");
	PShader multipleLights("../../../src/basicLight.vert", "../../../src/MultipleLights.frag");

	unsigned int VBO, VAO;
	//unsigned int EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	//glGenBuffers(1, &EBO);

	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	// Copia o Array vertices em um buffer para o OpenGL usar
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// LIGHT STUFF
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	// TEXTURE STUFF
	unsigned int texture1, texture2, diffuseMap;
	
	texture1 = loadTexture("../../../textures/brick_wall.jpg");

	texture2 = loadTexture("../../../textures/awesomeface.png");

	diffuseMap = loadTexture("../../../textures/container2.png");

	unsigned int specularMap = loadTexture("../../../textures/container2_specular.png");
	// ----
	
	lightingShader.use();

	lightingShader.setInt("texture1", 0);
	lightingShader.setInt("texture2", 1);
	lightingShader.setInt("material.diffuse", 2);
	lightingShader.setInt("material.specular", 3);
	lightingShader.setFloat("light.constant", 1.0f);
	lightingShader.setFloat("light.linear", 0.09f);
	lightingShader.setFloat("light.quadratic", 0.032f);
	

	multipleLights.use();

	multipleLights.setInt("material.diffuse", 2);
	multipleLights.setInt("material.specular", 3);
	multipleLights.setFloat("light.constant", 1.0f);
	multipleLights.setFloat("light.linear", 0.09f);
	multipleLights.setFloat("light.quadratic", 0.032f);
	


	/*
	directLightS.use();

	directLightS.setInt("texture1", 0);
	directLightS.setInt("texture2", 1);
	directLightS.setInt("material.diffuse", 2);
	directLightS.setInt("material.specular", 3);
	*/

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

		// Bind Textures
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, texture1);
		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_2D, texture2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, specularMap);

		// activate Shader
		/*
		lightingShader.use();
		lightingShader.setVec3("light.position", lightPos);
		lightingShader.setVec3("viewPos", cam.Position);

		glm::vec3 lightColor = glm::vec3(1.0f);
		glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
		glm::vec3 ambientColor= diffuseColor * glm::vec3(0.2f);
		lightingShader.setVec3("light.ambient", ambientColor);
		lightingShader.setVec3("light.diffuse", diffuseColor);
		lightingShader.setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));

		lightingShader.setVec3("material.ambient", glm::vec3(1.0f, 0.5f, 0.31f));
		lightingShader.setVec3("material.diffuse", glm::vec3(1.0f, 0.5f, 0.31f));
		lightingShader.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
		lightingShader.setFloat("material.shininess", 12.0);
		*/


		glm::vec3 lightColor = glm::vec3(1.0f);
		glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.3f);
		multipleLights.use();
		multipleLights.setVec3("viewPos", cam.Position);
		multipleLights.setFloat("material.shininess", 12.0);

		multipleLights.setVec3("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
		multipleLights.setVec3("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
		multipleLights.setVec3("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
		multipleLights.setVec3("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));
		// point light 1
		multipleLights.setVec3("pointLights[0].position", lightPos[0]);
		multipleLights.setVec3("pointLights[0].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
		multipleLights.setVec3("pointLights[0].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
		multipleLights.setVec3("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
		multipleLights.setFloat("pointLights[0].constant", 1.0f);
		multipleLights.setFloat("pointLights[0].linear", 0.09f);
		multipleLights.setFloat("pointLights[0].quadratic", 0.032f);
		//
		multipleLights.setVec3("pointLights[1].position", lightPos[1]);
		multipleLights.setVec3("pointLights[1].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
		multipleLights.setVec3("pointLights[1].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
		multipleLights.setVec3("pointLights[1].specular", glm::vec3(1.0f, 1.0f, 1.0f));
		multipleLights.setFloat("pointLights[1].constant", 1.0f);
		multipleLights.setFloat("pointLights[1].linear", 0.09f);
		multipleLights.setFloat("pointLights[1].quadratic", 0.032f);
		//
		multipleLights.setVec3("pointLights[2].position", lightPos[2]);
		multipleLights.setVec3("pointLights[2].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
		multipleLights.setVec3("pointLights[2].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
		multipleLights.setVec3("pointLights[2].specular", glm::vec3(1.0f, 1.0f, 1.0f));
		multipleLights.setFloat("pointLights[2].constant", 1.0f);
		multipleLights.setFloat("pointLights[2].linear", 0.09f);
		multipleLights.setFloat("pointLights[2].quadratic", 0.032f);
		//
		multipleLights.setVec3("pointLights[3].position", lightPos[3]);
		multipleLights.setVec3("pointLights[3].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
		multipleLights.setVec3("pointLights[3].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
		multipleLights.setVec3("pointLights[3].specular", glm::vec3(1.0f, 1.0f, 1.0f));
		multipleLights.setFloat("pointLights[3].constant", 1.0f);
		multipleLights.setFloat("pointLights[3].linear", 0.09f);
		multipleLights.setFloat("pointLights[3].quadratic", 0.032f);
		
		multipleLights.setVec3("spotLight.position", cam.Position);
		multipleLights.setVec3("spotLight.direction", cam.Front);
		multipleLights.setVec3("spotLight.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		multipleLights.setVec3("spotLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
		multipleLights.setVec3("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
		multipleLights.setFloat("spotLight.constant", 1.0f);
		multipleLights.setFloat("spotLight.linear", 0.09f);
		multipleLights.setFloat("spotLight.quadratic", 0.032f);
		multipleLights.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		multipleLights.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

		/*
		directLightS.use();
		directLightS.setVec3("light.direction", glm::vec3(12.2f, 13.0f, 0.3f));
		directLightS.setVec3("viewPos", cam.Position);

		glm::vec3 lightColor = glm::vec3(1.0f);
		glm::vec3 diffuseColor = lightColor * glm::vec3(0.7f);
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);
		directLightS.setVec3("light.ambient", ambientColor);
		directLightS.setVec3("light.diffuse", diffuseColor);
		directLightS.setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));

		directLightS.setVec3("material.ambient", glm::vec3(1.0f, 0.5f, 0.31f));
		directLightS.setVec3("material.diffuse", glm::vec3(1.0f, 0.5f, 0.31f));
		directLightS.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
		directLightS.setFloat("material.shininess", 12.0);
		*/

		glm::mat4 view			= cam.GetViewMatrix();
		glm::mat4 projection	= glm::perspective(glm::radians(45.0f), (float)scrWidth / (float)scrHeight, 0.1f, 100.0f);

		
		//lightingShader.setMat4("view", view);
		//lightingShader.setMat4("projection", projection);

		multipleLights.setMat4("view", view);
		multipleLights.setMat4("projection", projection);
		
		//directLightS.setMat4("view", view);
		//directLightS.setMat4("projection", projection);

		glm::mat4 model = glm::mat4(1.0);
		model = glm::translate(model, cubePos[0]);
		multipleLights.setMat4("model", model);
		//lightingShader.setMat4("model", model);
		//directLightS.setMat4("model", model);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		Mesh mesh();
		
		for (int i = 0; i < 10; i++) {
			glm::mat4 model = glm::mat4(1.0f); // Matrix Identidade
			float angle = 20.0f * i;
			model = glm::translate(model, cubePos[i]);
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			multipleLights.setMat4("model", model);
			//directLightS.setMat4("model", model);
			//lightingShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}


		
		lightSourceShader.use();
		lightSourceShader.setMat4("view", view);
		lightSourceShader.setMat4("projection", projection);
		
		glBindVertexArray(lightVAO);
		for (int i = 0; i < 4; i++)
		{
			model = glm::translate(glm::mat4(1.0), lightPos[i]);
			model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
			lightSourceShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		
		
		// checa e chama eventos e trocar os buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	//glDeleteBuffers(1, &EBO);

	glfwTerminate();
	return 0;
}
