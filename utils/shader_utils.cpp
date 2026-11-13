#include <string>
#include <fstream>

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