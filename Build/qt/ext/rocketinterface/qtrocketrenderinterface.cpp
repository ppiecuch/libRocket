#include "qtrocketrenderinterface.h"
#include "qtrocketfileinterface.h"

#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QtOpenGL/qgl.h>

#include <Rocket/Core.h>

#include "graphicsystem.h"

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
        glEnableClientState(GL_COLOR_ARRAY);
    }
    else
    {
        glEnable(GL_TEXTURE_2D);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
        glBindTexture(GL_TEXTURE_2D, (GLuint) texture);
        glTexCoordPointer(2, GL_FLOAT, sizeof(Rocket::Core::Vertex), &vertices[0].tex_coord);
    }

    glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, indices);
#if 1 // triangles outline
    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_COLOR_ARRAY);
    glColor4f(0,0.5,0.5,0.8); glLineWidth(2);
    for (int t=0; t<num_indices/3; ++t)
        glDrawElements(GL_LINE_LOOP, 3, GL_UNSIGNED_SHORT, indices+t*3);
    if (texture)
        glEnable(GL_TEXTURE_2D);
    glColor4f(1,1,1,1); glLineWidth(1);
#endif

    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
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

extern QImage _qt_load_tga(QIODevice *device); // custom tga loader

bool RocketRenderInterface::LoadTexture(Rocket::Core::TextureHandle& texture_handle, Rocket::Core::Vector2i& texture_dimensions, const Rocket::Core::String& source)
{
    QString real_path = source.CString();
    QFileInfo file_info = real_path;

    if(!file_info.exists()) {
        QFileInfo base_file_info = file_info;
        foreach(QString path, RocketFileInterface::GetSearchPaths())
        {
            // check if we can find the file directly:
            if (QFile::exists(QDir(path).filePath(real_path)))
            {
                file_info = QDir(path).filePath(real_path);
                break;
            }

            // iterate:
            QDirIterator directory_walker(path, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);

            while(directory_walker.hasNext())
            {
                directory_walker.next();

                if(!directory_walker.fileInfo().isDir()
                    && !directory_walker.fileInfo().isHidden()
                    && directory_walker.fileInfo().fileName() == base_file_info.fileName())
                {
                    file_info = directory_walker.fileInfo();
                    break;
                }
            }

            if(file_info.exists())
            {
                break;
            }
        }
    }

    QImage img(file_info.canonicalFilePath());

    if (img.isNull()) {
        QFile file(file_info.filePath());
        if (file.open(QIODevice::ReadOnly))
            img = _qt_load_tga(&file);
        if (img.isNull()) {
            qWarning() << "Failed to load image from source:" << real_path;
            return false;
        }
    }

    const int w = img.width(), h = img.height();
    const bool alpha_format = img.hasAlphaChannel();

    if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
        // OpenGL gives RGBA; Qt wants ARGB
        uint *p = (uint*)img.bits();
        uint *end = p + w*h;
        if (alpha_format) {
        while (p < end) {
            uint a = *p << 24;
            *p = (*p >> 8) | a;
            p++;
        }
        } else {
        // This is an old legacy fix for PowerPC based Macs, which
        // we shouldn't remove
        while (p < end) {
            *p = 0xff000000 | (*p>>8);
            ++p;
        }
        }
    } else {
        // OpenGL gives ABGR (i.e. RGBA backwards); Qt wants ARGB
        for (int y = 0; y < h; y++) {
        uint *q = (uint*)img.scanLine(y);
        for (int x=0; x < w; ++x) {
            const uint pixel = *q;
            if (alpha_format) {
                *q = ((pixel << 16) & 0xff0000) | ((pixel >> 16) & 0xff)
                    | (pixel & 0xff00ff00);
            } else {
                *q = 0xff000000 | ((pixel << 16) & 0xff0000)
                    | ((pixel >> 16) & 0xff) | (pixel & 0x00ff00);
            }

            q++;
        }
        }

    }

    texture_dimensions = Rocket::Core::Vector2i(w, h);

    return GenerateTexture(texture_handle, img.constBits(), texture_dimensions);
}

bool RocketRenderInterface::GenerateTexture(Rocket::Core::TextureHandle& texture_handle, const Rocket::Core::byte* source, const Rocket::Core::Vector2i& source_dimensions)
{
    GLuint texture_id = 0;

    glGenTextures(1, &texture_id);

    if (texture_id == 0) {
        qWarning() << "Failed to generate texture";
        return false;
    }

    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, source_dimensions.x, source_dimensions.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, source);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    texture_handle = (Rocket::Core::TextureHandle) texture_id;

    return true;
}

void RocketRenderInterface::ReleaseTexture(Rocket::Core::TextureHandle texture_handle)
{
    glDeleteTextures(1, (GLuint*) &texture_handle);
}

