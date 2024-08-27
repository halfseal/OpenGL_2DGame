#include "gl/glad/glad.h"	// https://github.com/Dav1dde/glad
#define GLFW_INCLUDE_NONE
#include "gl/glfw/glfw3.h"	// http://www.glfw.org
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "cgmath.h"
#include "ut.h"
#include "program.h"
#include "vertex_array.h"
#include "window.h"
#include "keyboard_mouse.h"
#include "namespace.h"
#include "rect.h"
#include "damage.h"
#include <windows.h>
#include <shellscalingapi.h>

#pragma comment(lib, "Shcore.lib")


int frame = 0;
float t = 0.0f, t_temp = 0.0f;
#ifndef GL_ES_VERSION_2_0
bool is_rotating = true;
#endif

bool init_text();

void GetDPI(GLFWwindow *window) {
    HDC screen = GetDC(0);
    int dpiX = GetDeviceCaps(screen, LOGPIXELSX);
    int dpiY = GetDeviceCaps(screen, LOGPIXELSY);
    ReleaseDC(0, screen);

    printf("Default DPI: %d x %d\n", dpiX, dpiY);
}

void update(double delta_time) {
    var::theta = static_cast<float>(glfwGetTime());
    var::changeable_aspect = var::window_size.x / float(var::window_size.y);
    mat4 aspect_matrix =
    {min(1.0f / var::changeable_aspect, 1.0f / var::aspect), 0, 0, 0,
     0, min(var::changeable_aspect / var::aspect, 1.0f), 0, 0,
     0, 0, 1, 0,
     0, 0, 0, 1
    };

    mat4 diag_matrix = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };

    switch (var::cur_scene) {

    case help_scene:
    {
        var::help_screen.update(delta_time);
        break;
    }
    case lose_scene:
    {
        var::lose_screen.update(delta_time);
        break;
    }
    case win_scene:
    {
        var::win_screen.update(delta_time);
        break;
    }
    case pause_scene:
    {
        break;
    }
    case ult_scene:
    {
        var::ult_screen.update(var::player, var::c_attack, delta_time);
    }
    case battle_scene:
    {
        if (var::stopper.is_stopped) {
            var::stopper.update(delta_time);
            break;
        }

        if (var::opponent.health <= 0) {
            if (!var::player_star.is_added_) {
                int divisor;

                auto diff = var::opponent.difficulty; // diff == 0.1 : hard, diff == 0.2 : easy
                if (diff < 0.15)
                    divisor = 1;
                else
                    divisor = 3;

                auto rand_num = rand() % divisor;
                if (rand_num == 0) {
                    var::player_star.add_new_star();
                    var::is_new_star_added = true;
                }
            }

            var::cur_scene = win_scene;
            break;
        }

        if (var::player.health <= 0) {
            var::cur_scene = lose_scene;
            break;
        }

        var::home_controler.update(delta_time);
        var::ult_controler.update(delta_time);

        var::poop.update(delta_time);


        var::player.move_character(var::move_key, delta_time);
        var::player.avoid_floor_collision(var::floor, delta_time);
        var::player.update_color(delta_time);
        var::player.hit_poop(var::poop);

        var::player_hp_bar.update(var::player.get_scale_vec(), var::player.get_trans_vec(),
                                  (float)var::player.health / (float)var::player.max_health);

        var::player_star.update(var::player.get_scale_vec(), var::player.get_trans_vec());

        var::op_ratio = (float)var::opponent.health / (float)var::opponent.max_health;
        var::bad_guy_hp_bar.update(var::op_ratio);

        var::skill_screen.update(var::home_controler, var::ult_controler, delta_time);

        var::c_attack.update(delta_time, var::opponent);
        var::d_attack.update(delta_time, var::opponent, var::c_attack);

        var::opponent.update(var::player, delta_time);
        update_text(delta_time);

        var::particle_set2.update(delta_time, var::opponent.get_trans_vec(), white, red_red, dark_light_red,
                                  light_light_red);

        break;
    }
    case title_scene:
    {
        var::title_screen.update(delta_time);
        break;
    }
    default:
        break;
    }


    var::program.bind();
    var::program.set_uniform<decltype(glUniformMatrix4fv), decltype(aspect_matrix)>(
        glUniformMatrix4fv, "aspect_matrix", 1, GL_TRUE, aspect_matrix);
    var::program.set_uniform<decltype(glUniformMatrix4fv), mat4>(
        glUniformMatrix4fv, "view_matrix", 1, GL_TRUE, mat4::look_at(var::cam.eye, var::cam.at, var::cam.up));

    var::program2.bind();
    var::program2.set_uniform<decltype(glUniformMatrix4fv), decltype(aspect_matrix)>(
        glUniformMatrix4fv, "aspect_matrix", 1, GL_TRUE, aspect_matrix);
    var::program2.set_uniform<decltype(glUniformMatrix4fv), mat4>(
        glUniformMatrix4fv, "view_matrix", 1, GL_TRUE, diag_matrix);
}

