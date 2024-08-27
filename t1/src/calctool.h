#pragma once
#ifndef __CALCTOOL_H__
#define __CALCTOOL_H__

#include "cgmath.h"

static const vec4 red = vec4(1.0f, 0.0f, 0.0f, 1.0f);
static const vec4 white = vec4(1.0f, 1.0f, 1.0f, 1.0f);
static const vec4 black = vec4(0.0f, 0.0f, 0.0f, 1.0f);
static const vec4 yellow = vec4(1.0f, 1.0f, 0.0f, 1.0f);
static const vec4 orange = vec4(1.0f, 0.5f, 0.0f, 1.0f);
static const vec4 red_red = vec4(227.0f / 255.0f, 135.0f / 255.0f, 108.0f / 255.0f, 1.0f);
static const vec4 dark_light_red = vec4(228.0f / 255.0f, 184.0f / 255.0f, 165.0f / 255.0f, 1.0f);
static const vec4 light_light_red = vec4(229.0f / 255.0f, 209.0f / 255.0f, 197.0f / 255.0f, 1.0f);

static const vec4 blue = vec4(8.0f / 255.0f, 96.0f / 255.0f, 168.0f / 255.0f, 1.0f);
static const vec4 dark_light_blue = vec4(80.0f / 255.0f, 133.0f / 255.0f, 188.0f / 255.0f, 1.0f);
static const vec4 light_light_blue = vec4(217.0f / 255.0f, 229.0f / 255.0f, 240.0f / 255.0f, 1.0f);


inline auto get_2D_scale_mx(const vec3 &tmp) -> mat4 {
    return mat4({tmp.x, 0, 0, 0, 0, tmp.y, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1});
}

inline auto get_2D_trans_mx(const vec3 &tmp) -> mat4 {
    return mat4({1, 0, 0, tmp.x, 0, 1, 0, tmp.y, 0, 0, 1, 0, 0, 0, 0, 1});
}

inline bool is_clicked_square(const vec3 &scale_vec, const vec3 &trans_vec, const vec2 &mouse_pos) {
    return trans_vec.x - scale_vec.x < mouse_pos.x && mouse_pos.x < trans_vec.x + scale_vec.x &&
        trans_vec.y - scale_vec.y < mouse_pos.y && mouse_pos.y < trans_vec.y + scale_vec.y;
}

inline std::vector<vec3> get_actual_coord(const vec3 &scale_vec, const vec3 &trans_vec) {
    return {vec3(scale_vec.x + trans_vec.x, scale_vec.y + trans_vec.y, scale_vec.z + trans_vec.z),
            vec3(-scale_vec.x + trans_vec.x, scale_vec.y + trans_vec.y, scale_vec.z + trans_vec.z),
            vec3(-scale_vec.x + trans_vec.x, -scale_vec.y + trans_vec.y, scale_vec.z + trans_vec.z),
            vec3(scale_vec.x + trans_vec.x, -scale_vec.y + trans_vec.y, scale_vec.z + trans_vec.z)};
}

inline std::vector<ivec2> get_window_coord(const vec3 &scale_vec, const vec3 &trans_vec, const ivec2 &window_size,
                                           float changeable_aspect, float aspect) {
    std::vector<ivec2> ret;
    auto ac = std::move(get_actual_coord(scale_vec, trans_vec));
    float wx = (float)window_size.x, wy = (float)window_size.y;
    for (auto &aci : ac) {
        float rx = aci.x, ry = aci.y;
        int mx = (int)((rx) * min(1.0f / changeable_aspect, 1.0f / aspect) * (wx / 2.0f) + (wx / 2.0f));
        int my = (int)((ry) * min(changeable_aspect / aspect, 1.0f) * (-wy / 2.0f) + (wy / 2.0f));
        ret.emplace_back(mx, my);
    }
    return ret;
}

inline bool is_theta_pos(float theta) { return cos(theta) >= 0.0f; }

inline mat4 get_2D_rotate_mx(float theta) {
    return mat4({cos(theta), -sin(theta), 0, 0, sin(theta), cos(theta), 0, 0, 0, 0, 1, 0, 0, 0, 0, 1});
}

inline bool aabb(std::vector<vec3> &a, std::vector<vec3> &b) {
    return (a[0].x > b[1].x) && (a[1].x < b[0].x) && (a[2].y < b[1].y) && (a[1].y > b[2].y);
}

#endif
