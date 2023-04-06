#include "Shader.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include "Renderer.h"


Shader::Shader(const std::string& filepath)
	: m_filepath(filepath)
{
	ShaderProgramSource source = ParseShader(filepath);
	m_rendererId = CreateShader(source.vertexSource, source.fragmentSource);
	GLCall(glUseProgram(m_rendererId));
}

Shader::~Shader()
{
	GLCall(glDeleteProgram(m_rendererId));
}

void Shader::bind() const
{
	GLCall(glUseProgram(m_rendererId));
}

void Shader::unbind() const
{
	GLCall(glUseProgram(0));
}

void Shader::setUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

ShaderProgramSource Shader::ParseShader(const std::string& filepath)
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

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
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

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
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

int Shader::GetUniformLocation(const std::string& name)
{
	if (m_uniformLocationCache.find(name) != m_uniformLocationCache.end())
	{
		return m_uniformLocationCache.at(name);
	}
	else
	{
		GLCall(int location = glGetUniformLocation(m_rendererId, name.c_str()));
		if (location == -1) {
			std::cerr << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;
		}
		else
		{
			m_uniformLocationCache[name] = location;
		}
		return location;
	}

}