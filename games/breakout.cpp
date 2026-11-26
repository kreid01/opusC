#include <iostream>
#include <optional>
#include <vector>
#include <string>
#include <fstream>
#include <memory>
#include <utility>

#define GLFW_INCLUDE_NONE
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <meshes.h>

#include <imgui.h>
#include <imgui_impl_opengl3.h>

#include "miniaudio.h"
#include "object.h"
#include "breakout.h"

extern int shader_program;

void Breakout::play(float deltaTime)
{
    draw_objects(&objects, deltaTime, shader_program);
}