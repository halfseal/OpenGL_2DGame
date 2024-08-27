#pragma once
#ifndef __RECT_H__
#define __RECT_H__

#include "ut.h"
#include "cgmath.h"
#include "namespace.h"
#include "calctool.h"


class rect_interface {
public :
    ~rect_interface() = default;
    const mat4 identity_matrix = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
    virtual mat4 const get_model_mx();
    virtual mat4 get_trans_mx() = 0;
    virtual mat4 get_scale_mx() = 0;
    virtual mat4 get_rotate_mx();
    virtual vec3 get_trans_vec() = 0;
    virtual vec3 get_scale_vec() = 0;
    void draw();
    void print_coordinate();
    void init_tex(char *path);
    texture_t texture;
private:
};

inline mat4 const rect_interface::get_model_mx() {
    return get_trans_mx() * get_rotate_mx() * get_scale_mx() * identity_matrix;
}

inline mat4 rect_interface::get_rotate_mx() { return identity_matrix; }

inline void rect_interface::draw() {
    if (texture.is_available()) {
        texture.bind(var::program, 1);
        var::program.set_uniform<decltype(glUniformMatrix4fv), mat4>
            (glUniformMatrix4fv, "model_matrix", 1, GL_TRUE, get_model_mx());
        glDrawElements(GL_TRIANGLES, var::rect_indices.size(), GL_UNSIGNED_INT, nullptr);
        texture.unbind();
    }
    else {
        var::program.set_uniform<decltype(glUniform1i), int>(glUniform1i, "color_type", 0);
        var::program.set_uniform<decltype(glUniformMatrix4fv), mat4>
            (glUniformMatrix4fv, "model_matrix", 1, GL_TRUE, get_model_mx());
        glDrawElements(GL_TRIANGLES, var::rect_indices.size(), GL_UNSIGNED_INT, nullptr);
    }
}

inline void rect_interface::init_tex(char *path) {
    texture.init_texture(path, true, true);
}


inline void rect_interface::print_coordinate() {
    auto tmp = std::move(get_actual_coord(get_scale_vec(), get_trans_vec()));
    printf("(%f %f %f),\t(%f %f %f),\t(%f %f %f),\t(%f %f %f)\n", tmp[0].x,
           tmp[0].y, tmp[0].z, tmp[1].x,
           tmp[1].y, tmp[1].z, tmp[2].x,
           tmp[2].y, tmp[2].z, tmp[3].x,
           tmp[3].y, tmp[3].z);
}

class rect : public rect_interface {
public:
    ~rect() = default;
    mat4 get_trans_mx() override;
    mat4 get_scale_mx() override;
    mat4 get_rotate_mx() override;
    vec3 get_trans_vec() override;
    vec3 get_scale_vec() override;
    void set_theta(float new_theta);
    void set_trans_vec(vec3 &trans);
    void set_scale_vec(vec3 &scale);
    float falling_speed;
    float get_theta() const;

    void initialize(const vec3 &scale, const vec3 &trans);

    vec3 velocity;
    vec3 accelerator;

    float born_day;

    vec4 color;

private:

    vec3 scale_vec_;
    vec3 trans_vec_;
    float theta_;
};


inline mat4 rect::get_trans_mx() { return get_2D_trans_mx(get_trans_vec()); }

inline mat4 rect::get_scale_mx() { return get_2D_scale_mx(get_scale_vec()); }

inline mat4 rect::get_rotate_mx() { return get_2D_rotate_mx(theta_); }

inline vec3 rect::get_trans_vec() { return trans_vec_; }

inline vec3 rect::get_scale_vec() { return scale_vec_; }

inline void rect::set_theta(float new_theta) { theta_ = new_theta; }

inline void rect::set_trans_vec(vec3 &trans) { trans_vec_ = trans; }

inline void rect::set_scale_vec(vec3 &scale) { scale_vec_ = scale; }

inline float rect::get_theta() const { return theta_; }

