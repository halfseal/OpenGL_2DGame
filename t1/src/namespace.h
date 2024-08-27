#pragma once
#ifndef __NAMESPACE_H__
#define __NAMESPACE_H__
#include "gl/glad/glad.h"	// https://github.com/Dav1dde/glad
#define GLFW_INCLUDE_NONE
#include "gl/glfw/glfw3.h"	// http://www.glfw.org
#include "ut.h"
#include "particle.h"
#include "damage.h"


class key {
public:
    bool is_pressed = false;
    int key_type;
};

struct time_stopper {
    bool is_stopped = false;
    float time_passed, should_pass;
    void turn_on(float should_pass);
    void update(double delta);
    void turn_off();
};

inline void time_stopper::turn_on(float should) {
    is_stopped = true;
    time_passed = 0.0f, should_pass = should;
}

inline void time_stopper::update(double delta) {
    if (should_pass < 0.0f || !is_stopped)
        return;

    time_passed += (float)delta;
    if (time_passed > should_pass)
        is_stopped = false;
}

inline void time_stopper::turn_off() { is_stopped = false; }

struct camera_t {
    const vec3 const_eye = vec3(0, 0, 1);
    const vec3 const_at = vec3(0, 0, 0);
    vec3 eye = const_eye;
    vec3 at = const_at;
    vec3 up = vec3(0, 1, 0);
    mat4 view_matrix = mat4::look_at(eye, at, up);

    const mat4 xyz_to_rhc = {0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1};
};

enum scene {
    battle_scene,
    title_scene,
    pause_scene,
    lose_scene,
    win_scene,
    help_scene,
    ult_scene,
};

namespace var {
    const char *window_name = "Battle";
    const char *vert_shader_path = "../bin/shaders/circ.vert";
    const char *frag_shader_path = "../bin/shaders/circ.frag";

    GLFWwindow *window = nullptr;
    ivec2 window_size = ivec2(1280, 720);

    program_t program;
    program_t program2;
    vao_t vao;
    camera_t cam;

    scene cur_scene = title_scene;
    scene last_scene = help_scene;


    irrklang::ISoundEngine *engine;
    irrklang::ISoundSource *bgm;
    irrklang::ISoundSource *player_jump_music;
    irrklang::ISoundSource *player_drinking_music;
    irrklang::ISoundSource *player_attack_music;
    irrklang::ISoundSource *player_hit_music;
    irrklang::ISoundSource *player_c_attack_music;
    irrklang::ISoundSource *player_teleport_music;
    irrklang::ISoundSource *enemy_attack1_music;
    irrklang::ISoundSource *enemy_attack2_music;
    irrklang::ISoundSource *enemy_dash1_music;
    irrklang::ISoundSource *enemy_dash2_music;
    irrklang::ISoundSource *enemy_jump1_music;
    irrklang::ISoundSource *enemy_jump2_music;
    irrklang::ISoundSource *enemy_jump3_music;
    irrklang::ISoundSource *poop_gone_music;

    std::vector<vertex> rect_vertexes;
    std::vector<uint> rect_indices;

    float aspect = window_size.x / float(window_size.y);
    float changeable_aspect = window_size.x / float(window_size.y);
    float theta;

    float op_ratio = 1.0f;

    time_stopper stopper;

    key attack_key;
    key move_key;


    // particles particle_set1;
    particles particle_set2;
#include "character.h"
    title title_screen;
    pause pause_screen;
    lose lose_screen;
    win win_screen;
    help help_screen;
    ult ult_screen;

    stars player_star;
    bool is_new_star_added;

    sound_button sound_control;

    int skill_type = -1;
    skill_descriptions still_descriptor;

    cooltime_controler home_controler;
    cooltime_controler ult_controler;

    skills skill_screen;
    rect background;
    rect floor;
    character player;
    player_hp_bar_r player_hp_bar;
    bad_guy_hp_bar_r bad_guy_hp_bar;
    attack_skill d_attack;
    card_skill c_attack;
    bad_guy opponent;
    poops poop;
}


#endif
