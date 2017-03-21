#ifndef ROCKETHELPER_H
#define ROCKETHELPER_H

#include <Rocket/Core.h>
#include <QString>

class OpenedDocument;

typedef Rocket::Core::Element Element;
typedef Rocket::Core::ElementDocument RMLDocument;
typedef Rocket::Core::Vector2f Vector2f;
typedef Rocket::Core::Colourb Color4b;

class RocketHelper
{
public:
    static RMLDocument * loadDocumentFromMemory(const QString &file_content);
    static void unloadDocument(RMLDocument *rml_document);
    static void unloadAllDocument();
    static void highlightElement(Element *element);
    static void drawBoxAroundElement(Element *element, const Color4b &color);
    static void replaceInlinedProperty(Element *element, const QString &property_name, const QString &property_value);
    static void addInlinedProperty(Element *element, const QString &property_name, const QString &property_value);
    static void removeInlinedProperty(Element *element, const QString &property_name);
    static bool getInlinedProperty(QString & property_value, Element *element, const QString &property_name);
    static void incrementInlinedDimensions(OpenedDocument *document, Element *element, const Vector2f &value);
    static Vector2f getBottomRightPosition(Element *element);
    static Vector2f getTopRightPosition(Element *element);
    static Element *getElementUnderMouse();
};

#endif
