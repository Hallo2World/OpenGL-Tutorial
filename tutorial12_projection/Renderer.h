#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define ASSERT(x) if(!(x)) __debugbreak();
#define GLCall(x) GLClearError(); \
					x;\
					ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();

bool GLLogCall(const char* function, const char* file, int line);

class VertexArray;
class IndexBuffer;
class Shader;
class Renderer 
{
public:
	void clear();

	void draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader);
};