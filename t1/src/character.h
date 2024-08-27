#pragma once
#ifndef __CHARACTER_H__
#define __CHARACTER_H__

#include "cgmath.h"
#include "namespace.h"
#include "rect.h"
#include "damage.h"

class character : public rect_interface {
public:
    ~character() = default;
    mat4 get_trans_mx() override;
    mat4 get_scale_mx() override;
    vec3 get_trans_vec() override;
    vec3 get_scale_vec() override;
    int get_looking_way() const;
    void initialize(const vec3 &scale, const vec3 &trans, rect *floor);
    void initialize_texture();
    void move_character(key &move_key, double delta_time);
    void avoid_floor_collision(rect &floor, double delta_time);
    void teleport(int glfw_key);
    void jump();
    void hit_poop(poops &poop);
    void set_trans(vec3 &trans);
    bool is_on_ground() const;
    void set_jump_count(int count);
    void got_hit(int damage);

    void update_color(double delta_time);

    bool is_ice_mode;
    bool is_invincible;

    int stars;

    const int max_health = 100;
    int health;

    texture_t textures[20];
    texture_t *main_texture;

    void draw();

    vec3 velocity_;

    rect ice;
    texture_t ice_tex;

private:
    struct hit_control {
        float hit_duration;
        bool is_hit;
    };

    hit_control hit;

    float walking_duration;
    int looking_at_;
    vec3 scale_vec_;
    vec3 trans_vec_;

    vec3 gravity_;
    bool is_on_ground_;
    int jump_count_;
    rect *floor_rect_;
    float last_move_time;
    float last_jump_time;
};

inline void character::teleport(int glfw_key) {
    if (is_ice_mode)
        return;

    if (!is_on_ground_) {
        if (glfw_key == GLFW_KEY_DOWN) {
            trans_vec_ =
                vec3(trans_vec_[0], -1.0f + (scale_vec_.y + floor_rect_->get_scale_vec()[1] * 2), trans_vec_[2]);
            is_on_ground_ = true;
            velocity_ = vec3(0.0f, 0.0f, 0.0f);
            var::engine->play2D(var::player_teleport_music);
        }
        return;
    }

    const float adder = 0.5f;
    switch (glfw_key) {
    case GLFW_KEY_UP:
    {
        if ((trans_vec_ + adder * vec3(0.0f, 1.0f, 0.0f)).y > 1.0f)
            break;
        trans_vec_ += adder * vec3(0.0f, 1.0f, 0.0f);

        var::engine->play2D(var::player_teleport_music);

        break;
    }
    case GLFW_KEY_DOWN:
        break;
    case GLFW_KEY_RIGHT:
    {
        if (abs((trans_vec_ + adder * vec3(1.0f, 0.0f, 0.0f))).x > var::aspect)
            break;

        trans_vec_ += adder * vec3(1.0f, 0.0f, 0.0f);
        looking_at_ = 1;

        var::engine->play2D(var::player_teleport_music);

        if (trans_vec_.x > var::cam.const_eye.x + var::changeable_aspect ||
            trans_vec_.x < var::cam.const_eye.x - var::changeable_aspect) {
            if (trans_vec_.x > 0.0f) {
                var::cam.eye.x = var::cam.const_eye.x + (trans_vec_.x - var::changeable_aspect);
                var::cam.at.x = var::cam.const_at.x + (trans_vec_.x - var::changeable_aspect);
            }
            else {
                var::cam.eye.x = var::cam.const_eye.x + (trans_vec_.x + var::changeable_aspect);
                var::cam.at.x = var::cam.const_at.x + (trans_vec_.x + var::changeable_aspect);
            }
        }
        else {
            var::cam.eye.x = var::cam.const_eye.x;
            var::cam.at.x = var::cam.const_at.x;
        }
        break;
    }
    case GLFW_KEY_LEFT:
    {
        if (abs((trans_vec_ + adder * vec3(-1.0f, 0.0f, 0.0f))).x > var::aspect)
            break;

        trans_vec_ += adder * vec3(-1.0f, 0.0f, 0.0f);
        looking_at_ = -1;

        var::engine->play2D(var::player_teleport_music);

        if (trans_vec_.x > var::cam.const_eye.x + var::changeable_aspect ||
            trans_vec_.x < var::cam.const_eye.x - var::changeable_aspect) {
            if (trans_vec_.x > 0.0f) {
                var::cam.eye.x = var::cam.const_eye.x + (trans_vec_.x - var::changeable_aspect);
                var::cam.at.x = var::cam.const_at.x + (trans_vec_.x - var::changeable_aspect);
            }
            else {
                var::cam.eye.x = var::cam.const_eye.x + (trans_vec_.x + var::changeable_aspect);
                var::cam.at.x = var::cam.const_at.x + (trans_vec_.x + var::changeable_aspect);
            }
        }
        else {
            var::cam.eye.x = var::cam.const_eye.x;
            var::cam.at.x = var::cam.const_at.x;
        }

        break;
    }
    }
}

inline void character::move_character(key &move_key, double delta_time) {
    if (is_ice_mode)
        return;

    float dir;
    if (move_key.is_pressed) {
        if (!(move_key.key_type == GLFW_KEY_RIGHT || move_key.key_type == GLFW_KEY_LEFT))
            return;

        if (move_key.key_type == GLFW_KEY_RIGHT) {
            dir = 1.0;
            looking_at_ = 1;
        }
        else {
            dir = -1.0;
            looking_at_ = -1;
        }
        if (var::attack_key.is_pressed) {

        }
        else if (is_on_ground_) {
            walking_duration += (float)delta_time;
            if (walking_duration < 0.11f) {
                main_texture = (looking_at_ == 1) ? &textures[3] : &textures[2];
            }
            else if (walking_duration < 0.22f) {
                main_texture = (looking_at_ == 1) ? &textures[1] : &textures[0];
            }
            else if (walking_duration < 0.33f) {
                main_texture = (looking_at_ == 1) ? &textures[5] : &textures[4];
            }
            else if (walking_duration < 0.44f) {
                main_texture = (looking_at_ == 1) ? &textures[1] : &textures[0];
            }
            else {
                walking_duration = 0.0f;
            }
        }


        vec3 adder = vec3(1.0f, 0.0f, 0.0f) * (float)delta_time;
        adder *= dir;

        if (abs((trans_vec_ + adder).x) > var::aspect)
            return;
        trans_vec_ += adder;

        if (trans_vec_.x > var::cam.const_eye.x + var::changeable_aspect ||
            trans_vec_.x < var::cam.const_eye.x - var::changeable_aspect) {
            if (trans_vec_.x > 0.0f) {
                var::cam.eye.x = var::cam.const_eye.x + (trans_vec_.x - var::changeable_aspect);
                var::cam.at.x = var::cam.const_at.x + (trans_vec_.x - var::changeable_aspect);
            }
            else {
                var::cam.eye.x = var::cam.const_eye.x + (trans_vec_.x + var::changeable_aspect);
                var::cam.at.x = var::cam.const_at.x + (trans_vec_.x + var::changeable_aspect);
            }
        }
        else {
            var::cam.eye.x = var::cam.const_eye.x;
            var::cam.at.x = var::cam.const_at.x;
        }
    }
    else {
        if (!var::attack_key.is_pressed) {
            main_texture = (looking_at_ == 1) ? &textures[1] : &textures[0];
        }
    }
}

