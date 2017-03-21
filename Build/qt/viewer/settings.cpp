#include "Settings.h"

#include <QSettings>
#include <QStringList>
#include "GraphicSystem.h"

#define APPNAME Rockete


QStringList Settings::getRecentFileList()
{
    return settings.value("File/Recents").value< QStringList >();
}

QString Settings::getProject()
{
    return settings.value("File/Project").value< QString >();
}

void Settings::setMostRecentFile(const QString &filePath)
{
    QStringList currentList;

    currentList = getRecentFileList();

    foreach(const QString &item, currentList) {
        if (item==filePath) {
            currentList.removeOne(item);
            break;
        }
    }

    currentList.push_front(filePath);

    while( currentList.count() > 30 ) {
        currentList.removeLast();
    }

    settings.setValue("File/Recents", currentList);
}

void Settings::setProject(const QString &filePath)
{
    settings.setValue("File/Project", filePath);
}

int Settings::getTabSize()
{
    // :TODO: Make a setting ;)
    return 4;
}

void Settings::setBackroundFileName(const QString &fileName)
{
    Rocket::Core::Vector2i dimensions;
    
    GraphicSystem::loadTexture( backgroundTextureHandle, dimensions, fileName.toUtf8().data() );
    settings.setValue("File/BackgroundFileName", fileName);
}

QString Settings::getBackgroundFileName()
{
    return settings.value("File/BackgroundFileName").value< QString >();
}


Rocket::Core::TextureHandle Settings::getBackroundTextureHandle()
{
    return backgroundTextureHandle;
}

void Settings::setSplitterState(const QString &splitter, const QByteArray & state)
{
    settings.setValue(QString("splitter%1Sizes").arg(splitter), state);
}

const QByteArray Settings::getSplitterState(const QString &splitter)
{
    QString key = QString("splitter%1Sizes").arg(splitter);
    if (settings.contains(key))
      return settings.value(key).toByteArray();
    else
      return QByteArray();
}

void Settings::setValue(const QString &key, const QString &value)
{
    settings.setValue(key, value);
}

void Settings::setValue(const QString &key, const int value)
{
    settings.setValue(key, value);
}

void Settings::setValue(const QString &key, const QVariant &value)
{
    settings.setValue(key, value);
}

QVariant Settings::getValue(const QString &key)
{
    return settings.value(key);
}

QString Settings::getString(const QString &key, const QString def)
{
     return settings.value(key, def).toString();
}

int Settings::getInt(const QString &key, const int def)
{
     return settings.value(key, def).toInt();
}

QSettings Settings::settings("FishingCactus", "Rockete");
Rocket::Core::TextureHandle Settings::backgroundTextureHandle = 0;