inline void rect::initialize(const vec3 &scale, const vec3 &trans) {
    scale_vec_ = scale;
    trans_vec_ = trans;
    falling_speed = 1.0f;
    theta_ = 0.0f;
}


class poops {
    const float size_min = 0.1f, size_max = 0.2f;
    float last_gen;

public:
    float difficulty;
    void initialize();
    void set_difficulty(float diff);
    ~poops() = default;
    std::list<rect> poop_list;
    void update(double time);
    void draw();
};

inline void poops::initialize() {
    float last_gen = 0.0f;
    poop_list.clear();
    difficulty = 0.1f;
}

inline void poops::set_difficulty(float diff) { difficulty = diff; }

inline void poops::update(double delta_time) {
    float gentime;

    gentime = 0.05f + var::op_ratio / 10.0f + difficulty;

    if (var::theta - last_gen > gentime) {
        last_gen = var::theta;

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> rand_size((double)size_min, (double)size_max);
        std::uniform_real_distribution<> rand_pos(-(double)var::aspect, (double)var::aspect);
        float f_random_size = (float)rand_size(gen);
        rect tmp;
        tmp.initialize(
            vec3(f_random_size, f_random_size, 1.0f),
            vec3((float)rand_pos(gen), 1.0f, 1.0f)
            );

        tmp.texture.init_texture("../bin/images/jelly.png", true, true);

        std::uniform_real_distribution<> fall_speed(1.0, 2.0);
        tmp.falling_speed = (float)fall_speed(gen);

        std::uniform_real_distribution<> rand_num(0.5, 1.0);

        float fst = (float)rand_num(gen);
        float snd = (float)rand_num(gen);
        float thd = (float)rand_num(gen);
        tmp.color = vec4(fst, snd, thd, 1.0f);

        poop_list.push_back(tmp);
    }

    for (rect &rect_ : poop_list) {
        rect_.set_trans_vec(vec3(rect_.get_trans_vec().x,
                                 rect_.get_trans_vec().y - rect_.falling_speed * (float)delta_time,
                                 1.0f));
    }


    for (auto it = poop_list.begin(); it != poop_list.end();) {
        if ((*it).get_trans_vec().y < -1.0f) {
            poop_list.erase(it++);
        }
        else
            ++it;
    }
}

inline void poops::draw() {
    std::random_device rd;
    std::mt19937 gen(rd());

    for (rect &rect_ : poop_list) {
        rect_.texture.bind(var::program, 2);
        var::program.set_uniform<decltype(glUniform4fv), vec4>(glUniform4fv, "custom_color", 1, rect_.color);

        var::program.set_uniform<decltype(glUniformMatrix4fv), mat4>(glUniformMatrix4fv, "model_matrix", 1, GL_TRUE,
                                                                     rect_.get_model_mx());
        glDrawElements(GL_TRIANGLES, var::rect_indices.size(), GL_UNSIGNED_INT, nullptr);
        rect_.texture.unbind();
    }
}

class player_hp_bar_r {
    const float c_height_ = 0.01f;
    const float c_width_ = 0.1f;
    const float above = 0.05f;

    rect red_bar;
    rect black_bar;

public:
    void initialize(vec3 &p_scale, vec3 &p_trans);
    void update(vec3 &p_scale, vec3 &p_trans, float ratio);
    void draw();
};

inline void player_hp_bar_r::initialize(vec3 &p_scale, vec3 &p_trans) {
    const float ratio = 1.0f;
    red_bar.initialize(
        vec3(c_width_ * ratio, c_height_, 1.0f),
        vec3(p_trans.x - (c_width_ - c_width_ * ratio), p_trans.y + p_scale.y + above, 1.0f)
        );
    black_bar.initialize(
        vec3(c_width_ * (1.0f - ratio), c_height_, 1.0f),
        vec3(p_trans.x + (c_width_ - c_width_ * (1.0f - ratio)), p_trans.y + p_scale.y + above, 1.0f)
        );
}

