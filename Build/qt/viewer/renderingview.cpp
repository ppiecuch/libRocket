#include <QDebug>
#include <QMouseEvent>
#include <QTimer>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QUrl>
#include <QLabel>
#include <QMimeData>

#include "Rocket/Core/Types.h"
#include "Rocket/Debugger.h"

#include "mainwindow.h"
#include "qtrocketsystem.h"
#include "renderingview.h"
#include "rockethelper.h"
#include "tools.h"
#include "settings.h"


RenderingView::RenderingView(QWidget *parent) : QOpenGLWidget(parent), vertRuler(NULL), horzRuler(NULL)
{
    setMouseTracking(true);
    setAcceptDrops(true);
    currentDocument = NULL;
    itMustUpdatePositionOffset = false;
    positionOffset.x=positionOffset.y=0;
    displayGrid = true;
}

void RenderingView::setRulers(QDRuler *horzRuler, QDRuler *vertRuler)
{
    this->horzRuler = horzRuler;
    this->vertRuler = vertRuler;
}

void RenderingView::setDebugVisibility(bool visible)
{
    Rocket::Debugger::SetVisible(visible);
    Settings::setValue("display_debugger", visible);
    update();
}

void RenderingView::setGridVisibility(bool visible)
{
    displayGrid = visible;
    Settings::setValue("display_grid", visible);
    update();
}

void RenderingView::keyPressEvent(QKeyEvent* event)
{
    if (!currentDocument)
        return;

    switch (event->key()) {
    case Qt::Key_Escape:
        MainWindow::getInstance().unselectElement();
        break;

    case Qt::Key_Delete:
        if (currentDocument->selectedElement) {
            currentDocument->selectedElement->GetParentNode()->RemoveChild(currentDocument->selectedElement);
            MainWindow::getInstance().unselectElement();
        }
        break;

    default:
        event->ignore();
        break;
    }
}

void RenderingView::changeCurrentDocument(OpenedDocument *document)
{
    if (currentDocument && currentDocument->rocketDocument)
        currentDocument->rocketDocument->Hide();

    currentDocument = document;

    if (document)
        document->rocketDocument->Show();

    update();
}

void RenderingView::reloadDocument()
{
    if(currentDocument->rocketDocument)
        currentDocument->rocketDocument->Hide();
    
    currentDocument->selectedElement = NULL;
    
    if(currentDocument->rocketDocument)
        RocketHelper::unloadDocument(currentDocument->rocketDocument);

    currentDocument->rocketDocument = RocketHelper::loadDocumentFromMemory(currentDocument->toPlainText());
    currentDocument->rocketDocument->RemoveReference();
    currentDocument->rocketDocument->Show();
    update();
}

void RenderingView::SetClearColor( float red, float green, float blue, float alpha )
{
    glClearColor( red, green, blue, alpha );
    update();
}

// Public slots:

void RenderingView::zoomIn()
{
    GraphicSystem::scaleFactor += 0.1f;
    if(GraphicSystem::scaleFactor > 2.0f)
    {
        GraphicSystem::scaleFactor = 2.0f;
    }
    MainWindow::getInstance().setZoomLevel(GraphicSystem::scaleFactor);
    update();
}

void RenderingView::zoomOut()
{
    GraphicSystem::scaleFactor -= 0.1f;
    if(GraphicSystem::scaleFactor < 0.5f)
    {
        GraphicSystem::scaleFactor = 0.5f;
    }
    MainWindow::getInstance().setZoomLevel(GraphicSystem::scaleFactor);
    update();
}

void RenderingView::zoomReset()
{
    GraphicSystem::scaleFactor = 1.0f;
    MainWindow::getInstance().setZoomLevel(GraphicSystem::scaleFactor);
    update();
}

// Protected:

void RenderingView::initializeGL() 
{
    qInfo() << "Initialize Graphic System";
    GraphicSystem::initialize();
    emit emitLogMsg(QString("OpenGL;%1").arg((char*)glGetString(GL_RENDERER)));
    emit emitLogMsg(QString("OpenGL;%1").arg((char*)glGetString(GL_VERSION)));
}

