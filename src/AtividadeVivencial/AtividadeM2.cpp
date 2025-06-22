/* Criando Triângulos a partir do Clique do Mouse
 *
 * Processamento Gráfico - Unisinos
 * Última atualização em 21/06/2025
 *
 */
#include <iostream>
#include <string>
#include <assert.h>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

// Prototipação
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
GLuint createTriangleFromPoints(vec3 v0, vec3 v1, vec3 v2);
int setupShader();

// Dimensões da janela
const GLuint WIDTH = 800, HEIGHT = 600;

// Vertex Shader (em GLSL)
const GLchar *vertexShaderSource = R"(
#version 400
layout (location = 0) in vec3 position;
uniform mat4 projection;
uniform mat4 model;
void main()
{
	gl_Position = projection * model * vec4(position, 1.0);
}
)";

// Fragment Shader (em GLSL)
const GLchar *fragmentShaderSource = R"(
#version 400
uniform vec4 inputColor;
out vec4 color;
void main()
{
	color = inputColor;
}
)";

// Estrutura para armazenar triângulos
struct Triangle
{
	vec3 v0, v1, v2;
	vec3 color;
	GLuint vao;
};

// Variáveis globais
vector<Triangle> triangles;
vector<vec3> temp_vertices;
vector<vec3> colors;
int iColor = 0;

int main()
{
	// Inicialização da GLFW
	glfwInit();

	// Criação da janela GLFW
	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Triângulos com Mouse", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da função de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	// GLAD: carrega todos os ponteiros de funções da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Erro ao inicializar GLAD" << endl;
		return -1;
	}

	// Verificando a versão do OpenGL
	cout << "Renderer: " << glGetString(GL_RENDERER) << endl;
	cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;

	// Paleta de cores
	colors = {
		vec3(200, 191, 231), vec3(174, 217, 224), vec3(181, 234, 215), vec3(255, 241, 182),
		vec3(255, 188, 188), vec3(246, 193, 199), vec3(255, 216, 190), vec3(220, 198, 224),
		vec3(208, 230, 165), vec3(183, 201, 226)};

	for (auto &c : colors)
		c /= 255.0f;

	// Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// Compilando e buildando o programa de shader
	GLuint shaderID = setupShader();
	glUseProgram(shaderID);

	// Enviando a matriz de projeção ortográfica para o shader
	mat4 projection = ortho(0.0f, (float)WIDTH, (float)HEIGHT, 0.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, value_ptr(projection));

	// Loop da aplicação
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		GLint colorLoc = glGetUniformLocation(shaderID, "inputColor");

		for (const auto &tri : triangles)
		{
			glBindVertexArray(tri.vao);

			mat4 model = mat4(1.0f);
			glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, value_ptr(model));
			glUniform4f(colorLoc, tri.color.r, tri.color.g, tri.color.b, 1.0f);

			glDrawArrays(GL_TRIANGLES, 0, 3);
		}

		glBindVertexArray(0);
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

// Função de callback de teclado
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

// Função de callback do mouse
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
	// Verifica se o botão esquerdo do mouse foi pressionado
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		double xpos, ypos;

		// Obtém a posição do cursor do mouse
		glfwGetCursorPos(window, &xpos, &ypos);

		// Converte para coordenadas do mundo (viewport já está ajustada com ortho)
		temp_vertices.push_back(vec3((float)xpos, (float)ypos, 0.0f));

		// Verifica se já temos três vértices para formar um triângulo
		if (temp_vertices.size() == 3)
		{
			Triangle tri;
			// Define os vértices do triângulo e a cor
			tri.v0 = temp_vertices[0];
			tri.v1 = temp_vertices[1];
			tri.v2 = temp_vertices[2];
			tri.color = colors[iColor];
			iColor = (iColor + 1) % colors.size();

			// Cria o VAO para o triângulo
			tri.vao = createTriangleFromPoints(tri.v0, tri.v1, tri.v2);

			// Adiciona o triângulo ao vetor de triângulos
			triangles.push_back(tri);

			// Limpa os vértices temporários para o próximo triângulo
			temp_vertices.clear();
		}
	}
}

// Função que cria um triângulo a partir de três pontos
// Retorna o identificador do VAO (Vertex Array Object) que contém a geometria
GLuint createTriangleFromPoints(vec3 v0, vec3 v1, vec3 v2)
{
	GLuint VAO, VBO;

	// Array de vértices do triângulo
	// Cada vértice é composto por 3 coordenadas (x, y, z)
	GLfloat vertices[] = {
		v0.x, v0.y, v0.z,
		v1.x, v1.y, v1.z,
		v2.x, v2.y, v2.z};

	// Geração do identificador do VAO (Vertex Array Object) e VBO (Vertex Buffer Object)
	glGenVertexArrays(1, &VAO);

	// Geração do identificador do VBO
	glGenBuffers(1, &VBO);

	// Faz a conexão (vincula) do VAO primeiro, e em seguida conecta e seta o(s) buffer(s) de vértices
	// e os ponteiros para os atributos
	glBindVertexArray(VAO);

	// Faz a conexão (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Envia os dados do array de floats para o buffer da OpenGL
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Para cada atributo do vértice, criamos um "AttribPointer" (ponteiro para o atributo), indicando:
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

	// Ativa o atributo do vértice
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	return VAO;
}

// Função que compila os shaders e cria o programa de shader
// Retorna o identificador do programa de shader
int setupShader()
{
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		cout << "Erro Vertex Shader:\n"
			 << infoLog << endl;
	}

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		cout << "Erro Fragment Shader:\n"
			 << infoLog << endl;
	}

	// Linkando os shaders e criando o identificador do programa de shader
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		cout << "Erro Shader Program:\n"
			 << infoLog << endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}