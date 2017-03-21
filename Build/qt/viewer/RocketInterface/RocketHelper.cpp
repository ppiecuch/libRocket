#include "RocketHelper.h"

#include <QtCore>
#include <Rocket/Core.h>
#include "RocketSystem.h"
#include "ActionManager.h"
#include "ActionGroup.h"
#include "ActionSetInlineProperty.h"
#include <QString>
#include "OpenedDocument.h"

RMLDocument * RocketHelper::loadDocumentFromMemory(const QString &file_content)
{
    return RocketSystem::getInstance().getContext()->LoadDocumentFromMemory(file_content.toUtf8().data());
}

void RocketHelper::unloadDocument(RMLDocument * rml_document)
{
    RocketSystem::getInstance().getContext()->UnloadDocument(rml_document);
    Rocket::Core::Factory::ClearStyleSheetCache();
    RocketSystem::getInstance().getContext()->Update(); // force the actual unload instead of setting a flag
}

void RocketHelper::unloadAllDocument()
{
    RocketSystem::getInstance().getContext()->UnloadAllDocuments();
    Rocket::Core::Factory::ClearStyleSheetCache();
    RocketSystem::getInstance().getContext()->Update();
}

void RocketHelper::highlightElement(Element * element)
{
    Q_ASSERT(element);

    for (int i = 0; i < element->GetNumBoxes(); i++)
    {
        const Rocket::Core::Box & element_box = element->GetBox(i);

        // Content area:
        GraphicSystem::drawBox(element->GetAbsoluteOffset(Rocket::Core::Box::BORDER) + element_box.GetPosition(Rocket::Core::Box::CONTENT), element_box.GetSize(), Color4b(158, 214, 237, 128));

        // Padding area:
        GraphicSystem::drawBox(element->GetAbsoluteOffset(Rocket::Core::Box::BORDER) + element_box.GetPosition(Rocket::Core::Box::PADDING), element_box.GetSize(Rocket::Core::Box::PADDING), element->GetAbsoluteOffset(Rocket::Core::Box::BORDER) + element_box.GetPosition(Rocket::Core::Box::CONTENT), element_box.GetSize(), Color4b(135, 122, 214, 128));

        // Border area:
        GraphicSystem::drawBox(element->GetAbsoluteOffset(Rocket::Core::Box::BORDER) + element_box.GetPosition(Rocket::Core::Box::BORDER), element_box.GetSize(Rocket::Core::Box::BORDER), element->GetAbsoluteOffset(Rocket::Core::Box::BORDER) + element_box.GetPosition(Rocket::Core::Box::PADDING), element_box.GetSize(Rocket::Core::Box::PADDING), Color4b(133, 133, 133, 128));

        // Border area:
        GraphicSystem::drawBox(element->GetAbsoluteOffset(Rocket::Core::Box::BORDER) + element_box.GetPosition(Rocket::Core::Box::MARGIN), element_box.GetSize(Rocket::Core::Box::MARGIN), element->GetAbsoluteOffset(Rocket::Core::Box::BORDER) + element_box.GetPosition(Rocket::Core::Box::BORDER), element_box.GetSize(Rocket::Core::Box::BORDER), Color4b(240, 255, 131, 128));
    }
}

void RocketHelper::drawBoxAroundElement(Element *element, const Color4b &color)
{
    Q_ASSERT(element);

    for (int i = 0; i < element->GetNumBoxes(); i++)
    {
        const Rocket::Core::Box & element_box = element->GetBox(i);

        GraphicSystem::drawBox(element->GetAbsoluteOffset(Rocket::Core::Box::BORDER) + element_box.GetPosition(Rocket::Core::Box::CONTENT), element_box.GetSize(), color, false);
    }
}

