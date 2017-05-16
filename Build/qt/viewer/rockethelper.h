#ifndef ROCKETHELPER_H
#define ROCKETHELPER_H

#include <Rocket/Core.h>
#include <QDebug>
#include <QMap>
#include <QRect>
#include <QString>

class OpenedDocument;

typedef Rocket::Core::Element Element;
typedef Rocket::Core::ElementDocument RMLDocument;
typedef Rocket::Core::Vector2f Vector2f;
typedef Rocket::Core::Colourb Color4b;

typedef QMap<QString, QMap<QString, QRect> > TexturesAtlasInfo;

struct PatchCuttingInfo {
    int left, top, right, bottom;
    PatchCuttingInfo() : left(0), top(0), right(0), bottom(0) { }
    PatchCuttingInfo(int l, int t, int r, int b) : left(l), top(t), right(r), bottom(b) { }
    PatchCuttingInfo(const QString &inf) {
        int n = sscanf(inf.toLatin1().constData(), "%d;%d;%d;%d", &left, &top, &right, &bottom);
        if (n!=4)
            qDebug() << "Invalid cutting data: " << inf;
    }
    operator const QString() const { return QString("%1;%2;%3;%4").arg(left).arg(top).arg(right).arg(bottom); }
    bool isEmpty() const { return (left == 0 && top == 0 && right == 0 && bottom == 0); }
};

class RocketHelper
{
public:
    static RMLDocument * loadDocumentFromMemory(const QString &file_content);
    QString readSpriteSheetInfo(TexturesAtlasInfo &texturesAtlasInf, const QString &texture);
    static void unloadDocument(RMLDocument *rml_document);
    static void unloadAllDocument();
    static void highlightElement(Element *element);
    static void drawBoxAroundElement(Element *element, const Color4b &color);
    static void incrementInlinedDimensions(OpenedDocument *document, Element *element, const Vector2f &value);
    static void replaceInlinedProperty(Element *element, const QString &property_name, const QString &property_value);
    static void addInlinedProperty(Element *element, const QString &property_name, const QString &property_value);
    static void removeInlinedProperty(Element *element, const QString &property_name);
    static bool getInlinedProperty(QString & property_value, Element *element, const QString &property_name);
    static Vector2f getBottomRightPosition(Element *element);
    static Vector2f getTopRightPosition(Element *element);
    static Element *getElementUnderMouse();
};

#endif
