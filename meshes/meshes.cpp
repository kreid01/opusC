#include <glad/glad.h>
#include <meshes.h>

void Square::draw()
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

// void Triangle::draw()
// {
//     glBindVertexArray(VAO);
//     glDrawArrays(GL_TRIANGLES, 0, 3);
//     glBindVertexArray(0);
// }