inline void player_hp_bar_r::update(vec3 &p_scale, vec3 &p_trans, float ratio) {
    ratio = max(ratio, 0.0f);

    red_bar.set_scale_vec(
        vec3(c_width_ * ratio, c_height_, 1.0f));
    red_bar.set_trans_vec(
        vec3(p_trans.x - (c_width_ - c_width_ * ratio), p_trans.y + p_scale.y + above, 1.0f)
        );

    black_bar.set_scale_vec(
        vec3(c_width_ * (1.0f - ratio), c_height_, 1.0f));
    black_bar.set_trans_vec(
        vec3(p_trans.x + (c_width_ - c_width_ * (1.0f - ratio)), p_trans.y + p_scale.y + above, 1.0f)
        );
}

inline void player_hp_bar_r::draw() {
    var::program.set_uniform<decltype(glUniform1i), int>(glUniform1i, "color_type", 3);

    var::program.set_uniform<decltype(glUniform4fv), vec4>(glUniform4fv, "custom_color", 1, red);
    var::program.set_uniform<decltype(glUniformMatrix4fv), mat4>
        (glUniformMatrix4fv, "model_matrix", 1, GL_TRUE, red_bar.get_model_mx());
    glDrawElements(GL_TRIANGLES, var::rect_indices.size(), GL_UNSIGNED_INT, nullptr);

    var::program.set_uniform<decltype(glUniform4fv), vec4>(glUniform4fv, "custom_color", 1, black);
    var::program.set_uniform<decltype(glUniformMatrix4fv), mat4>
        (glUniformMatrix4fv, "model_matrix", 1, GL_TRUE, black_bar.get_model_mx());
    glDrawElements(GL_TRIANGLES, var::rect_indices.size(), GL_UNSIGNED_INT, nullptr);
}


class bad_guy_hp_bar_r {
    const float c_height_ = 0.02f;
    const float c_width_ = var::aspect;
    const float above = 1.0f - c_height_ / 2.0f;

    rect red_bar;
    rect black_bar;

public:
    void initialize();
    void update(float ratio);
    void draw();
};

inline void bad_guy_hp_bar_r::initialize() {
    const float ratio = 1.0f;
    red_bar.initialize(
        vec3(c_width_ * ratio, c_height_, 1.0f),
        vec3(- (c_width_ - c_width_ * ratio), + above, 1.0f)
        );
    black_bar.initialize(
        vec3(c_width_ * (1.0f - ratio), c_height_, 1.0f),
        vec3(+ (c_width_ - c_width_ * (1.0f - ratio)), + above, 1.0f)
        );
}

inline void bad_guy_hp_bar_r::update(float ratio) {
    ratio = max(ratio, 0.0f);

    red_bar.set_scale_vec(
        vec3(c_width_ * ratio, c_height_, 1.0f));
    red_bar.set_trans_vec(
        vec3(- (c_width_ - c_width_ * ratio), + above, 1.0f)
        );

    black_bar.set_scale_vec(
        vec3(c_width_ * (1.0f - ratio), c_height_, 1.0f));
    black_bar.set_trans_vec(
        vec3(+ (c_width_ - c_width_ * (1.0f - ratio)), + above, 1.0f)
        );
}

inline void bad_guy_hp_bar_r::draw() {
    var::program2.set_uniform<decltype(glUniform1i), int>(glUniform1i, "color_type", 3);

    var::program2.set_uniform<decltype(glUniform4fv), vec4>(glUniform4fv, "custom_color", 1, red);
    var::program2.set_uniform<decltype(glUniformMatrix4fv), mat4>
        (glUniformMatrix4fv, "model_matrix", 1, GL_TRUE, red_bar.get_model_mx());
    glDrawElements(GL_TRIANGLES, var::rect_indices.size(), GL_UNSIGNED_INT, nullptr);

    var::program2.set_uniform<decltype(glUniform4fv), vec4>(glUniform4fv, "custom_color", 1, black);
    var::program2.set_uniform<decltype(glUniformMatrix4fv), mat4>
        (glUniformMatrix4fv, "model_matrix", 1, GL_TRUE, black_bar.get_model_mx());
    glDrawElements(GL_TRIANGLES, var::rect_indices.size(), GL_UNSIGNED_INT, nullptr);
}


