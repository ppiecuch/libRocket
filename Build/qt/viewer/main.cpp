
#include <QtCore/qabstractanimation.h>
#include <QtCore/qdir.h>
#include <QtCore/qmath.h>
#include <QtCore/qtimer.h>
#include <QtCore/qdatetime.h>
#include <QtCore/qsettings.h>
#include <QtCore/qpointer.h>
#include <QtCore/qscopedpointer.h>
#include <QtCore/qtextstream.h>

#include <QtWidgets/QApplication>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QCheckBox>

#include <QtCore/QLibraryInfo>

#include "qtrocketsystem.h"
#include "mainwindow.h"
#include "settings.h"
#include "tools.h"

struct Options
{
    Options()
        : originalQml(false)
        , originalQmlRaster(false)
        , maximized(false)
        , fullscreen(false)
        , transparent(false)
        , clip(false)
        , versionDetection(true)
        , slowAnimations(false)
        , quitImmediately(false)
        , resizeViewToRootItem(false)
        , multisample(false)
        , contextSharing(true)
    {
    }

    QUrl file;
    bool originalQml;
    bool originalQmlRaster;
    bool maximized;
    bool fullscreen;
    bool transparent;
    bool clip;
    bool versionDetection;
    bool slowAnimations;
    bool quitImmediately;
    bool resizeViewToRootItem;
    bool multisample;
    bool contextSharing;
    QString translationFile;
};

QFileInfoList findRmlFiles(const QString &dirName)
{
    QDir dir(dirName);

    QFileInfoList ret;
    if (dir.exists()) {
        QFileInfoList fileInfos = dir.entryInfoList(QStringList() << "*.rml",
                                                    QDir::Files | QDir::AllDirs | QDir::NoDotAndDotDot);

        foreach (QFileInfo fileInfo, fileInfos) {
            if (fileInfo.isDir())
                ret += findRmlFiles(fileInfo.filePath());
            else if (fileInfo.fileName().length() > 0 && fileInfo.fileName().at(0).isLower())
                ret.append(fileInfo);
        }
    }

    return ret;
}

static int displayOptionsDialog(Options *options)
{
    QDialog dialog;

    QFormLayout *layout = new QFormLayout(&dialog);

    QComboBox *qmlFileComboBox = new QComboBox(&dialog);
    QFileInfoList fileInfos = findRmlFiles(":/bundle") + findRmlFiles("./qmlscene-resources");

    foreach (QFileInfo fileInfo, fileInfos)
        qmlFileComboBox->addItem(fileInfo.dir().dirName() + "/" + fileInfo.fileName(), QVariant::fromValue(fileInfo));

    QCheckBox *originalCheckBox = new QCheckBox(&dialog);
    originalCheckBox->setText("Use original QML viewer");
    originalCheckBox->setChecked(options->originalQml);

    QCheckBox *fullscreenCheckBox = new QCheckBox(&dialog);
    fullscreenCheckBox->setText("Start fullscreen");
    fullscreenCheckBox->setChecked(options->fullscreen);

    QCheckBox *maximizedCheckBox = new QCheckBox(&dialog);
    maximizedCheckBox->setText("Start maximized");
    maximizedCheckBox->setChecked(options->maximized);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                                       Qt::Horizontal,
                                                       &dialog);
    QObject::connect(buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    layout->addRow("Rml file:", qmlFileComboBox);
    layout->addWidget(originalCheckBox);
    layout->addWidget(maximizedCheckBox);
    layout->addWidget(fullscreenCheckBox);
    layout->addWidget(buttonBox);

    int result = dialog.exec();
    if (result == QDialog::Accepted) {
        QVariant variant = qmlFileComboBox->itemData(qmlFileComboBox->currentIndex());
        QFileInfo fileInfo = variant.value<QFileInfo>();

        if (fileInfo.canonicalFilePath().startsWith(":"))
            options->file = QUrl("qrc" + fileInfo.canonicalFilePath());
        else
            options->file = QUrl::fromLocalFile(fileInfo.canonicalFilePath());
        options->originalQml = originalCheckBox->isChecked();
        options->maximized = maximizedCheckBox->isChecked();
        options->fullscreen = fullscreenCheckBox->isChecked();
    }
    return result;
}

static void displayFileDialog(Options *options)
{
    QString fileName = QFileDialog::getOpenFileName(0, "Open RML file", Settings::getLastPath(), "RML Files (*.rml)");
    if (!fileName.isEmpty()) {
        QFileInfo fi(fileName);
        options->file = QUrl::fromLocalFile(fi.canonicalFilePath());
        Settings::setLastPath(fi.absolutePath());
    }
}

