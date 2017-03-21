#ifndef RENDERINGVIEW_H
#define RENDERINGVIEW_H

#include <QtOpenGL/QGLWidget>
#include <Rocket/Core.h>
#include "OpenedDocument.h"
#include "GraphicSystem.h"
#include <QDropEvent>
#include <QDragEnterEvent>

class QDRuler;
class QLabel;
class RenderingView : public QGLWidget 
{
    Q_OBJECT

public:
    RenderingView(QWidget *parent = NULL);
    void setRulers(QDRuler *horzRuler, QDRuler *vertRuler);
    void setPosLabel(QLabel *label);
    void keyPressEvent(QKeyEvent *event);
    void changeCurrentDocument(OpenedDocument *document);
    OpenedDocument *getCurrentDocument(){return currentDocument;};
    void reloadDocument();
    void SetClearColor(float red, float green, float blue, float alpha);

public slots:
    void zoomIn();
    void zoomOut();
    void zoomReset();
    void setDebugVisibility(bool visible);
    void setGridVisibility(bool visible);

protected:
    void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dropEvent(QDropEvent *event);
    virtual void wheelEvent(QWheelEvent *event);

private:
    void drawAxisGrid();
    void drawBackground();

    inline Vector2f getMousePositionInDocument(const int x, const int y) const {
        Vector2f result;

        result.x = x / GraphicSystem::scaleFactor - int(positionOffset.x);
        result.y = y / GraphicSystem::scaleFactor - int(positionOffset.y);

        return result;
    }

    OpenedDocument *currentDocument;
    Vector2f positionOffset;
    Vector2f oldPositionOffset;
    Vector2f startMousePosition;
    Vector2f mousePositionOffset;
    bool itMustUpdatePositionOffset;

    QDRuler *vertRuler, *horzRuler;
    QLabel *posLabel;

    bool displayGrid;
};

#endif