inline void character::avoid_floor_collision(rect &floor, double delta_time) {
    auto floor_coor = std::move(get_actual_coord(floor.get_scale_vec(), floor.get_trans_vec()));
    auto char_coor = std::move(get_actual_coord(get_scale_vec(), get_trans_vec()));

    auto next_velocity = velocity_ + gravity_ * (float)delta_time;

    auto next_vec = trans_vec_ + next_velocity * (float)delta_time;

    auto next_mx =
        vec3(get_scale_vec().x + next_vec.x, -get_scale_vec().y + next_vec.y, get_scale_vec().z + next_vec.z);
    if (floor_coor[0].y > char_coor[3].y) {
        trans_vec_ = vec3(trans_vec_[0], -1.0f + (scale_vec_.y + floor.get_scale_vec()[1] * 2), trans_vec_[2]);
    }

    if (abs(floor_coor[0].y - next_mx.y) >= 0.001f) {
        is_on_ground_ = false;
        if (!var::attack_key.is_pressed) {
            main_texture = (looking_at_ == 1) ? &textures[7] : &textures[6];
        }
    }
    else {
        jump_count_ = 2;
        velocity_ = vec3(0.0f, 0.0f, 0.0f);
        is_on_ground_ = true;
    }


    if (floor_coor[0].y >= next_mx.y) {
        if (is_on_ground_)
            return;
        jump_count_ = 2;
        velocity_ = vec3(0.0f, 0.0f, 0.0f);
    }
    else {
        velocity_ = next_velocity;
        trans_vec_ = next_vec;
    }
}

inline void character::jump() {
    if (jump_count_ <= 0)
        return;

    velocity_ += vec3(0.0f, 1.7f, 0.0f);
    is_on_ground_ = false;
    jump_count_ -= 1;

    var::engine->play2D(var::player_jump_music);

    main_texture = (looking_at_ == 1) ? &textures[7] : &textures[6];
}

inline void character::hit_poop(poops &poop) {
    auto player_coor = std::move(get_actual_coord(scale_vec_, trans_vec_));

    for (auto it = poop.poop_list.begin(); it != poop.poop_list.end();) {
        auto square_coor = std::move(get_actual_coord((*it).get_scale_vec(), (*it).get_trans_vec()));

        if (aabb(player_coor, square_coor)) {
            var::engine->play2D(var::poop_gone_music);
            float poop_size = (*it).get_scale_vec().x;
            int damage = 10 + (int)((poop_size - 0.15f) * 50.0f);
            got_hit(damage);
            poop.poop_list.erase(it++);
        }
        else
            ++it;
    }
}

inline void character::set_trans(vec3 &trans) { trans_vec_ = trans; }

inline bool character::is_on_ground() const { return is_on_ground_; }

inline void character::set_jump_count(int count) { jump_count_ = count; }

inline void character::got_hit(int damage = 0) {
    if (is_ice_mode || is_invincible)
        return;

    health -= damage;
    var::engine->play2D(var::player_hit_music);
    hit.is_hit = true;
    hit.hit_duration = 0.0f;

    auto pos = std::move(
        get_window_coord(get_scale_vec(), get_trans_vec(), var::window_size, var::changeable_aspect, var::aspect));

    std::string tmp = "-";
    tmp += std::to_string(damage);

    int tmp1 = rand() % 10;
    tmp += std::to_string(tmp1);

    // tmp += ",";

    tmp1 = rand() % 10;
    tmp += std::to_string(tmp1);
    tmp1 = rand() % 10;
    tmp += std::to_string(tmp1);
    tmp1 = rand() % 10;
    tmp += std::to_string(tmp1);

    // tmp += ",";

    tmp1 = rand() % 10;
    tmp += std::to_string(tmp1);
    tmp1 = rand() % 10;
    tmp += std::to_string(tmp1);
    tmp1 = rand() % 10;
    tmp += std::to_string(tmp1);

    auto new_pos = pos.at(1) + pos.at(0);
    new_pos.x /= 2, new_pos.y /= 2;
    my_damage = damage_info({tmp, 0.0f, new_pos});
}

inline void character::update_color(double delta_time) {
    if (hit.is_hit) {
        hit.hit_duration += (float)delta_time;

        if (hit.hit_duration > 0.5f)
            hit.is_hit = false;
    }
}

inline void character::draw() {
    main_texture->bind(var::program, 2);
    var::program.set_uniform<decltype(glUniform4fv), vec4>(glUniform4fv, "custom_color", 1, hit.is_hit ? red : white);

    var::program.set_uniform<decltype(glUniformMatrix4fv), mat4>(glUniformMatrix4fv, "model_matrix", 1, GL_TRUE,
                                                                 get_model_mx());
    glDrawElements(GL_TRIANGLES, var::rect_indices.size(), GL_UNSIGNED_INT, nullptr);
    main_texture->unbind();

    if (is_ice_mode) {
        ice_tex.bind(var::program, 1);

        var::program.set_uniform<decltype(glUniformMatrix4fv), mat4>(glUniformMatrix4fv, "model_matrix", 1, GL_TRUE,
                                                                     get_model_mx());
        glDrawElements(GL_TRIANGLES, var::rect_indices.size(), GL_UNSIGNED_INT, nullptr);
        ice_tex.unbind();
    }
}

inline mat4 character::get_trans_mx() { return get_2D_trans_mx(get_trans_vec()); }

inline mat4 character::get_scale_mx() { return get_2D_scale_mx(get_scale_vec()); }

inline vec3 character::get_trans_vec() { return trans_vec_; }

inline vec3 character::get_scale_vec() { return scale_vec_; }

inline int character::get_looking_way() const { return looking_at_; }

inline void character::initialize(const vec3 &scale, const vec3 &trans, rect *floor) {
    is_ice_mode = false;
    health = max_health;
    velocity_ = vec3(0.0f, 0.0f, 0.0f);
    looking_at_ = 1;
    gravity_ = vec3(0.0f, -5.0f, 0.0f);
    scale_vec_ = std::move(scale);
    trans_vec_ = std::move(trans);
    is_on_ground_ = false;
    jump_count_ = 2;
    floor_rect_ = floor;
    last_move_time = 0.0f;
    last_jump_time = 0.0f;
    walking_duration = 0.0f;
    is_invincible = false;

    stars = 0;
    ice.initialize(scale, trans);

    hit.hit_duration = 0.0f;
    hit.is_hit = false;
}

inline void character::initialize_texture() {
    textures[0].init_texture("../bin/images/player_standing.png", true, true);
    textures[1].init_texture("../bin/images/player_standing_f.png", true, true);
    textures[2].init_texture("../bin/images/player_walking1.png", true, true);
    textures[3].init_texture("../bin/images/player_walking1_f.png", true, true);
    textures[4].init_texture("../bin/images/player_walking2.png", true, true);
    textures[5].init_texture("../bin/images/player_walking2_f.png", true, true);
    textures[6].init_texture("../bin/images/player_jump.png", true, true);
    textures[7].init_texture("../bin/images/player_jump_f.png", true, true);
    textures[8].init_texture("../bin/images/player_attack1.png", true, true);
    textures[9].init_texture("../bin/images/player_attack1_f.png", true, true);
    textures[10].init_texture("../bin/images/player_attack2.png", true, true);
    textures[11].init_texture("../bin/images/player_attack2_f.png", true, true);

    ice_tex.init_texture("../bin/images/player_ice.png", true, true);

    main_texture = &textures[0];
}

/*===========================================================*/
/*===========================================================*/
/*===========================================================*/
/*===========================================================*/

class bad_guy : public rect_interface {
public:
    ~bad_guy() = default;

