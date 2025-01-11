#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Heightmap.hpp"


Heightmap::Heightmap(void* handle) noexcept:
    Scene(handle),
    m_camera(handle),
    m_mapWidth(0),
    m_mapDepth(0)
{
    m_imageMap.loadFromFile("res/textures/heightmap.png");
    m_imgCrackedEarth.loadFromFile("res/textures/cracked_earth.jpg");
    m_imgRock.loadFromFile("res/textures/rock.jpg");
    m_imgGrass.loadFromFile("res/textures/grass.jpg");
    m_imgClover.loadFromFile("res/textures/clover.png");

    m_texCrackedEarth = std::make_unique<Texture2D>(m_imgCrackedEarth, Texture2D::WrapMode::Repeat, Texture2D::FilterMode::Linear);
    m_texRock         = std::make_unique<Texture2D>(m_imgRock, Texture2D::WrapMode::Repeat,         Texture2D::FilterMode::Linear);
    m_texGrass        = std::make_unique<Texture2D>(m_imgGrass, Texture2D::WrapMode::Repeat,        Texture2D::FilterMode::Linear);
    m_texClover       = std::make_unique<Texture2D>(m_imgClover, Texture2D::WrapMode::Repeat,       Texture2D::FilterMode::Linear);

    const uint8_t* pixels = m_imageMap.getPixels();
    m_mapDepth = m_imageMap.getHeight();
    m_mapWidth = m_imageMap.getWidth();

    m_heightmap.resize(m_mapDepth * m_mapWidth);
    std::vector<float> vertices(m_mapDepth * m_mapWidth * 5);
    std::vector<uint32_t> indices;
    indices.reserve(static_cast<size_t>(m_mapDepth * m_mapWidth * 2));

    size_t index = 0;
    size_t stride = 0;

    for (size_t z = 0; z < m_mapDepth; ++z)
    {
        for (size_t x = 0; x < m_mapWidth; ++x)
        {
            const uint8_t* pixel = pixels + ((z * m_mapWidth + x) * m_imageMap.getBytePerPixel());
            int32_t y = static_cast<int32_t>(pixel[0]);
            float Y = y * 0.03f;

            float* vertex = vertices.data() + stride;

            vertex[0] = x;
            vertex[1] = Y;
            vertex[2] = z;
            vertex[3] = x * 0.3f;
            vertex[4] = z * 0.3f;

            m_heightmap[index] = Y;

            ++index;
            stride += 5;
        }
    }

    for(uint32_t i = 0; i < m_mapDepth - 1; ++i) // 19 800
    {
        for(uint32_t j = 0; j < m_mapWidth; ++j)
        {
            indices.push_back(m_mapWidth * i + j);
            indices.push_back(m_mapWidth * (i + 1) + j);       
        }
    }

    BufferLayout layout
    {
        AttributeInfo::Float3,
        AttributeInfo::Float2
    };

    m_vbo = std::make_unique<VertexBuffer>(vertices.data(), vertices.size() * sizeof(float), layout);
    m_ebo = std::make_unique<IndexBuffer>(indices.data(), indices.size());
    m_vao = std::make_unique<VertexArray>();

    m_vao->addVertexBuffer(*m_vbo);
    m_vao->setIndexBuffer(*m_ebo);

    ShaderProgram program = 
    { 
        { "res/shaders/heightmap.vert", Shader::Vertex   }, 
        { "res/shaders/heightmap.frag", Shader::Fragment } 
    };

    m_program = std::make_unique<ShaderProgram>(std::move(program));
    ShaderProgram::bind(m_program.get());
    ShaderProgram::setUniform1i(m_program->getUniformLocation("cracked_earth"), 0);
    ShaderProgram::setUniform1i(m_program->getUniformLocation("rock"), 1);
    ShaderProgram::setUniform1i(m_program->getUniformLocation("grass"), 2);
    ShaderProgram::setUniform1i(m_program->getUniformLocation("clover"), 3);
}


Heightmap::~Heightmap() noexcept
{

}


void Heightmap::draw() noexcept
{
    auto get_height_in_point = [this](float x, float z) -> float
    {
        if((x >= 0) && (x < m_mapWidth) && (z >= 0) && (z < m_mapDepth))
        {
            int32_t cX = static_cast<int32_t>(x);
            int32_t cZ = static_cast<int32_t>(z);

            float* h = m_heightmap.data() + (static_cast<int32_t>(z) * m_mapWidth + static_cast<int32_t>(x));

            float h0 = *h;
            float h1 = *(h + 1);
            float h2 = *(h + m_mapWidth);
            float h3 = *(h + m_mapWidth + 1);

            float h5 = ((1 - (x - cX)) * h0 + (x - cX) * h1);
            float h6 = ((1 - (x - cX)) * h2 + (x - cX) * h3);

            return (1 - (z - cZ)) * h5 + (z - cZ) * h6;
        }

        return 0.f;
    };

    glm::vec3 pos = m_camera.getPosition();
    pos.y = get_height_in_point(pos.x, pos.z) + 2;
    m_camera.setPosition(pos);
    m_camera.update(0.01f);

    glm::mat4 projection = glm::perspective(glm::radians(45.f), (float)1200 / (float)800, 0.1f, 1000.0f);
    glm::mat4 model_view = m_camera.getViewMatrix();
    auto MVP = projection * model_view;
    int32_t mvpLoc = m_program->getUniformLocation("MVP");
    ShaderProgram::setUniformMatrix4fv(mvpLoc, 1, 0, glm::value_ptr(MVP));

    Texture2D::enable(0);
    Texture2D::bind(m_texCrackedEarth.get());

    Texture2D::enable(1);
    Texture2D::bind(m_texRock.get());

    Texture2D::enable(2);
    Texture2D::bind(m_texGrass.get());

    Texture2D::enable(3);
    Texture2D::bind(m_texClover.get());

    m_vao->bind(m_vao.get());

    const uint32_t numStrips = m_mapWidth - 1;
    const uint32_t numTrisPerStrip = m_mapWidth * 2 - 2;

    drawHeightmap(numStrips, numTrisPerStrip);

    m_vao->bind(nullptr);
}