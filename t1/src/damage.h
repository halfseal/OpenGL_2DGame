#pragma once
#ifndef __DAMAGE_H__
#define __DAMAGE_H__
#include "gl/glad/glad.h"	// https://github.com/Dav1dde/glad
#define GLFW_INCLUDE_NONE
#include "gl/glfw/glfw3.h"	// http://www.glfw.org
#include <string>

#include "cgmath.h"
#include "ut.h"

void render_text(ivec2 &window_size, float tmp, std::string text, GLint x, GLint y, GLfloat scale,
                 vec4 color);

struct damage_info {
    std::string damage;
    float past_time;
    ivec2 pos;
};

damage_info last_damage = damage_info({"", 0.0f, ivec2(0, 0)});
damage_info my_damage = damage_info({"", 0.0f, ivec2(0, 0)});

inline void render_texts(ivec2 &window_size, float cur_as, float fix_as) {
    float widty_r = (float)window_size.x / 1280.0f;
    float height_r = (float)window_size.y / 720.0f;

    render_text(window_size, last_damage.past_time, last_damage.damage,
                last_damage.pos.x, last_damage.pos.y,
                min(widty_r, height_r),
                vec4(1.0f, 0.0f, 0.0f, 1.0f - last_damage.past_time));

    render_text(window_size, my_damage.past_time, my_damage.damage,
                my_damage.pos.x, my_damage.pos.y,
                min(widty_r, height_r),
                vec4(0.0f, 0.0f, 0.0f, 1.0f - my_damage.past_time));
}

inline void update_text(double delta_time) {
    last_damage.past_time += (float)delta_time;
    my_damage.past_time += (float)delta_time;
}

#endif