static void usage()
{
    puts("Usage: rmlscene [options] <filename>");
    puts(" ");
    puts(" Options:");
    puts("  --maximized ............................... Run maximized");
    puts("  --fullscreen .............................. Run fullscreen");
    puts("  --transparent ............................. Make the window transparent");
    puts("  --multisample ............................. Enable multisampling (OpenGL anti-aliasing)");
    puts("  --resize-to-root .......................... Resize the window to the size of the root item");
    puts("  --quit .................................... Quit immediately after starting");
    puts("  --disable-context-sharing ................. Disable the use of a shared GL context for QtQuick Windows");
    puts("  -I <path> ................................. Add <path> to the list of import paths");
    puts("  -P <path> ................................. Add <path> to the list of plugin paths");
    puts("  -translation <translationfile> ............ Set the language to run in");

    puts(" ");
    exit(1);
}

int main(int argc, char ** argv)
{
    Options options;

    QStringList imports;
    QStringList pluginPaths;
    for (int i = 1; i < argc; ++i) {
        if (*argv[i] != '-' && QFileInfo(QFile::decodeName(argv[i])).exists()) {
            options.file = QUrl::fromLocalFile(argv[i]);
        } else {
            const QString lowerArgument = QString::fromLatin1(argv[i]).toLower();
            if (lowerArgument == QLatin1String("--maximized"))
                options.maximized = true;
            else if (lowerArgument == QLatin1String("--fullscreen"))
                options.fullscreen = true;
            else if (lowerArgument == QLatin1String("--transparent"))
                options.transparent = true;
            else if (lowerArgument == QLatin1String("--clip"))
                options.clip = true;
            else if (lowerArgument == QLatin1String("--no-version-detection"))
                options.versionDetection = false;
            else if (lowerArgument == QLatin1String("--slow-animations"))
                options.slowAnimations = true;
            else if (lowerArgument == QLatin1String("--quit"))
                options.quitImmediately = true;
           else if (lowerArgument == QLatin1String("-translation"))
                options.translationFile = QLatin1String(argv[++i]);
            else if (lowerArgument == QLatin1String("--resize-to-root"))
                options.resizeViewToRootItem = true;
            else if (lowerArgument == QLatin1String("--multisample"))
                options.multisample = true;
            else if (lowerArgument == QLatin1String("--disable-context-sharing"))
                options.contextSharing = false;
            else if (lowerArgument == QLatin1String("-i") && i + 1 < argc)
                imports.append(QString::fromLatin1(argv[++i]));
            else if (lowerArgument == QLatin1String("-p") && i + 1 < argc)
                pluginPaths.append(QString::fromLatin1(argv[++i]));
            else if (lowerArgument == QLatin1String("--help")
                     || lowerArgument == QLatin1String("-help")
                     || lowerArgument == QLatin1String("--h")
                     || lowerArgument == QLatin1String("-h"))
                usage();
        }
    }

    QApplication app(argc, argv);

    app.setApplicationName("QtRmlViewer");
    app.setOrganizationName("KomSoft");
    app.setOrganizationDomain("komsoft.ath.cx");

    if (options.file.isEmpty())
#if defined(RMLSCENE_BUNDLE)
        displayOptionsDialog(&options);
#else
        displayFileDialog(&options);
#endif

    int exitCode = 0;

    if (!options.file.isEmpty()) {
        QScopedPointer<MainWindow> window(new MainWindow);

        int winw = Settings::getInt("ScreenSizeWidth", 800);
        int winh = Settings::getInt("ScreenSizeHeight", 600);

        ToolManager::getInstance().changeCurrentTool(0);

        if(RocketSystem::getInstance().initialize(winw, winh))
        {
            if (window) {
                window->resize(winw, winh);

                if (options.fullscreen)
                    window->showFullScreen();
                else if (options.maximized)
                    window->showMaximized();
                else if (!window->isVisible())
                    window->show();

                window->setFilename(options.file.path());
                QTimer::singleShot(0, window.data(), SLOT(openDocument()));
            }

            if (options.quitImmediately)
                QMetaObject::invokeMethod(QCoreApplication::instance(), "quit", Qt::QueuedConnection);

            // Now would be a good time to inform the debug service to start listening.

            exitCode = app.exec();

            RocketSystem::getInstance().finalize();
        }
    }

    return exitCode;
}
