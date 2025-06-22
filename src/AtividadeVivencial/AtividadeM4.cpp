/* Parallax com movimentação do player em OpenGL + GLFW
 *
 * Processamento Gráfico - Unisinos
 * Última atualização em 22/06/2025
 */

#include <iostream>
#include <string>
#include <assert.h>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

struct Sprite
{
	GLuint VAO;
	GLuint texID;
	vec3 position;
	vec3 dimensions;
};

struct Layer
{
	GLuint texture;
	float depthFactor;
	vec2 position;
	vec2 scale;
	float rotation;
};

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
GLuint setupShader();
GLuint setupSprite();
GLuint loadTexture(string filePath);

const GLuint WIDTH = 800, HEIGHT = 600;
Sprite player;
vector<Layer> backgroundLayers;

const GLchar *vertexShaderSource = R"(
 #version 400
 layout (location = 0) in vec3 position;
 layout (location = 1) in vec2 texc;
 out vec2 tex_coord;
 uniform mat4 model;
 uniform mat4 projection;
 void main()
 {
	tex_coord = vec2(texc.s, 1.0 - texc.t);
	gl_Position = projection * model * vec4(position, 1.0);
 }
 )";

const GLchar *fragmentShaderSource = R"(
 #version 400
 in vec2 tex_coord;
 out vec4 color;
 uniform sampler2D tex_buff;
 uniform vec2 offsetTex;

 void main()
 {
	 color = texture(tex_buff,tex_coord + offsetTex);
 }
 )";

int main()
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Criação da janela GLFW
	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Parallax Scene", nullptr, nullptr);
	if (!window)
	{
		std::cerr << "Falha ao criar a janela GLFW" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Falha ao inicializar GLAD" << std::endl;
		return -1;
	}

	GLuint shaderID = setupShader();
	glUseProgram(shaderID);

	GLuint vao = setupSprite();
	GLuint playerTex = loadTexture("../assets/sprites/player.png");
	player = {vao, playerTex, vec3(WIDTH / 2, HEIGHT / 2, 0), vec3(64, 64, 1)};

	vector<const char *> texturePaths = {
		"../assets/textures/sky.png",
		"../assets/textures/rocks.png",
		"../assets/textures/clouds.png",
		"../assets/textures/hills2.png",
		"../assets/textures/hills1.png",
		"../assets/textures/trees.png",
		"../assets/textures/ground.png"};

	int layerIndex = 0;
	for (auto &path : texturePaths)
	{
		Layer layer;
		layer.texture = loadTexture(path);
		layer.depthFactor = 0.2f * layerIndex++;
		layer.position = vec2(WIDTH / 2.0f, HEIGHT / 2.0f);
		layer.scale = vec2(WIDTH, HEIGHT);
		layer.rotation = 0.0f;
		backgroundLayers.push_back(layer);
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUniform1i(glGetUniformLocation(shaderID, "tex_buff"), 0);
	mat4 projection = ortho(0.0f, (float)WIDTH, 0.0f, (float)HEIGHT, -1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, value_ptr(projection));

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		GLuint modelLoc = glGetUniformLocation(shaderID, "model");
		for (const auto &layer : backgroundLayers)
		{
			for (int i = -1; i <= 1; ++i)
			{
				mat4 model = mat4(1.0f);
				vec2 pos = layer.position + vec2(i * WIDTH - player.position.x, 0.0f);
				model = translate(model, vec3(pos, 0.0f));
				model = rotate(model, layer.rotation, vec3(0.0f, 0.0f, 1.0f));
				model = scale(model, vec3(layer.scale, 1.0f));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));

				glUniform2f(glGetUniformLocation(shaderID, "offsetTex"), 0.0f, 0.0f);
				glBindVertexArray(player.VAO);
				glBindTexture(GL_TEXTURE_2D, layer.texture);
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			}
		}

		mat4 model = mat4(1.0f);
		model = translate(model, player.position);
		model = scale(model, player.dimensions);
		glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, value_ptr(model));

		glUniform2f(glGetUniformLocation(shaderID, "offsetTex"), 0.0f, 0.0f);
		glBindVertexArray(player.VAO);
		glBindTexture(GL_TEXTURE_2D, player.texID);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

// Esta função define a posição do sprite, o número de animações e frames
// Retorna o identificador do VAO (Vertex Array Object) do sprite
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		switch (key)
		{
		case GLFW_KEY_LEFT:
			player.position.x -= 10.0f;
			break;
		case GLFW_KEY_RIGHT:
			player.position.x += 10.0f;
			break;
		case GLFW_KEY_UP:
			player.position.y += 10.0f;
			break;
		case GLFW_KEY_DOWN:
			player.position.y -= 10.0f;
			break;
		}
	}

	if (player.position.x < 0)
		player.position.x = 800.0f;
	if (player.position.x > 800)
		player.position.x = 0.0f;

	if (player.position.y < 100)
		player.position.y = 100.0f;
	if (player.position.y > 600)
		player.position.y = 600.0f;
}

// Esta função compila os shaders e retorna o identificador do programa de shader
// O Vertex Shader é responsável por transformar as coordenadas dos vértices
GLuint setupShader()
{
	// Vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
				  << infoLog << std::endl;
	}

	// Fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
				  << infoLog << std::endl;
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
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
				  << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

// Esta função cria um sprite (um quadrado) que pode ser animado
// O sprite é definido por um VAO (Vertex Array Object) e uma textura
GLuint setupSprite()
{
	GLfloat vertices[] = {
		-0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
		0.5f, -0.5f, 0.0f, 1.0f, 0.0f};

	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Ponteiro pro atributo 0 - Posição - coordenadas x, y, z
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);

	// Ponteiro pro atributo 1 - Coordenada de textura s, t
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return VAO;
}

// Esta função carrega uma textura de um arquivo de imagem
// Retorna o identificador da textura carregada
GLuint loadTexture(string filePath)
{
	GLuint texID;

	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int width, height, nrChannels;

	unsigned char *imageData = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);

	if (!imageData)
	{
		std::cout << "Falha ao carregar textura: " << filePath << std::endl;
		return 0;
	}

	GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, imageData);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(imageData);

	return texID;
}