class title {
public:
    rect title_background;
    rect title_character;
    rect easy_key;
    rect hard_key;

    float past_time;

    texture_t background_texture;
    texture_t character_textures[3];
    texture_t *character_texture;
    texture_t button_textures[4];
    texture_t *easy_button_texture;
    texture_t *hard_button_texture;
    void initialize();
    void update(double delta_time);
    void draw();
    void click(int type);
    void release(int type);
};

inline void title::update(double delta_time) {
    past_time += (float)delta_time;

    if (past_time < 0.5f) {
        character_texture = &character_textures[1];
    }
    else if (past_time < 1.0f) {
        character_texture = &character_textures[2];
    }
    else if (past_time < 1.5f) {
        character_texture = &character_textures[1];
    }
    else if (past_time < 2.0f) {
        character_texture = &character_textures[0];
    }
    else {
        past_time = 0.0f;
    }
}

inline void title::initialize() {
    past_time = 0.0f;

    title_background.initialize(vec3(var::aspect, 1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f));
    title_character.initialize(vec3(var::aspect, 1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f));

    easy_key.initialize(vec3(0.15f, 0.08f, 1.0f), vec3(0.84f, -0.7f, 1.0f));
    hard_key.initialize(vec3(0.15f, 0.08f, 1.0f), vec3(1.35f, -0.7f, 1.0f));

    background_texture.init_texture("../bin/images/title_screen.png", true, true);
    character_textures[0].init_texture("../bin/images/background_1.png", true, true);
    character_textures[1].init_texture("../bin/images/background_2.png", true, true);
    character_textures[2].init_texture("../bin/images/background_3.png", true, true);
    character_texture = &character_textures[0];

    button_textures[0].init_texture("../bin/images/button_baby_free.png", true, true);
    button_textures[1].init_texture("../bin/images/button_baby_pressed.png", true, true);
    button_textures[2].init_texture("../bin/images/button_hard_free.png", true, true);
    button_textures[3].init_texture("../bin/images/button_hard_pressed.png", true, true);
    easy_button_texture = &button_textures[0];
    hard_button_texture = &button_textures[2];
}

inline void title::draw() {
    background_texture.bind(var::program2, 1);
    var::program2.set_uniform<decltype(glUniformMatrix4fv), mat4>
        (glUniformMatrix4fv, "model_matrix", 1, GL_TRUE, title_background.get_model_mx());
    glDrawElements(GL_TRIANGLES, var::rect_indices.size(), GL_UNSIGNED_INT, nullptr);

    character_texture->bind(var::program2, 1);
    var::program2.set_uniform<decltype(glUniformMatrix4fv), mat4>
        (glUniformMatrix4fv, "model_matrix", 1, GL_TRUE, title_character.get_model_mx());
    glDrawElements(GL_TRIANGLES, var::rect_indices.size(), GL_UNSIGNED_INT, nullptr);

    easy_button_texture->bind(var::program2, 1);
    var::program2.set_uniform<decltype(glUniformMatrix4fv), mat4>
        (glUniformMatrix4fv, "model_matrix", 1, GL_TRUE, easy_key.get_model_mx());
    glDrawElements(GL_TRIANGLES, var::rect_indices.size(), GL_UNSIGNED_INT, nullptr);

    hard_button_texture->bind(var::program2, 1);
    var::program2.set_uniform<decltype(glUniformMatrix4fv), mat4>
        (glUniformMatrix4fv, "model_matrix", 1, GL_TRUE, hard_key.get_model_mx());
    glDrawElements(GL_TRIANGLES, var::rect_indices.size(), GL_UNSIGNED_INT, nullptr);
}

inline void title::click(int type) {
    switch (type) {
    case 1:
    {
        easy_button_texture = &button_textures[1];
        break;
    }
    case 2:
    {
        hard_button_texture = &button_textures[3];
        break;
    }
    }
}

