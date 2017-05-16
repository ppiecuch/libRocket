#include "settings.h"

#include <QDir>
#include <QSettings>
#include <QStringList>


QString Settings::getLastPath()
{
    QString v = settings.value("File/LastPath",QDir::homePath()).value< QString >();
    if (QDir().exists(v))
        return v;
    else
        return QDir::homePath();
}

void Settings::setLastPath(const QString &path)
{
    settings.setValue("File/LastPath", path);
}

QStringList Settings::getRecentFileList()
{
    return settings.value("File/Recents").value< QStringList >();
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

QSettings Settings::settings;
