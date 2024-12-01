#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Image.hpp"
#include "Texture.hpp"
#include "ShaderProgram.hpp"

// static void framebufferResizeCallback(GLFWwindow *window, int width, int height)
// {
//     auto app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
//     app->framebufferResized = true;
// }

float lastX = 1200 / 2.0f;
float lastY = 800 / 2.0f;
float pitch = 20;
float yaw = 180;

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos; // Обратный порядок вычитания потому что оконные Y-координаты возрастают с верху вниз 
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

int main()
{
    const GLint SCR_WIDTH = 1200;
    const GLint SCR_HEIGHT = 800;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "World3D", nullptr, nullptr);

    if (!window)
    {
        glfwTerminate();

        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // glfwSetWindowUserPointer(window, this);

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int32_t width, int32_t height)
    {
        glViewport(0, 0, width, height);
    });

    glfwSetCursorPosCallback(window, mouse_callback);

    if (!gladLoadGL())
    {
        glfwTerminate();

        return -1;
    }

    auto is_key_pressed = [window](int32_t key)
    {
        return glfwGetKey(window, key) == GLFW_PRESS;
    };

    glEnable(GL_DEPTH_TEST);

    Image imageMap;
    imageMap.loadFromFile("res/textures/heightmap.png");

    const uint8_t* pixels = imageMap.getPixels();
 
    const GLuint mapHeight = imageMap.getHeight();
    const GLuint mapWidth = imageMap.getWidth();

    std::vector<glm::vec3> vertices(mapHeight * mapWidth);
    std::vector<glm::vec2> tex_coords(vertices.size());

    std::vector<GLuint> indices;
    indices.reserve(vertices.size() * 2);

    const float offsetZ = 1.0f / mapHeight;
    const float offsetX = 1.0f / mapWidth;

    size_t index = 0;

    float Z = 0;

    for (size_t z = 0; z < mapHeight; ++z)
    {
        float X = 0;
        
        for (size_t x = 0; x < mapWidth; ++x)
        {
            const uint8_t* pixel = pixels + ((z * mapWidth + x) * imageMap.getBytePerPixel());
            int y = (int)pixel[0];
            float Y = y * 0.03f;

            vertices[index].x = x;
            vertices[index].y = Y;
            vertices[index].z = z;
            
            tex_coords[index].x = x;
            tex_coords[index].y = z;

            X += offsetX;

            index++;
        }
        Z += offsetZ;
    }

    for(GLuint i = 0; i < mapHeight - 1; ++i) // 19 800
    {
        for(GLuint j = 0; j < mapWidth; ++j)
        {
            indices.push_back(mapWidth * i + j);
            indices.push_back(mapWidth * (i + 1) + j);       
        }
    }

    Image imgCrackedEarth; imgCrackedEarth.loadFromFile("res/textures/cracked_earth.jpg");
    Image imgRock;         imgRock.loadFromFile("res/textures/rock.jpg");
    Image imgGrass;        imgGrass.loadFromFile("res/textures/grass.jpg");
    Image imgClover;       imgClover.loadFromFile("res/textures/clover.png");

    GLuint texCrackedEarth = Texture().createFromImage(imgCrackedEarth, GL_REPEAT, GL_LINEAR);
    GLuint texRock         = Texture().createFromImage(imgRock, GL_REPEAT, GL_LINEAR);
    GLuint texGrass        = Texture().createFromImage(imgGrass, GL_REPEAT, GL_LINEAR);
    GLuint texClover       = Texture().createFromImage(imgClover, GL_REPEAT, GL_LINEAR);

    GLuint VAO, VBO[2], EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(2, VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * tex_coords.size(), tex_coords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), NULL);
    glEnableVertexAttribArray(1);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);
   
    glBindVertexArray(0);

    GLuint shader = ShaderProgram().compile("res/shaders/terrain_shader.vert", "res/shaders/terrain_shader.frag");
    glUseProgram(shader);
    int mvpLoc = glGetUniformLocation(shader, "MVP");

    glUniform1i(glGetUniformLocation(shader, "cracked_earth"), 0);
    glUniform1i(glGetUniformLocation(shader, "rock"), 1);
    glUniform1i(glGetUniformLocation(shader, "grass"), 2);
    glUniform1i(glGetUniformLocation(shader, "clover"), 3);

    glm::mat4 projection = glm::perspective(glm::radians(45.f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);

    glm::vec2 pos = { 3, 5 };

    const GLuint numStrips = mapWidth - 1;
    const GLuint numTrisPerStrip = mapWidth * 2 - 2;

    while (!glfwWindowShouldClose(window))
    {
        if(is_key_pressed(GLFW_KEY_ESCAPE)) 
        {
            glfwSetWindowShouldClose(window, true);
            continue;
        }

        float radians = glm::radians(yaw);
        float speed = 0.0f;

        if (is_key_pressed(GLFW_KEY_W))   speed = -0.1f;
        if (is_key_pressed(GLFW_KEY_S))   speed =  0.1f;
        if (is_key_pressed(GLFW_KEY_A)) { speed =  0.1f; radians -= M_PI_2; }
        if (is_key_pressed(GLFW_KEY_D)) { speed =  0.1f; radians += M_PI_2; }

        if (speed != 0.0f)
        {
            pos.x += sinf(radians) * speed;
            pos.y += cosf(radians) * speed;
        }

        glm::mat4 model_view = glm::mat4(1.0f);
        model_view = glm::rotate(model_view, glm::radians(-pitch), glm::vec3(1.0f, 0.0f, 0.0f));
        model_view = glm::rotate(model_view, glm::radians(-yaw), glm::vec3(0.0f, 1.0f, 0.0f));
        model_view = glm::translate(model_view, glm::vec3(-pos.x, -3.0f, -pos.y));

        auto MVP = projection * model_view;
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(MVP));

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texCrackedEarth);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texRock);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, texGrass);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, texClover);

        glBindVertexArray(VAO);

        for(GLuint strip = 0; strip < numStrips; ++strip)
        {
            glDrawElements(GL_TRIANGLE_STRIP,   // primitive type
                           numTrisPerStrip + 2, // number of indices to render
                           GL_UNSIGNED_INT,     // index data type
            reinterpret_cast<const void*>(sizeof(GLuint) * (numTrisPerStrip + 2) * strip)); // offset to starting index
        }

        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(shader);
    glDeleteBuffers(2, VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}