    mat4 get_trans_mx() override;
    mat4 get_scale_mx() override;
    vec3 get_trans_vec() override;
    vec3 get_scale_vec() override;

    void initialize(const vec3 &scale, const vec3 &trans, rect *floor);
    void initialize_texture();

    void draw();
    void update(character &player, double delta_time);

    const int max_health = 3000000;
    int health;

    struct attack {
    public:
        void initialize(vec3 &scale, vec3 &trans);
        texture_t textures[20];
        texture_t *main_texture;
        rect attack_rect;
        void draw();
    };

    texture_t textures[20];
    texture_t *main_texture;

    attack weak;

    void give_damage(int damage);

    float wandering_duration;
    float wandering_duration_animation;
    float weak_attack_duration;
    float push_attack_duration;
    float super_jump_duration;
    float difficulty;
    void set_difficulty(float diff);

private:
    enum phases { wandering, weak_attack, push_attack, super_jump };

    int weak_attack_wait;
    int hit_by_weak_attack;
    int hit_by_push_attack;
    int push_attack_go;
    int push_attack_wait;
    int jump_attack_go;
    int jump_attack_wait;

    vec3 scale_vec_;
    vec3 trans_vec_;
    rect *floor_rect_;
    int looking_at_;
    phases cur_phase;
};

inline vec3 bad_guy::get_trans_vec() { return trans_vec_; }

inline vec3 bad_guy::get_scale_vec() { return scale_vec_; }

inline void bad_guy::initialize(const vec3 &scale, const vec3 &trans, rect *floor) {
    weak.initialize(vec3(0.13f, 0.13f, 1.0f), vec3(0.0f, -0.85f, 0.0f));

    weak.attack_rect.set_trans_vec(vec3(trans.x + scale.x + weak.attack_rect.get_scale_vec().x, trans.y, trans.z));

    wandering_duration = 0.0f;
    wandering_duration_animation = 0.0f;
    weak_attack_duration = 0.0f;
    push_attack_duration = 0.0f;
    super_jump_duration = 0.0f;

    difficulty = 1.0f;

    weak_attack_wait = 0;
    hit_by_weak_attack = 0;
    hit_by_push_attack = 0;
    push_attack_go = 0;
    push_attack_wait = 0;
    jump_attack_go = 0;
    jump_attack_wait = 0;

    health = max_health;

    scale_vec_ = scale;
    trans_vec_ = vec3(trans[0], -1.0f + (scale.y + floor->get_scale_vec()[1] * 2), trans[2]);
    floor_rect_ = floor;
    looking_at_ = 1;
    cur_phase = wandering;
}

inline void bad_guy::initialize_texture() {
    weak.textures[0].init_texture("../bin/images/knife0.png", true, true);
    weak.textures[1].init_texture("../bin/images/knife1.png", true, true);
    weak.textures[2].init_texture("../bin/images/knife2.png", true, true);
    weak.textures[3].init_texture("../bin/images/knife3.png", true, true);
    weak.textures[4].init_texture("../bin/images/knife0_f.png", true, true);
    weak.textures[5].init_texture("../bin/images/knife1_f.png", true, true);
    weak.textures[6].init_texture("../bin/images/knife2_f.png", true, true);
    weak.textures[7].init_texture("../bin/images/knife3_f.png", true, true);

    weak.main_texture = (looking_at_ == 1) ? &weak.textures[0] : &weak.textures[1];

    textures[0].init_texture("../bin/images/bad_stop.png", true, true);
    textures[1].init_texture("../bin/images/bad_stop_f.png", true, true);

    textures[2].init_texture("../bin/images/bad_dash.png", true, true);
    textures[3].init_texture("../bin/images/bad_dash_f.png", true, true);

    textures[4].init_texture("../bin/images/bad_jump.png", true, true);
    textures[13].init_texture("../bin/images/bad_jump_f.png", true, true);
    textures[5].init_texture("../bin/images/bad_landing.png", true, true);
    textures[12].init_texture("../bin/images/bad_landing_f.png", true, true);

    textures[6].init_texture("../bin/images/bad_stop.png", true, true);
    textures[7].init_texture("../bin/images/bad_stop_f.png", true, true); // ???
    textures[8].init_texture("../bin/images/bad_go1.png", true, true);
    textures[9].init_texture("../bin/images/bad_go1_f.png", true, true);
    textures[10].init_texture("../bin/images/bad_go2.png", true, true);
    textures[11].init_texture("../bin/images/bad_go2_f.png", true, true);

    main_texture = (looking_at_ == 1) ? &textures[7] : &textures[6];
}

inline void bad_guy::draw() {
    if (cur_phase == weak_attack)
        weak.draw();
    main_texture->bind(var::program, 1);
    var::program.set_uniform<decltype(glUniformMatrix4fv), mat4>
        (glUniformMatrix4fv, "model_matrix", 1, GL_TRUE, get_model_mx());
    glDrawElements(GL_TRIANGLES, var::rect_indices.size(), GL_UNSIGNED_INT, nullptr);
    main_texture->unbind();
}

inline mat4 bad_guy::get_trans_mx() { return get_2D_trans_mx(get_trans_vec()); }

inline mat4 bad_guy::get_scale_mx() { return get_2D_scale_mx(get_scale_vec()); }