inline void title::release(int type) {
    switch (type) {
    case 1:
    {
        easy_button_texture = &button_textures[0];
        break;
    }
    case 2:
    {
        hard_button_texture = &button_textures[2];
        break;
    }
    }
}

class pause {
public:
    rect pause_background;
    rect restart_key;
    rect continue_key;
    rect to_title_key;

    texture_t background_texture;
    texture_t button_textures[6];
    texture_t *restart_button_texture;
    texture_t *continue_button_texture;
    texture_t *to_title_button_texture;

    void initialize();
    // void update(double delta_time);
    void draw();
    void click(int type);
    void release(int type);
};

inline void pause::initialize() {
    pause_background.initialize(vec3(var::aspect, 1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f));

    restart_key.initialize(vec3(0.15f, 0.08f, 1.0f), vec3(0.84f, -0.7f, 1.0f));
    continue_key.initialize(vec3(0.15f, 0.08f, 1.0f), vec3(1.35f, -0.7f, 1.0f));
    to_title_key.initialize(vec3(0.15f, 0.08f, 1.0f), vec3(0.33f, -0.7f, 1.0f));

    background_texture.init_texture("../bin/images/pause_screen.png", true, true);

    button_textures[0].init_texture("../bin/images/button_restart_free.png", true, true);
    button_textures[1].init_texture("../bin/images/button_restart_pressed.png", true, true);
    button_textures[2].init_texture("../bin/images/button_continue_free.png", true, true);
    button_textures[3].init_texture("../bin/images/button_continue_pressed.png", true, true);
    button_textures[4].init_texture("../bin/images/button_totitle_free.png", true, true);
    button_textures[5].init_texture("../bin/images/button_totitle_pressed.png", true, true);
    restart_button_texture = &button_textures[0];
    continue_button_texture = &button_textures[2];
    to_title_button_texture = &button_textures[4];
}

inline void pause::draw() {
    background_texture.bind(var::program2, 1);
    var::program2.set_uniform<decltype(glUniformMatrix4fv), mat4>
        (glUniformMatrix4fv, "model_matrix", 1, GL_TRUE, pause_background.get_model_mx());
    glDrawElements(GL_TRIANGLES, var::rect_indices.size(), GL_UNSIGNED_INT, nullptr);

    restart_button_texture->bind(var::program2, 1);
    var::program2.set_uniform<decltype(glUniformMatrix4fv), mat4>
        (glUniformMatrix4fv, "model_matrix", 1, GL_TRUE, restart_key.get_model_mx());
    glDrawElements(GL_TRIANGLES, var::rect_indices.size(), GL_UNSIGNED_INT, nullptr);

    continue_button_texture->bind(var::program2, 1);
    var::program2.set_uniform<decltype(glUniformMatrix4fv), mat4>
        (glUniformMatrix4fv, "model_matrix", 1, GL_TRUE, continue_key.get_model_mx());
    glDrawElements(GL_TRIANGLES, var::rect_indices.size(), GL_UNSIGNED_INT, nullptr);

    to_title_button_texture->bind(var::program2, 1);
    var::program2.set_uniform<decltype(glUniformMatrix4fv), mat4>
        (glUniformMatrix4fv, "model_matrix", 1, GL_TRUE, to_title_key.get_model_mx());
    glDrawElements(GL_TRIANGLES, var::rect_indices.size(), GL_UNSIGNED_INT, nullptr);
}

inline void pause::click(int type) {
    switch (type) {
    case 1:
    {
        restart_button_texture = &button_textures[1];
        break;
    }
    case 2:
    {
        continue_button_texture = &button_textures[3];
        break;
    }
    case 3:
    {
        to_title_button_texture = &button_textures[5];
        break;
    }
    }
}

inline void pause::release(int type) {
    switch (type) {
    case 1:
    {
        restart_button_texture = &button_textures[0];
        break;
    }
    case 2:
    {
        continue_button_texture = &button_textures[2];
        break;
    }
    case 3:
    {
        to_title_button_texture = &button_textures[4];
        break;
    }
    }
}

