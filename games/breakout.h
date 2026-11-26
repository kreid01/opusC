
class Colours
{
public:
    static glm::vec4 red() { return {1.0f, 0.0f, 0.0f, 1.0f}; }
    static glm::vec4 orange() { return {1.0f, 0.5f, 0.0f, 1.0f}; }
    static glm::vec4 yellow() { return {1.0f, 1.0f, 0.0f, 1.0f}; }
    static glm::vec4 green() { return {0.0f, 1.0f, 0.0f, 1.0f}; }
    static glm::vec4 blue() { return {0.0f, 0.0f, 1.0f, 1.0f}; }
    static glm::vec4 indigo() { return {0.29f, 0.0f, 0.51f, 1.0f}; }
    static glm::vec4 violet() { return {0.56f, 0.0f, 1.0f, 1.0f}; }

    static glm::vec4 white() { return {1.0f, 1.0f, 1.0f, 1.0f}; }
    static glm::vec4 black() { return {0.0f, 0.0f, 0.0f, 1.0f}; }
};

class Breakout
{
public:
    std::vector<std::unique_ptr<GameObject>> objects;
    void play(float deltaTime);

    Breakout(Mesh mesh)
    {
        GameObject player = GameObject(
            glm::vec3(0.3f, 0.05f, 1.0f),
            glm::vec3(0.0f, -0.8f, 1.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            false,
            true,
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

        objects.push_back(std::make_unique<GameObject>(player));
        objects.push_back(std::make_unique<GameObject>(ball));

        bricks(18, mesh, 0, Colours::red());
        bricks(18, mesh, 3, Colours::orange());
        bricks(18, mesh, 1, Colours::green());
        bricks(18, mesh, 2, Colours::blue());
        bricks(18, mesh, 4, Colours::indigo());

        std::cout << objects.size() << std::endl;
    }

private:
    void bricks(int count, Mesh mesh, int layer, glm::vec4 colour)
    {
        for (float i = 0.0f; i < count; i++)
        {
            float offset = i / 9.5f;
            float layer_offset = 0.06f * layer;

            GameObject brick = GameObject(
                glm::vec3(0.1f, 0.05f, 1.0f),
                glm::vec3(-0.9 + offset, 0.9f - layer_offset, 1.0f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                false,
                true,
                false,
                mesh);

            brick.colour = colour;
            objects.push_back(std::make_unique<GameObject>(brick));
        }
    }
};