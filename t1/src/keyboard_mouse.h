#pragma once
#ifndef __KEYBOARD_MOUSE_H__
#define __KEYBOARD_MOUSE_H__
#include "cgmath.h"
#include "ut.h"
#include "namespace.h"
#include "window.h"

void initialize_game();

vec2 return_pos(dvec2 &mpos) {
    vec2 ret;
    float x = (float)mpos.x, y = (float)mpos.y;
    ret.x = (x - (float)var::window_size.x / 2.0f) / ((float)var::window_size.x / 2.0f);
    ret.x = ret.x / min(1.0f / var::changeable_aspect, 1.0f / var::aspect);
    ret.y = (y - (float)var::window_size.y / 2.0f) / (-(float)var::window_size.y / 2.0f);
    ret.y = ret.y / min(var::changeable_aspect / var::aspect, 1.0f);
    return ret;
}

int num_down = -1;
int click_where = 0;

void mouse(GLFWwindow *window, const int button, const int action, int mods) {
    auto &button_s = var::sound_control.button.get_scale_vec();
    auto &button_t = var::sound_control.button.get_trans_vec();

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        dvec2 pos;
        glfwGetCursorPos(window, &pos.x, &pos.y);
        vec2 &f_pos = std::move(return_pos(pos));

        if (is_clicked_square(button_s, button_t, f_pos)) {
            var::sound_control.release(true);
            if (var::sound_control.is_muted) {
                var::sound_control.is_muted = false;
                var::engine->setSoundVolume(1.0f);
            } else {
                var::sound_control.is_muted = true;
                var::engine->setSoundVolume(0.0f);
            }
        } else {
            var::sound_control.release(false);
            
        }

    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        dvec2 pos;
        glfwGetCursorPos(window, &pos.x, &pos.y);
        vec2 &f_pos = std::move(return_pos(pos));


        if (is_clicked_square(button_s, button_t, f_pos)) {
            var::sound_control.click();
        }
    }
    switch (var::cur_scene) {
    case lose_scene:
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
            dvec2 pos;
            glfwGetCursorPos(window, &pos.x, &pos.y);
            vec2 &f_pos = std::move(return_pos(pos));

            auto &totitle_s = var::lose_screen.to_title_key.get_scale_vec();
            auto &totitle_t = var::lose_screen.to_title_key.get_trans_vec();
            if (is_clicked_square(totitle_s, totitle_t, f_pos)) {
                var::cur_scene = title_scene;
            }

            var::lose_screen.release(click_where);
            click_where = 0;
        }
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            dvec2 pos;
            glfwGetCursorPos(window, &pos.x, &pos.y);
            vec2 f_pos = std::move(return_pos(pos));

            auto &totitle_s = var::lose_screen.to_title_key.get_scale_vec();
            auto &totitle_t = var::lose_screen.to_title_key.get_trans_vec();
            if (is_clicked_square(totitle_s, totitle_t, f_pos)) {
                click_where = 1;
                var::lose_screen.click(1);
            }
        }
        break;
    }
    case win_scene:
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
            dvec2 pos;
            glfwGetCursorPos(window, &pos.x, &pos.y);
            vec2 &f_pos = std::move(return_pos(pos));

            auto &totitle_s = var::win_screen.to_title_key.get_scale_vec();
            auto &totitle_t = var::win_screen.to_title_key.get_trans_vec();
            if (is_clicked_square(totitle_s, totitle_t, f_pos)) {
                var::cur_scene = title_scene;
            }

            var::win_screen.release(click_where);
            click_where = 0;
        }
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            dvec2 pos;
            glfwGetCursorPos(window, &pos.x, &pos.y);
            vec2 f_pos = std::move(return_pos(pos));

            auto &totitle_s = var::win_screen.to_title_key.get_scale_vec();
            auto &totitle_t = var::win_screen.to_title_key.get_trans_vec();
            if (is_clicked_square(totitle_s, totitle_t, f_pos)) {
                click_where = 1;
                var::win_screen.click(1);
            }
        }
        break;
    }
    case pause_scene:
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
            dvec2 pos;
            glfwGetCursorPos(window, &pos.x, &pos.y);
            vec2 &f_pos = std::move(return_pos(pos));

            auto &restart_s = var::pause_screen.restart_key.get_scale_vec();
            auto &restart_t = var::pause_screen.restart_key.get_trans_vec();
            auto &continue_s = var::pause_screen.continue_key.get_scale_vec();
            auto &continue_t = var::pause_screen.continue_key.get_trans_vec();
            auto &totitle_s = var::pause_screen.to_title_key.get_scale_vec();
            auto &totitle_t = var::pause_screen.to_title_key.get_trans_vec();

            if (is_clicked_square(restart_s, restart_t, f_pos)) {
                if (click_where == 1) {
                    auto p_d = var::poop.difficulty;
                    auto o_d = var::opponent.difficulty;
                    initialize_game();
                    var::poop.set_difficulty(p_d);
                    var::opponent.set_difficulty(o_d);
                    var::cur_scene = battle_scene;
                }
            }
            else if (is_clicked_square(continue_s, continue_t, f_pos)) {
                if (click_where == 2) {
                    var::move_key.is_pressed = false;
                    var::move_key.key_type = -1;
                    num_down = -1;
                    var::attack_key.is_pressed = false;
                    var::player.is_ice_mode = false;
                    var::player.is_invincible = false;

                    var::cur_scene = battle_scene;
                }
            }
            else if (is_clicked_square(totitle_s, totitle_t, f_pos)) {
                if (click_where == 3) {
                    var::cur_scene = title_scene;
                }
            }

            var::pause_screen.release(click_where);
            click_where = 0;
        }
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            dvec2 pos;
            glfwGetCursorPos(window, &pos.x, &pos.y);
            vec2 f_pos = std::move(return_pos(pos));

            auto &restart_s = var::pause_screen.restart_key.get_scale_vec();
            auto &restart_t = var::pause_screen.restart_key.get_trans_vec();
            auto &continue_s = var::pause_screen.continue_key.get_scale_vec();
            auto &continue_t = var::pause_screen.continue_key.get_trans_vec();
            auto &totitle_s = var::pause_screen.to_title_key.get_scale_vec();
            auto &totitle_t = var::pause_screen.to_title_key.get_trans_vec();

            if (is_clicked_square(restart_s, restart_t, f_pos)) {
                click_where = 1;
                var::pause_screen.click(1);
            }
            else if (is_clicked_square(continue_s, continue_t, f_pos)) {
                click_where = 2;
                var::pause_screen.click(2);
            }
            else if (is_clicked_square(totitle_s, totitle_t, f_pos)) {
                click_where = 3;
                var::pause_screen.click(3);
            }
        }
        break;
    }
    case title_scene:
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
            dvec2 pos;
            glfwGetCursorPos(window, &pos.x, &pos.y);
            vec2 &f_pos = std::move(return_pos(pos));

            auto &easy_s = var::title_screen.easy_key.get_scale_vec();
            auto &easy_t = var::title_screen.easy_key.get_trans_vec();
            auto &hard_s = var::title_screen.hard_key.get_scale_vec();
            auto &hard_t = var::title_screen.hard_key.get_trans_vec();
            if (is_clicked_square(easy_s, easy_t, f_pos) && click_where == 1) {
                initialize_game();
                var::poop.set_difficulty(0.2f);
                var::opponent.set_difficulty(0.2f);
                var::cur_scene = battle_scene;
            }
            else if (is_clicked_square(hard_s, hard_t, f_pos) && click_where == 2) {
                initialize_game();
                var::poop.set_difficulty(0.05f);
                var::opponent.set_difficulty(0.1f);
                var::cur_scene = battle_scene;
            }

            var::title_screen.release(click_where);
            click_where = 0;

        }
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            dvec2 pos;
            glfwGetCursorPos(window, &pos.x, &pos.y);
            vec2 f_pos = std::move(return_pos(pos));

            auto &easy_s = var::title_screen.easy_key.get_scale_vec();
            auto &easy_t = var::title_screen.easy_key.get_trans_vec();
            auto &hard_s = var::title_screen.hard_key.get_scale_vec();
            auto &hard_t = var::title_screen.hard_key.get_trans_vec();
            if (is_clicked_square(easy_s, easy_t, f_pos)) {
                click_where = 1;
                var::title_screen.click(1);
            }
            else if (is_clicked_square(hard_s, hard_t, f_pos)) {
                click_where = 2;
                var::title_screen.click(2);
            }
        }
        break;
    }
    }

}

