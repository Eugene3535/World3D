#include <cstring>
#include <span>

#include "geometry/Geometry2D.hpp"


std::vector<float> Geometry2D::createRectWithRoundedEdges(const RoundedRect2D& info) noexcept
{
    const int32_t cornerSegments = 8;

    float x = info.x;
    float y = info.y;
    float w = info.width;
    float h = info.height;
    float r = info.radius;

    static constexpr size_t totalVertices = 8 + 4 * cornerSegments;
    std::vector<float>      vertices(totalVertices << 1);
    size_t                  vertex = 0;

    constexpr float angleStep = M_PI_2 / (float)(cornerSegments - 1);

    vertices[vertex++] = x + r;
    vertices[vertex++] = y;
    vertices[vertex++] = x + w - r;
    vertices[vertex++] = y;

    float cx = x + w - r;
    float cy = y + r;
    for (int32_t i = 0; i < cornerSegments; ++i)
    {
        const float angle  = M_PI * 3.f / 2.f + angleStep * i;
        vertices[vertex++] = cx + cos(angle) * r;
        vertices[vertex++] = cy + sin(angle) * r;
    }

    vertices[vertex++] = x + w;
    vertices[vertex++] = y + r;
    vertices[vertex++] = x + w;
    vertices[vertex++] = y + h - r;

    cx = x + w - r;
    cy = y + h - r;
    for (int32_t i = 0; i < cornerSegments; ++i)
    {
        const float angle  = angleStep * i;
        vertices[vertex++] = cx + cos(angle) * r;
        vertices[vertex++] = cy + sin(angle) * r;
    }

    vertices[vertex++] = x + w - r;
    vertices[vertex++] = y + h;
    vertices[vertex++] = x + r;
    vertices[vertex++] = y + h;

    cx = x + r;
    cy = y + h - r;
    for (int32_t i = 0; i < cornerSegments; ++i)
    {
        const float angle  = M_PI_2 + angleStep * i;
        vertices[vertex++] = cx + cos(angle) * r;
        vertices[vertex++] = cy + sin(angle) * r;
    }

    vertices[vertex++] = x;
    vertices[vertex++] = y + h - r;
    vertices[vertex++] = x;
    vertices[vertex++] = y + r;

    cx = x + r;
    cy = y + r;
    for (int32_t i = 0; i < cornerSegments; ++i)
    {
        const float angle  = M_PI + angleStep * i;
        vertices[vertex++] = cx + cos(angle) * r;
        vertices[vertex++] = cy + sin(angle) * r;
    }

    return vertices;
}


void Geometry2D::calculateOutline(std::span<vec2s> points, std::vector<float>& result, float thickness) noexcept
{
    auto compute_normal = [](const vec2s& p1, const vec2s& p2) -> vec2s // Compute the normal of a segment
    {
        vec2s v = glms_vec2_sub(p2, p1);
        vec2s normal = {-v.y, v.x}; // make perpendicular
        float length = sqrt(normal.x * normal.x + normal.y * normal.y);

        if (length != 0.f)
        {
            normal.x /= length;
            normal.y /= length;
        }

        return normal;
    };

    const size_t       count = points.size() - 2;
    std::vector<vec2s> vertices((count + 1) * 2);

    for (size_t i = 0; i < count; ++i)
    {
        const size_t index = i + 1;

        //  Get the two segments shared by the current point
        size_t j = (i == 0) ? count : index - 1;

        vec2s p0 = points[j];
        vec2s p1 = points[index];
        vec2s p2 = points[index + 1];

        //  Compute their normal
        vec2s n1 = compute_normal(p0, p1);
        vec2s n2 = compute_normal(p1, p2);

        //  Make sure that the normals point towards the outside of the shape
        //  (this depends on the order in which the points were defined)
        {
            vec2s vertex = glms_vec2_sub(points[0], p1);

            if (glms_vec2_dot(n1, vertex) > 0.f)
            {
                n1.x = -n1.x;
                n1.y = -n1.y;
            }

            if (glms_vec2_dot(n2, vertex) > 0.f)
            {
                n2.x = -n2.x;
                n2.y = -n2.y;
            }
        }

        //  Combine them to get the extrusion direction
        const float factor = 1.f + (n1.x * n2.x + n1.y * n2.y);
        vec2s       normal;
        normal = glms_vec2_add(n1, n2);
        normal.x /= factor;
        normal.y /= factor;

        //  Update the outline points
        vertices[i * 2 + 0] = p1;
        {
            vec2s v = glms_vec2_scale(normal, thickness);
            vertices[i * 2 + 1] = glms_vec2_add(p1, v);
        }
    }

    //  Duplicate the first point at the end, to close the outline
    vertices[count * 2 + 0] = vertices[0];
    vertices[count * 2 + 1] = vertices[1];

    result.resize(vertices.size() << 1);

    memcpy(result.data(), vertices.data(), vertices.size() * sizeof(vec2s));
}