void RocketHelper::replaceInlinedProperty(Element *element,const QString &property_name, const QString &property_value)
{
    QString properties;
    int property_index;
    int start_index;
    int end_index;

    properties = (element->GetAttribute<Rocket::Core::String>("style","")).CString();

    if(properties.isEmpty())
        return;
    // :TODO: Ensure found property is the right one.

    property_index = properties.indexOf(property_name);
    Q_ASSERT(property_index != -1);
    start_index = properties.indexOf(':',property_index);
    Q_ASSERT(start_index != -1);
    end_index = properties.indexOf(';',start_index);
    Q_ASSERT(end_index != -1);

    properties.replace(start_index + 1,end_index - start_index - 1,property_value);

    element->SetAttribute("style",properties.toLatin1().data());
}

void RocketHelper::addInlinedProperty(Element *element,const QString &property_name, const QString &property_value)
{
    QString properties;
    QString string_to_append;

    properties = (element->GetAttribute<Rocket::Core::String>("style","")).CString();

    string_to_append = property_name;
    string_to_append.append(":");
    string_to_append.append(property_value);
    string_to_append.append(";");

    properties.append(string_to_append);

    element->SetAttribute("style", properties.toLatin1().data());
}

void RocketHelper::removeInlinedProperty(Element *element, const QString &property_name)
{
    QString properties;
    int property_index;
    int end_index;

    properties = (element->GetAttribute<Rocket::Core::String>("style","")).CString();

    property_index = properties.indexOf(property_name);
    Q_ASSERT(property_index != -1);
    end_index = properties.indexOf(';',property_index);
    Q_ASSERT(end_index != -1);

    properties.remove(property_index, property_index - end_index - 1);
    element->SetAttribute("style",properties.toLatin1().data());
}

bool RocketHelper::getInlinedProperty(QString & property_value, Element *element, const QString &property_name)
{
    QString properties;
    int property_index;
    int start_index;
    int end_index;

    properties = (element->GetAttribute<Rocket::Core::String>("style","")).CString();

    property_index = properties.indexOf(property_name);

    if (property_index != -1)
    {
        Q_ASSERT(property_index != -1);
        start_index = properties.indexOf(':', property_index);
        Q_ASSERT(start_index != -1);
        end_index = properties.indexOf(';', start_index);
        Q_ASSERT(end_index != -1);

        property_value = properties.mid(start_index + 1, end_index - start_index - 1);

        return true;
    }

    return false;
}

void RocketHelper::incrementInlinedDimensions(OpenedDocument *document, Element *element, const Vector2f &value)
{
    float width;
    float height;

    width = element->GetProperty<float>("width");
    height = element->GetProperty<float>("height");

    width += value.x;
    height += value.y;

    // :TODO: Do something about minimum values.

    if (width < 10)
        width = 10;

    if (height < 10)
        height = 10;

    ActionGroup *actionGroup = new ActionGroup();

    actionGroup->add(new ActionSetInlineProperty(document,element,"width", QString::number(width)));
    actionGroup->add(new ActionSetInlineProperty(document,element,"height", QString::number(height)));

    ActionManager::getInstance().applyNew(actionGroup);
}

Vector2f RocketHelper::getBottomRightPosition(Element *element)
{
    Vector2f result;

    result.x = 0.0f;
    result.y = 0.0f;

    for (int i = 0; i < element->GetNumBoxes(); i++) {
        const Rocket::Core::Box & element_box = element->GetBox(i);
        result = element->GetAbsoluteOffset(Rocket::Core::Box::BORDER) + element_box.GetPosition(Rocket::Core::Box::CONTENT) + element_box.GetSize();
    }

    return result;
}

Vector2f RocketHelper::getTopRightPosition(Element *element)
{
    Vector2f result;

    result.x = 0.0f;
    result.y = 0.0f;

    for (int i = 0; i < element->GetNumBoxes(); i++) {
        const Rocket::Core::Box & element_box = element->GetBox(i);
        result = element->GetAbsoluteOffset(Rocket::Core::Box::BORDER) + element_box.GetPosition(Rocket::Core::Box::CONTENT);
        result.x += element_box.GetSize().x;
    }

    return result;
}

Element *RocketHelper::getElementUnderMouse()
{
    return RocketSystem::getInstance().getContext()->GetHoverElement();
}
