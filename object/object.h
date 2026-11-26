#include <meshes.h>

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

static void check_collision(GameObject &ball, GameObject &paddle)
{
    glm::vec4 ballDimensions = ball.get_dimensions();
    glm::vec4 paddleDimensions = paddle.get_dimensions();

    if (ballDimensions.y < paddleDimensions.x ||
        ballDimensions.x > paddleDimensions.y ||
        ballDimensions.w < paddleDimensions.z ||
        ballDimensions.z > paddleDimensions.w)
    {
        return;
    }

    if (ball._velocity.x > 0 && ball._position.x < paddle._position.x && ball._bounce)
    {
        ball._velocity.y *= -1;
        // play_boom();
    }
    else if (ball._velocity.x < 0 && ball._position.x > paddle._position.x && ball._bounce)
    {
        ball._velocity.y *= -1;
        // play_boom();
    }
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
        for (auto &object2 : *objects)
        {
            if (object2->_collision)
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
