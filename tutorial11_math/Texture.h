#pragma once

#include "Renderer.h"
#include <string>

class Texture
{
private:
	unsigned int m_rendererId;
	std::string m_filepath;
	unsigned char* m_localBuffer;
	int m_width;
	int m_height;
	int m_bpp;

public:
	Texture(const std::string& filepath);
	~Texture();

	void bind(unsigned int slot = 0) const;
	void unbind() const;

	inline int getWidth() const { return m_width;  }
	inline int getHeight() const { return m_height; }
};

