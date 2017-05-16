#include <QtCore>
#include <QString>

#include <Rocket/Core.h>

#include "actions.h"
#include "qtrocketsystem.h"
#include "graphicsystem.h"
#include "rockethelper.h"
#include "qtplist/PListParser.h"

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

QString RocketHelper::readSpriteSheetInfo(TexturesAtlasInfo &texturesAtlasInf, const QString &texture)
{
    // 1. texpack catalog info:
    bool valid_found;
    QImage image(texture);
    QFileInfo tinfo(texture);

    texturesAtlasInf[tinfo.absoluteFilePath()][tinfo.fileName()] = QRect(0, 0, image.width(), image.height()); // texture atlas info

    QString cat_filename = tinfo.absolutePath()+QDir::separator()+QString(tinfo.completeBaseName()+".cat");
    QFile cat_file( cat_filename);
    QTextStream stm(&cat_file);
    QString ln;
    if (!cat_file.open(QIODevice::ReadOnly))
        goto format2;
    valid_found = false; while (stm.readLineInto(&ln)) {
        unsigned index;
        char holder[255],texname[255], texpath[255];
        unsigned format, bpp;
        char rotated;
        unsigned left, top, right, bottom;
        unsigned cl, cr, ct, cb;
        float cl_r, cr_r, ct_r, cb_r;
        float left_r, right_r, top_r, bottom_r, width_r, height_r;
        struct {
            unsigned w, h;
        } rect;
        int pnum = sscanf( ln.toUtf8().constData(), "%d;%255s;%255s;%255s;%c;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%f;%f;%f;%f;%f;%f;%f;%f;%d;%d;%f;%f;",
                    /*  0 */ &index,
                    /*  1 */ holder,
                    /*  2 */ texname,
                    /*  3 */ texpath,
                    /*  4 */ &rotated,
                    /*  5 */ &format,
                    /*  6 */ &bpp,
                    /*  7 */ &cl,
                    /*  8 */ &cr,
                    /*  9 */ &ct,
                    /* 10 */ &cb,
                    /* 11 */ &left,
                    /* 12 */ &top,
                    /* 13 */ &right,
                    /* 14 */ &bottom,
                    /* 15 */ &cl_r,
                    /* 16 */ &cr_r,
                    /* 17 */ &ct_r,
                    /* 18 */ &cb_r,
                    /* 19 */ &left_r,
                    /* 20 */ &top_r,
                    /* 21 */ &right_r,
                    /* 22 */ &bottom_r,
                    /* 23 */ &rect.w,
                    /* 24 */ &rect.h,
                    /* 25 */ &width_r,
                    /* 26 */ &height_r );
        if (pnum > 4) {
            texturesAtlasInf[tinfo.absoluteFilePath()][texname] = QRect(left, bottom, right-left, top-bottom); // update texture atlas info
            valid_found = true;
        }
    };
    if (valid_found) {
        return cat_filename;
    } // if no valid entry found, continue with next format

    // <?xml version="1.0" encoding="UTF-8"?>
    // <!DOCTYPE plist PUBLIC "-//Apple Computer//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
    // <plist version="1.0">
    //     <dict>
    //         <key>frames</key>
    //         <dict>
    //             <key>Icon.png</key>
    //             <dict>
    //                 <key>frame</key>
    //                 <string>{{2,2},{57,57}}</string>
    //                 <key>offset</key>
    //                 <string>{0,0}</string>
    //                 <key>rotated</key>
    //                 <false/>
    //                 <key>sourceColorRect</key>
    //                 <string>{{0,0},{57,57}}</string>
    //                 <key>sourceSize</key>
    //                 <string>{57,57}</string>
    //             </dict>
    //         </dict>
    //         <key>metadata</key>
    //         <dict>
    //             <key>format</key>
    //             <integer>2</integer>
    //             <key>realTextureFileName</key>
    //             <string>nonencryptedAtlas.pvr.ccz</string>
    //             <key>size</key>
    //             <string>{64,64}</string>
    //             <key>smartupdate</key>
    //             <string>$TexturePacker:SmartUpdate:5b30a75137a4f533396670236d41f11c$</string>
    //             <key>textureFileName</key>
    //             <string>nonencryptedAtlas.pvr.ccz</string>
    //         </dict>
    //     </dict>
    // </plist>
format2:
    // 2. cocos2d plist info:
    QString plist_filename = tinfo.absolutePath()+QDir::separator()+QString(tinfo.completeBaseName()+".plist");
    QFile plistf(plist_filename);
    PListParser plist;
    const QVariant &result = plist.parsePList(&plistf);
    if (!result.isValid())
        goto format3;
    else {
        valid_found = false;

        const QVariantMap &map = result.toMap(); if (map.contains("frames")) {
            const QVariantMap &frames = map["frames"].toMap();
            foreach(const QString &tname, frames.keys()) {
                const QVariantMap &frame = frames.value(tname).toMap();
                const QString rect_s = frame["frame"].toString();
                int l, b, w, h, pnum = sscanf( rect_s.toUtf8().constData(), "{{%d,%d},{%d,%d}}", &l, &b, &w, &h); if (pnum != 4)
                    qInfo("Invalid rect data %s", rect_s.toUtf8().constEnd());
                else {
                    texturesAtlasInf[tinfo.absoluteFilePath()][tname] = QRect(l, b, w, h); // update texture atlas info
                    valid_found = true;
                }
            }
        }
    }
    if (valid_found) {
        return plist_filename;
    } // if no valid entry found, continue with next format

    // ;Sprite Monkey Coordinates, UTF-8
    // ;Transparency, Sprite Sheet Name, Image Width, Image Height
    // Alpha,/Users/jbaker/Desktop/elf_run.png,1024,1024
    // ;Image/Frame Name, Clip X, Clip Y, Clip Width, Clip Height
    // elf run 00001,0,0,256,256
    // elf run 00002,256,0,256,256
    // elf run 00003,512,0,256,256
    // elf run 00004,768,0,256,256
    // elf run 00005,0,256,256,256
    // elf run 00006,256,256,256,256
    // elf run 00007,512,256,256,256
    // elf run 00008,768,256,256,256
    // elf run 00009,0,512,256,256
    // elf run 00010,256,512,256,256
    // elf run 00011,512,512,256,256
    // elf run 00012,768,512,256,256
    // elf run 00013,0,768,256,256
    // elf run 00014,256,768,256,256
    // elf run 00015,512,768,256,256
format3:
    // 3. smc
    QString smc_filename = tinfo.absolutePath()+QDir::separator()+QString(tinfo.completeBaseName()+".smc");
    FILE *smc_file = fopen( smc_filename.toUtf8().constData(), "r" );
    if (!smc_file)
        return "";
    valid_found = false;

    // no catalog found or no valid entries.
    return "";
}
