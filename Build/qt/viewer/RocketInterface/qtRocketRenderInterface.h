#ifndef ROCKETRENDERINTERFACE_H
#define ROCKETRENDERINTERFACE_H

#include <Rocket/Core/RenderInterface.h>
#include "OpenGL.h"

class RocketRenderInterface : public Rocket::Core::RenderInterface
{
public:
    RocketRenderInterface();

    virtual void RenderGeometry(Rocket::Core::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rocket::Core::TextureHandle texture, const Rocket::Core::Vector2f& translation);

    virtual Rocket::Core::CompiledGeometryHandle CompileGeometry(Rocket::Core::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rocket::Core::TextureHandle texture);

    virtual void RenderCompiledGeometry(Rocket::Core::CompiledGeometryHandle geometry, const Rocket::Core::Vector2f& translation);
    virtual void ReleaseCompiledGeometry(Rocket::Core::CompiledGeometryHandle geometry);

    virtual void EnableScissorRegion(bool enable);
    virtual void SetScissorRegion(int x, int y, int width, int height);

    virtual bool LoadTexture(Rocket::Core::TextureHandle& texture_handle, Rocket::Core::Vector2i& texture_dimensions, const Rocket::Core::String& source);
    virtual bool GenerateTexture(Rocket::Core::TextureHandle& texture_handle, const Rocket::Core::byte* source, const Rocket::Core::Vector2i& source_dimensions);
    virtual void ReleaseTexture(Rocket::Core::TextureHandle texture_handle);
};

#endif
