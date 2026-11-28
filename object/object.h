#include <meshes.h>
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
    Mesh _mesh;

    std::optional<glm::vec4> colour;
    std::optional<int> health;

    glm::vec4 get_dimensions()
    {
        glm::vec3 half_scale = _scale * 0.5f;

        return glm::vec4(
            _position.x - half_scale.x,
            _position.x + half_scale.x,
            _position.y - half_scale.y,
            _position.y + half_scale.y);
    }

    GameObject(glm::vec3 scale, glm::vec3 position,
               glm::vec3 velocity, bool circle, bool collision,
               bool bounce, Mesh mesh)
        : _scale(scale),
          _position(position),
          _og_position(position),
          _velocity(velocity),
          _circle(circle),
          _collision(collision),
          _bounce(bounce),
          _mesh(std::move(mesh))
    {
    }
};

// void play_boom()
// {
//     ma_engine_play_sound(&engine, "assets/boom.mp3", NULL);
// }

static bool check_collision(GameObject &ball, GameObject &brick)
{
    glm::vec4 ballDimensions = ball.get_dimensions();
    glm::vec4 brickDimensions = brick.get_dimensions();

    if (ball._bounce == false || brick.health == 0)
        return false;

    // Check AABB intersection
    if (ballDimensions.y < brickDimensions.x ||
        ballDimensions.x > brickDimensions.y ||
        ballDimensions.w < brickDimensions.z ||
        ballDimensions.z > brickDimensions.w)
    {
        return false;
    }

    // Calculate penetration
    float overlapX = std::min(ballDimensions.y, brickDimensions.y) -
                     std::max(ballDimensions.x, brickDimensions.x);

    float overlapY = std::min(ballDimensions.w, brickDimensions.w) -
                     std::max(ballDimensions.z, brickDimensions.z);

    if (brick.health.has_value())
    {
        brick.health.value() -= 1;
    }

    if (overlapX < overlapY)
    {
        ball._velocity.x *= -1.0f;

        // Push ball out horizontally
        float direction = ball._position.x < brick._position.x ? -1.0f : 1.0f;
        ball._position.x += direction * overlapX;
    }
    else
    {
        ball._velocity.y *= -1.0f;

        // Push ball out vertically
        float direction = ball._position.y < brick._position.y ? -1.0f : 1.0f;
        ball._position.y += direction * overlapY;
    }

    return true;
}

static void check_wall_collision(std::unique_ptr<GameObject> &obj, float deltaTime)
{
    glm::vec4 dimensions = obj->get_dimensions();

    if (dimensions.x + (obj->_velocity.x * deltaTime) <= -1.0f ||
        dimensions.y + (obj->_velocity.x * deltaTime) >= 1.0f)
    {
        if (obj->_bounce)
        {
            obj->_velocity.x *= -1.0f;
            // reset(obj->_velocity.x > 0.0f);
            return;
        }
    }

    if (dimensions.z + (obj->_velocity.y * deltaTime) <= -1.0f ||
        dimensions.w + (obj->_velocity.y * deltaTime) >= 1.0f)
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

static void draw_objects(std::vector<std::unique_ptr<GameObject>> *objects, float deltaTime, int shader_program)
{
    for (auto &object : *objects)
    {
        if (object->health.has_value() && object->health.value() == 0)
        {
            continue;
        }

        for (auto &object2 : *objects)
        {
            if (object2->_collision && object != object2)
            {
                check_collision(*object, *object2);
            }
        }

        check_wall_collision(object, deltaTime);
        object->_position += object->_velocity * deltaTime;

        int scale_location = glGetUniformLocation(shader_program, "scale");
        glUniform3f(scale_location, object->_scale.r,
                    object->_scale.g, object->_scale.b);

        int position = glGetUniformLocation(shader_program, "position");
        glUniform3f(position, object->_position.r,
                    object->_position.g, object->_position.b);

        int colour = glGetUniformLocation(shader_program, "colour");
        if (object.get()->colour.has_value())
        {
            glm::vec4 objColour = object->colour.value();
            glUniform4f(colour, objColour.r, objColour.g, objColour.b, objColour.a);
        }
        else
        {
            glUniform4f(colour, 1.0f, 1.0f, 1.0f, 1.0f);
        }

        int circle_location = glGetUniformLocation(shader_program, "circle");
        glUniform1i(circle_location, object->_circle);

        object->_mesh.draw();
    }
}
