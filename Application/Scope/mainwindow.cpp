#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>

static QString findPluginLibraryPath()
{
    QDir dir(QCoreApplication::applicationDirPath());
#if defined(Q_OS_WIN)
    QString libName = "example_plugin.dll";
#elif defined(Q_OS_MAC)
    QString libName = "libexample_plugin.dylib";
#else
    QString libName = "libexample_plugin.so";
#endif

    QDir searchDir(dir);
    for (int i = 0; i < 8; ++i)
    {
        QString pluginDir = searchDir.filePath("Plugins/example");
        QString candidate = QDir(pluginDir).filePath(libName);
        if (QFile::exists(candidate))
            return QDir(pluginDir).absolutePath();
        if (!searchDir.cdUp())
            break;
    }
    return QDir(dir).filePath("Plugins/example");
}

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), _ui(new Ui::MainWindow)
{
    _ui->setupUi(this);

    connect(_ui->btnPluginInfo, &QPushButton::clicked,
            [this]() { // Load plugin directory determined at runtime
                if (_pluginsLoaded)
                {
                    statusBar()->showMessage("Plugins already loaded", 3000);
                    return;
                }

                QString pluginPath = findPluginLibraryPath();
                bool ok = _pluginManager.loadPluginsFromDir(pluginPath);
                if (!ok)
                {
                    QString err = _pluginManager.lastError();
                    if (err.isEmpty())
                        err = QString("No plugins loaded from %1").arg(pluginPath);
                    _ui->txtPluginInfo->appendPlainText(QString("Failed to load plugins: %1").arg(err));
                    statusBar()->showMessage(QString("Failed to load plugins: %1").arg(err), 5000);
                    return;
                }

                // Mark loaded and disable repeated reloads
                _pluginsLoaded = true;
                _ui->btnPluginInfo->setEnabled(false);

                // Clear previous info to avoid duplicate entries
                _ui->txtPluginInfo->clear();

                // Loop over plugin and print info to status bar for demo purposes
                for (PluginInterface::AbstractProtocolPlugin* plugin : _pluginManager.plugins())
                {
                    auto meta = plugin->metadata();
                    _ui->txtPluginInfo->appendPlainText(QString("Loaded plugin: %1 (%2)").arg(meta.name).arg(meta.id));
                }
            });
}

MainWindow::~MainWindow()
{
    delete _ui;
}