void motion(GLFWwindow *window, double x, double y) {
    switch (var::cur_scene) {
    case battle_scene:
    {
        dvec2 pos;
        glfwGetCursorPos(window, &pos.x, &pos.y);
        vec2 &f_pos = std::move(return_pos(pos));


        auto &askill_s = var::skill_screen.button_skill_a.get_scale_vec();
        auto &askill_t = var::skill_screen.button_skill_a.get_trans_vec();
        auto &dskill_s = var::skill_screen.button_skill_d.get_scale_vec();
        auto &dskill_t = var::skill_screen.button_skill_d.get_trans_vec();
        auto &fskill_s = var::skill_screen.button_skill_f.get_scale_vec();
        auto &fskill_t = var::skill_screen.button_skill_f.get_trans_vec();
        auto &rskill_s = var::skill_screen.button_skill_r.get_scale_vec();
        auto &rskill_t = var::skill_screen.button_skill_r.get_trans_vec();
        auto &endskill_s = var::skill_screen.button_skill_end.get_scale_vec();
        auto &endskill_t = var::skill_screen.button_skill_end.get_trans_vec();

        if (is_clicked_square(askill_s, askill_t, f_pos)) {
            var::stopper.turn_on(-1);
            var::skill_type = 0;
        }
        else if (is_clicked_square(dskill_s, dskill_t, f_pos)) {
            var::stopper.turn_on(-1);
            var::skill_type = 1;
        }
        else if (is_clicked_square(rskill_s, rskill_t, f_pos)) {
            var::stopper.turn_on(-1);
            var::skill_type = 4;
        }
        else if (is_clicked_square(fskill_s, fskill_t, f_pos)) {
            var::stopper.turn_on(-1);
            var::skill_type = 2;
        }
        else if (is_clicked_square(endskill_s, endskill_t, f_pos)) {
            var::stopper.turn_on(-1);
            var::skill_type = 3;
        }
        else {
            var::skill_type = -1;
            var::stopper.turn_off();
        }
        break;
    }

    }
}


