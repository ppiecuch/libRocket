#ifndef DOCUMENTS_H
#define DOCUMENTS_H

#include "rockethelper.h"

#include <QList>
#include <QFileInfo>
#include <QTreeWidget>
#include <QTreeWidgetItem>

class OpenedDocument : public QObject
{
    Q_OBJECT
public:
    OpenedDocument();
    OpenedDocument(QFileInfo file_info);

    QString toPlainText();
    QStringList getRCSSFileList();

    RMLDocument *rocketDocument;
    Element *selectedElement;
    QString highlightedString;

    void regenerateBodyContent();

private:
    QFileInfo fileInfo;

    void replaceInnerRMLFromTagName(const QString &tag_name, const QString &new_content);

    QStringList getRCSSFileList(Element *element);
    void populateHierarchyTreeView(QTreeWidget *tree);
    QTreeWidgetItem *getChildrenTree(QTreeWidgetItem *parent, Element *element);

    typedef std::pair< Rocket::Core::String, const Rocket::Core::Property* > NamedProperty;
    typedef std::vector< NamedProperty > NamedPropertyList;
    typedef std::map< Rocket::Core::PseudoClassList, NamedPropertyList > NamedPropertyMap;
};

#endif // DOCUMENTS_H
