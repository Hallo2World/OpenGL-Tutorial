#pragma once
#include <string>
#include <unordered_map>

struct ShaderProgramSource {
	std::string vertexSource;
	std::string fragmentSource;
};

enum class ShaderType
{
	NONE = -1, VERTEX = 0, FRAGMENT = 1
};

class Shader
{
private:
	unsigned int m_rendererId;
	std::string m_filepath;
	std::unordered_map<std::string, int> m_uniformLocationCache;

public:
	Shader(const std::string& filepath);

	~Shader();

	void bind() const;

	void unbind() const;

	void setUniform4f(const std::string& name, float v0, float v1, float v2, float v3);

private:
	static ShaderProgramSource ParseShader(const std::string& filepath);

	static unsigned int CompileShader(unsigned int type, const std::string& source);

	static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);

	int GetUniformLocation(const std::string& name);
};

