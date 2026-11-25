#include <iostream>
#include <optional>
#include <vector>
#include <string>
#include <fstream>
#include <memory>
#include <utility>

#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <meshes.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "miniaudio.h"
#include "object.h"
#include "pong.h"

extern ma_engine engine;
extern std::vector<std::unique_ptr<GameObject>> objects;
extern int shader_program;
extern float deltaTime;

void play_boom()
{
    ma_engine_play_sound(&engine, "assets/boom.mp3", NULL);
}

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

void check_collision(GameObject &ball, GameObject &paddle)
{
    glm::vec4 ballDimensions = ball.get_dimensions();
    glm::vec4 paddleDimensions = paddle.get_dimensions();

    if (ballDimensions.y < paddleDimensions.x ||
        ballDimensions.x > paddleDimensions.y ||
        ballDimensions.w < paddleDimensions.z ||
        ballDimensions.z > paddleDimensions.w)
    {
        return;
    }

    if (ball._velocity.x > 0 && ball._position.x < paddle._position.x)
    {
        ball._velocity.x *= -1;
        play_boom();
    }
    else if (ball._velocity.x < 0 && ball._position.x > paddle._position.x)
    {
        ball._velocity.x *= -1;
        play_boom();
    }
}

void draw_button(std::string name, std::function<void()> callback)
{
    ImGui::Begin("Button", nullptr, ImGuiWindowFlags_NoDecoration);
    if (ImGui::Button(name.c_str()))
    {
        callback();
    }

    ImGui::End();
}

int p1_score = 0;
int p2_score = 0;

void reset_pos()
{
    for (const auto &object : objects)
    {
        object->_position = object->_og_position;
    }
}

void reset_score()
{
    p1_score = 0;
    p2_score = 0;
}

void reset_game()
{
    reset_pos();
    reset_score();
}

void reset(bool player1)
{
    if (player1)
    {
        p1_score += 1;
        play_boom();
    }
    else
    {
        p2_score += 1;
        play_boom();
    }

    reset_pos();
}

void draw_imgui()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    draw_button("Reset", reset_game);
}

void check_wall_collision(std::unique_ptr<GameObject> &obj)
{
    glm::vec4 dimensions = obj->get_dimensions();

    if (dimensions.x <= -1.0f || dimensions.y >= 1.0f)
    {
        if (obj->_bounce)
        {
            reset(obj->_velocity.x > 0.0f);
            return;
        }
    }

    if (dimensions.z + (obj->_velocity.y * deltaTime) <= -1.0f ||
        dimensions.w + (obj->_velocity.y * deltaTime) >= 1.0f)
    {
        if (obj->_bounce)
        {
            obj->_velocity.y *= -1.0f;
        }
        else
        {
            obj->_velocity.y = 0.0f;
        }
    }
}

void Pong::play(float deltaTime, bool p1_forward, bool p1_backwards,
                bool p2_forward, bool p2_backwards)
{
    draw_imgui();

    render_score(1, p1_score, 200);
    render_score(2, p2_score, 420);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    std::unique_ptr<GameObject> &ball = objects.at(0);
    std::unique_ptr<GameObject> &paddle = objects.at(1);
    std::unique_ptr<GameObject> &paddle2 = objects.at(2);

    check_collision(*ball, *paddle2);
    check_collision(*ball, *paddle);

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

    for (auto &object : objects)
    {
        check_wall_collision(object);
        object->_position += object->_velocity * deltaTime;

        int scale_location = glGetUniformLocation(shader_program, "scale");
        glUniform3f(scale_location, object->_scale.r,
                    object->_scale.g, object->_scale.b);

        int position = glGetUniformLocation(shader_program, "position");
        glUniform3f(position, object->_position.r,
                    object->_position.g, object->_position.b);

        int circle_location = glGetUniformLocation(shader_program, "circle");
        glUniform1i(circle_location, object->_circle);

        object->_mesh.draw();
    }
}
