#ifndef TOOLS_H
#define TOOLS_H


#include <QWidget>
#include <QObject>
#include <QString>
#include <QLayout>
#include <QAction>
#include <QMenu>
#include <QToolBar>

#include "tools.h"
#include "rockethelper.h"

class Tool : public QObject
{
    Q_OBJECT

public:
    struct Marker
    {
        Marker(const Vector2f &_position, const Vector2f &_extent) : position(_position), extent(_extent) {}
        Marker(const Vector2f &_position, const float _size) : position(_position), extent(_size,_size) {}
        Marker(const float _x, const float _y, const float _size) : position(_x, _y), extent(_size,_size) {}
        Vector2f position;
        Vector2f extent;
    };
    Tool();
    virtual ~Tool();
    const QString & getName() const { return name; }
    const QString & getImageName() const { return imageName; }
    QAction *getAction() { return action; }
    const QList<Marker> & getMarkerList() const;
    bool acceptsDrop() const { return itAcceptsDrop; }
    void onSelect();
    void setAction(QAction *_action) { action = _action; }
    virtual void onElementClicked(Element *){}
    virtual void onRender(){}
    virtual void onMousePress(const Qt::MouseButton, const Vector2f &) {}
    virtual void onMouseRelease(const Qt::MouseButton, const Vector2f &) {}
    virtual void onMouseMove(const Vector2f &) {}
    virtual void onUnselect() {}
    virtual void onFileDrop(const QString &) {}

public slots:
    void onActionTriggered();

protected:
    void renderMarkers() const;
    bool findMarkerFromPosition(int& markerIndex, const Vector2f &position) const;

    QString name;
    QString imageName;
    QWidget *widget;
    QAction *action;
    QList<Marker> markerList;
    bool itAcceptsDrop;
};


class ToolSelecter : public Tool
{
public:
    ToolSelecter();
    virtual void onElementClicked(Element *element);
    virtual void onRender();
};

class ToolDiv : public Tool
{
    Q_OBJECT

public:
    ToolDiv();
    virtual void onElementClicked(Element *element);
    virtual void onRender();
    virtual void onMousePress(const Qt::MouseButton button, const Vector2f &position);
    virtual void onMouseRelease(const Qt::MouseButton button, const Vector2f &position);
    virtual void onMouseMove(const Vector2f &position);
    virtual void onUnselect();
    static Element *getDivParent(Element *element);

private slots:
    void insertDiv();
    void expandWidth();
    void setInlineBlockDisplay();
    void setLeftAlignment();
    void setRightAlignment();

private:
    void processElement(Element *element);
    void insertDiv(Element *element);
    void setupMarkers();
    Element *selectedElement;
    Vector2f initialPosition;
    bool itIsResizing;
    int currentMarkerIndex;

};


class ToolManager
{
public:
    ToolManager();
    ~ToolManager();

    static ToolManager & getInstance() {
        static ToolManager instance;
        return instance;
    }
    Tool * getCurrentTool() const {
        return currentTool;
    }

    void initialize();
    void setup(QToolBar *tool_bar, QMenu *menu);
    void changeCurrentTool(const int index);
    void changeCurrentTool(Tool *tool);

private:
    QList<Tool*> toolList;
    Tool *currentTool;
};


#endif
