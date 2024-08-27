#pragma once
#ifndef __VERTEX_ARRAY_H__
#define __VERTEX_ARRAY_H__

#include "cgmath.h"
#include "ut.h"


class vao_t
{
	GLuint vao_id_ = 0;
	GLuint vbo_id_ = 0;
	GLuint ibo_id_ = 0;
public:
	void bind() const;
	GLuint get_id() const;
	int set_vao();
	int set_vbo(const std::vector<vertex>& vertices);
	int set_ibo(const std::vector<uint>& indices);
};

inline int vao_t::set_vbo(const std::vector<vertex>& vertices)
{
	if (vbo_id_)
		glDeleteBuffers(1, &vbo_id_);
	vbo_id_ = 0;

	if (vertices.empty())
	{
		printf("[error] vertices is empty.\n");
		return 1;
	}

	glGenBuffers(1, &vbo_id_);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_id_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
	return 0;
}

inline int vao_t::set_ibo(const std::vector<uint>& indices)
{
	if (ibo_id_)
		glDeleteBuffers(1, &ibo_id_);
	ibo_id_ = 0;

	if (indices.empty())
	{
		printf("[error] indices is empty.\n");
		return 1;
	}

	glGenBuffers(1, &ibo_id_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_id_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);
	return 0;
}

inline int vao_t::set_vao()
{
	if (vao_id_)
		glDeleteVertexArrays(1, &vao_id_);
	vao_id_ = create_va(vbo_id_, ibo_id_);

	if (!vao_id_)
	{
		printf("%s(): failed to create vertex aray\n", __func__);
		return 1;
	}
	return 0;
}


inline void vao_t::bind() const
{
	glBindVertexArray(vao_id_);
}

inline GLuint vao_t::get_id() const
{
	return vao_id_;
}


#endif
