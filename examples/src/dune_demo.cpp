#include <array>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Image.hpp"
#include "Texture2D.hpp"
#include "Vertex.hpp"
#include "ShaderProgram.hpp"

void dune_demo(GLFWwindow* window, GLint scr_width, GLint scr_height)
{
    auto is_key_pressed = [window](int32_t key)
    {
        return glfwGetKey(window, key) == GLFW_PRESS;
    };

    auto cp = std::filesystem::current_path().string();

    Image imgMask;
    Image imgSand;
    Image imgSpace;
    Image imgRock;
    Image imgStone;

    if(!imgMask.loadFromFile("res/textures/mask.png"))   return;
    if(!imgSand.loadFromFile("res/textures/sand.jpg"))   return;
    if(!imgSpace.loadFromFile("res/textures/spice.jpg")) return;
    if(!imgRock.loadFromFile("res/textures/rock01.jpg")) return;
    if(!imgStone.loadFromFile("res/textures/cracked_earth.jpg")) return;

    Texture2D textures[5] = 
    {
        Texture2D(imgMask, GL_CLAMP_TO_BORDER, GL_LINEAR),
        Texture2D(imgSand, GL_REPEAT, GL_LINEAR),
        Texture2D(imgSpace, GL_REPEAT, GL_LINEAR),
        Texture2D(imgStone, GL_REPEAT, GL_LINEAR),
        Texture2D(imgRock, GL_REPEAT, GL_LINEAR)
    };

    float mapWidth  = static_cast<float>(imgMask.getWidth());
    float mapHeight = static_cast<float>(imgMask.getHeight());

    std::array<Vertex, 4> vertices = 
    {
        Vertex(glm::vec3(0.0f, 0.0f, 0.0f),      glm::vec2(0.0f, 0.0f)),
        Vertex(glm::vec3(1200.0f, 0.0f, 0.0f),   glm::vec2(1.0f, 0.0f)),
        Vertex(glm::vec3(1200.0f, 800.0f, 0.0f), glm::vec2(1.0f, 1.0f)),
        Vertex(glm::vec3(0.0f,    800.0f, 0.0f), glm::vec2(0.0f, 1.0f))
    };

    GLuint VAO, VBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const void*>(offsetof(Vertex, uv)));
    glEnableVertexAttribArray(1);
   
    glBindVertexArray(0);

    ShaderProgram program = { { "res/shaders/dune.vert", GL_VERTEX_SHADER }, { "res/shaders/dune.frag", GL_FRAGMENT_SHADER } };

    if (!program.isCompiled())
        return;

    program.bind(true);
    int32_t mvpLoc = program.getUniformLocation("MVP");

    glUniform1i(program.getUniformLocation("texMap"), 0);
    glUniform1i(program.getUniformLocation("texSand"), 1);
    glUniform1i(program.getUniformLocation("texSpice"), 2);
    glUniform1i(program.getUniformLocation("texStone"), 3);
    glUniform1i(program.getUniformLocation("texRock"), 4);

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(scr_width), 0.0f, static_cast<float>(scr_height), -1.0f, 1.0f);
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glm::vec2 pos = { 3, 5 };

    while (!glfwWindowShouldClose(window))
    {
        if(is_key_pressed(GLFW_KEY_ESCAPE)) 
        {
            glfwSetWindowShouldClose(window, true);
            continue;
        }

        glm::mat4 model_view = glm::identity<glm::mat4>();

        auto MVP = projection * model_view;
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(MVP));

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        for(size_t i = 0; i < std::size(textures); ++i)
        {
            Texture2D::enable(GL_TEXTURE0 + i);
            Texture2D::bind(&textures[i]);
        }
        
        glBindVertexArray(VAO);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glBindVertexArray(0);

        for (size_t i = 0; i < std::size(textures); ++i)
            textures[i].bind(nullptr);
        Texture2D::enable(GL_TEXTURE0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}