inline void bad_guy::update(character &player, double delta_time) {
    switch (cur_phase) {
    case wandering:
    {
        float wandering_limit = 0.05f + var::op_ratio + difficulty;

        wandering_duration += (float)delta_time;
        if (wandering_duration > wandering_limit) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> rand_num(0, 2);
            int next_one = rand_num(gen);

            if (next_one == 0) {
                cur_phase = super_jump;
                super_jump_duration = 0.0f;
            }
            else if (next_one == 1) {
                cur_phase = weak_attack;
                weak_attack_duration = 0.0f;
            }
            else {
                cur_phase = push_attack;
                push_attack_duration = 0.0f;
            }
            break;
        }

        auto player_pos = player.get_trans_vec().x;
        auto bad_pos = trans_vec_.x;

        looking_at_ = (player_pos > bad_pos) ? 1 : -1;

        float length_btw = abs(player_pos - bad_pos);
        if (length_btw > 0.5f) {

            vec3 adder = vec3(0.3f, 0.0f, 0.0f) * (float)delta_time;
            adder *= (float)looking_at_;

            wandering_duration_animation += (float)delta_time;
            if (wandering_duration_animation < 0.11f) {
                main_texture = (looking_at_ == 1) ? &textures[9] : &textures[8];
            }
            else if (wandering_duration_animation < 0.22f) {
                main_texture = (looking_at_ == 1) ? &textures[7] : &textures[6];
            }
            else if (wandering_duration_animation < 0.33f) {
                main_texture = (looking_at_ == 1) ? &textures[11] : &textures[10];
            }
            else if (wandering_duration_animation < 0.44f) {
                main_texture = (looking_at_ == 1) ? &textures[7] : &textures[6];
            }
            else {
                wandering_duration_animation = 0.0f;
            }

            if (abs((trans_vec_ + adder).x) > var::aspect) {
                main_texture = (looking_at_ == 1) ? &textures[7] : &textures[6];
                break;
            }
            trans_vec_ += adder;
        }
        else {
            main_texture = (looking_at_ == 1) ? &textures[7] : &textures[6];
        }
        break;
    }

    case weak_attack:
    {
        main_texture = (looking_at_ == 1) ? &textures[7] : &textures[6];

        weak_attack_duration += (float)delta_time;
        weak.attack_rect.set_trans_vec(vec3(
            trans_vec_.x +
            looking_at_ * (get_scale_vec().x + weak.attack_rect.get_scale_vec().x),
            trans_vec_.y, trans_vec_.z
            ));
        if (weak_attack_duration < 0.7f) {
            weak.main_texture = std::move((looking_at_ == 1) ? &weak.textures[4] : &weak.textures[0]);

            if (weak_attack_wait == 0) {
                var::engine->play2D(var::enemy_attack1_music);
                weak_attack_wait++;
            }
        }
        else if (weak_attack_duration < 0.8f) {
            if (hit_by_weak_attack == 0) {
                auto good_coord = std::move(get_actual_coord(player.get_scale_vec(), player.get_trans_vec()));
                auto attack_coord =
                    std::move(get_actual_coord(weak.attack_rect.get_scale_vec(), weak.attack_rect.get_trans_vec()));

                var::engine->play2D(var::enemy_attack2_music);

                if (aabb(good_coord, attack_coord)) {
                    player.got_hit(10);
                }

                hit_by_weak_attack = 1;
            }
            weak.main_texture = std::move((looking_at_ == 1) ? &weak.textures[5] : &weak.textures[1]);
        }
        else if (weak_attack_duration < 0.9f) {
            if (hit_by_weak_attack == 1 || hit_by_weak_attack == 0) {
                auto good_coord = std::move(get_actual_coord(player.get_scale_vec(), player.get_trans_vec()));
                auto attack_coord =
                    std::move(get_actual_coord(weak.attack_rect.get_scale_vec(), weak.attack_rect.get_trans_vec()));

                var::engine->play2D(var::enemy_attack2_music);

                if (aabb(good_coord, attack_coord)) {
                    player.got_hit(10);
                }

                hit_by_weak_attack = 2;
            }
            weak.main_texture = std::move((looking_at_ == 1) ? &weak.textures[6] : &weak.textures[2]);
        }
        else if (weak_attack_duration < 1.0f) {
            if (hit_by_weak_attack == 2 || hit_by_weak_attack == 1 || hit_by_weak_attack == 0) {
                auto good_coord = std::move(get_actual_coord(player.get_scale_vec(), player.get_trans_vec()));
                auto attack_coord =
                    std::move(get_actual_coord(weak.attack_rect.get_scale_vec(), weak.attack_rect.get_trans_vec()));

                var::engine->play2D(var::enemy_attack2_music);

                if (aabb(good_coord, attack_coord)) {
                    player.got_hit(10);
                }

                hit_by_weak_attack = 3;
            }
            weak.main_texture = std::move((looking_at_ == 1) ? &weak.textures[7] : &weak.textures[3]);
        }
        else {
            weak_attack_wait = 0;
            hit_by_weak_attack = 0;
            cur_phase = wandering;
            wandering_duration = 0.0f;
        }
        break;
    }

    case push_attack:
    {
        push_attack_duration += (float)delta_time;

        if (push_attack_duration < 1.0f) {
            main_texture = (looking_at_ == 1) ? &textures[3] : &textures[2];

            if (push_attack_wait == 0) {
                push_attack_wait = 1;
                var::engine->play2D(var::enemy_dash2_music);
            }
            // printf("is going to push attack!\n");
        }
        else if (push_attack_duration < 1.6f) {
            vec3 adder = vec3(3.0f, 0.0f, 0.0f) * (float)delta_time;
            adder *= (float)looking_at_;

            if (abs((trans_vec_ + adder).x) > var::aspect)
                return;

            trans_vec_ += adder;

            if (push_attack_go == 0) {
                var::engine->play2D(var::enemy_dash1_music);
                push_attack_go = 1;
            }

            auto good_coord = std::move(get_actual_coord(player.get_scale_vec(), player.get_trans_vec()));
            auto bad_coord = std::move(get_actual_coord(get_scale_vec(), get_trans_vec()));

            if (!player.is_ice_mode && !player.is_invincible && aabb(good_coord, bad_coord)) {
                auto new_x = trans_vec_.x + (float)looking_at_ * (scale_vec_.x + player.get_scale_vec().x);
                new_x = min(var::aspect, new_x);
                new_x = max(-var::aspect, new_x);

                if (push_attack_duration < 1.2f) {
                    if (hit_by_push_attack == 0) {
                        player.got_hit(10);
                        hit_by_push_attack = 1;
                    }
                }
                else if (push_attack_duration < 1.4f) {
                    if (hit_by_push_attack == 0 || hit_by_push_attack == 1) {
                        player.got_hit(15);
                        hit_by_push_attack = 2;
                    }
                }
                else if (push_attack_duration < 1.6f) {
                    if (hit_by_push_attack == 0 || hit_by_push_attack == 1 || hit_by_push_attack == 2) {
                        player.got_hit(20);
                        hit_by_push_attack = 3;
                    }
                }


                player.set_trans(vec3(new_x,
                                      player.get_trans_vec().y,
                                      player.get_trans_vec().z));

                if (player.get_trans_vec().x > var::cam.const_eye.x + var::changeable_aspect ||
                    player.get_trans_vec().x < var::cam.const_eye.x - var::changeable_aspect) {
                    if (player.get_trans_vec().x > 0.0f) {
                        var::cam.eye.x = var::cam.const_eye.x + (player.get_trans_vec().x - var::changeable_aspect);
                        var::cam.at.x = var::cam.const_at.x + (player.get_trans_vec().x - var::changeable_aspect);
                    }
                    else {
                        var::cam.eye.x = var::cam.const_eye.x + (player.get_trans_vec().x + var::changeable_aspect);
                        var::cam.at.x = var::cam.const_at.x + (player.get_trans_vec().x + var::changeable_aspect);
                    }
                }
                else {
                    var::cam.eye.x = var::cam.const_eye.x;
                    var::cam.at.x = var::cam.const_at.x;
                }
            }
        }
        else {
            hit_by_push_attack = 0;
            cur_phase = wandering;
            wandering_duration = 0.0f;
            push_attack_go = 0;
            push_attack_wait = 0;
        }


        break;
    }

    case super_jump:
    {
        super_jump_duration += (float)delta_time;
        if (super_jump_duration < 0.5f) {
            main_texture = (looking_at_ == 1) ? &textures[13] : &textures[4];
            trans_vec_ += vec3(0.0f, 1.5f, 0.0f) * (float)delta_time;
            if (jump_attack_wait == 0) {
                var::engine->play2D(var::enemy_jump1_music);
                jump_attack_wait = 1;
            }
        }
        else if (super_jump_duration < 0.6f) {
            main_texture = (looking_at_ == 1) ? &textures[12] : &textures[5];
        }
        else {
            if (jump_attack_go == 0) {
                var::engine->play2D(var::enemy_jump2_music);
                jump_attack_go = 1;
            }

            main_texture = (looking_at_ == 1) ? &textures[12] : &textures[5];
            auto adder = vec3(0.0f, -8.0f, 0.0f) * (float)delta_time;

            auto floor_coor =
                std::move(get_actual_coord(floor_rect_->get_scale_vec(), floor_rect_->get_trans_vec()));

            auto next_vec = trans_vec_ + adder;
            auto next_mx =
                vec3(get_scale_vec().x + next_vec.x, -get_scale_vec().y + next_vec.y, get_scale_vec().z + next_vec.z);

            if (abs(floor_coor[0].y - next_mx.y) >= 0.0001f) {
                trans_vec_ = next_vec;
            }

            if (floor_coor[0].y >= next_mx.y) {
                trans_vec_ = vec3(trans_vec_[0], -1.0f + (scale_vec_.y + floor_rect_->get_scale_vec()[1] * 2),
                                  trans_vec_[2]);

                if (!player.is_ice_mode && !player.is_invincible && player.is_on_ground()) {
                    player.velocity_ += vec3(0.0f, 3.0f, 0.0f);
                    player.got_hit(30);
                }

                var::engine->play2D(var::enemy_jump3_music);
                jump_attack_wait = 0;
                jump_attack_go = 0;
                cur_phase = wandering;
                wandering_duration = 0.0f;
            }

        }
        break;
    }
    }
}

