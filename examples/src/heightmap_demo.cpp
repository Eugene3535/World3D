#include <glad/glad.h>
#include <GLFW/glfw3.h>

#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#include <math.H>
#endif

#include <Windows.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Image.hpp"
#include "Texture2D.hpp"
#include "ShaderProgram.hpp"
#include "VertexArray.hpp"

static float lastX = 1200 / 2.0f;
static float lastY = 800 / 2.0f;

static float pitch;
static float yaw;


class Player
{
public:
    Player(float x0, float y0, float z0)
    {
        x = x0; y = y0; z = z0;
        dx = 0; dy = 0; dz = 0;
        w = 5; h = 5; d = 5; // да, игрок кубик ;)
        speed = 50;
        onGround = true;
    }

    void update(float dt)
    {
        if (!onGround)
            dy -= 120 * dt;

        onGround = 0;

        x += dx * dt;
        //y += dy * dt;
        z += dz * dt;
        dx = dz = 0;
    }


    void keyboard(GLFWwindow* rw)
    {
        if (isKeyPressed(rw, GLFW_KEY_W))
        {
            dx = -sin(glm::radians(pitch)) * speed;
            dz = -cos(glm::radians(pitch)) * speed;
        }

        if (isKeyPressed(rw, GLFW_KEY_S))
        {
            dx = sin(glm::radians(pitch)) * speed;
            dz = cos(glm::radians(pitch)) * speed;
        }

        if (isKeyPressed(rw, GLFW_KEY_D))
        {
            dx = sin(glm::radians(pitch + 90)) * speed;
            dz = cos(glm::radians(pitch + 90)) * speed;
        }

        if (isKeyPressed(rw, GLFW_KEY_A))
        {
            dx = sin(glm::radians(pitch - 90)) * speed;
            dz = cos(glm::radians(pitch - 90)) * speed;
        }
    }

    bool isKeyPressed(GLFWwindow* window, int key)
    {
        return glfwGetKey(window, key) == GLFW_PRESS;
    }

    float x, y, z;
    float dx, dy, dz;
    float w, h, d; // width, height, depth
    bool onGround;
    float speed;
};


void heightmap_demo(GLFWwindow* window, GLint scr_width, GLint scr_height)
{
    ShowCursor(FALSE);

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

    Player player(20, 10, 20);

    const GLuint numStrips = mapWidth - 1;
    const GLuint numTrisPerStrip = mapWidth * 2 - 2;

    while (!glfwWindowShouldClose(window))
    {
        if(is_key_pressed(GLFW_KEY_ESCAPE)) 
        {
            glfwSetWindowShouldClose(window, true);
            continue;
        }

        player.y = get_height_in_point(player.x, player.z);

        static int cnt = 0;

        if(++cnt > 100)
        {
            cnt = 0;
            printf("x = %f, z = %f\n", player.x, player.z);
        }

        POINT mousexy;
        GetCursorPos(&mousexy);
        int xt;
        int yt;
        int w;
        int h;

        glfwGetWindowPos(window, &xt, &yt);
        glfwGetWindowSize(window, &w, &h);

        xt += w / 2;
        yt += h / 2;

        pitch += (xt - mousexy.x) * 0.25f; // 4 — чувствительность
        yaw += (yt - mousexy.y) * 0.25f;

        if (yaw < -89.0)
            yaw = -89.0;

        if (yaw > 89.0)
            yaw = 89.0;

        SetCursorPos(xt, yt);

        player.keyboard(window);
        player.update(0.01f);

        glm::mat4 model_view = glm::lookAt(
            glm::vec3(player.x, player.y + player.h, player.z),
            glm::vec3(player.x - sin(glm::radians(pitch)),
                player.y + player.h + (tan(glm::radians(yaw))),
                player.z - cos(glm::radians(pitch))),
            glm::vec3(0, 1, 0));

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