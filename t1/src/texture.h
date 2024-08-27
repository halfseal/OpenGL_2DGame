#pragma once
#ifndef __TEXTURE_H__
#define __TEXTURE_H__
#include "gl/glad/glad.h"	// https://github.com/Dav1dde/glad
#define GLFW_INCLUDE_NONE
#include "gl/glfw/glfw3.h"	// http://www.glfw.org
#include "ut.h"
#include "cgmath.h"
#include "program.h"


class texture_t {
public:
    void init_texture(char *image_path, bool b_mipmap, bool b_alpha);
    bool is_available() const;
    void bind(program_t &program, int color_type) const;
    void unbind();
private:
    GLuint id_ = 0;
    static GLuint create_texture(const char *image_path, bool b_mipmap, bool b_alpha);
};

inline void texture_t::init_texture(char *image_path, bool b_mipmap, bool b_alpha = false) {
    this->id_ = create_texture(image_path, b_mipmap, b_alpha);
}

inline bool texture_t::is_available() const { return this->id_ != 0; }

inline void texture_t::bind(program_t &program, int color_type = 1) const {
    // if (is_available()) {
    program.set_uniform<decltype(glUniform1i), int>(glUniform1i, "color_type", color_type);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, id_);
    program.set_uniform<decltype(glUniform1i), int>(glUniform1i, "TEX", 0);
    // }
    // else {
    // program.set_uniform<decltype(glUniform1i), int>(glUniform1i, "color_type", 0);
    // }
}

inline void texture_t::unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

inline GLuint texture_t::create_texture(const char *image_path, bool b_mipmap, bool b_alpha) {
    // load the image with vertical flipping
    std::unique_ptr<Image> img = LoadImage(image_path, b_alpha);
    if (!img)
        return 0; // Return 0 for invalid texture ID if image loading fails

    int w = img->size.x;
    int h = img->size.y;

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, b_alpha ? GL_RGBA : GL_RGB8, w, h, 0, b_alpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE,
                 img->data.get());

    // build mipmap
    if (b_mipmap && glGenerateMipmap) {
        int mip_levels = 0;
        for (int k = max(w, h); k; k >>= 1)
            mip_levels++;
        for (int l = 1; l < mip_levels; l++)
            glTexImage2D(GL_TEXTURE_2D, l, b_alpha ? GL_RGBA : GL_RGB8, max(1, w >> l), max(1, h >> l), 0,
                         b_alpha ? GL_RGBA : GL_RGB,
                         GL_UNSIGNED_BYTE, 0);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    // set up texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, b_mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    return texture;
}

#endif