inline void bad_guy::attack::initialize(vec3 &scale, vec3 &trans) { attack_rect.initialize(scale, trans); }

inline void bad_guy::attack::draw() {
    // glDisable(GL_DEPTH_TEST);
    main_texture->bind(var::program, 1);
    var::program.set_uniform<decltype(glUniformMatrix4fv), mat4>
        (glUniformMatrix4fv, "model_matrix", 1, GL_TRUE, attack_rect.get_model_mx());
    glDrawElements(GL_TRIANGLES, var::rect_indices.size(), GL_UNSIGNED_INT, nullptr);
    main_texture->unbind();
    // glEnable(GL_DEPTH_TEST);
}

inline void bad_guy::give_damage(int damage) {
    health -= damage;

    auto pos = std::move(get_window_coord(get_scale_vec(), get_trans_vec(),
                                          var::window_size, var::changeable_aspect, var::aspect));


    std::string tmp = "";
    tmp += std::to_string(damage);

    int tmp1 = rand() % 10;
    tmp += std::to_string(tmp1);

    // tmp += ",";

    tmp1 = rand() % 10;
    tmp += std::to_string(tmp1);
    tmp1 = rand() % 10;
    tmp += std::to_string(tmp1);
    tmp1 = rand() % 10;
    tmp += std::to_string(tmp1);

    // tmp += ",";

    tmp1 = rand() % 10;
    tmp += std::to_string(tmp1);
    tmp1 = rand() % 10;
    tmp += std::to_string(tmp1);
    tmp1 = rand() % 10;
    tmp += std::to_string(tmp1);

    auto new_pos = pos.at(1) + pos.at(0);
    new_pos.x /= 2, new_pos.y /= 2;
    last_damage = damage_info({tmp, 0.0f, new_pos});
}

inline void bad_guy::set_difficulty(float diff) { difficulty = diff; }

/*===========================================================*/
/*===========================================================*/
/*===========================================================*/
/*===========================================================*/

class card_skill {
private :
    const vec3 standard_size = vec3(0.03f, 0.05f, 1.0f);
    character *player_;

public :
    float time_past;
    int attacking;
    texture_t tex;
    ~card_skill() = default;
    std::list<rect> card_list;
    void push();
    void update(double delta_time, bad_guy &enemy);
    void draw();
    void initialize(character *c);
    void attack();
};

inline void card_skill::push() {
    rect tmp;
    tmp.initialize(standard_size, player_->get_trans_vec());
    tmp.born_day = 0.0f;
    tmp.accelerator = vec3(-player_->get_looking_way() * 0.1f, 0.1f, 1.0f);
    const auto rand_float = [](int rand_int) -> float {
        switch (rand_int) {
        case 0:
            return -0.5f;
        case 1:
            return 0.5f;
        }
        return 0.0f;
    }(rand() % 3);
    tmp.velocity = vec3(-player_->get_looking_way() * 2.0f/* + rand_float*/, 1.0f + rand_float, 1.0f);
    tmp.set_theta(0.0f);
    card_list.push_back(tmp);
}

inline void card_skill::update(double delta_time, bad_guy &enemy) {
    if (attacking > 0) {
        time_past += (float)delta_time;
        if (time_past > 0.005f) {
            push();
            attacking--;
            time_past = 0.0f;
        }
    }

    for (rect &rect_ : card_list) {
        rect_.born_day += (float)delta_time;

        vec3 next_vel;
        if (rect_.born_day < 2.0f) {
            auto acc = (enemy.get_trans_vec() - rect_.get_trans_vec()).normalize() * vec3(20.0f, 20.0f, 1.0f);
            next_vel = rect_.velocity + acc * (float)delta_time;
        }
        else {
            next_vel = (enemy.get_trans_vec() - rect_.get_trans_vec()).normalize() * 10.0f;
        }

        auto next_pos = rect_.get_trans_vec() + next_vel * (float)delta_time;
        rect_.velocity = next_vel;
        rect_.set_trans_vec(next_pos);
        rect_.set_theta(rect_.get_theta() + (float)delta_time);
    }

    for (auto it = card_list.begin(); it != card_list.end();) {
        auto card_coor = std::move(get_actual_coord((*it).get_scale_vec(), (*it).get_trans_vec()));
        auto enemy_coor = std::move(get_actual_coord(enemy.get_scale_vec(), enemy.get_trans_vec()));

        if ((*it).born_day > 0.3f && aabb(card_coor, enemy_coor)) {
            card_list.erase(it++);
            enemy.give_damage(int(1000.0f * (1.0f + float(player_->stars) / 20.0f)));
            var::engine->play2D(var::player_c_attack_music);
        }
        else
            ++it;
    }
}

inline void card_skill::draw() {
    for (rect &rect_ : card_list) {
        tex.bind(var::program, 1);

        var::program.set_uniform<decltype(glUniformMatrix4fv), mat4>(glUniformMatrix4fv, "model_matrix", 1, GL_TRUE,
                                                                     rect_.get_model_mx());
        glDrawElements(GL_TRIANGLES, var::rect_indices.size(), GL_UNSIGNED_INT, nullptr);
        tex.unbind();
    }
}

inline void card_skill::initialize(character *c) {
    time_past = 0.0f;
    player_ = c;
    card_list.clear();
    attacking = 0;
    tex.init_texture("../bin/images/card.png", true, true);
}

inline void card_skill::attack() {
    time_past = 0.0f;
    attacking = 100;
}

/*===========================================================*/
/*===========================================================*/
/*===========================================================*/
/*===========================================================*/

class attack_skill : public rect_interface {
public:
    ~attack_skill() = default;
    mat4 get_trans_mx() override;
    mat4 get_scale_mx() override;
    mat4 get_rotate_mx() override;
    vec3 get_trans_vec() override;
    vec3 get_scale_vec() override;
    void draw();
    void update(double delta_time, bad_guy &enemy, card_skill &c_attack);
    void initialize(character *c);

private:
    const float const_theta = 0.2f;
    float theta = const_theta;

    float theta_for_clock = 0.0f;
    vec3 scale_vec_ = vec3(0.25f, 0.02f, 1.0f);
    vec3 trans_vec_;
    character *player_;
    rect cape_;
};

inline void attack_skill::draw() {
    if (!var::attack_key.is_pressed)
        return;

    // cape_.draw();
    if (texture.is_available()) {
        texture.bind(var::program, 1);
        var::program.set_uniform<decltype(glUniformMatrix4fv), mat4>(glUniformMatrix4fv, "model_matrix", 1, GL_TRUE,
                                                                     get_model_mx());
        glDrawElements(GL_TRIANGLES, var::rect_indices.size(), GL_UNSIGNED_INT, nullptr);
        texture.unbind();
    }
    else {
        var::program.set_uniform<decltype(glUniform1i), int>(glUniform1i, "color_type", 0);
        var::program.set_uniform<decltype(glUniformMatrix4fv), mat4>(glUniformMatrix4fv, "model_matrix", 1, GL_TRUE,
                                                                     get_model_mx());
        glDrawElements(GL_TRIANGLES, var::rect_indices.size(), GL_UNSIGNED_INT, nullptr);

    }
}

