#include <iostream>
#include <optional>
#include <vector>
#include <string>
#include <fstream>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <meshes.h>
#include <shader.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "miniaudio.h"
#include "object.h"
#include "shader.h"
#include "pong.h"

std::vector<std::unique_ptr<GameObject>> objects;

unsigned int shader_program = 0;

double timeElapsed = glfwGetTime();
float deltaTime = 0;

ma_engine engine;

bool p1_forward = false;
bool p1_backwards = false;

bool p2_forward = false;
bool p2_backwards = false;

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        p1_forward = true;
        return;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        p1_backwards = true;
        return;
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        p2_forward = true;
        return;
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        p2_backwards = true;
        return;
    }

    p1_backwards = false;
    p1_forward = false;

    p2_backwards = false;
    p2_forward = false;
}

void clear()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

bool started = false;
void start()
{
    started = true;
}

void tick()
{
    double currentTime = glfwGetTime();
    deltaTime = currentTime - timeElapsed;
    timeElapsed = currentTime;

    if (deltaTime > 0.1)
        deltaTime = 0.1;

    clear();
    Pong pong;
    pong.play(deltaTime, p1_forward, p1_backwards, p2_forward, p2_backwards);
}

GLFWwindow *window;

void ui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void init_program()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    ui();
    ma_engine_init(NULL, &engine);
}

std::optional<Mesh> _mesh;

void vertex_specification()
{
    std::vector<float> triangle{
        -0.5f, 0.5f, 0.f,
        -0.5f, -0.f, 0.f,
        0.f, 0.f, 0.0f};

    std::vector<float> vertices{
        0.5f, 0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f};

    std::vector<unsigned int> indices{
        0, 1, 3,
        1, 2, 3};

    _mesh = Mesh{vertices, indices};
}

int create_graphics_pipeline()
{
    int vertex_shader = shader_loader::get_shader(GL_VERTEX_SHADER, "../shaders/vert.glsl");
    int fragment_shader = shader_loader::get_shader(GL_FRAGMENT_SHADER, "../shaders/frag.glsl");
    shader_program = shader_loader::create_shader_program(vertex_shader, fragment_shader);
    return shader_program;
}

void add_game_obj(std::unique_ptr<GameObject> obj)
{
    objects.push_back(std::move(obj));
}

void main_loop()
{
    Mesh mesh = _mesh.value();

    GameObject paddle = GameObject(
        glm::vec3(0.05f, 0.3f, 1.0f),
        glm::vec3(-0.8f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        false,
        true,
        false,
        mesh);
    GameObject paddle2 = GameObject(
        glm::vec3(0.05f, 0.3f, 1.0f),
        glm::vec3(0.8f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        false,
        true,
        false,
        mesh);

    GameObject centerLine = GameObject(
        glm::vec3(0.01f, 2.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        false,
        false,
        false,
        mesh);
    GameObject ball = GameObject(
        glm::vec3(0.03f, 0.04f, 1.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.5f, 1.f, 0.0f),
        true,
        true,
        true,
        mesh);

    objects.push_back(std::make_unique<GameObject>(ball));
    objects.push_back(std::make_unique<GameObject>(paddle));
    objects.push_back(std::make_unique<GameObject>(paddle2));
    objects.push_back(std::make_unique<GameObject>(centerLine));

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glfwSwapBuffers(window);
        glfwPollEvents();

        tick();
    }
}

void clean_up()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    ma_engine_uninit(&engine);
}

int main()
{
    init_program();
    vertex_specification();
    create_graphics_pipeline();
    main_loop();
    clean_up();

    return 0;
}
