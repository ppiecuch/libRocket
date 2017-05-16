#include "tools.h"
#include "actions.h"
#include "documents.h"
#include "mainwindow.h"
#include "graphicsystem.h"

#include <QtOpenGL/qgl.h>
#include <QLabel>
#include <QGridLayout>

#define MARKER_INDEX_BOTTOM_RIGHT 0
#define MARKER_INDEX_TOP_RIGHT 1


Tool::Tool() : QObject(), widget(NULL), itAcceptsDrop(false) { }

Tool::~Tool() { }

void Tool::onSelect()
{
    MainWindow::getInstance().unselectElement();
    MainWindow::getInstance().repaintRenderingView();
}

// Public slots:

void Tool::onActionTriggered()
{
    ToolManager::getInstance().changeCurrentTool(this);
}

// Protected:

void Tool::renderMarkers() const
{
    foreach (const Marker &marker, markerList) {
        GraphicSystem::drawBox(marker.position - marker.extent * 0.5f, marker.extent, Color4b(255,255,255,255), true);
        GraphicSystem::drawBox(marker.position - marker.extent * 0.5f, marker.extent, Color4b(0,0,255,255), false);
    }
}

bool Tool::findMarkerFromPosition(int& markerIndex, const Vector2f &position) const
{
    for (markerIndex=0; markerIndex<markerList.size(); ++markerIndex) {
        const Vector2f & markerPosition = markerList[markerIndex].position;
        Vector2f halfExtent = markerList[markerIndex].extent * 0.5f;


        if (position.x > markerPosition.x - halfExtent.x
            && position.x < markerPosition.x + halfExtent.x
            && position.y > markerPosition.y - halfExtent.y
            && position.y < markerPosition.y + halfExtent.y
            )
        {
            return true;
        }
    }

    return false;
}

// ----------------------------------------------

ToolSelecter::ToolSelecter() : Tool()
{
    QLayout *layout;

    name = "Selecter";
    imageName = ":/res/tool_select.png";
    layout = new QGridLayout();
    layout->addWidget(new QLabel("<center><img src=':/res/tool_select.png'><br/>Tool: <b>Selecter</b></center>"));
    widget = new QWidget();
    widget->setLayout(layout);
}

void ToolSelecter::onElementClicked(Element *element)
{
    MainWindow::getInstance().selectElement(element);
}

void ToolSelecter::onRender()
{
    if(MainWindow::getInstance().getCurrentDocument() && MainWindow::getInstance().getCurrentDocument()->selectedElement)
        RocketHelper::highlightElement(MainWindow::getInstance().getCurrentDocument()->selectedElement);
}

// ----------------------------------------------

ToolDiv::ToolDiv()
: Tool(), selectedElement(NULL), itIsResizing(false), currentMarkerIndex(-1)
{
    QVBoxLayout *layout;
    QToolBar *tool_bar;

    name = "DIV tool";
    imageName = ":/res/tool_div.png";

    widget = new QWidget();
    layout = new QVBoxLayout();

    layout->addWidget(new QLabel("Insert:"));
    tool_bar = new QToolBar();
    tool_bar->addAction(QIcon(), "New", this, SLOT(insertDiv()));

    layout->addWidget(tool_bar);

    layout->addWidget(new QLabel("Modify:"));
    tool_bar = new QToolBar();
    tool_bar->addAction(QIcon(), "Expand width", this, SLOT(expandWidth()));
    tool_bar->addAction(QIcon(), "display:inline-block", this, SLOT(setInlineBlockDisplay()));
    tool_bar->addAction(QIcon(), "To the left", this, SLOT(setLeftAlignment()));
    tool_bar->addAction(QIcon(), "To the right", this, SLOT(setRightAlignment()));

    layout->addWidget(tool_bar);

    widget->setLayout(layout);
}

void ToolDiv::onElementClicked(Element *_element)
{
    if (selectedElement == _element
        || itIsResizing)
        return;

    Element *element = getDivParent(_element);

    markerList.clear();

    if (element) {
        selectedElement = element;
        MainWindow::getInstance().selectElement(element);
        setupMarkers();
        return;
    }

    selectedElement = NULL;
}

void ToolDiv::onRender()
{
    OpenedDocument *document;

    document = MainWindow::getInstance().getCurrentDocument();

    if (document) {
        glLineWidth(2.0f);
        processElement(document->rocketDocument);

        if(document->selectedElement) {
            RocketHelper::drawBoxAroundElement(document->selectedElement, Color4b(10, 10, 240, 255));
        }

        glLineWidth(1.0f);
    }

    renderMarkers();
}

void ToolDiv::onMousePress(const Qt::MouseButton /*button*/, const Vector2f &position)
{
    if (findMarkerFromPosition(currentMarkerIndex, position)) {
        initialPosition = position;
    }
}

void ToolDiv::onMouseRelease(const Qt::MouseButton, const Vector2f &position)
{
    if (itIsResizing) {
        switch (currentMarkerIndex) {
        case MARKER_INDEX_BOTTOM_RIGHT:
            RocketHelper::incrementInlinedDimensions(MainWindow::getInstance().getCurrentDocument(), selectedElement, position - initialPosition);
            break;
        case MARKER_INDEX_TOP_RIGHT:
            Vector2f difference;
            difference.x = position.x - initialPosition.x;
            difference.y = initialPosition.y - position.y;
            RocketHelper::incrementInlinedDimensions(MainWindow::getInstance().getCurrentDocument(), selectedElement, difference);
            break;
        }

        markerList.clear();
        setupMarkers();
        itIsResizing = false;
    }

}

