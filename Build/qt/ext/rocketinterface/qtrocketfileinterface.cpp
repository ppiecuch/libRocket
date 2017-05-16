#include <QDebug>
#include <QString>
#include <QDirIterator>
#include <QFileInfo>

#include <Rocket/Core.h>

#include "qtrocketfileinterface.h"


RocketFileInterface::RocketFileInterface()
{

}

QStringList RocketFileInterface::GetSearchPaths()
{
    return searchPaths;
}

void RocketFileInterface::AddToSearchPaths(QString path)
{
    QFileInfo fi(path);
    if (fi.isDir())
        searchPaths << path;
    else
        searchPaths << fi.canonicalPath();
}

// Opens a file.
Rocket::Core::FileHandle RocketFileInterface::Open(const Rocket::Core::String& path)
{
    // since we load from memory, the files will not always have the good path
    // if it doesn't exists, we check for project's paths
    QString real_path = path.CString();
    QFileInfo file_info = real_path;

    if(file_info.exists()) {
        QFile *f = new QFile(real_path);
        if (f->open(QIODevice::ReadOnly))
            return (Rocket::Core::FileHandle)f;
        delete f;
        return 0;
    } else {
        QFileInfo base_file_info = file_info;
        foreach(QString path, searchPaths)
        {
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

        if(!file_info.exists())
        {
            qWarning() << "File not found:" << real_path;
        }
        else
        {
            QFile *f = new QFile(file_info.filePath());
            if (f->open(QIODevice::ReadOnly))
                return (Rocket::Core::FileHandle)f;
            delete f;
            return 0;
        }
    }

    return 0;
}

// Closes a previously opened file.
void RocketFileInterface::Close(Rocket::Core::FileHandle file)
{
    delete reinterpret_cast<QFile *>(file);
}

// Reads data from a previously opened file.
size_t RocketFileInterface::Read(void* buffer, size_t size, Rocket::Core::FileHandle file)
{
    return reinterpret_cast<QFile *>(file)->read((char *)buffer, size);
}

// Seeks to a point in a previously opened file.
bool RocketFileInterface::Seek(Rocket::Core::FileHandle file, long offset, int origin)
{
    QFile *f = reinterpret_cast<QFile *>(file);
    switch(origin) {
        case SEEK_SET: return f->seek(offset)?f->pos():-1;
        case SEEK_CUR: return f->seek(f->pos()+offset)?f->pos():-1;
        case SEEK_END: return f->seek(f->size()+offset)?f->pos():-1;
        default: qFatal("*** Seek incorrect argument.");
    }
}

// Returns the current position of the file pointer.
size_t RocketFileInterface::Tell(Rocket::Core::FileHandle file)
{
    return reinterpret_cast<QFile *>(file)->pos();
}


QStringList RocketFileInterface::searchPaths;