void RenderingView::resizeGL(int w, int h) 
{
    GraphicSystem::resize(w, h, devicePixelRatio());
}

void RenderingView::paintGL() 
{
    glDisable(GL_SCISSOR_TEST);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (currentDocument)
        RocketSystem::getInstance().getContext()->Update();

    glScalef(GraphicSystem::scaleFactor, GraphicSystem::scaleFactor, 1.0f);
    glTranslatef(positionOffset.x, positionOffset.y, 0.0f);

    GraphicSystem::scissorOffset = positionOffset;

    glEnable(GL_TEXTURE_2D);
    glDisable( GL_BLEND );
    GraphicSystem::drawBackground();
    glEnable( GL_BLEND );
    glDisable(GL_TEXTURE_2D);

    drawAxisGrid();
    if (displayGrid)
        RenderGrid(RocketSystem::getInstance().getContext()->GetDimensions().x, RocketSystem::getInstance().getContext()->GetDimensions().y, GraphicSystem::scaleFactor, 10, 10, 4, true);

    if (currentDocument)
        RocketSystem::getInstance().getContext()->Render();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_SCISSOR_TEST);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScalef(GraphicSystem::scaleFactor, GraphicSystem::scaleFactor, 1.0f);
    glTranslatef(positionOffset.x, positionOffset.y, 0.0f);

    if (!currentDocument)
        return;

    ToolManager::getInstance().getCurrentTool()->onRender();
}

void RenderingView::mousePressEvent(QMouseEvent *event) 
{
    MainWindow::getInstance().setFocus(Qt::MouseFocusReason);

    if (!currentDocument)
        return;

    // panning:
    if (event->button()==Qt::RightButton) {
        startMousePosition.x = event->x();
        startMousePosition.y = event->y();
        oldPositionOffset = positionOffset;
        itMustUpdatePositionOffset = true;
        return;
    }

    Vector2f mouse_position = getMousePositionInDocument(event->x(), event->y());

    RocketSystem::getInstance().getContext()->ProcessMouseButtonDown(event->button()-1, 0);

    ToolManager::getInstance().getCurrentTool()->onMousePress(event->button(), mouse_position);

    update();
}

void RenderingView::mouseReleaseEvent(QMouseEvent *event) 
{
    if (event->button()==Qt::RightButton) {
        itMustUpdatePositionOffset = false;
        return;
    }

    Vector2f mouse_position = getMousePositionInDocument(event->x(), event->y());

    RocketSystem::getInstance().getContext()->ProcessMouseButtonUp(event->button()-1, 0);

    ToolManager::getInstance().getCurrentTool()->onMouseRelease(event->button(), mouse_position);

    update();
}

void RenderingView::mouseMoveEvent(QMouseEvent *event) 
{
    if (horzRuler) horzRuler->setCursorPos(this->mapToGlobal(event->pos()));
    if (vertRuler) vertRuler->setCursorPos(this->mapToGlobal(event->pos()));

    emit emitMouseSize(QPoint(event->x(), event->y()));

    if (itMustUpdatePositionOffset) {
        mousePositionOffset.x=event->x()-startMousePosition.x;
        mousePositionOffset.y=event->y()-startMousePosition.y;
        positionOffset = oldPositionOffset+mousePositionOffset;
        update();
        return;
    }

    Vector2f mouse_position = getMousePositionInDocument(event->x(), event->y());

    RocketSystem::getInstance().getContext()->ProcessMouseMove(mouse_position.x,mouse_position.y, 0);

    ToolManager::getInstance().getCurrentTool()->onMouseMove(mouse_position);

    update();
}

void RenderingView::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls() && ToolManager::getInstance().getCurrentTool()->acceptsDrop())
        event->acceptProposedAction();
}

void RenderingView::dropEvent(QDropEvent *event)
{
    QString result = event->mimeData()->urls()[0].toString();
    ToolManager::getInstance().getCurrentTool()->onFileDrop(result);
}

void RenderingView::wheelEvent(QWheelEvent *event)
{
    if ( event->delta() > 0 )
    {
        zoomIn();
    }
    else
    {
        zoomOut();
    }
}

// Private:

