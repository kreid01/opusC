class Pong
{
public:
    std::vector<std::unique_ptr<GameObject>> objects;

    Pong(Mesh mesh)
    {
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

        std::cout << objects.size() << std::endl;
    }

    void play(float deltaTime, bool p1_forward, bool p1_backwards, bool p2_forward, bool p2_backwards);

private:
    void reset_pos();
    void reset_score();
    void reset_game();
    void reset(bool player1);
    void check_wall_collision(std::unique_ptr<GameObject> &obj);
};