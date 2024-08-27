#pragma once
#ifndef __WINDOW_H__
#define __WINDOW_H__
#include "cgmath.h"
#include "ut.h"
#include "namespace.h"


inline void reshape(GLFWwindow *window, const int width, const int height)
{
    var::window_size = ivec2(width, height);
    glViewport(0, 0, width, height);
}

void print_help()
{
    printf("|========================= [help] =========================|\n");
    printf("|- press 'q' to terminate the program                      |\n");
    printf("|- press F1 to see help                                    |\n");
    printf("|- click buttons to play game                              |\n");
    printf("|- put your mouse on the skill to see description          |\n");
    printf("|==========================================================|\n");
#ifndef GL_ES_VERSION_2_0
#endif
    printf("\n");
}


void user_finalize()
{
}

#endif
