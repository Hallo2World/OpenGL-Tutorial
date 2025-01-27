#pragma once
#include <vector>
#include "Renderer.h"

struct VertexBufferElement
{
	unsigned int type;
	unsigned int count;
	bool normalized;
	static unsigned int getSizeOfType(unsigned int type)
	{
		switch (type)
		{
		case GL_FLOAT: return 4;
		case GL_UNSIGNED_INT: return 4;
		case GL_UNSIGNED_BYTE: return 1;
		}
		ASSERT(false);
		return 0;
	}
};

class VertexBufferLayout
{
private:
	std::vector<VertexBufferElement> m_elements;
	unsigned int m_stride;
public:
	VertexBufferLayout()
		: m_stride(0)
	{
	}

	~VertexBufferLayout() { }

	template<typename T>
	void push(unsigned int count)
	{
		static_assert(false);
	}

	template<>
	void push<float>(unsigned int count)
	{
		m_elements.push_back({GL_FLOAT, count, false});
		m_stride += VertexBufferElement::getSizeOfType(GL_FLOAT) * count;
	}

	template<>
	void push<unsigned int>(unsigned int count)
	{
		m_elements.push_back({ GL_UNSIGNED_INT, count, false });
		m_stride += VertexBufferElement::getSizeOfType(GL_UNSIGNED_INT) * count;
	}

	template<>
	void push<unsigned char>(unsigned int count)
	{
		m_elements.push_back({ GL_UNSIGNED_BYTE, count, true });
		m_stride += VertexBufferElement::getSizeOfType(GL_UNSIGNED_BYTE) * count;
	}

	inline const std::vector<VertexBufferElement> getElements() const { return m_elements; }

	inline unsigned int getStride() const { return m_stride;  }
};

