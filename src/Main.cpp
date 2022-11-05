#include "Main.hpp"

using namespace std;

float vertices[] = {
	0.5f,  0.5f, 0.0f,  
	0.5f, -0.5f, 0.0f,  
	-0.5f, -0.5f, 0.0f,  
	-0.5f,  0.5f, 0.0f
};

unsigned int indices[] = {
	0, 1, 3, // first triangle
	1, 2, 3	 // second triangle
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {

}

// Para leitura dos arquivos de Shader.
void GetFile(char const *file, char **data) {
	std::ifstream fp(file);
	if (!fp.is_open()) {
		std::cout << "ARQUIVO " << file << " NAO ENCONTRADO" << std::endl;
		return;
	}

	std::stringstream buffer;
	buffer << fp.rdbuf();
	std::string ch = buffer.str();

	char* file_data = (char *)malloc((ch.size() + 1) * sizeof(char));
	if(file_data) // Teste de retorno NULL do malloc
		strcpy(file_data, ch.c_str());

	*data = file_data;

	fp.close();
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "The Witness", NULL, NULL);
	glfwMakeContextCurrent(window);
	// Callback para Resize de tela
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	
	// Lendo Shaders para Array
	char* vertexShaderSource; 
	GetFile("../../../src/VertexShader.vert", &vertexShaderSource);
	char* fragmentShaderSource;
	GetFile("../../../src/FragShader.frag", &fragmentShaderSource);

	// VertexShader
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// checa erros
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERRO::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// FragmentShader
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// checa erros
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERRO::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Linkando os Shaders
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERRO::SHADERPROGRAM::LINKER::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	// Copia o Array vertices em um buffer para o OpenGL usar
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Binding o EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Unbind do VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Unbind do VAO
	glBindVertexArray(0);


	// render loop
	while (!glfwWindowShouldClose(window)) {
		
		//input 
		processInput(window);

		// rendering
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // WireFrame

		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// checa e chama eventos e trocar os buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);

	glfwTerminate();
	return 0;
}
