#include "documents.h"

#include "qtrocketsystem.h"
#include "rockethelper.h"

#include <QDebug>
#include <QTextStream>
#include <QMessageBox>


OpenedDocument::OpenedDocument() : selectedElement(NULL)
{
}
OpenedDocument::OpenedDocument(QFileInfo file_info) : selectedElement(NULL), fileInfo(file_info)
{
}

QString OpenedDocument::toPlainText()
{
    QFile file(fileInfo.filePath()); if (file.open(QIODevice::ReadOnly))
        return file.readAll();
    qWarning() << "Cannot read" << fileInfo.filePath();
    return QString();
}

QStringList OpenedDocument::getRCSSFileList()
{
    QStringList return_list;

    return_list = getRCSSFileList(rocketDocument);

    return_list.removeDuplicates();
    return return_list;
}

QStringList OpenedDocument::getRCSSFileList(Element *element)
{
    NamedPropertyMap property_map;
    int property_index = 0;
    Rocket::Core::String property_name;
    Rocket::Core::PseudoClassList property_pseudo_classes;
    const Rocket::Core::Property* property;

    QStringList return_list;

    while (element->IterateProperties(property_index, property_pseudo_classes, property_name, property))
    {
        if(QString(property->source.CString()).contains(".rcss"))
            return_list.append(property->source.CString());
    }

    for (int i = 0; i < element->GetNumChildren(); i++)
    {
        return_list.append(getRCSSFileList(element->GetChild(i)));
    }

    return_list.removeDuplicates();
    return return_list;
}


void OpenedDocument::replaceInnerRMLFromTagName(const QString &tag_name, const QString &new_content)
{
    QString content;
    QString tag;
    QString new_text = new_content;
    QString end_string;
    int tag_index;
    int start_index;
    int end_index;

    tag = "<" + tag_name;

    content = toPlainText();
    end_string = "</" + tag_name + ">";
    tag_index = content.indexOf(tag);

    Q_ASSERT(tag_index != -1);

    start_index = content.indexOf('>',tag_index) + 1;
    end_index = content.indexOf(end_string,start_index+1);

    content.replace(start_index, end_index - start_index, new_text);
}

void OpenedDocument::regenerateBodyContent()
{
    Rocket::Core::String rocket_string_content;
    Element *content_element;

    RocketSystem::getInstance().getContext()->Update();

    content_element = rocketDocument->GetElementById("content");

    if(!content_element)
    {
        printf("WARNING: nothing changed - document without template.\n");
        return;
    }
    content_element->GetInnerRML(rocket_string_content);

    replaceInnerRMLFromTagName("body", QString(rocket_string_content.CString()));
}

void OpenedDocument::populateHierarchyTreeView(QTreeWidget *tree)
{
    tree->clear();
    tree->addTopLevelItem(getChildrenTree(NULL, rocketDocument));
}

QTreeWidgetItem *OpenedDocument::getChildrenTree(QTreeWidgetItem *parent, Element *element)
{
    QStringList list;
    list << element->GetTagName().CString();
    list << element->GetId().CString();
    list << element->GetClassNames().CString();
    QTreeWidgetItem *item = new QTreeWidgetItem(parent, list);
    item->setTextColor(1, QColor::fromRgb(0,0,255));
    item->setTextColor(2, QColor::fromRgb(255,0,0));
    item->setData(0, Qt::UserRole, qVariantFromValue<void*>(element));
    QList<QTreeWidgetItem *> items;

    for (int i = 0; i < element->GetNumChildren(); i++)
    {
        if(element->GetChild(i)->GetTagName().CString()[0] != '#')
            items.append(getChildrenTree(item, element->GetChild(i)));
    }

    item->addChildren(items);
    return item;
}
