#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <optional>
#include <vector>
#include <string>
#include <fstream>

class Mesh
{
public:
    unsigned int VAO = 0, VBO = 0, EBO = 0;
    GLsizei indexCount = 0;
    bool indexed = false;

    Mesh(const std::vector<float> &vertices,
         const std::optional<std::vector<unsigned int>> &indices)
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(
            GL_ARRAY_BUFFER,
            vertices.size() * sizeof(float),
            vertices.data(),
            GL_STATIC_DRAW);

        if (indices.has_value())
        {
            indexed = true;
            indexCount = static_cast<GLsizei>(indices->size());

            glGenBuffers(1, &EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

            glBufferData(
                GL_ELEMENT_ARRAY_BUFFER,
                indices->size() * sizeof(unsigned int),
                indices->data(),
                GL_STATIC_DRAW);
        }

        glVertexAttribPointer(
            0, 3, GL_FLOAT, GL_FALSE,
            3 * sizeof(float), nullptr);

        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
    }

    virtual void draw() = 0;

    virtual ~Mesh() = default;
};

class Square : public Mesh
{
public:
    Square(const std::vector<float> &vertices,
           const std::vector<unsigned int> &indices)
        : Mesh(vertices, indices) {}

    void draw() override
    {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }
};

class Triangle : public Mesh
{
public:
    Triangle(const std::vector<float> &vertices)
        : Mesh(vertices, std::nullopt) {}

    void draw() override
    {
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);
    }
};

std::vector<std::unique_ptr<Mesh>> meshes;

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
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

std::string load_shader_as_string(const std::string filename)
{
    std::string result = std::string();
    std::string line = std::string();
    std::ifstream shader(filename.c_str());

    if (shader.is_open())
    {
        while (std::getline(shader, line))
        {
            result += line + '\n';
        }

        shader.close();
    }

    return result;
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
    unsigned int shader_program;
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

int create_graphics_pipeline()
{
    int vertex_shader = get_shader(GL_VERTEX_SHADER, "../shaders/vert.glsl");
    int fragment_shader = get_shader(GL_FRAGMENT_SHADER, "../shaders/frag.glsl");
    int shader_program = create_shader_program(vertex_shader, fragment_shader);

    int scale_location = glGetUniformLocation(shader_program, "u_scale");
    glUniform1f(scale_location, 0.5f);

    return shader_program;
}

void draw()
{
    for (const auto &mesh : meshes)
    {
        mesh->draw();
    }
}

double timeElapsed = 0;

void tick()
{
    draw();
    double time = glfwGetTime();

    double deltaTime = time - timeElapsed;
    double fps = (1.0 / deltaTime);

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