class lose {
public:
    rect lose_background;
    rect lose_character;
    rect to_title_key;

    float past_time;

    texture_t background_texture;
    texture_t character_textures[3];
    texture_t *character_texture;
    texture_t button_textures[2];
    texture_t *totitle_button_texture;

    void initialize();
    void update(double delta_time);
    void draw();
    void click(int type);
    void release(int type);
};

inline void lose::initialize() {
    past_time = 0.0f;

    lose_background.initialize(vec3(var::aspect, 1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f));
    lose_character.initialize(vec3(0.1f, 0.13f, 1.0f), vec3(0.0f, 0.0f, 1.0f));

    to_title_key.initialize(vec3(0.15f, 0.08f, 1.0f), vec3(0.0f, -0.7f, 1.0f));

    background_texture.init_texture("../bin/images/pause_screen.png", true, true);
    character_textures[0].init_texture("../bin/images/bad_go1_f.png", true, true);
    character_textures[1].init_texture("../bin/images/bad_stop_f.png", true, true);
    character_textures[2].init_texture("../bin/images/bad_go2_f.png", true, true);
    character_texture = &character_textures[0];

    button_textures[0].init_texture("../bin/images/button_totitle_free.png", true, true);
    button_textures[1].init_texture("../bin/images/button_totitle_pressed.png", true, true);
    totitle_button_texture = &button_textures[0];
}

inline void lose::update(double delta_time) {
    past_time += (float)delta_time;

    if (past_time < 0.2f) {
        character_texture = &character_textures[1];
    }
    else if (past_time < 0.4f) {
        character_texture = &character_textures[2];
    }
    else if (past_time < 0.6f) {
        character_texture = &character_textures[1];
    }
    else if (past_time < 0.8f) {
        character_texture = &character_textures[0];
    }
    else {
        past_time = 0.0f;
    }
}

inline void lose::draw() {
    background_texture.bind(var::program2, 1);
    var::program2.set_uniform<decltype(glUniformMatrix4fv), mat4>
        (glUniformMatrix4fv, "model_matrix", 1, GL_TRUE, lose_background.get_model_mx());
    glDrawElements(GL_TRIANGLES, var::rect_indices.size(), GL_UNSIGNED_INT, nullptr);

    character_texture->bind(var::program2, 1);
    var::program2.set_uniform<decltype(glUniformMatrix4fv), mat4>
        (glUniformMatrix4fv, "model_matrix", 1, GL_TRUE, lose_character.get_model_mx());
    glDrawElements(GL_TRIANGLES, var::rect_indices.size(), GL_UNSIGNED_INT, nullptr);

    totitle_button_texture->bind(var::program2, 1);
    var::program2.set_uniform<decltype(glUniformMatrix4fv), mat4>
        (glUniformMatrix4fv, "model_matrix", 1, GL_TRUE, to_title_key.get_model_mx());
    glDrawElements(GL_TRIANGLES, var::rect_indices.size(), GL_UNSIGNED_INT, nullptr);

}

inline void lose::click(int type) {
    switch (type) {
    case 1:
    {
        totitle_button_texture = &button_textures[1];
        break;
    }
    }
}

inline void lose::release(int type) {
    switch (type) {
    case 1:
    {
        totitle_button_texture = &button_textures[0];
        break;
    }
    }
}


class win {
public:
    rect win_background;
    rect win_character;
    rect to_title_key;

    float past_time;

    texture_t background_texture;
    texture_t character_textures[3];
    texture_t *character_texture;
    texture_t button_textures[2];
    texture_t *totitle_button_texture;

    void initialize();
    void update(double delta_time);
    void draw();
    void click(int type);
    void release(int type);
};

