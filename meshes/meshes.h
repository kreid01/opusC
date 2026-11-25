#include <iostream>
#include <glad/glad.h>
#include <optional>
#include <vector>
#include <fstream>

#pragma once

class Mesh
{
public:
    unsigned int VAO = 0, VBO = 0, EBO = 0;
    GLsizei indexCount = 0;
    bool indexed = false;

    Mesh(const std::vector<float> &vertices,
         const std::optional<std::vector<unsigned int>> &indices)
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(
            GL_ARRAY_BUFFER,
            vertices.size() * sizeof(float),
            vertices.data(),
            GL_STATIC_DRAW);

        if (indices.has_value())
        {
            indexed = true;
            indexCount = static_cast<GLsizei>(indices->size());

            glGenBuffers(1, &EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

            glBufferData(
                GL_ELEMENT_ARRAY_BUFFER,
                indices->size() * sizeof(unsigned int),
                indices->data(),
                GL_STATIC_DRAW);
        }

        glVertexAttribPointer(
            0, 3, GL_FLOAT, GL_FALSE,
            3 * sizeof(float), nullptr);

        glEnableVertexAttribArray(0);
        glBindVertexArray(0);
    }

    void draw()
    {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }
};