inline void attack_skill::update(double delta_time, bad_guy &enemy, card_skill &c_attack) {
    if (!var::attack_key.is_pressed)
        return;

    if (player_->get_looking_way() == 1) {
        trans_vec_ = player_->get_trans_vec() + vec3(scale_vec_.x / 2, 0.0f, 0.0f);
        cape_.set_trans_vec(player_->get_trans_vec() - vec3(cape_.get_scale_vec().x, 0.0f, 0.0f));
    }
    else {
        trans_vec_ = player_->get_trans_vec() + vec3(-scale_vec_.x / 2, 0.0f, 0.0);
        cape_.set_trans_vec(player_->get_trans_vec() + vec3(cape_.get_scale_vec().x, 0.0f, 0.0f));
    }

    int our = (int)ceil((double)cos(theta_for_clock * 30.0f));
    int their = (int)ceil((double)cos(var::theta * 30.0f));
    if (our != their) {
        theta_for_clock = var::theta;

        var::engine->play2D(var::player_attack_music);

        if (cos(theta_for_clock * 30.0f) > 0.0f) {
            cape_.set_theta((player_->get_looking_way() == 1) ? -const_theta * 1.5f : PI + const_theta * 1.5f);
            theta = (player_->get_looking_way() == 1) ? const_theta * 1.5f : PI - const_theta * 1.5f;

            player_->main_texture = (player_->get_looking_way() == 1) ? &player_->textures[9] : &player_->textures[8];
        }
        else {
            cape_.set_theta(0.0f);
            theta = (player_->get_looking_way() == 1) ? -const_theta / 2.0f : PI - -const_theta / 2.0f;
            player_->main_texture = (player_->get_looking_way() == 1) ? &player_->textures[11] : &player_->textures[10];
        }

        auto skill_coor = std::move(get_actual_coord(scale_vec_, trans_vec_));
        auto enemy_coor = std::move(get_actual_coord(enemy.get_scale_vec(), enemy.get_trans_vec()));
        /*    auto player_coor = std::move(get_actual_coord(player_.get_scale_vec(), player_.get_trans_vec()));*/
        if (aabb(skill_coor, enemy_coor)/* || aabb(player_coor, enemy_coor)*/) {
            //       printf("hit! %d\n", num++);
            enemy.give_damage((int)((3000.0f * var::op_ratio) * (1.0f + float(player_->stars) / 20.0f)));
            c_attack.push();
        }
    }
}

inline void attack_skill::initialize(character *c) {
    player_ = c;
    cape_.initialize(vec3(0.12f, 0.06f, 1.0f), vec3(0.1f, 0.1f, 0.1f));

    theta = const_theta;
    theta_for_clock = 0.0f;
    scale_vec_ = vec3(0.25f, 0.02f, 1.0f);

    init_tex("../bin/images/cotton_swap.png");
}

inline vec3 attack_skill::get_trans_vec() { return trans_vec_; }

inline vec3 attack_skill::get_scale_vec() { return scale_vec_; }

inline mat4 attack_skill::get_trans_mx() { return get_2D_trans_mx(get_trans_vec()); }

inline mat4 attack_skill::get_scale_mx() { return get_2D_scale_mx(get_scale_vec()); }

inline mat4 attack_skill::get_rotate_mx() { return get_2D_rotate_mx(theta); }

/*===========================================================*/
/*===========================================================*/
/*===========================================================*/
/*===========================================================*/

class ult {
public:
    rect ult_card;
    rect ult_card2;
    rect ult_card3;
    rect ult_card4;
    rect ult_card5;
    rect ult_card6;
    rect ult_card7;
    rect ult_card8;
    rect ult_card9;

    float past_time;

    void ult_pressed();

    texture_t ult_card_texture;

    void initialize();
    void update(character &player, card_skill &c_attack, double delta_time);
    void draw();
};

inline void ult::ult_pressed() {
    past_time = 0.0f;
    ult_card.set_trans_vec(vec3(0.0f, 0.0f, 0.0f));
    ult_card2.set_trans_vec(vec3(0.0f, 0.0f, 0.0f));
    ult_card3.set_trans_vec(vec3(0.0f, 0.0f, 0.0f));
    ult_card4.set_trans_vec(vec3(0.0f, 0.0f, 0.0f));
    ult_card5.set_trans_vec(vec3(0.0f, 0.0f, 0.0f));
    ult_card6.set_trans_vec(vec3(0.0f, 0.0f, 0.0f));
    ult_card7.set_trans_vec(vec3(0.0f, 0.0f, 0.0f));
    ult_card8.set_trans_vec(vec3(0.0f, 0.0f, 0.0f));
}

inline void ult::initialize() {
    past_time = 0.0f;

    ult_card.initialize(vec3(0.42f, 0.7f, 1.0f), vec3(0.0f, 0.0f, 1.0f));
    ult_card2.initialize(vec3(0.42f, 0.7f, 1.0f), vec3(0.0f, 0.0f, 1.0f));
    ult_card3.initialize(vec3(0.42f, 0.7f, 1.0f), vec3(0.0f, 0.0f, 1.0f));
    ult_card4.initialize(vec3(0.42f, 0.7f, 1.0f), vec3(0.0f, 0.0f, 1.0f));
    ult_card5.initialize(vec3(0.42f, 0.7f, 1.0f), vec3(0.0f, 0.0f, 1.0f));
    ult_card6.initialize(vec3(0.42f, 0.7f, 1.0f), vec3(0.0f, 0.0f, 1.0f));
    ult_card7.initialize(vec3(0.42f, 0.7f, 1.0f), vec3(0.0f, 0.0f, 1.0f));
    ult_card8.initialize(vec3(0.42f, 0.7f, 1.0f), vec3(0.0f, 0.0f, 1.0f));

    ult_card_texture.init_texture("../bin/images/card.png", true, true);
}

inline void ult::update(character &player, card_skill &c_attack, double delta_time) {
    past_time += (float)delta_time;

    player.is_invincible = true;

    if (past_time < 1.0f) {
        ult_card.set_theta(var::theta * 40.0f);
    }
    else if (past_time < 1.5f) {
        ult_card.set_theta(0.0f);
        ult_card.set_trans_vec(ult_card.get_trans_vec() + vec3(0.0f, 10.0f, 0.0f) * (float)delta_time);
        ult_card2.set_theta(-1.0f * PI / 4.0f);
        ult_card2.set_trans_vec(ult_card2.get_trans_vec() + vec3(7.0f, 7.0f, 0.0f) * (float)delta_time);
        ult_card3.set_theta(-2.0f * PI / 4.0f);
        ult_card3.set_trans_vec(ult_card3.get_trans_vec() + vec3(10.0f, 0.0f, 0.0f) * (float)delta_time);
        ult_card4.set_theta(-3.0f * PI / 4.0f);
        ult_card4.set_trans_vec(ult_card4.get_trans_vec() + vec3(7.0f, -7.0f, 0.0f) * (float)delta_time);
        ult_card5.set_theta(-4.0f * PI / 4.0f);
        ult_card5.set_trans_vec(ult_card5.get_trans_vec() + vec3(0.0f, -10.0f, 0.0f) * (float)delta_time);
        ult_card6.set_theta(-5.0f * PI / 4.0f);
        ult_card6.set_trans_vec(ult_card6.get_trans_vec() + vec3(-7.0f, -7.0f, 0.0f) * (float)delta_time);
        ult_card7.set_theta(-6.0f * PI / 4.0f);
        ult_card7.set_trans_vec(ult_card7.get_trans_vec() + vec3(-10.0f, 0.0f, 0.0f) * (float)delta_time);
        ult_card8.set_theta(-7.0f * PI / 4.0f);
        ult_card8.set_trans_vec(ult_card8.get_trans_vec() + vec3(-7.0f, 7.0f, 0.0f) * (float)delta_time);
    }
    else {
        c_attack.attack();
        player.is_invincible = false;
        var::cur_scene = battle_scene;
    }
}

