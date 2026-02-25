#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), _ui(new Ui::MainWindow)
{
    _ui->setupUi(this);

    connect(_ui->btnPluginInfo, &QPushButton::clicked,
            [this]() { _ui->statusbar->showMessage("Plugin Info requested", 1000); });
}

MainWindow::~MainWindow()
{
    delete _ui;
}