// float tmp = 0.0f;

void render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    var::vao.bind();

    const scene scene_to_show = (var::cur_scene == help_scene) ? var::last_scene : var::cur_scene;
    switch (scene_to_show) {
    case ult_scene:
    case pause_scene:
    case lose_scene:
    case win_scene:
    case battle_scene:
    {
        var::program.bind();
        // var::vao.bind();


        var::background.draw();
        var::floor.draw();

        var::poop.draw();

        var::particle_set2.draw(var::program, var::opponent.get_trans_vec(), var::rect_indices.size());

        var::opponent.draw();

        var::player.draw();
        var::player_hp_bar.draw();
        var::player_star.draw();
        var::c_attack.draw();
        var::d_attack.draw();

        var::program2.bind();
        var::bad_guy_hp_bar.draw();
        var::skill_screen.draw();

        render_texts(var::window_size, var::changeable_aspect, var::aspect);
        var::vao.bind();

        if (var::cur_scene == pause_scene) {
            var::program2.bind();
            var::pause_screen.draw();

            render_text(var::window_size, 0.0f, "stars : " + std::to_string(var::player.stars), var::window_size.x / 2,
                        var::window_size.y / 2, 1.0f,
                        vec4(1.0f, 1.0f, 1.0f, 1.0f));

            var::vao.bind();
        }
        else if (var::cur_scene == lose_scene) {
            var::program2.bind();
            var::lose_screen.draw();
            render_text(var::window_size, 0.0f, "you lose", var::window_size.x / 2, var::window_size.y / 4, 1.0f,
                        vec4(1.0f, 1.0f, 1.0f, 1.0f));

        }
        else if (var::cur_scene == win_scene) {
            var::program2.bind();
            var::win_screen.draw();


            render_text(var::window_size, 0.0f, "you win", var::window_size.x / 2, var::window_size.y / 4, 1.0f,
                        vec4(1.0f, 1.0f, 1.0f, 1.0f));

            if (var::is_new_star_added) {
                render_text(var::window_size, 0.0f, "you got new star!", var::window_size.x / 2, var::window_size.y / 3,
                            1.0f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
            }
            else {
                render_text(var::window_size, 0.0f, "no star this time...", var::window_size.x / 2,
                            var::window_size.y / 3,
                            1.0f, vec4(1.0f, 1.0f, 1.0f, 1.0f));
            }
        }
        else if (var::cur_scene == ult_scene) {
            var::program2.bind();
            var::ult_screen.draw();
        }


        if (var::stopper.is_stopped) {
            if (0 <= var::skill_type && var::skill_type <= 4) {
                var::program.bind();
                var::still_descriptor.draw(var::skill_type);
            }
        }
        var::sound_control.draw();

        break;
    }
    case title_scene:
    {
        var::program2.bind();
        var::title_screen.draw();
        var::sound_control.draw();

        break;
    }
    }

    if (var::cur_scene == help_scene) {
        var::program2.bind();
        var::help_screen.draw();
    }

    glfwSwapBuffers(var::window);
}


void update_vertex_buffer() {
    if (var::vao.set_vbo(var::rect_vertexes))
        return;

    var::vao.set_ibo(var::rect_indices);
    var::vao.set_vao();
}


