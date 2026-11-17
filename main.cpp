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
float velocity = 0.00f;

class GameObject
{
public:
    glm::vec3 _position;
    glm::vec3 _scale;

    GameObject(glm::vec3 scale, glm::vec3 position)
    {
        _scale = scale;
        _position = position;
    }
};

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        velocity += 0.01f;
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

void draw()
{
    GameObject paddle = GameObject(glm::vec3(0.1f, 0.5f, 1.0f), glm::vec3(-0.8f, 0.0f + velocity, 1.0f));
    GameObject paddle2 = GameObject(glm::vec3(0.1f, 0.5f, 1.0f), glm::vec3(0.8f, 0.0f, 1.0f));
    std::vector<GameObject> objects = std::vector<GameObject>{paddle, paddle2};

    clear();

    for (const auto &object : objects)
    {
        int scale_location = glGetUniformLocation(shader_program, "scale");
        glUniform3f(scale_location, object._scale.r, object._scale.g, object._scale.b);

        int position = glGetUniformLocation(shader_program, "position");
        glUniform3f(position, object._position.r, object._position.g, object._position.b);

        meshes[0]->draw();
    }
}

double timeElapsed = 0;

void tick()
{
    draw();
    double time = glfwGetTime();

    double deltaTime = time - timeElapsed;

    // double fps = (1.0 / deltaTime);
    // std::cout << fps << std::endl;

    timeElapsed = time;
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
