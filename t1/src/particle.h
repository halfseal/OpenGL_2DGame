#pragma once
#ifndef __PARTICLE_H__
#define __PARTICLE_H__
#include "cgmath.h"
#include "texture.h"
#include "calctool.h"

#define max_particle 1000


class particle {
public:
    const vec3 size = vec3(0.025f, 0.025f, 1.0f);
    vec3 pos;
    vec3 velocity;
    vec3 gravity;
    vec4 color;
    float life;
    float fade;
};

class particles {
    particle particle_container_[max_particle];
    texture_t texture_;
    void gen_a_particle(particle &a_particle, vec3 pos, const vec4 &color);

public:
    void update(double delta_time, vec3 &player_pos, const vec4 &new_col, const vec4 &col1, const vec4 &col2,
                const vec4 &col3);
    void draw(program_t &program, vec3 player_pos, int size);
    void initialize_particles(const vec4 &color);
};

inline void particles::gen_a_particle(particle &a_particle, vec3 pos, const vec4 &color) {
    a_particle.pos = pos;
    a_particle.velocity =
        vec3(float(rand() % 200 - 100) * 0.01f, float(rand() % 400 - 50) * 0.001f, 1.0f);
    a_particle.gravity = vec3(0.0f, -0.1f, 0.0f);
    a_particle.color = color;
    a_particle.life = 1.0f;
    a_particle.fade = float(rand() % 1000 + 1) * 0.5f;
}

inline void particles::update(double delta_time, vec3 &player_pos, const vec4 &new_col, const vec4 &col1,
                              const vec4 &col2,
                              const vec4 &col3) {
    for (int i = 0; i < max_particle; i++) {
        particle &tmp = particle_container_[i];
        auto new_vel = tmp.velocity + tmp.gravity * (float)delta_time;
        auto new_pos = tmp.pos + tmp.velocity * (float)delta_time;
        tmp.pos = new_pos;

        tmp.velocity = new_vel;
        tmp.life -= tmp.fade * (float)delta_time;

        if (tmp.life < 0.0f)
            gen_a_particle(tmp, player_pos, new_col);
        else if (tmp.life < 0.4f)
            tmp.color = col1;
        else if (tmp.life < 0.6f)
            tmp.color = col2;
        else if (tmp.life < 0.75f)
            tmp.color = col3;
    }
}

inline void particles::draw(program_t &program, vec3 player_pos, int size) {
    mat4 model_matrix = mat4({1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1});
    // glDisable(GL_DEPTH_TEST);
    texture_.bind(program, 2);
    for (int i = 0; i < max_particle; i++) {
        particle &tmp = particle_container_[i];

        program.set_uniform<decltype(glUniform4fv), vec4>
            (glUniform4fv, "custom_color", 1, tmp.color);
        program.set_uniform<decltype(glUniformMatrix4fv), mat4>
            (glUniformMatrix4fv, "model_matrix", 1, GL_TRUE,
             get_2D_trans_mx(tmp.pos) * /*get_2D_trans_mx(player_pos) **/
             get_2D_scale_mx(tmp.size) * model_matrix);
        glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, nullptr);
    }
    texture_.unbind();
    // glEnable(GL_DEPTH_TEST);
}

inline void particles::initialize_particles(const vec4 &color) {
    texture_.init_texture("../bin/images/light.png", true, true);
    for (int i = 0; i < max_particle; i++) {
        gen_a_particle(particle_container_[i], vec3(0.0f, 0.0f, 0.0f), color);
    }
}
#endif
