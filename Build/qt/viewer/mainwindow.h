#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "rockethelper.h"


QT_BEGIN_NAMESPACE
class QFileSystemWatcher;
class QLabel;
QT_END_NAMESPACE

class OpenedDocument;
class RenderingView;
class QDRuler;
class QLogTable;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);

    void buildCentralWidget();
    void setFilename(QString file_path) { filePath = file_path; }
    OpenedDocument *getCurrentDocument();
    static MainWindow & getInstance() { return *instance; }

public slots:
    void about();
    void openFile(QString path = QString());
    bool openDocument();
    bool openDocument(QString file_path);

    void unselectElement();
    void selectElement(Element *element);
    void setZoomLevel(float level);
    void setMousePos(QPoint pos);
    void repaintRenderingView();
    void reloadCurrentDocument();
    void newScreenSizeAction();

    void fileHasChanged(const QString &path);

private:
    void setupToolsMenu();
    void setupHelpMenu();

    void setScreenSize(int width, int height);
    void setScreenSize(int width, int height, int orientation);

    RenderingView *renderingView;
    QDRuler *horzRuler, *vertRuler;
    QLabel *labelZoom, *labelMousePos;
    QLogTable *log;
    QFileSystemWatcher *fileWatcher;
    QToolBar *toolbar;

    QString filePath;

protected:
    static MainWindow *instance;
};

#endif // MAINWINDOW_H
