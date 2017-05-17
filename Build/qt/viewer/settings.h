#ifndef SETTINGS_H
#define SETTINGS_H

#include <QStringList>
#include <QSettings>
#include <QString>
#include <Rocket/Core.h>

class Settings
{
public:
    static QStringList getRecentFileList();
    static void setMostRecentFile(const QString &filePath);
    static QString getLastPath();
    static void setLastPath(const QString &filePath);

    static void setValue(const QString &key, const QString &value);
    static void setValue(const QString &key, const int value);
    static void setValue(const QString &key, const QVariant &value);
    static QVariant getValue(const QString &key);
    static QString getString(const QString &key, const QString def = "");
    static int getInt(const QString &key, const int def = 0);

    static QSettings *instance() { return &settings; }
private:
    static QSettings settings;
};

#define DEF_FONT_INFO "Courier",10
#define appSettings Settings::instance()

#endif
