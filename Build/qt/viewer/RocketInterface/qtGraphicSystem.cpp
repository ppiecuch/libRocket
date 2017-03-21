#include "qtGraphicSystem.h"
#include <QtOpenGL/qgl.h>
#include <QImage>
#include <QDir>
#include <QDirIterator>

#pragma pack(1) 
struct TGAHeader 
{
    char  idLength;
    char  colourMapType;
    char  dataType;
    short int colourMapOrigin;
    short int colourMapLength;
    char  colourMapDepth;
    short int xOrigin;
    short int yOrigin;
    short int width;
    short int height;
    char  bitsPerPixel;
    char  imageDescriptor;
};
#pragma pack()

// Public:

bool GraphicSystem::generateTexture(Rocket::Core::TextureHandle &texture_handle, const Rocket::Core::byte *source, const Rocket::Core::Vector2i &source_dimensions)
{
    GLuint texture_id = 0;

    glGenTextures(1, &texture_id);

    if (texture_id == 0) {
        printf("Failed to generate textures\n");
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

bool GraphicSystem::loadTexture(Rocket::Core::TextureHandle &texture_handle, Rocket::Core::Vector2i &texture_dimensions, const QString &source)
{
    unsigned char *bytes = NULL;
    QFileInfo base_file_info(source);
    QFileInfo final_file_info;

    static QStringList texturePaths;
    if (texturePaths.empty())
        texturePaths << "." << "textures";

    if(base_file_info.exists())
    {
        final_file_info = base_file_info;
    }
    else
    {
        final_file_info = base_file_info.fileName();
        if(!final_file_info.exists()) // if the asset is not in the project files, recurse through the texture directory
        {
            foreach(QString path, texturePaths)
            {
                QDirIterator directory_walker(path, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);

                while(directory_walker.hasNext())
                {
                    directory_walker.next();

                    if(!directory_walker.fileInfo().isDir() && !directory_walker.fileInfo().isHidden() && directory_walker.fileInfo().baseName() == base_file_info.baseName())
                    {
                        final_file_info = directory_walker.fileInfo();
                        break;
                    }
                }

                if(final_file_info.exists())
                {
                    break;
                }
            }

            if(!final_file_info.exists())
            {
                printf("texture not found: %s.\n", final_file_info.fileName().toLatin1().data());
            }
        }
    }

    if (final_file_info.suffix() == "tga")
        bytes = loadTGA(final_file_info.absoluteFilePath(), texture_dimensions);
    else
        bytes = loadOther(final_file_info.absoluteFilePath(), texture_dimensions);

    if (!bytes)
        return false;

    bool success = generateTexture(texture_handle, bytes, texture_dimensions);

    delete [] bytes;

    return success;
}

void GraphicSystem::scissor(int x, int y, int width, int height)
{
    x+=scissorOffset.x;
    y+=scissorOffset.y;
    x*=scaleFactor;
    y*=scaleFactor;
    width*=scaleFactor;
    height*=scaleFactor;
    glScissor(x, GraphicSystem::height - (y + height), width, height);
}

void GraphicSystem::drawBox(const Vector2f &origin, const Vector2f &dimensions, const Color4b &color, const bool filled)
{
    glColor4ub(color.red, color.green, color.blue, color.alpha);
    glBegin(filled ? GL_POLYGON : GL_LINE_LOOP);
        glVertex2f(origin.x, origin.y);
        glVertex2f(origin.x+dimensions.x, origin.y);
        glVertex2f(origin.x+dimensions.x, origin.y+dimensions.y);
        glVertex2f(origin.x, origin.y+dimensions.y);
    glEnd();
}

void GraphicSystem::drawBox(const Vector2f &origin, const Vector2f &dimensions, const Vector2f &hole_origin, const Vector2f &hole_dimensions, const Color4b &color)
{
    // Top box.
    float top_y_dimensions = hole_origin.y - origin.y;
    if (top_y_dimensions > 0)
        drawBox(origin, Vector2f(dimensions.x, top_y_dimensions), color);

    // Bottom box.
    float bottom_y_dimensions = (origin.y + dimensions.y) - (hole_origin.y + hole_dimensions.y);
    if (bottom_y_dimensions > 0)
        drawBox(Vector2f(origin.x, hole_origin.y + hole_dimensions.y), Vector2f(dimensions.x, bottom_y_dimensions), color);

    //Left box.
    float left_x_dimensions = hole_origin.x - origin.x;
    if (left_x_dimensions > 0)
        drawBox(Vector2f(origin.x, hole_origin.y), Vector2f(left_x_dimensions, hole_dimensions.y), color);

    //Right box.
    float right_x_dimensions = (origin.x + dimensions.x) - (hole_origin.x + hole_dimensions.x);
    if (right_x_dimensions > 0)
        drawBox(Vector2f(hole_origin.x + hole_dimensions.x, hole_origin.y), Vector2f(right_x_dimensions, hole_dimensions.y), color);
}

void GraphicSystem::drawBackground()
{
    if ( backgroundTextureHandle != 0 )
    {
        drawTexturedBox(Vector2f(0, 0), Vector2f(width, height), backgroundTextureHandle);
    }
}

void GraphicSystem::drawTexturedBox(const Vector2f &origin, const Vector2f &dimensions, Rocket::Core::TextureHandle texture_handle)
{
    glBindTexture(GL_TEXTURE_2D, texture_handle);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(origin.x, origin.y);

    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(origin.x+dimensions.x, origin.y);

    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(origin.x+dimensions.x, origin.y+dimensions.y);

    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(origin.x, origin.y+dimensions.y);
    glEnd();

    GLint gl_error = glGetError();

    if ( gl_error != GL_NO_ERROR )
    {
        printf("GL ERROR");
    }
}

// Private:

unsigned char * GraphicSystem::loadTGA(const QString &path, Rocket::Core::Vector2i &texture_dimensions)
{
    Rocket::Core::FileInterface *file_interface = Rocket::Core::GetFileInterface();
    Rocket::Core::FileHandle file_handle = file_interface->Open(path.toLatin1().data()); // TODO: switch to QT file handling

    if (!file_handle)
        return NULL;

    file_interface->Seek(file_handle, 0, SEEK_END);
    size_t buffer_size = file_interface->Tell(file_handle);
    file_interface->Seek(file_handle, 0, SEEK_SET);

    char* buffer = new char[buffer_size];
    file_interface->Read(buffer, buffer_size, file_handle);
    file_interface->Close(file_handle);

    TGAHeader header;
    memcpy(&header, buffer, sizeof(TGAHeader));

    int color_mode = header.bitsPerPixel / 8;
    int image_size = header.width * header.height * 4; // We always make 32bit textures 

    if (header.dataType != 2) {
        Rocket::Core::Log::Message(Rocket::Core::Log::LT_ERROR, "Only 24/32bit uncompressed TGAs are supported.");
        return NULL;
    }

    // Ensure we have at least 3 colors
    if (color_mode < 3) {
        Rocket::Core::Log::Message(Rocket::Core::Log::LT_ERROR, "Only 24 and 32bit textures are supported");
        return NULL;
    }

    const char* image_src = buffer + sizeof(TGAHeader);
    unsigned char* image_dest = new unsigned char[image_size];

    // Targa is BGR, swap to RGB and flip Y axis
    for (long y = 0; y < header.height; y++) {
        long read_index = y * header.width * color_mode;
        long write_index = ((header.imageDescriptor & 32) != 0) ? read_index : (header.height - y - 1) * header.width * color_mode;
        for (long x = 0; x < header.width; x++) {
            image_dest[write_index] = image_src[read_index+2];
            image_dest[write_index+1] = image_src[read_index+1];
            image_dest[write_index+2] = image_src[read_index];
            if (color_mode == 4)
                image_dest[write_index+3] = image_src[read_index+3];
            else
                image_dest[write_index+3] = 255;

            write_index += 4;
            read_index += color_mode;
        }
    }

    texture_dimensions.x = header.width;
    texture_dimensions.y = header.height;

    delete []buffer;

    return image_dest;
}

unsigned char * GraphicSystem::loadOther(const QString &path, Rocket::Core::Vector2i &texture_dimensions)
{
    QImage image;
    unsigned char * bytes;

    if (image.load( path )) {
        image = image.rgbSwapped();
        image = image.convertToFormat(QImage::Format_ARGB32);

        texture_dimensions.x = image.width();
        texture_dimensions.y = image.height();

        bytes = new unsigned char [ image.width() * image.height() * 4 ];

        memcpy( bytes, image.bits(), image.width() * image.height() * 4 );

        return bytes;
    }

    return NULL;
}

Vector2f GraphicSystem::scissorOffset;
float GraphicSystem::scaleFactor = 1.0f;
int GraphicSystem::width;
int GraphicSystem::height;

Rocket::Core::TextureHandle GraphicSystem::backgroundTextureHandle;
