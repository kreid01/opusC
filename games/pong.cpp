#include <iostream>
#include <optional>
#include <vector>
#include <string>
#include <fstream>
#include <memory>

#define GLFW_INCLUDE_NONE

#include <glm/glm.hpp>
#include <meshes.h>

#include <imgui.h>

#include "miniaudio.h"
#include "object.h"
#include "pong.h"

extern ma_engine engine;
extern int shader_program;
extern float deltaTime;

void render_score(int player, int score, int posX)
{
    ImGui::SetNextWindowPos(ImVec2(posX, 30));
    ImGui::SetNextWindowSize(ImVec2(200, 60));

    ImGui::Begin(("Player " + std::to_string(player)).c_str(),
                 nullptr,
                 ImGuiWindowFlags_NoDecoration |
                     ImGuiWindowFlags_NoBackground |
                     ImGuiWindowFlags_NoInputs);

    ImGui::SetWindowFontScale(2.0f);
    ImGui::Text("Player %d: %d", player, score);
    ImGui::End();
}

int p1_score = 0;
int p2_score = 0;

void Pong::reset_pos()
{
    for (const auto &object : objects)
    {
        object->_position = object->_og_position;
    }
}

void Pong::reset_score()
{
    p1_score = 0;
    p2_score = 0;
}

void Pong::reset_game()
{
    reset_pos();
    reset_score();
}

void Pong::reset(bool player1)
{
    if (player1)
    {
        p1_score += 1;
    }
    else
    {
        p2_score += 1;
    }

    reset_pos();
}

void Pong::play(float deltaTime, bool p1_forward, bool p1_backwards,
                bool p2_forward, bool p2_backwards)
{
    ImGui::Begin("Options");
    if (ImGui::Button("Reset"))
    {
        reset_game();
    }
    ImGui::End();

    render_score(1, p1_score, 200);
    render_score(2, p2_score, 420);

    std::unique_ptr<GameObject> &paddle = objects.at(1);
    std::unique_ptr<GameObject> &paddle2 = objects.at(2);

    if (p1_forward)
        paddle->_velocity.y = 1.0f;
    else if (p1_backwards)
        paddle->_velocity.y = -1.0f;
    else
        paddle->_velocity.y = 0;

    if (p2_forward)
        paddle2->_velocity.y = 1.0f;
    else if (p2_backwards)
        paddle2->_velocity.y = -1.0f;
    else
        paddle2->_velocity.y = 0;

    draw_objects(&objects, deltaTime, shader_program);
}