bool user_init() {
    print_help();

    glLineWidth(1.0f);
    glClearColor(39 / 255.0f, 40 / 255.0f, 34 / 255.0f, 1.0f);
    glEnable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    var::rect_vertexes =
    {
        {vec3(1.0f, 1.0f, 0.0f), vec3(0, 0, -1.0f), vec2(1.0f, 1.0f)},
        {vec3(-1.0f, 1.0f, 0.0f), vec3(0, 0, -1.0f), vec2(0.0f, 1.0f)},
        {vec3(-1.0f, -1.0f, 0.0f), vec3(0, 0, -1.0f), vec2(0.0f, 0.0f)},
        {vec3(1.0f, -1.0f, 0.0f), vec3(0, 0, -1.0f), vec2(1.0f, 0.0f)}
    };

    var::rect_indices =
    {
        0, 1, 2,
        2, 3, 0
    };

    var::engine = irrklang::createIrrKlangDevice();
    if (!var::engine)
        return false;
    var::bgm = var::engine->addSoundSourceFromFile("../bin/musics/bgm.wav");
    // music from https://blog.naver.com/gbamaster/221563076208

    var::player_jump_music = var::engine->addSoundSourceFromFile("../bin/musics/player_jump.wav");
    var::player_drinking_music = var::engine->addSoundSourceFromFile("../bin/musics/player_drinking.wav");
    var::player_attack_music = var::engine->addSoundSourceFromFile("../bin/musics/player_attack.wav");
    var::player_hit_music = var::engine->addSoundSourceFromFile("../bin/musics/player_hit.wav");
    var::player_c_attack_music = var::engine->addSoundSourceFromFile("../bin/musics/player_card_attack.wav");
    var::player_teleport_music = var::engine->addSoundSourceFromFile("../bin/musics/player_teleport.wav");
    var::enemy_attack1_music = var::engine->addSoundSourceFromFile("../bin/musics/bad_guy_attack1.wav");
    var::enemy_attack2_music = var::engine->addSoundSourceFromFile("../bin/musics/bad_guy_attack2.wav");
    var::enemy_dash1_music = var::engine->addSoundSourceFromFile("../bin/musics/bad_guy_dash_go.wav");
    var::enemy_dash2_music = var::engine->addSoundSourceFromFile("../bin/musics/bad_guy_dash_wait.wav");
    var::enemy_jump1_music = var::engine->addSoundSourceFromFile("../bin/musics/bad_guy_flying.wav");
    var::enemy_jump2_music = var::engine->addSoundSourceFromFile("../bin/musics/bad_guy_diving1.wav");
    var::enemy_jump3_music = var::engine->addSoundSourceFromFile("../bin/musics/bad_guy_diving2.wav");
    var::poop_gone_music = var::engine->addSoundSourceFromFile("../bin/musics/poop_gone.wav");

    var::player_hit_music->setDefaultVolume(0.5f);
    var::player_attack_music->setDefaultVolume(0.2f);
    var::player_c_attack_music->setDefaultVolume(0.3f);
    var::enemy_attack1_music->setDefaultVolume(0.1f);
    var::enemy_dash2_music->setDefaultVolume(0.3f);
    var::poop_gone_music->setDefaultVolume(0.7f);

    var::bgm->setDefaultVolume(0.1f);
    var::engine->play2D(var::bgm, true);

    var::title_screen.initialize();
    var::pause_screen.initialize();
    var::lose_screen.initialize();
    var::win_screen.initialize();
    var::help_screen.initialize();
    var::ult_screen.initialize();

    var::skill_screen.initialize();
    var::skill_screen.initialize_texture();

    var::still_descriptor.initialize();

    var::sound_control.initialize();

    initialize_game();
    var::opponent.initialize_texture();
    var::player.initialize_texture();
    var::floor.init_tex("../bin/images/bar.png");
    var::background.init_tex("../bin/images/background.png");

    if (!init_text())
        return false;

    update_vertex_buffer();
    return true;
}


int main(int argc, char *argv[]) {
    srand((unsigned int)time(NULL));
    var::window = create_window(var::window_name, var::window_size.x, var::window_size.y);
    
    GetDPI(var::window);

    if (!user_init()) {
        printf("Failed to user_init()\n");
        glfwTerminate();
        return 1;
    } // user initialization

    glfwSetWindowSizeCallback(var::window, reshape);
    glfwSetKeyCallback(var::window, keyboard);
    glfwSetMouseButtonCallback(var::window, mouse);
    glfwSetCursorPosCallback(var::window, motion);

    var::program.init(var::vert_shader_path, var::frag_shader_path);
    var::program2.init(var::vert_shader_path, var::frag_shader_path);

    const double fps_limit = 1 / 60.0;
    double last_update = 0.0, last_frame = 0.0;

    // enters rendering/event loop
    for (frame = 0; !glfwWindowShouldClose(var::window); frame++) {
        double now = glfwGetTime();
        double delta_time = now - last_update;

        glfwPollEvents(); // polling and processing of events

        // if (var::stopper.is_stopped)
        // var::stopper.update(delta_time);
        // else
        update(delta_time); // per-frame update

        last_update = now;

        if (now - last_frame >= fps_limit) {
            render(); // per-frame render
            last_frame = now;
        }
    }

    // normal termination
    user_finalize();

	glfwDestroyWindow(var::window);
	glfwTerminate();

    return 0;
}