inline void ult::draw() {
    // background_texture.bind(var::program2, 1);
    // var::program2.set_uniform<decltype(glUniformMatrix4fv), mat4>
    //     (glUniformMatrix4fv, "model_matrix", 1, GL_TRUE, help_background.get_model_mx());
    // glDrawElements(GL_TRIANGLES, var::rect_indices.size(), GL_UNSIGNED_INT, nullptr);

    ult_card_texture.bind(var::program, 1);
    var::program2.set_uniform<decltype(glUniformMatrix4fv), mat4>(glUniformMatrix4fv, "model_matrix", 1, GL_TRUE,
                                                                  ult_card.get_model_mx());
    glDrawElements(GL_TRIANGLES, var::rect_indices.size(), GL_UNSIGNED_INT, nullptr);

    if (past_time > 1.0f) {
        var::program2.set_uniform<decltype(glUniformMatrix4fv), mat4>(glUniformMatrix4fv, "model_matrix", 1, GL_TRUE,
                                                                      ult_card2.get_model_mx());
        glDrawElements(GL_TRIANGLES, var::rect_indices.size(), GL_UNSIGNED_INT, nullptr);
        var::program2.set_uniform<decltype(glUniformMatrix4fv), mat4>(glUniformMatrix4fv, "model_matrix", 1, GL_TRUE,
                                                                      ult_card3.get_model_mx());
        glDrawElements(GL_TRIANGLES, var::rect_indices.size(), GL_UNSIGNED_INT, nullptr);
        var::program2.set_uniform<decltype(glUniformMatrix4fv), mat4>(glUniformMatrix4fv, "model_matrix", 1, GL_TRUE,
                                                                      ult_card4.get_model_mx());
        glDrawElements(GL_TRIANGLES, var::rect_indices.size(), GL_UNSIGNED_INT, nullptr);
        var::program2.set_uniform<decltype(glUniformMatrix4fv), mat4>(glUniformMatrix4fv, "model_matrix", 1, GL_TRUE,
                                                                      ult_card5.get_model_mx());
        glDrawElements(GL_TRIANGLES, var::rect_indices.size(), GL_UNSIGNED_INT, nullptr);
        var::program2.set_uniform<decltype(glUniformMatrix4fv), mat4>(glUniformMatrix4fv, "model_matrix", 1, GL_TRUE,
                                                                      ult_card6.get_model_mx());
        glDrawElements(GL_TRIANGLES, var::rect_indices.size(), GL_UNSIGNED_INT, nullptr);
        var::program2.set_uniform<decltype(glUniformMatrix4fv), mat4>(glUniformMatrix4fv, "model_matrix", 1, GL_TRUE,
                                                                      ult_card7.get_model_mx());
        glDrawElements(GL_TRIANGLES, var::rect_indices.size(), GL_UNSIGNED_INT, nullptr);
        var::program2.set_uniform<decltype(glUniformMatrix4fv), mat4>(glUniformMatrix4fv, "model_matrix", 1, GL_TRUE,
                                                                      ult_card8.get_model_mx());
        glDrawElements(GL_TRIANGLES, var::rect_indices.size(), GL_UNSIGNED_INT, nullptr);
    }
    ult_card_texture.unbind();
}

class cooltime_controler {
public:
    bool is_cool;
    float time_past;
    float target_time;
    void start_cool();
    void update(double delta_time);
    void initialize(float target_time);
};

inline void cooltime_controler::start_cool() {
    time_past = 0.0f;
    is_cool = true;
}

inline void cooltime_controler::update(double delta_time) {
    if (is_cool) {
        time_past += (float)delta_time;

        if (time_past > target_time) {
            time_past = 0.0f;
            is_cool = false;
        }
    }
}

inline void cooltime_controler::initialize(float target_time) {
    is_cool = false;
    time_past = 0.0f;
    this->target_time = target_time;
}

class skills {
public:
    const float wh = 0.1f;
    // const vec3 starting_vec = vec3(0.55f, -0.85f, 0.0f);
    const vec3 starting_vec = vec3(0.7f, -0.85f, 0.0f);
    const vec3 interval = vec3(0.01f, 0.0f, 0.0f);

    rect button_skill_a;
    // rect button_skill_s;
    rect button_skill_d;
    rect button_skill_f;
    rect button_skill_r;
    rect button_skill_end;

    float past_time;

    texture_t texture_a;
    texture_t texture_d;
    texture_t texture_f;
    texture_t texture_r;
    texture_t *texture_r_main;
    texture_t texture_end;
    texture_t *texture_end_main;

    texture_t texture_ends[8];
    texture_t texture_rs[8];

    void initialize();
    void initialize_texture();
    void update(cooltime_controler &home, cooltime_controler &ult, double delta_time);
    void draw();
};


inline void skills::initialize() {
    past_time = 0.0f;

    vec3 vecs = starting_vec;
    button_skill_a.initialize(vec3(wh, wh, 1.0f), vecs);
    // vecs += vec3(wh * 2.0f, 0.0f, 0.0f) + interval;
    // button_skill_s.initialize(vec3(wh, wh, 1.0f), vecs);
    vecs += vec3(wh * 2.0f, 0.0f, 0.0f) + interval;
    button_skill_d.initialize(vec3(wh, wh, 1.0f), vecs);
    vecs += vec3(wh * 2.0f, 0.0f, 0.0f) + interval;
    button_skill_f.initialize(vec3(wh, wh, 1.0f), vecs);
    vecs += vec3(wh * 2.0f, 0.0f, 0.0f) + interval;
    button_skill_r.initialize(vec3(wh, wh, 1.0f), vecs);
    vecs += vec3(wh * 2.0f, 0.0f, 0.0f) + interval;
    button_skill_end.initialize(vec3(wh, wh, 1.0f), vecs);

}

inline void skills::initialize_texture() {
    texture_a.init_texture("../bin/images/askill_icon.png", true, false);
    texture_d.init_texture("../bin/images/dskill_icon.png", true, false);
    texture_f.init_texture("../bin/images/fskill_icon.png", true, false);

    texture_end.init_texture("../bin/images/endskill_icon.png", true, false);
    texture_end_main = &texture_end;

    texture_ends[0].init_texture("../bin/images/hskill_icon_1.png", true, false);
    texture_ends[1].init_texture("../bin/images/hskill_icon_2.png", true, false);
    texture_ends[2].init_texture("../bin/images/hskill_icon_3.png", true, false);
    texture_ends[3].init_texture("../bin/images/hskill_icon_4.png", true, false);
    texture_ends[4].init_texture("../bin/images/hskill_icon_5.png", true, false);
    texture_ends[5].init_texture("../bin/images/hskill_icon_6.png", true, false);
    texture_ends[6].init_texture("../bin/images/hskill_icon_7.png", true, false);
    texture_ends[7].init_texture("../bin/images/hskill_icon_8.png", true, false);

    texture_r.init_texture("../bin/images/rskill_icon.png", true, false);
    texture_r_main = &texture_r;

    texture_rs[0].init_texture("../bin/images/rskill_icon_1.png", true, false);
    texture_rs[1].init_texture("../bin/images/rskill_icon_2.png", true, false);
    texture_rs[2].init_texture("../bin/images/rskill_icon_3.png", true, false);
    texture_rs[3].init_texture("../bin/images/rskill_icon_4.png", true, false);
    texture_rs[4].init_texture("../bin/images/rskill_icon_5.png", true, false);
    texture_rs[5].init_texture("../bin/images/rskill_icon_6.png", true, false);
    texture_rs[6].init_texture("../bin/images/rskill_icon_7.png", true, false);
    texture_rs[7].init_texture("../bin/images/rskill_icon_8.png", true, false);
}

