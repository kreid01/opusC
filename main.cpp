#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <optional>
#include <vector>
#include <string>
#include <fstream>
#include <glm/glm.hpp>

#include <meshes.h>
#include <shader_utils.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "miniaudio.h"

class GameObject
{
public:
    glm::vec3 _og_position;
    glm::vec3 _position;
    glm::vec3 _scale;
    glm::vec3 _velocity;
    bool _circle;
    bool _collision;
    bool _bounce;

    glm::vec4 get_dimensions()
    {
        glm::vec3 half_scale = _scale * 0.5f;

        return glm::vec4(
            _position.x - half_scale.x,
            _position.x + half_scale.x,
            _position.y - half_scale.y,
            _position.y + half_scale.y);
    }

    GameObject(glm::vec3 scale, glm::vec3 position, glm::vec3 velocity, bool circle, bool collision, bool bounce)
    {
        _scale = scale;
        _position = position;
        _velocity = velocity;
        _og_position = position;
        _circle = circle;
        _collision = collision;
        _bounce = bounce;
    }
};

std::vector<std::unique_ptr<Mesh>> meshes;
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

void printLn(std::string s)
{
    std::cout << s << std::endl;
}

void clear()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

int get_shader(GLenum shaderType, std::string shaderFileSource)
{
    std::string shaderSource = load_shader_as_string(shaderFileSource);

    unsigned int shader;
    shader = glCreateShader(shaderType);

    const char *source = shaderSource.c_str();

    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    return shader;
}

int create_shader_program(int vertext_shader, int fragment_shader)
{
    shader_program = glCreateProgram();

    glAttachShader(shader_program, vertext_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    int success;
    char infoLog[512];
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shader_program, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::LINK_FAILED\n"
                  << infoLog << std::endl;
    }

    glDeleteShader(vertext_shader);
    glDeleteShader(fragment_shader);

    glUseProgram(shader_program);

    return shader_program;
}

void play_boom()
{
    ma_engine_play_sound(&engine, "assets/boom.mp3", NULL);
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

int p1_score = 0;
int p2_score = 0;

void reset_pos()
{
    for (const auto &object : objects)
    {
        object.get()->_position = object->_og_position;
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

    if (dimensions.z + (obj->_velocity.y * deltaTime) <= -1.0f || dimensions.w + (obj->_velocity.y * deltaTime) >= 1.0f)
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

void draw_button(std::string name, std::function<void()> callback)
{
    ImGui::Begin("Button", nullptr, ImGuiWindowFlags_NoDecoration);
    if (ImGui::Button(name.c_str()))
    {
        callback();
    }

    ImGui::End();
}

void draw_imgui()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    draw_button("Reset", reset_game);
}

bool started = false;
void start()
{
    started = true;
}

void draw()
{
    clear();
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

    if (p1_forward == true)
    {
        paddle->_velocity.y = 1.0f;
    }
    else if (p1_backwards == true)
    {
        paddle->_velocity.y = -1.0f;
    }
    else
    {
        paddle->_velocity.y = 0;
    }

    if (p2_forward == true)
    {
        paddle2->_velocity.y = 1.0f;
    }
    else if (p2_backwards == true)
    {
        paddle2->_velocity.y = -1.0f;
    }
    else
    {
        paddle2->_velocity.y = 0;
    }

    for (auto &object : objects)
    {
        check_wall_collision(object);
        object->_position += object->_velocity * deltaTime;

        int scale_location = glGetUniformLocation(shader_program, "scale");
        glUniform3f(scale_location, object->_scale.r, object->_scale.g, object->_scale.b);

        int position = glGetUniformLocation(shader_program, "position");
        glUniform3f(position, object->_position.r, object->_position.g, object->_position.b);

        int circle_location = glGetUniformLocation(shader_program, "circle");
        glUniform1i(circle_location, object->_circle);

        meshes[0]->draw();
    }
}

void tick()
{
    double currentTime = glfwGetTime();
    deltaTime = currentTime - timeElapsed;
    timeElapsed = currentTime;

    if (deltaTime > 0.1)
        deltaTime = 0.1;

    draw();
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

    meshes.push_back(std::make_unique<Square>(vertices, indices));
}

int create_graphics_pipeline()
{
    int vertex_shader = get_shader(GL_VERTEX_SHADER, "../shaders/vert.glsl");
    int fragment_shader = get_shader(GL_FRAGMENT_SHADER, "../shaders/frag.glsl");
    int shader_program = create_shader_program(vertex_shader, fragment_shader);

    return shader_program;
}

void add_game_obj(std::unique_ptr<GameObject> obj)
{
    objects.push_back(std::move(obj));
}

void main_loop()
{
    GameObject paddle = GameObject(
        glm::vec3(0.05f, 0.3f, 1.0f),
        glm::vec3(-0.8f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        false,
        true,
        false);
    GameObject paddle2 = GameObject(
        glm::vec3(0.05f, 0.3f, 1.0f),
        glm::vec3(0.8f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        false,
        true,
        false);

    GameObject centerLine = GameObject(
        glm::vec3(0.01f, 2.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        false,
        false,
        false);
    GameObject ball = GameObject(
        glm::vec3(0.03f, 0.04f, 1.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.5f, 1.f, 0.0f),
        true,
        true,
        true);

    add_game_obj(std::make_unique<GameObject>(ball));
    add_game_obj(std::make_unique<GameObject>(paddle));
    add_game_obj(std::make_unique<GameObject>(paddle2));
    add_game_obj(std::make_unique<GameObject>(centerLine));

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
