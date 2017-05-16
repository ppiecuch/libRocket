#ifndef ACTIONS_H
#define ACTIONS_H

#include <QList>
#include <QString>

#include "actions.h"
#include "rockethelper.h"


class OpenedDocument;

class Action
{
public:
    enum Type
    {
        ActionTypeUnknown,
        ActionTypeSetProperty,
        ActionTypeSetInlineProperty,
        ActionTypeSetAttribute,
        ActionTypeInsertElement,
        ActionTypeGroup
    };
    Action() {};
    virtual ~Action() {};
    Type getType() const { return type; };
    virtual void apply() = 0;
    virtual void unapply() = 0;

protected:
    Type type;
};


class ActionInsertElement : public Action
{
public:
    ActionInsertElement(OpenedDocument *document, Element *element, Element *_elementToInsert);
    virtual void apply();
    virtual void unapply();

private:
    OpenedDocument *targetDocument;
    Element *targetElement;
    Element *elementToInsert;
};

class ActionSetAttribute : public Action
{
public:
    ActionSetAttribute(OpenedDocument *document, Element *element, const QString &variable_name, const QString &current_value, const QString &new_value);
    ActionSetAttribute(OpenedDocument *document, Element *element, const QString &variable_name, const QString &new_value);
    virtual void apply();
    virtual void unapply();

private:
    QString newValue;
    QString oldValue;
    QString attributeName;
    OpenedDocument *targetDocument;
    Element *targetElement;
};


class ActionSetInlineProperty : public Action
{
public:
    ActionSetInlineProperty(OpenedDocument *document, Element *element, const QString &property_name, const QString &new_value);
    virtual void apply();
    virtual void unapply();

private:
    QString newValue;
    QString oldValue;
    QString propertyName;
    OpenedDocument *targetDocument;
    Element *targetElement;
    bool propertyExists;
};


class ActionGroup : public Action
{
public:
    ActionGroup();
    ActionGroup(QList<Action*> &actionList);
    virtual void apply();
    virtual void unapply();
    void add(Action *action);

private:
    QList<Action*> actionList;
};

class ActionManager
{
public:
    ActionManager() {};
    ~ActionManager();
    static ActionManager & getInstance()
    {
        static ActionManager instance;
        return instance;
    }
    void applyNew(Action * action);
    void applyPrevious();
    void applyNext();

private:
    QList<Action*> previousActionList;
    QList<Action*> nextActionList;
};

#endif // ACTIONS_H