inline void win::initialize() {
    past_time = 0.0f;

    win_background.initialize(vec3(var::aspect, 1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f));
    win_character.initialize(vec3(0.1f, 0.13f, 1.0f), vec3(0.0f, 0.0f, 1.0f));

    to_title_key.initialize(vec3(0.15f, 0.08f, 1.0f), vec3(0.0f, -0.7f, 1.0f));

    background_texture.init_texture("../bin/images/pause_screen.png", true, true);
    character_textures[0].init_texture("../bin/images/player_walking1_f.png", true, true);
    character_textures[1].init_texture("../bin/images/player_standing_f.png", true, true);
    character_textures[2].init_texture("../bin/images/player_walking2_f.png", true, true);
    character_texture = &character_textures[0];

    button_textures[0].init_texture("../bin/images/button_totitle_free.png", true, true);
    button_textures[1].init_texture("../bin/images/button_totitle_pressed.png", true, true);
    totitle_button_texture = &button_textures[0];
}

inline void win::update(double delta_time) {
    past_time += (float)delta_time;

    if (past_time < 0.2f) {
        character_texture = &character_textures[1];
    }
    else if (past_time < 0.4f) {
        character_texture = &character_textures[2];
    }
    else if (past_time < 0.6f) {
        character_texture = &character_textures[1];
    }
    else if (past_time < 0.8f) {
        character_texture = &character_textures[0];
    }
    else {
        past_time = 0.0f;
    }
}

inline void win::draw() {
    background_texture.bind(var::program2, 1);
    var::program2.set_uniform<decltype(glUniformMatrix4fv), mat4>(glUniformMatrix4fv, "model_matrix", 1, GL_TRUE,
                                                                  win_background.get_model_mx());
    glDrawElements(GL_TRIANGLES, var::rect_indices.size(), GL_UNSIGNED_INT, nullptr);

    character_texture->bind(var::program2, 1);
    var::program2.set_uniform<decltype(glUniformMatrix4fv), mat4>(glUniformMatrix4fv, "model_matrix", 1, GL_TRUE,
                                                                  win_character.get_model_mx());
    glDrawElements(GL_TRIANGLES, var::rect_indices.size(), GL_UNSIGNED_INT, nullptr);

    totitle_button_texture->bind(var::program2, 1);
    var::program2.set_uniform<decltype(glUniformMatrix4fv), mat4>(glUniformMatrix4fv, "model_matrix", 1, GL_TRUE,
                                                                  to_title_key.get_model_mx());
    glDrawElements(GL_TRIANGLES, var::rect_indices.size(), GL_UNSIGNED_INT, nullptr);
}

inline void win::click(int type) {
    switch (type) {
    case 1:
    {
        totitle_button_texture = &button_textures[1];
        break;
    }
    }
}

inline void win::release(int type) {
    switch (type) {
    case 1:
    {
        totitle_button_texture = &button_textures[0];
        break;
    }
    }
}

class help {
public:
    rect help_background;
    rect press_again;

    float past_time;

    texture_t background_texture;
    texture_t press_again_textures[4];
    texture_t *press_again_texture;

    void initialize();
    void update(double delta_time);
    void draw();
};

inline void help::initialize() {
    past_time = 0.0f;

    help_background.initialize(vec3(1.281f, 0.833f, 1.0f), vec3(0.0f, 0.0f, 1.0f));
    press_again.initialize(vec3(1.281f, 0.833f, 1.0f), vec3(0.0f, 0.0f, 1.0f));

    background_texture.init_texture("../bin/images/help.png", true, true);
    press_again_textures[0].init_texture("../bin/images/help1.png", true, true);
    press_again_textures[1].init_texture("../bin/images/help2.png", true, true);
    press_again_textures[2].init_texture("../bin/images/help3.png", true, true);
    press_again_textures[3].init_texture("../bin/images/help4.png", true, true);
    press_again_texture = &press_again_textures[0];
}

