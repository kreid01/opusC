#include <ranges>

#include "json.h"

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

        for (const BrickLayer &layer : load_level())
        {
            add_bricks(layer, mesh);
        }
    }

private:
    void export_level(std::vector<BrickLayer> brickLayers)
    {
        Json().export_bricks_json(brickLayers);
    }

    std::vector<BrickLayer> load_level()
    {
        return Json().load_bricks_level(1);
    }

    void add_bricks(BrickLayer brickLayer, Mesh mesh)
    {
        for (float i = 0.0f; i < 19; i++)
        {
            if (std::ranges::find(brickLayer.skipped, i) != brickLayer.skipped.end())
            {
                continue;
            }

            float offset = i / 9.5f;
            float layer_offset = 0.06f * brickLayer.layer;

            GameObject brick = GameObject(
                glm::vec3(0.1f, 0.05f, 1.0f),
                glm::vec3(-0.95 + offset, 0.95f - layer_offset, 1.0f),
                glm::vec3(0.0f, 0.0f, 0.0f),
                false,
                true,
                false,
                mesh);

            brick.health = brickLayer.health;
            brick.colour = brickLayer.colour;

            objects.push_back(std::make_unique<GameObject>(brick));
        }
    }
};