#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setupConnections();

    _setting = new QSettings(QApplication::applicationDirPath() + "/ConnectionSetting.ini", QSettings::Format::IniFormat, this);
    _setting->setValue("/user/username","namename");
    _setting->setValue("/user/password","123456");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onSettingPageButtonClicked()
{
    ui->stackedWidget->setCurrentIndex(stackedPageIndex::settingPage);
    ui->testPageButton->setEnabled(true);
    ui->settingPageButton->setEnabled(false);
}

void MainWindow::onTestPageButtonClicked()
{
    ui->stackedWidget->setCurrentIndex(stackedPageIndex::testPage);
    ui->testPageButton->setEnabled(false);
    ui->settingPageButton->setEnabled(true);
}

void MainWindow::setupConnections()
{
    connect(ui->testPageButton, &QAbstractButton::clicked, this, &MainWindow::onTestPageButtonClicked);
    connect(ui->settingPageButton, &QAbstractButton::clicked, this, &MainWindow::onSettingPageButtonClicked);
}
