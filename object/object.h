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

    glm::vec4
    get_dimensions()
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
