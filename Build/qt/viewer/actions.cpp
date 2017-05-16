#include "actions.h"
#include "documents.h"
#include "mainwindow.h"


ActionInsertElement::ActionInsertElement(OpenedDocument *document, Element *element, Element *element_to_insert)
{
    type = ActionTypeInsertElement;
    targetElement = element;
    targetDocument = document;
    elementToInsert = element_to_insert;
}

void ActionInsertElement::apply()
{
    targetElement->AppendChild(elementToInsert);
    targetDocument->regenerateBodyContent();
    MainWindow::getInstance().repaintRenderingView();
}

void ActionInsertElement::unapply()
{
    targetElement->RemoveChild(elementToInsert);

    if (targetDocument->selectedElement && targetDocument->selectedElement == elementToInsert)
        targetDocument->selectedElement = NULL;

    targetDocument->regenerateBodyContent();
    MainWindow::getInstance().repaintRenderingView();
}


ActionSetAttribute::ActionSetAttribute(OpenedDocument *document, Element *element, const QString &_attributeName, const QString &current_value, const QString &new_value)
{
    type = ActionTypeSetAttribute;
    targetElement = element;
    targetDocument = document;
    oldValue = current_value;
    newValue = new_value;
    attributeName = _attributeName;
}

ActionSetAttribute::ActionSetAttribute(OpenedDocument *document, Element *element, const QString &_attributeName, const QString &new_value)
{
    type = ActionTypeSetAttribute;
    targetElement = element;
    targetDocument = document;
    oldValue = element->GetAttribute<Rocket::Core::String>(_attributeName.toLatin1().data(), "").CString();
    newValue = new_value;
    attributeName = _attributeName;
}

void ActionSetAttribute::apply()
{
    targetElement->SetAttribute(attributeName.toLatin1().data(), newValue.toLatin1().data());
    targetDocument->regenerateBodyContent();
    MainWindow::getInstance().repaintRenderingView();
}

void ActionSetAttribute::unapply()
{
    targetElement->SetAttribute(attributeName.toLatin1().data(), oldValue.toLatin1().data());
    targetDocument->regenerateBodyContent();
    MainWindow::getInstance().repaintRenderingView();
}


ActionSetInlineProperty::ActionSetInlineProperty(OpenedDocument *document, Element *element, const QString &property_name, const QString &new_value)
{
    type = ActionTypeSetInlineProperty;
    targetElement = element;
    targetDocument = document;
    propertyName = property_name;
    newValue = new_value;
    propertyExists = RocketHelper::getInlinedProperty(oldValue, element,propertyName);
}

void ActionSetInlineProperty::apply()
{
    if (!propertyExists)
        RocketHelper::addInlinedProperty(targetElement, propertyName, newValue);
    else
        RocketHelper::replaceInlinedProperty(targetElement, propertyName, newValue);

    targetDocument->regenerateBodyContent();
    MainWindow::getInstance().repaintRenderingView();
}

void ActionSetInlineProperty::unapply()
{
    if (!propertyExists)
        RocketHelper::removeInlinedProperty(targetElement, propertyName);
    else
        RocketHelper::replaceInlinedProperty(targetElement, propertyName, oldValue);

    targetDocument->regenerateBodyContent();
    MainWindow::getInstance().repaintRenderingView();
}



ActionGroup::ActionGroup()
{
    type = ActionTypeGroup;
}

ActionGroup::ActionGroup(QList<Action*> &action_list)
: actionList(action_list)
{
    type = ActionTypeGroup;
}

void ActionGroup::apply()
{
    for (int i=0; i<actionList.size(); ++i) {
        actionList[i]->apply();
    }
}

void ActionGroup::unapply()
{
    for (int i=actionList.size()-1; i>=0; --i) {
        actionList[i]->unapply();
    }
}

void ActionGroup::add(Action *action)
{
    actionList.push_back(action);
}


ActionManager::~ActionManager()
{
    for(int i=0;i<previousActionList.size();++i) {
        delete previousActionList[i];
    }

    for(int i=0;i<nextActionList.size();++i) {
        delete nextActionList[i];
    }
}

void ActionManager::applyNew(Action * action)
{
    action->apply();
    previousActionList.push_back(action);
}

void ActionManager::applyPrevious()
{
    if (!previousActionList.isEmpty()) {
        Action *action = previousActionList.takeLast();
        action->unapply();
        nextActionList.push_back(action);
    }
}

void ActionManager::applyNext()
{
    if (!nextActionList.isEmpty()) {
        Action *action = nextActionList.takeLast();
        action->apply();
        previousActionList.push_back(action);
    }
}
