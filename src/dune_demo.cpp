#include <array>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Image.hpp"
#include "Texture.hpp"
#include "Vertex.hpp"
#include "ShaderProgram.hpp"

static float lastX = 1200 / 2.0f;
static float lastY = 800 / 2.0f;
static float pitch = 20;
static float yaw = 180;

static void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    GLfloat sensitivity = 0.05f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   -= xoffset;
    pitch += yoffset;

    if(pitch > 89.0f)
    pitch =  89.0f;
    if(pitch < -89.0f)
    pitch = -89.0f;
}

void dune_demo(GLFWwindow* window, GLint scr_width, GLint scr_height)
{
    glfwSetCursorPosCallback(window, mouse_callback);

    auto is_key_pressed = [window](int32_t key)
    {
        return glfwGetKey(window, key) == GLFW_PRESS;
    };

    Image imgMask;
    Image imgSand;
    Image imgSpace;
    Image imgRock;
    Image imgStone;

    if(!imgMask.loadFromFile("res/textures/mask.png"))   return;
    if(!imgSand.loadFromFile("res/textures/sand.jpg"))   return;
    if(!imgSpace.loadFromFile("res/textures/spice.jpg")) return;
    if(!imgRock.loadFromFile("res/textures/rock.jpg"))   return;
    if(!imgStone.loadFromFile("res/textures/stone.jpg")) return;

    GLuint textures[5];

    textures[0]= Texture().createFromImage(imgMask, GL_CLAMP_TO_BORDER, GL_LINEAR);
    textures[1]= Texture().createFromImage(imgSand, GL_REPEAT, GL_LINEAR);
    textures[2]= Texture().createFromImage(imgSpace, GL_REPEAT, GL_LINEAR);
    textures[3]= Texture().createFromImage(imgStone, GL_REPEAT, GL_LINEAR);
    textures[4]= Texture().createFromImage(imgRock, GL_REPEAT, GL_LINEAR);

    float mapWidth  = static_cast<float>(imgMask.getWidth());
    float mapHeight = static_cast<float>(imgMask.getHeight());

    std::array<Vertex, 4> vertices = 
    {
        Vertex(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)),
        Vertex(glm::vec3(1200.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)),
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

    GLuint shader = ShaderProgram().compile("res/shaders/dune.vert", "res/shaders/dune.frag");
    glUseProgram(shader);
    int32_t mvpLoc = glGetUniformLocation(shader, "MVP");

    glUniform1i(glGetUniformLocation(shader, "texMap"), 0);
    glUniform1i(glGetUniformLocation(shader, "texSand"), 1);
    glUniform1i(glGetUniformLocation(shader, "texSpice"), 2);
    glUniform1i(glGetUniformLocation(shader, "texStone"), 3);
    glUniform1i(glGetUniformLocation(shader, "texRock"), 4);

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

        float radians = glm::radians(yaw);
        float speed = 0.0f;

        if (is_key_pressed(GLFW_KEY_W))   speed = -1.1f;
        if (is_key_pressed(GLFW_KEY_S))   speed =  1.1f;
        if (is_key_pressed(GLFW_KEY_A)) { speed =  1.1f; radians -= M_PI_2; }
        if (is_key_pressed(GLFW_KEY_D)) { speed =  1.1f; radians += M_PI_2; }

        glm::mat4 model_view = glm::identity<glm::mat4>();
        // model_view = glm::rotate(model_view, glm::radians(-pitch), glm::vec3(1.0f, 0.0f, 0.0f));
        // model_view = glm::rotate(model_view, glm::radians(-yaw), glm::vec3(0.0f, 1.0f, 0.0f));
        //model_view = glm::translate(model_view, glm::vec3(-1, -1, 0));

        auto MVP = projection * model_view;
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(MVP));

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        for(size_t i = 0; i < std::size(textures); ++i)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, textures[i]);
        }
        
        glBindVertexArray(VAO);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glBindVertexArray(0);

        for(size_t i = 0; i < std::size(textures); ++i)
            glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(shader);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteTextures(std::size(textures), textures);
}