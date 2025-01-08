#include "Scene.hpp"


Scene::Scene(void* handle) noexcept:
    m_handle(handle)
{

}


Scene::~Scene() noexcept = default;

void Scene::draw() noexcept
{

}

void Scene::drawHeightmap(uint32_t numStrips, uint32_t numTrisPerStrip) noexcept
{
    for(uint32_t strip = 0; strip < numStrips; ++strip)
    {
        glDrawElements(GL_TRIANGLE_STRIP,   // primitive type
                        numTrisPerStrip + 2, // number of indices to render
                        GL_UNSIGNED_INT,     // index data type
        reinterpret_cast<const void*>(sizeof(uint32_t) * (numTrisPerStrip + 2) * strip)); // offset to starting index
    }
}