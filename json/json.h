#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

struct BrickLayer
{
    int layer;
    int health;
    glm::vec4 colour;
    std::vector<int> skipped;
};

class Json
{
public:
    static void export_bricks_json(std::vector<BrickLayer> brickLayer)
    {
        json data;

        for (const BrickLayer &brick : brickLayer)
        {
            data["level_1"].push_back({{"layer", brick.layer},
                                       {"health", brick.health},
                                       {"colour", {brick.colour.r, brick.colour.g, brick.colour.b, brick.colour.a}},
                                       {"skipped", brick.skipped}});

            data["level_2"].push_back({{"layer", brick.layer},
                                       {"health", brick.health},
                                       {"colour", {brick.colour.r, brick.colour.g, brick.colour.b, brick.colour.a}},
                                       {"skipped", brick.skipped}});
        }

        std::ofstream file("breakout.json");
        file << data.dump(4);
    }

    static std::vector<BrickLayer> load_bricks_level(int level)
    {
        std::ifstream file("breakout.json");

        if (!file.is_open())
            throw std::runtime_error("Could not open breakout.json");

        json data = json::parse(file);

        std::vector<BrickLayer> bricks;
        std::string level_string = "level_" + std::to_string(level);

        for (const auto &brick : data[level_string])
        {
            BrickLayer layer;

            layer.layer = brick["layer"];
            layer.health = brick["health"];

            layer.colour = glm::vec4(
                brick["colour"][0],
                brick["colour"][1],
                brick["colour"][2],
                brick["colour"][3]);

            layer.skipped = brick["skipped"].get<std::vector<int>>();

            bricks.push_back(layer);
        }

        return bricks;
    }
};
