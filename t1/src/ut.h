#ifndef __UT_H__
#define __UT_H__

// Standard headers
#include <stdio.h>
#include "gl/glad/glad.h" // https://github.com/Dav1dde/glad
#define GLFW_INCLUDE_NONE
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <stdlib.h>
#include <vector>
#include "gl/glfw/glfw3.h" // http://www.glfw.org

#include "irrKlang/irrKlang.h"
#pragma comment(lib, "irrKlang.lib")

#pragma comment(lib, "glfw3.x86.lib")

class MemoryBuffer {
public:
    std::unique_ptr<char[]> data;
    size_t size = 0;

    MemoryBuffer() = default;
    MemoryBuffer(size_t size) : data(std::make_unique<char[]>(size + 1)), size(size) {
        data[size] = '\0'; // Null-terminate
    }

    // Delete copy constructor and copy assignment operator
    MemoryBuffer(const MemoryBuffer &) = delete;
    MemoryBuffer &operator=(const MemoryBuffer &) = delete;

    // Define move constructor and move assignment operator
    MemoryBuffer(MemoryBuffer &&other) noexcept : data(std::move(other.data)), size(other.size) { other.size = 0; }

    MemoryBuffer &operator=(MemoryBuffer &&other) noexcept {
        if (this != &other) {
            data = std::move(other.data);
            size = other.size;
            other.size = 0;
        }
        return *this;
    }

    ~MemoryBuffer() = default;
};

// Vertex structure definition
struct vertex {
    vec3 pos;
    vec3 norm;
    vec2 tex;
};

// Function to read binary file into a MemoryBuffer
inline MemoryBuffer read_binary(const char *file_path) {
    FILE *fp = fopen(file_path, "rb");
    if (!fp)
        return MemoryBuffer();

    fseek(fp, 0L, SEEK_END);
    size_t file_size = ftell(fp);
    rewind(fp);

    MemoryBuffer buffer(file_size);
    fread(buffer.data.get(), 1, file_size, fp);
    fclose(fp);

    return buffer;
}

// Function to create a GLFW window
inline GLFWwindow *create_window(const char *name, int &width, int &height, bool show_window = true) {
    if (!glfwInit())
        return nullptr;

    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *win = glfwCreateWindow(width, height, name, nullptr, nullptr);
    if (!win)
        return nullptr;

    glfwMakeContextCurrent(win);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    if (show_window)
        glfwShowWindow(win);

    return win;
}

// Function to create a shader from source
inline GLuint create_shader(const char *shader_source, GLenum shader_type, std::string &log) {
    const char *src[] = {"#version 330\n", shader_source};
    GLuint shader = glCreateShader(shader_type);
    glShaderSource(shader, 2, src, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        log = infoLog;
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

// Function to create a shader program from vertex and fragment shaders
inline GLuint create_program(const char *vert_path, const char *frag_path) {
    MemoryBuffer vert_shader_code = read_binary(vert_path);
    MemoryBuffer frag_shader_code = read_binary(frag_path);

    if (!vert_shader_code.data || !frag_shader_code.data)
        return 0;

    std::string log;
    GLuint vertex_shader = create_shader(vert_shader_code.data.get(), GL_VERTEX_SHADER, log);
    GLuint fragment_shader = create_shader(frag_shader_code.data.get(), GL_FRAGMENT_SHADER, log);

    if (!vertex_shader || !fragment_shader)
        return 0;

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Program Linking Failed: " << infoLog << std::endl;
        glDeleteProgram(program);
        return 0;
    }

    return program;
}

// Function to create a Vertex Array Object (VAO)
inline GLuint create_va(GLuint vertex_buffer, GLuint index_buffer = 0) {
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    if (index_buffer)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)offsetof(vertex, pos));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)offsetof(vertex, norm));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)offsetof(vertex, tex));

    glBindVertexArray(0);
    return vao;
}

// Image structure using smart pointers
struct Image {
    std::unique_ptr<unsigned char[]> data;
    ivec2 size{0, 0};
    int channels = 0;

    Image() = default;
    ~Image() = default;
};

extern "C" {
void stbi_set_flip_vertically_on_load(int flag_true_if_should_flip);
unsigned char *stbi_load(const char *filename, int *x, int *y, int *comp, int req_comp);
}

// Function to load an image with optional alpha and vertical flip
inline std::unique_ptr<Image> LoadImage(const char *filePath, bool useAlpha = false, bool flipVertically = true) {
    std::unique_ptr<Image> img = std::make_unique<Image>();
    stbi_set_flip_vertically_on_load(flipVertically);
    int desiredChannels = useAlpha ? 4 : 3;
    img->data.reset(stbi_load(filePath, &img->size.x, &img->size.y, &img->channels, desiredChannels));
    if (!img->data)
        return nullptr;
    return img;
}

#endif // __UT_H__
