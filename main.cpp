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

std::vector<std::unique_ptr<Mesh>> meshes;
unsigned int shader_program = 0;

double timeElapsed = glfwGetTime();
double deltaTime = 0;

float paddle_pos = 0.0f;
float velocity = 1.f;

float ball_pos = 0.0f;
float ball_velo = 1.f;

class GameObject
{
public:
    glm::vec3 _position;
    glm::vec3 _scale;
    bool _circle;

    GameObject(glm::vec3 scale, glm::vec3 position, bool circle)
    {
        _scale = scale;
        _position = position;
        _circle = circle;
    }
};

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

bool in_bounds(bool up)
{
    float topBound = 1.0f - 0.25f;
    float bottomBound = -1.0f + 0.25f;

    if (up)
    {
        return paddle_pos <= topBound;
    }
    else
    {
        return paddle_pos >= bottomBound;
    }
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        if (in_bounds(true))
        {
            paddle_pos += velocity * deltaTime;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        if (in_bounds(false))
        {
            paddle_pos -= velocity * deltaTime;
        }
    }
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

bool collision(glm::vec3 scale, glm::vec3 next_position)
{
    if (next_position.y > 1.0f - scale.y)
    {
        ball_velo = ball_velo * -1;
        return true;
    }

    if (next_position.x > 1.0f - scale.x)
    {
        ball_velo = ball_velo * -1;
        return true;
    }

    if (next_position.y < -1.0f + scale.y)
    {
        ball_velo = ball_velo * -1;
        return true;
    }

    if (next_position.x < -1.0f + scale.x)
    {
        ball_velo = ball_velo * -1;
        return true;
    }

    return false;
}

void draw()
{
    GameObject paddle = GameObject(glm::vec3(0.05f, 0.3f, 1.0f), glm::vec3(-0.8f, paddle_pos, 1.0f), false);
    GameObject paddle2 = GameObject(glm::vec3(0.05f, 0.3f, 1.0f), glm::vec3(0.8f, 0.0f, 1.0f), false);

    ball_pos += ball_velo * deltaTime;
    glm::vec3 ball_pos_vec = glm::vec3(ball_pos / 2, ball_pos / 3, 0.0f);

    collision(glm::vec3(0.03f, 0.04f, 1.0f), ball_pos_vec);

    GameObject centerLine = GameObject(glm::vec3(0.01f, 2.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), false);
    GameObject ball = GameObject(glm::vec3(0.03f, 0.04f, 1.0f), ball_pos_vec, true);

    std::vector<GameObject> objects = std::vector<GameObject>{paddle, paddle2, ball, centerLine};

    clear();

    for (const auto &object : objects)
    {
        int scale_location = glGetUniformLocation(shader_program, "scale");
        glUniform3f(scale_location, object._scale.r, object._scale.g, object._scale.b);

        int position = glGetUniformLocation(shader_program, "position");
        glUniform3f(position, object._position.r, object._position.g, object._position.b);

        int circle_location = glGetUniformLocation(shader_program, "circle");
        glUniform1i(circle_location, object._circle);

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

void main_loop()
{
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
    glfwTerminate();
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