void RenderingView::drawAxisGrid()
{
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_LINES);

    GraphicSystem::putXAxisVertices(0);
    GraphicSystem::putXAxisVertices(RocketSystem::getInstance().getContext()->GetDimensions().y);
    GraphicSystem::putYAxisVertices(0);
    GraphicSystem::putYAxisVertices(RocketSystem::getInstance().getContext()->GetDimensions().x);

    glEnd();
}


//
// ++==+==+==++==+==+==++
// ||  |  |  ||  |  |  ||
// ||  |  |  ||  |  |  ||
// ++--+--+--++--+--+--++
// ||  |  |  ||  |  |  ||
// ||  |  |  ||  |  |  ||
// ++==+==+==++==+==+==++
// ||  |  |  ||  |  |  ||
// ||  |  |  ||  |  |  ||
// ++--+--+--++--+--+--++
// ||  |  |  ||  |  |  ||
// ||  |  |  ||  |  |  ||
// ++==+==+==++==+==+==++
//

void RenderGrid(int main_window_width, int main_window_height, float scale, int rows, int columns, int subdivs, bool bg) {

    GLboolean isDepth = glIsEnabled(GL_DEPTH_TEST);
    glDisable(GL_DEPTH_TEST);

    /* background gradient */
    glBegin(GL_QUADS);
    {
        glColor3ub(194, 227, 253);
        glVertex2f(0, 0);
        glVertex2f(main_window_width, 0);
        glColor3ub(243, 250, 255);
        glVertex2f(main_window_width, main_window_height);
        glVertex2f(0, main_window_height);
    }
    glEnd();

    /* Render grid over 0..rows, 0..columns. */
    GLboolean isBlend = glIsEnabled(GL_BLEND);
    glEnable(GL_BLEND);

    /* Subdivisions */
    glColor4f(0, 0, 0, 0.25);
    // glLineStipple(0, 0x8888); glEnable(GL_LINE_STIPPLE);
    glBegin(GL_LINES);
    {
        /* Horizontal lines. */
        float stepx = float(main_window_height) / float(rows*subdivs);
        for (int i=0; i<=rows*subdivs; i++) {
            glVertex2f(0, i*stepx);
            glVertex2f(main_window_width, i*stepx);
        }

        /* Vertical lines. */
        float stepy = float(main_window_width) / float(columns*subdivs);
        for (int i=0; i<=columns*subdivs; i++) {
            glVertex2f(i*stepy, 0);
            glVertex2f(i*stepy, main_window_height);
        }
    }
    glEnd(); glDisable(GL_LINE_STIPPLE);

    /* Regular grid */
    glColor4f(0, 0, 0, 0.25);
    glBegin(GL_LINES);
    {
        /* Horizontal lines. */
        float stepx = float(main_window_height) / float(rows);
        for (int i=0; i<=rows; i++) {
            glVertex2f(0, i*stepx);
            glVertex2f(main_window_width, i*stepx);
        }

        /* Vertical lines. */
        float stepy = float(main_window_width) / float(columns);
        for (int i=0; i<=columns; i++) {
            glVertex2f(i*stepy, 0);
            glVertex2f(i*stepy, main_window_height);
        }
    }
    glEnd();
    /* Borders */
    glColor4f(0, 0, 0, 0.2);
    glLineWidth(2.5); glBegin(GL_LINES);
    {
        { glVertex2f(0, 0); glVertex2f(main_window_width, 0); }
        { glVertex2f(main_window_width, 0); glVertex2f(main_window_width, main_window_height); }
        { glVertex2f(main_window_width, main_window_height); glVertex2f(0, main_window_height); }
        { glVertex2f(0, main_window_height); glVertex2f(0, 0); }
        { glVertex2f(main_window_width/2, 0); glVertex2f(main_window_width/2, main_window_height); }
        { glVertex2f(0, main_window_height/2); glVertex2f(main_window_width, main_window_height/2); }
    }
    glEnd();

    glLineWidth(1.0); glColor4f(1, 1, 1, 1);
    if (!isBlend) glDisable(GL_BLEND);
    if (isDepth) glEnable(GL_DEPTH_TEST);
}
