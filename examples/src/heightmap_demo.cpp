#include <glad/glad.h>
#include <GLFW/glfw3.h>

#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#include <math.H>
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Image.hpp"
#include "Texture2D.hpp"
#include "ShaderProgram.hpp"
#include "VertexArray.hpp"

static float lastX = 1200 / 2.0f;
static float lastY = 800 / 2.0f;
static float pitch = 0;
static float yaw = 0;

static void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos; // Обратный порядок вычитания потому что оконные Y-координаты возрастают сверху вниз 
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

void heightmap_demo(GLFWwindow* window, GLint scr_width, GLint scr_height)
{
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    auto is_key_pressed = [window](int32_t key)
    {
        return glfwGetKey(window, key) == GLFW_PRESS;
    };

    glEnable(GL_DEPTH_TEST);

    Image imageMap;
    imageMap.loadFromFile("res/textures/heightmap.png");

    const uint8_t* pixels = imageMap.getPixels();
    const GLuint mapDepth = imageMap.getHeight();
    const GLuint mapWidth = imageMap.getWidth();

    std::vector<float> vertices(mapDepth * mapWidth * 5);
    std::vector<GLuint> indices;
    indices.reserve(static_cast<size_t>(mapDepth * mapWidth * 2));

    std::vector<float> heightmap(mapDepth * mapWidth);

    auto get_height_in_point = [&heightmap, mapWidth, mapDepth](float x, float z) -> float
    {
        if((x >= 0) && (x < mapWidth) && (z >= 0) && (z < mapDepth))
        {
            int32_t cX = static_cast<int32_t>(x);
            int32_t cZ = static_cast<int32_t>(z);

            float* h = heightmap.data() + (static_cast<int32_t>(z) * mapWidth + static_cast<int32_t>(x));

            float h0 = *h;
            float h1 = *(h + 1);
            float h2 = *(h + mapWidth);
            float h3 = *(h + mapWidth + 1);

            float h5 = ((1 - (x - cX)) * h0 + (x - cX) * h1);
            float h6 = ((1 - (x - cX)) * h2 + (x - cX) * h3);

            return (1 - (z - cZ)) * h5 + (z - cZ) * h6;
        }

        return 0.f;
    };

    size_t index = 0;
    size_t stride = 0;

    for (size_t z = 0; z < mapDepth; ++z)
    {
        for (size_t x = 0; x < mapWidth; ++x)
        {
            const uint8_t* pixel = pixels + ((z * mapWidth + x) * imageMap.getBytePerPixel());
            int32_t y = static_cast<int32_t>(pixel[0]);
            float Y = y * 0.03f;

            float* vertex = vertices.data() + stride;

            vertex[0] = x;
            vertex[1] = Y;
            vertex[2] = z;
            vertex[3] = x * 0.3f;
            vertex[4] = z * 0.3f;

            heightmap[index] = Y;

            ++index;
            stride += 5;
        }
    }

    for(GLuint i = 0; i < mapDepth - 1; ++i) // 19 800
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

    Texture2D texCrackedEarth = { imgCrackedEarth, Texture2D::WrapMode::Repeat, Texture2D::FilterMode::Linear };
    Texture2D texRock         = { imgRock, Texture2D::WrapMode::Repeat,         Texture2D::FilterMode::Linear };
    Texture2D texGrass        = { imgGrass, Texture2D::WrapMode::Repeat,        Texture2D::FilterMode::Linear };
    Texture2D texClover       = { imgClover, Texture2D::WrapMode::Repeat,       Texture2D::FilterMode::Linear };

    BufferLayout layout
    {
        AttributeInfo::Float3,
        AttributeInfo::Float2
    };

    auto vbo = std::make_unique<VertexBuffer>(vertices.data(), vertices.size() * sizeof(GLfloat), layout);
    auto ebo = std::make_unique<IndexBuffer>(indices.data(), indices.size());
    auto vao = std::make_unique<VertexArray>();

    vao->addVertexBuffer(*vbo);
    vao->setIndexBuffer(*ebo);

    ShaderProgram program = 
    { 
        { "res/shaders/heightmap.vert", Shader::Vertex   }, 
        { "res/shaders/heightmap.frag", Shader::Fragment } 
    };

    if (!program.isCompiled())
        return;

    ShaderProgram::bind(&program);
    int32_t mvpLoc = program.getUniformLocation("MVP");

    ShaderProgram::setUniform1i(program.getUniformLocation("cracked_earth"), 0);
    ShaderProgram::setUniform1i(program.getUniformLocation("rock"), 1);
    ShaderProgram::setUniform1i(program.getUniformLocation("grass"), 2);
    ShaderProgram::setUniform1i(program.getUniformLocation("clover"), 3);

    glm::mat4 projection = glm::perspective(glm::radians(45.f), (float)scr_width / (float)scr_height, 0.1f, 1000.0f);

    glm::vec3 pos = { 3, 0, 5 };

    const GLuint numStrips = mapWidth - 1;
    const GLuint numTrisPerStrip = mapWidth * 2 - 2;

    while (!glfwWindowShouldClose(window))
    {
        if(is_key_pressed(GLFW_KEY_ESCAPE)) 
        {
            glfwSetWindowShouldClose(window, true);
            continue;
        }

        float angleY = glm::radians(yaw);
        float speed = 0.0f;

        if (is_key_pressed(GLFW_KEY_W))   speed = -0.1f;
        if (is_key_pressed(GLFW_KEY_S))   speed =  0.1f;
        if (is_key_pressed(GLFW_KEY_A)) { speed =  0.1f; angleY -= M_PI_2; }
        if (is_key_pressed(GLFW_KEY_D)) { speed =  0.1f; angleY += M_PI_2; }

        if (speed != 0.0f)
        {
            pos.x += sin(angleY) * speed;
            pos.z += cos(angleY) * speed;
        }

        pos.y = get_height_in_point(pos.x, pos.z);

        glm::mat4 model_view = glm::mat4(1.0f);
        model_view = glm::rotate(model_view, glm::radians(-pitch), glm::vec3(1.0f, 0.0f, 0.0f));
        model_view = glm::rotate(model_view, glm::radians(-yaw), glm::vec3(0.0f, 1.0f, 0.0f));
        model_view = glm::translate(model_view, glm::vec3(-pos.x, -(pos.y + 1.7f), -pos.z));

        auto MVP = projection * model_view;
        ShaderProgram::setUniformMatrix4fv(mvpLoc, 1, 0, glm::value_ptr(MVP));

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        Texture2D::enable(GL_TEXTURE0);
        Texture2D::bind(&texCrackedEarth);

        Texture2D::enable(GL_TEXTURE1);
        Texture2D::bind(&texRock);

        Texture2D::enable(GL_TEXTURE2);
        Texture2D::bind(&texGrass);

        Texture2D::enable(GL_TEXTURE3);
        Texture2D::bind(&texClover);

        vao->bind(vao.get());

        for(GLuint strip = 0; strip < numStrips; ++strip)
        {
            glDrawElements(GL_TRIANGLE_STRIP,   // primitive type
                           numTrisPerStrip + 2, // number of indices to render
                           GL_UNSIGNED_INT,     // index data type
            reinterpret_cast<const void*>(sizeof(GLuint) * (numTrisPerStrip + 2) * strip)); // offset to starting index
        }

        vao->bind(nullptr);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}