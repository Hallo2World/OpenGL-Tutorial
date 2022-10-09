#include <GL/glew.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <cassert>

#define ASSERT(x) if(!(x)) __debugbreak();
#define GLCall(x) GLClearError(); \
					x;\
					ASSERT(GLLogCall(#x, __FILE__, __LINE__))

static void GLClearError() 
{
	while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError()) {
		std::cout << "[OpenGL Error] (" << error << "): " << function << " " << file << " " << line << " " << std::endl;
		return false;
	}
	return true;
}

enum class ShaderType
{
	NONE=-1, VERTEX = 0, FRAGMENT=1
};

struct ShaderProgramSource {
	std::string vertexSource;
	std::string fragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath)
{
	std::ifstream fin(filepath);
	std::string line;
	std::stringstream ss[2];

	ShaderType shaderType = ShaderType::NONE;
	while (getline(fin, line)) {
		if (line.find("#shader") != std::string::npos) {
			if (line.find("vertex") != std::string::npos) {
				shaderType = ShaderType::VERTEX;
			}
			else
			{
				shaderType = ShaderType::FRAGMENT;
			}
		}
		else
		{
			ss[static_cast<int>(shaderType)] << line << std::endl;
		}
	}
	return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
	unsigned int shader = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(shader, 1, &src, nullptr);
	glCompileShader(shader);

	int result;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(shader, length, &length, message);

		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
		std::cout << message << std::endl;
		return 0;
	}
	return shader;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	unsigned int program = glCreateProgram();
	
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);

	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}


int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	/* Initialize GLEW */
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	fprintf(stdout, "%s\n", glGetString(GL_VERSION));

	GLfloat positions[] = {
		-0.5f, -0.5f,
		0.5f, -0.5f,
		0.5f, 0.5f,
		-0.5f, 0.5f
	};

	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};
	
	/* Create vertex array object(VAO) */
	unsigned int vao;
	GLCall(glGenVertexArrays(1, &vao));
	GLCall(glBindVertexArray(vao));

	/* Create vertex array buffer */
	GLuint buffer;
	GLCall(glGenBuffers(1, &buffer));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW));

	/* Set vertex layout */
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2, 0));	// Link vertex buffer to VAO

	/* Create index buffer object */
	unsigned int ibo;
	GLCall(glGenBuffers(1, &ibo));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW));

	/* Create shader */
	ShaderProgramSource source = ParseShader("res/shader/Basic.shader");
	std::cout << source.vertexSource << std::endl;
	std::cout << source.fragmentSource << std::endl;
	unsigned int shader = CreateShader(source.vertexSource, source.fragmentSource);
	glUseProgram(shader);


	GLCall(int location = glGetUniformLocation(shader, "u_Color"));
	ASSERT(location != -1);
	glUniform4f(location, 0.2f, 0.3f, 0.8f, 1.0f);

	GLCall(glBindVertexArray(0));
	GLCall(glUseProgram(0));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

	float r = 0.0f;
	float increment = 0.05f;
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */ 
		glClear(GL_COLOR_BUFFER_BIT);

		GLCall(glUseProgram(shader));
		GLCall(glBindVertexArray(vao));
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));

		//GLClearError();
		GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
		//assert(glGetError() == GL_NO_ERROR);
		//ASSERT(GLCheckError()); 

		/* Chanage shader fragment red channel value by Uniform*/
		glUniform4f(location, r, 0.3f, 0.8f, 1.0f);
		if (r > 1.0f) {
			increment  = -0.05f;
		}
		else if (r < 0.0) {
			increment = 0.05f;
		}
		r += increment;

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glDeleteProgram(shader);

	glfwTerminate();
	return 0;
}