#include "Renderer.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include "Shader.h"
#include <stdio.h>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <cassert>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"


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
		-0.5f, -0.5f, 0.0f, 0.0f,
		0.5f, -0.5f, 1.0f, 0.0f,
		0.5f, 0.5f, 1.0f, 1.0f,
		-0.5f, 0.5f, 0.0f, 1.0f,
	};

	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	//GLCall(glBlendEquation(GL_FUNC_ADD));

	/* Create vertex array object(VAO) */
	VertexArray va;


	/* Create vertex buffer */
	VertexBuffer vb(positions, sizeof(positions));
{
	/* Set vertex buffer layout */
	VertexBufferLayout layout;
	layout.push<float>(2);
	layout.push<float>(2);

	va.addBuffer(vb, layout);

	/* Create index buffer object */
	IndexBuffer ib(indices, sizeof(indices));

	glm::mat4 proj = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);

	/* Create shader */
	Shader shader("res/shader/Basic.shader");
	shader.setUniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);
	shader.setUniformMat4f("u_MVP", proj);

	/* Create texture */
	Texture texture("res/texture/ground.png");
	texture.bind();
	shader.setUniform1i("u_Texture", 0);

	va.unbind();
	shader.unbind();
	vb.unbind();
	ib.unbind();

	Renderer renderer;

	float r = 0.0f;
	float increment = 0.05f;
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		renderer.clear();
		
		/* Chanage shader fragment red channel value by Uniform*/
		shader.bind();
		//shader.setUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);

		renderer.draw(va, ib, shader);

		if (r > 1.0f) {
			increment = -0.05f;
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
}
	glfwTerminate();
	return 0;
}