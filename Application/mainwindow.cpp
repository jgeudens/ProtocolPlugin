#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), _ui(new Ui::MainWindow)
{
    _ui->setupUi(this);

    _pluginManager.loadPluginsFromDir("/home/jens/Projects/ProtocolPlugin/Application/build/plugins/cpp/example");

    // Loop over plugin and print info to status bar for demo purposes
    for (ProtocolCore::AbstractProtocolPlugin* plugin : _pluginManager.plugins())
    {
        auto meta = plugin->metadata();
        _ui->statusbar->showMessage(QString("Loaded plugin: %1 (%2)").arg(meta.name).arg(meta.id), 5000);
    }

    connect(_ui->btnPluginInfo, &QPushButton::clicked,
            [this]() { _ui->statusbar->showMessage("Plugin Info requested", 1000); });
}

MainWindow::~MainWindow()
{
    delete _ui;
}
