#pragma once

class shader_loader
{
public:
    static int create_shader_program(int vertext_shader, int fragment_shader)
    {
        int shader_program = glCreateProgram();

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

    static int get_shader(GLenum shaderType, std::string shaderFileSource)
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

private:
    static std::string load_shader_as_string(const std::string filename)
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
};