void initialize_game() {
    var::background.initialize(vec3(var::aspect, 1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f));

    var::floor.initialize(vec3(var::aspect, 0.15f, 1.0f), vec3(0.0f, -0.85f, 1.0f));

    var::player.initialize(vec3(0.06f, 0.08f, 1.0f), vec3(-0.5f, 0.0f, 1.0f), &var::floor);

    var::player_hp_bar.initialize(var::player.get_scale_vec(), var::player.get_trans_vec());
    var::bad_guy_hp_bar.initialize();

    var::c_attack.initialize(&var::player);

    var::d_attack.initialize(&var::player);

    var::poop.initialize();

    var::opponent.initialize(vec3(0.1f, 0.13f, 1.0f), vec3(0.0f, 0.0f, 1.0f), &var::floor);

    var::particle_set2.initialize_particles(white);

    var::player_star.initialize(&var::player);
    var::is_new_star_added = false;

    var::home_controler.initialize(8.0f);
    var::ult_controler.initialize(8.0f);

    last_damage.damage = "";
    my_damage.damage = "";

    var::move_key.is_pressed = false;
    var::move_key.key_type = -1;
    num_down = -1;
    var::attack_key.is_pressed = false;
}


void keyboard(GLFWwindow *window, int key, int scancode, int action, int mods) {
    switch (var::cur_scene) {
    case pause_scene:
    {
        if (action == GLFW_PRESS) {
            if (key == GLFW_KEY_ESCAPE) {
                var::move_key.is_pressed = false;
                var::move_key.key_type = -1;
                num_down = -1;
                var::attack_key.is_pressed = false;
                var::player.is_ice_mode = false;
                var::player.is_invincible = false;

                click_where = 0;

                var::cur_scene = battle_scene;
            }
            else if (key == GLFW_KEY_F1) {
                var::last_scene = pause_scene;
                var::cur_scene = help_scene;
            }
            else if (key == GLFW_KEY_Q)
                glfwSetWindowShouldClose(window, GL_TRUE);

        }
        break;
    }
    case help_scene:
    {
        if (action == GLFW_PRESS) {
            if (key == GLFW_KEY_F1) {
                if (var::last_scene == battle_scene) {
                    var::move_key.is_pressed = false;
                    var::move_key.key_type = -1;
                    num_down = -1;
                    var::attack_key.is_pressed = false;
                    var::player.is_ice_mode = false;

                    click_where = 0;
                }
                var::cur_scene = var::last_scene;
            }
            else if (key == GLFW_KEY_Q)
                glfwSetWindowShouldClose(window, GL_TRUE);
        }

        break;
    }
    case ult_scene:
    case battle_scene:
    {
        if (action == GLFW_PRESS) {
            if (key == GLFW_KEY_Q)
                glfwSetWindowShouldClose(window, GL_TRUE);
            else if (key == GLFW_KEY_ESCAPE) {
                var::cur_scene = pause_scene;
            }
            else if (key == GLFW_KEY_F1) {
                var::last_scene = battle_scene;
                var::cur_scene = help_scene;
            }
            else {
                if (var::stopper.is_stopped)
                    break;

                if (key == GLFW_KEY_H)
                    print_help();
                else if (key == GLFW_KEY_UP) {
                    num_down = GLFW_KEY_UP;
                }
                else if (key == GLFW_KEY_DOWN) {
                    num_down = GLFW_KEY_DOWN;
                }
                else if (key == GLFW_KEY_RIGHT) {
                    var::move_key.is_pressed = true;
                    var::move_key.key_type = GLFW_KEY_RIGHT;
                    num_down = GLFW_KEY_RIGHT;
                }
                else if (key == GLFW_KEY_LEFT) {
                    var::move_key.is_pressed = true;
                    var::move_key.key_type = GLFW_KEY_LEFT;
                    num_down = GLFW_KEY_LEFT;
                }
                else if (key == GLFW_KEY_F) {
                    var::player.jump();
                }
                else if (key == GLFW_KEY_D) {
                    var::attack_key.is_pressed = true;
                }
                else if (key == GLFW_KEY_R) {
                    if (!var::ult_controler.is_cool) {
                        var::ult_controler.start_cool();
                        var::ult_screen.ult_pressed();
                        var::cur_scene = ult_scene;
                    }
                }
                else if (key == GLFW_KEY_S) {
                    // var::player.is_ice_mode = true;
                }
                else if (key == GLFW_KEY_HOME || key == GLFW_KEY_END) {
                    if (!var::home_controler.is_cool) {
                        var::home_controler.start_cool();
                        var::engine->play2D(var::player_drinking_music);
                        var::player.health = var::player.max_health;
                    }
                }
                if (key == GLFW_KEY_A) {
                    var::player.teleport(num_down);
                }
            }

        }
        else if (action == GLFW_RELEASE) {
            if (key == GLFW_KEY_A) {
            }
            if (key == GLFW_KEY_RIGHT) {
                if (var::move_key.key_type != GLFW_KEY_RIGHT)
                    return;

                var::move_key.is_pressed = false;
                var::move_key.key_type = -1;
                num_down = -1;
            }
            else if (key == GLFW_KEY_S) {
                var::player.is_ice_mode = false;
            }
            else if (key == GLFW_KEY_LEFT) {
                if (var::move_key.key_type != GLFW_KEY_LEFT)
                    return;

                var::move_key.is_pressed = false;
                var::move_key.key_type = -1;
                num_down = -1;
            }
            else if (key == GLFW_KEY_D) {
                var::attack_key.is_pressed = false;
            }
        }
        break;
    }
    case title_scene:
    {
        if (action == GLFW_PRESS) {
            if (key == GLFW_KEY_Q)
                glfwSetWindowShouldClose(window, GL_TRUE);
            else if (key == GLFW_KEY_F1) {
                var::last_scene = title_scene;
                var::cur_scene = help_scene;
            }
        }

        break;
    }
    }

}


#endif
