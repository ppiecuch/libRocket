#ifndef QTGRAPHICSYSTEM_H
#define QTGRAPHICSYSTEM_H

#include "RocketHelper.h"
#include <Rocket/Core.h>

class GraphicSystem
{
public:
    static bool loadTexture(Rocket::Core::TextureHandle &texture_handle, Rocket::Core::Vector2i &texture_dimensions, const QString &source);
    static bool generateTexture(Rocket::Core::TextureHandle &texture_handle, const Rocket::Core::byte *source, const Rocket::Core::Vector2i &source_dimensions);
    static void scissor(int x, int y, int width, int height);
    static void putXAxisVertices(const float y);
    static void putYAxisVertices(const float x);
    static void drawBox(const Vector2f &origin, const Vector2f &dimensions, const Color4b &color, const bool filled=true);
    static void drawBox(const Vector2f &origin, const Vector2f &dimensions, const Vector2f &hole_origin, const Vector2f &hole_dimensions, const Color4b &color);
    static void drawBackground();
    static void drawTexturedBox(const Vector2f &origin, const Vector2f &dimensions, Rocket::Core::TextureHandle texture_handle);

    static Vector2f scissorOffset;
    static float scaleFactor;
    static int width;
    static int height;
    
    static Rocket::Core::TextureHandle backgroundTextureHandle;

private:
    static unsigned char * loadTGA(const QString &path, Rocket::Core::Vector2i &texture_dimensions);
    static unsigned char * loadOther(const QString &path, Rocket::Core::Vector2i &texture_dimensions);
};

#endif // QTGRAPHICSYSTEM_H
