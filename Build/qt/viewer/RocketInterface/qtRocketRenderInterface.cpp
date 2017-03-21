#include "qtRocketRenderInterface.h"
#include "qtGraphicSystem.h"
#include <QtOpenGL/qgl.h>
#include <Rocket/Core.h>

RocketRenderInterface::RocketRenderInterface()
{
}

void RocketRenderInterface::RenderGeometry(Rocket::Core::Vertex* vertices, int ROCKET_UNUSED(num_vertices), int* indices, int num_indices, const Rocket::Core::TextureHandle texture, const Rocket::Core::Vector2f& translation)
{
    glPushMatrix();
    glTranslatef(translation.x, translation.y, 0);

    glVertexPointer(2, GL_FLOAT, sizeof(Rocket::Core::Vertex), &vertices[0].position);
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Rocket::Core::Vertex), &vertices[0].colour);

    if (!texture)
    {
        glDisable(GL_TEXTURE_2D);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
    else
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, (GLuint) texture);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, sizeof(Rocket::Core::Vertex), &vertices[0].tex_coord);
    }

    glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, indices);

    glPopMatrix();
}

Rocket::Core::CompiledGeometryHandle RocketRenderInterface::CompileGeometry(Rocket::Core::Vertex* ROCKET_UNUSED(vertices), int ROCKET_UNUSED(num_vertices), int* ROCKET_UNUSED(indices), int ROCKET_UNUSED(num_indices), const Rocket::Core::TextureHandle ROCKET_UNUSED(texture))
{
    return (Rocket::Core::CompiledGeometryHandle) NULL;
}

void RocketRenderInterface::RenderCompiledGeometry(Rocket::Core::CompiledGeometryHandle ROCKET_UNUSED(geometry), const Rocket::Core::Vector2f& ROCKET_UNUSED(translation))
{
}

void RocketRenderInterface::ReleaseCompiledGeometry(Rocket::Core::CompiledGeometryHandle ROCKET_UNUSED(geometry))
{
}

void RocketRenderInterface::EnableScissorRegion(bool enable)
{
    if (enable)
        glEnable(GL_SCISSOR_TEST);
    else
        glDisable(GL_SCISSOR_TEST);
}

void RocketRenderInterface::SetScissorRegion(int x, int y, int width, int height)
{
    GraphicSystem::scissor(x, y, width, height);
}

bool RocketRenderInterface::LoadTexture(Rocket::Core::TextureHandle& texture_handle, Rocket::Core::Vector2i& texture_dimensions, const Rocket::Core::String& source)
{
    return GraphicSystem::loadTexture(texture_handle,texture_dimensions,source.CString());
}

bool RocketRenderInterface::GenerateTexture(Rocket::Core::TextureHandle& texture_handle, const Rocket::Core::byte* source, const Rocket::Core::Vector2i& source_dimensions)
{
    return GraphicSystem::generateTexture(texture_handle,source,source_dimensions);
}

void RocketRenderInterface::ReleaseTexture(Rocket::Core::TextureHandle texture_handle)
{
    glDeleteTextures(1, (GLuint*) &texture_handle);
}