void ToolDiv::onMouseMove(const Vector2f &position)
{
    if (itIsResizing)
        markerList[currentMarkerIndex].position = position;
}

void ToolDiv::onUnselect()
{
    markerList.clear();
}

Element *ToolDiv::getDivParent(Element *element)
{
    while (element) {
        if (element->GetTagName() == "div") {
            return  element;
        }

        element = element->GetParentNode();
    }

    return NULL;
}

// Private slots:

void ToolDiv::insertDiv()
{
    OpenedDocument *document;

    document = MainWindow::getInstance().getCurrentDocument();

    if (document && document->selectedElement)
        insertDiv(document->selectedElement);
    else if(document->rocketDocument)
        insertDiv(document->rocketDocument);
}

void ToolDiv::expandWidth()
{
    OpenedDocument *document;

    document = MainWindow::getInstance().getCurrentDocument();

    if (document && document->selectedElement) {
        ActionManager::getInstance().applyNew(new ActionSetInlineProperty(document, document->selectedElement, "width", "100%"));
    }
}

void ToolDiv::setInlineBlockDisplay()
{
    OpenedDocument *document;

    document = MainWindow::getInstance().getCurrentDocument();

    if (document && document->selectedElement) {
        ActionManager::getInstance().applyNew(new ActionSetInlineProperty(document, document->selectedElement, "display", "inline-block"));
    }
}

void ToolDiv::setLeftAlignment()
{
    OpenedDocument *document;

    document = MainWindow::getInstance().getCurrentDocument();

    if (document && document->selectedElement) {
        ActionGroup *actionGroup = new ActionGroup();
        actionGroup->add(new ActionSetInlineProperty(document, document->selectedElement, "display", "inline-block"));
        actionGroup->add(new ActionSetInlineProperty(document, document->selectedElement, "float", "left"));
        ActionManager::getInstance().applyNew(actionGroup);
    }
}

void ToolDiv::setRightAlignment()
{
    OpenedDocument *document;

    document = MainWindow::getInstance().getCurrentDocument();

    if (document && document->selectedElement) {
        ActionGroup *actionGroup = new ActionGroup();
        actionGroup->add(new ActionSetInlineProperty(document, document->selectedElement, "display", "inline-block"));
        actionGroup->add(new ActionSetInlineProperty(document, document->selectedElement, "float", "right"));
        ActionManager::getInstance().applyNew(actionGroup);
    }
}

// Private:

void ToolDiv::processElement(Element *element)
{
    if (element->GetTagName() == "div") {
        RocketHelper::drawBoxAroundElement(element, Color4b(10, 240, 10, 255));
    }

    for (int child_index=0; child_index < element->GetNumChildren(); ++child_index) {
        processElement(element->GetChild(child_index));
    }
}

void ToolDiv::insertDiv(Element *element)
{
    Rocket::Core::XMLAttributes attributes;
    attributes.Set("style", "width:50px; height:50px;");
    Element* div = Rocket::Core::Factory::InstanceElement(NULL, "div", "div", attributes);

    ActionManager::getInstance().applyNew(new ActionInsertElement(MainWindow::getInstance().getCurrentDocument(), element, div));
    MainWindow::getInstance().selectElement(div);
}

void ToolDiv::setupMarkers()
{
    OpenedDocument *document;

    document = MainWindow::getInstance().getCurrentDocument();

    if (document && document->selectedElement) {
        markerList.append(Marker(RocketHelper::getBottomRightPosition(document->selectedElement), 10.0f));
        markerList.append(Marker(RocketHelper::getTopRightPosition(document->selectedElement), 10.0f));
    }

}


// ----------------------------------------------

ToolManager::ToolManager() { }

ToolManager::~ToolManager() { }

void ToolManager::initialize()
{
    toolList.push_back(new ToolSelecter());
    toolList.push_back(new ToolDiv());
    // toolList.push_back(new ToolImage());
    // toolList.push_back(new ToolTest());
}

void ToolManager::setup(QToolBar *tool_bar, QMenu *menu)
{
    QString shortcut_string;
    Tool *tool;

    for(int tool_index=0; tool_index<toolList.size(); ++tool_index)
    {
        tool = toolList[tool_index];

        tool->setAction(tool_bar->addAction(QIcon(tool->getImageName()), tool->getName(), tool, SLOT(onActionTriggered())));

        menu->addAction(tool->getAction());

        shortcut_string = "F" + QString::number(tool_index+9);

        tool->getAction()->setShortcut(QKeySequence(shortcut_string));
        tool->getAction()->setCheckable(true);
    }
    tool_bar->addSeparator();
}

void ToolManager::changeCurrentTool(const int index)
{
    Q_ASSERT(index < toolList.size());

    if(currentTool == toolList[index])
    {
        return;
    }

    foreach(Tool *tool, toolList)
    {
        tool->getAction()->setChecked(false);
    }

    currentTool = toolList[index];
    currentTool->onSelect();
    currentTool->getAction()->setChecked(true);
}

void ToolManager::changeCurrentTool(Tool *tool)
{
    changeCurrentTool(toolList.indexOf(tool));
}
