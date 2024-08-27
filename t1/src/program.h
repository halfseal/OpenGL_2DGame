#pragma once
#ifndef __PROGRAM_H__
#define __PROGRAM_H__

#include "ut.h"

class program_t
{
	GLuint id_ = 0;
public:
	void init(const char*, const char*);
	void bind() const;
	template <typename F, typename T1>
	void set_uniform(F, const GLchar*, T1);
	template <typename F, typename T1>
	void set_uniform(F, const GLchar*, GLsizei, T1);
	template <typename F, typename T1>
	void set_uniform(F, const GLchar*, GLboolean, GLsizei, T1);
};

inline void program_t::init(const char* vert_shader_path, const char* frag_shader_path)
{
	if (!(id_ = create_program(vert_shader_path, frag_shader_path)))
	{
		printf("error creating program_t\n");
		glfwTerminate();
		exit(1);
	}
}

template <typename F, typename T1>
inline void program_t::set_uniform(F func, const GLchar* uniform_name, T1 value)
{
	GLint uloc = glGetUniformLocation(id_, uniform_name);
	if (uloc > -1) func(uloc, value);
}

template <typename F, typename T1>
inline void program_t::set_uniform(F func, const GLchar* uniform_name, GLsizei num, T1 value)
{
	GLint uloc = glGetUniformLocation(id_, uniform_name);
	if (uloc > -1) func(uloc, num, value);
}

template <typename F, typename T1>
inline void program_t::set_uniform(F func, const GLchar* uniform_name, GLboolean is_transpose, GLsizei num, T1 value)
{
	GLint uloc = glGetUniformLocation(id_, uniform_name);
	if (uloc > -1) func(uloc, num, is_transpose, value);
}


inline void program_t::bind() const
{
	glUseProgram(id_);
}


#endif