inline void skills::update(cooltime_controler &home, cooltime_controler &ult, double delta_time) {
    past_time += (float)delta_time;
    if (home.is_cool) {
        auto curtime = home.time_past, targettime = home.target_time;
        float ratio = curtime / targettime;
        if (ratio < 0.125f)
            texture_end_main = &texture_ends[7];
        else if (ratio < 0.25f)
            texture_end_main = &texture_ends[6];
        else if (ratio < 0.375f)
            texture_end_main = &texture_ends[5];
        else if (ratio < 0.5f)
            texture_end_main = &texture_ends[4];
        else if (ratio < 0.625f)
            texture_end_main = &texture_ends[3];
        else if (ratio < 0.75f)
            texture_end_main = &texture_ends[2];
        else if (ratio < 0.875f)
            texture_end_main = &texture_ends[1];
        else
            texture_end_main = &texture_ends[0];
    }
    else {
        texture_end_main = &texture_end;
    }

    if (ult.is_cool) {
        auto curtime = ult.time_past, targettime = ult.target_time;
        float ratio = curtime / targettime;
        if (ratio < 0.125f)
            texture_r_main = &texture_rs[7];
        else if (ratio < 0.25f)
            texture_r_main = &texture_rs[6];
        else if (ratio < 0.375f)
            texture_r_main = &texture_rs[5];
        else if (ratio < 0.5f)
            texture_r_main = &texture_rs[4];
        else if (ratio < 0.625f)
            texture_r_main = &texture_rs[3];
        else if (ratio < 0.75f)
            texture_r_main = &texture_rs[2];
        else if (ratio < 0.875f)
            texture_r_main = &texture_rs[1];
        else
            texture_r_main = &texture_rs[0];
    }
    else {
        texture_r_main = &texture_r;
    }
}

inline void skills::draw() {
    texture_a.bind(var::program2, 1);
    var::program2.set_uniform<decltype(glUniformMatrix4fv), mat4>(glUniformMatrix4fv, "model_matrix", 1, GL_TRUE,
                                                                  button_skill_a.get_model_mx());
    glDrawElements(GL_TRIANGLES, var::rect_indices.size(), GL_UNSIGNED_INT, nullptr);

    texture_d.bind(var::program2, 1);
    var::program2.set_uniform<decltype(glUniformMatrix4fv), mat4>(glUniformMatrix4fv, "model_matrix", 1, GL_TRUE,
                                                                  button_skill_d.get_model_mx());
    glDrawElements(GL_TRIANGLES, var::rect_indices.size(), GL_UNSIGNED_INT, nullptr);

    texture_f.bind(var::program2, 1);
    var::program2.set_uniform<decltype(glUniformMatrix4fv), mat4>(glUniformMatrix4fv, "model_matrix", 1, GL_TRUE,
                                                                  button_skill_f.get_model_mx());
    glDrawElements(GL_TRIANGLES, var::rect_indices.size(), GL_UNSIGNED_INT, nullptr);

    texture_r_main->bind(var::program2, 1);
    var::program2.set_uniform<decltype(glUniformMatrix4fv), mat4>(glUniformMatrix4fv, "model_matrix", 1, GL_TRUE,
                                                                  button_skill_r.get_model_mx());
    glDrawElements(GL_TRIANGLES, var::rect_indices.size(), GL_UNSIGNED_INT, nullptr);

    texture_end_main->bind(var::program2, 1);
    var::program2.set_uniform<decltype(glUniformMatrix4fv), mat4>(glUniformMatrix4fv, "model_matrix", 1, GL_TRUE,
                                                                  button_skill_end.get_model_mx());
    glDrawElements(GL_TRIANGLES, var::rect_indices.size(), GL_UNSIGNED_INT, nullptr);
    texture_end.unbind();
}

class stars {
public :
    void initialize(character *player);
    void add_new_star();
    bool is_added_;
    rect star_rect;
    texture_t star_tex;
    void update(vec3 &p_scale, vec3 &p_trans);
    vec3 pos;
    void draw();
private:
    std::string file_path = "../bin/save_file.txt";
    int num_stars;
    character *player_;
    std::fstream file;
};


inline void stars::update(vec3 &p_scale, vec3 &p_trans) {
    pos.x = p_trans.x - 0.08f;
    pos.y = p_trans.y + p_scale.y;
    pos.z = 1.0f;
}

inline void stars::draw() {
    star_tex.bind(var::program, 1);
    for (int i = 0; i < min(num_stars, 9); i++) {
        star_rect.set_trans_vec(pos + vec3(0.02f * i, 0.02f, 0.0f));

        var::program.set_uniform<decltype(glUniformMatrix4fv), mat4>(glUniformMatrix4fv, "model_matrix", 1, GL_TRUE,
                                                                     star_rect.get_model_mx());
        glDrawElements(GL_TRIANGLES, var::rect_indices.size(), GL_UNSIGNED_INT, nullptr);

    }
    star_tex.unbind();
}

inline void stars::initialize(character *player) {
    num_stars = 0;
    is_added_ = false;
    player_ = player;

    star_rect.initialize(vec3(0.02f, 0.02f, 1.0f), vec3(0.0f, 0.0f, 1.0f));
    star_tex.init_texture("../bin/images/star.png", true, true);

    file.open(file_path, std::fstream::in | std::fstream::out | std::fstream::app);
    if (!file) {
        std::cout << "no save file. creating new one.\n";
        file.close();
        file.open(file_path, std::fstream::in | std::fstream::out | std::fstream::trunc);
        file << "0";
        file.close();
    }
    else {
        std::string line;
        bool is_valid_file = true;
        std::getline(file, line);
        if (line.size() == 0)
            is_valid_file = false;
        else
            for (auto c : line) {
                if (c < '0' || '9' < c) {
                    is_valid_file = false;
                }
            }

        if (is_valid_file) {
            std::cout << "save file opened.\n";
            num_stars = std::stoi(line);
            file.close();
        }
        else {
            std::cout << "invalid save file. creating new one.\n";
            file.close();
            file.open(file_path, std::fstream::in | std::fstream::out | std::fstream::trunc);
            file << "0";
            file.close();
        }
    }

    std::cout << "save file loaded " << num_stars << " stars\n";
    player_->stars = num_stars;
}

inline void stars::add_new_star() {
    if (!is_added_) {
        num_stars++;
        file.open(file_path, std::fstream::in | std::fstream::out | std::fstream::trunc);
        if (!file) {
            std::cout << "no save file. creating new one.\n";
            file.close();
            file.open(file_path, std::fstream::in | std::fstream::out | std::fstream::trunc);
            file << num_stars;
            file.close();
        }
        else {
            file << num_stars;
            file.close();
        }
        std::cout << "stars added to " << num_stars << std::endl;
        is_added_ = true;
    }
}


#endif