inline void help::update(double delta_time) {
    past_time += (float)delta_time;

    if (past_time < 0.5f) {
        press_again_texture = &press_again_textures[0];
    }
    else if (past_time < 1.0f) {
        press_again_texture = &press_again_textures[1];
    }
    else if (past_time < 1.5f) {
        press_again_texture = &press_again_textures[2];
    }
    else if (past_time < 2.0f) {
        press_again_texture = &press_again_textures[3];
    }
    else {
        past_time = 0.0f;
    }
}

inline void help::draw() {
    background_texture.bind(var::program2, 1);
    var::program2.set_uniform<decltype(glUniformMatrix4fv), mat4>
        (glUniformMatrix4fv, "model_matrix", 1, GL_TRUE, help_background.get_model_mx());
    glDrawElements(GL_TRIANGLES, var::rect_indices.size(), GL_UNSIGNED_INT, nullptr);

    press_again_texture->bind(var::program2, 1);
    var::program2.set_uniform<decltype(glUniformMatrix4fv), mat4>
        (glUniformMatrix4fv, "model_matrix", 1, GL_TRUE, press_again.get_model_mx());
    glDrawElements(GL_TRIANGLES, var::rect_indices.size(), GL_UNSIGNED_INT, nullptr);
}


class skill_descriptions {
public:
    rect descriptor;
    texture_t textures[5];
    texture_t *cur_descriptor;

    void draw(int type);
    void initialize();
};

inline void skill_descriptions::draw(int type) {
    textures[type].bind(var::program, 1);
    var::program.set_uniform<decltype(glUniformMatrix4fv), mat4>(glUniformMatrix4fv, "model_matrix", 1, GL_TRUE,
                                                                 descriptor.get_model_mx());
    glDrawElements(GL_TRIANGLES, var::rect_indices.size(), GL_UNSIGNED_INT, nullptr);
    textures[type].unbind();
}

inline void skill_descriptions::initialize() {
    descriptor.initialize(vec3(0.899f, 0.6f, 1.0f), vec3(0.0f, 0.0f, 1.0f));

    textures[0].init_texture("../bin/images/askill_discription.png", true, true);
    textures[1].init_texture("../bin/images/dskill_discription.png", true, true);
    textures[2].init_texture("../bin/images/fskill_discription.png", true, true);
    textures[3].init_texture("../bin/images/endskill_discription.png", true, true);
    textures[4].init_texture("../bin/images/rskill_discription.png", true, true);
    cur_descriptor = &textures[0];
}

class sound_button {
public:
    rect button;
    bool is_muted;
    texture_t textures[4];
    texture_t *main_texture;

    void draw();
    void initialize();
    void click();
    void release(bool is_happened);
};

inline void sound_button::draw() {
    var::program.bind();
    main_texture->bind(var::program, 1);
    var::program.set_uniform<decltype(glUniformMatrix4fv), mat4>(glUniformMatrix4fv, "model_matrix", 1, GL_TRUE,
                                                                 button.get_model_mx());
    glDrawElements(GL_TRIANGLES, var::rect_indices.size(), GL_UNSIGNED_INT, nullptr);
    main_texture->unbind();
}

inline void sound_button::initialize() {
    button.initialize(vec3(0.05f, 0.05f, 1.0f), vec3(-var::aspect + 0.05f, -1.0f + 0.05f, 1.0f));
    is_muted = false;

    textures[0].init_texture("../bin/images/unmute_icon.png", true, true);
    textures[1].init_texture("../bin/images/unmute_icon_clicked.png", true, true);
    textures[2].init_texture("../bin/images/mute_icon.png", true, true);
    textures[3].init_texture("../bin/images/mute_icon_clicked.png", true, true);
    main_texture = &textures[0];
}

inline void sound_button::click() {
    if (is_muted) {
        main_texture = &textures[3];
    }
    else {
        main_texture = &textures[1];
    }
}

inline void sound_button::release(bool is_happened) {
    if (is_happened) {
        if (is_muted) {
            main_texture = &textures[0];
        }
        else {
            main_texture = &textures[2];
        }
    }
    else {
        if (is_muted) {
            main_texture = &textures[2];
        }
        else {
            main_texture = &textures[0];
        }
    }

}


#endif
