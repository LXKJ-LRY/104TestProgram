#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    ui->testPageButton->setEnabled(false);
    setupConnections();

    _dbThread = new QThread(this);
    _dbDataHandler = DBDataHandler::instance();
    _dbDataHandler->moveToThread(_dbThread);
    emit refreshSettingComboBox();

    _setting = new QSettings(QApplication::applicationDirPath() + "/ConnectionSetting.ini", QSettings::Format::IniFormat, this);
    _setting->setValue("/ConnectionSetting/SettingName","配置");
    _setting->setValue("/ConnectionSetting/LocalMasterAddr","0.0.0.0");
    _setting->setValue("/ConnectionSetting/LocalPort","2404");
    _setting->setValue("/ConnectionSetting/RemoteSlaveAddr","0.0.0.0");
    _setting->setValue("/ConnectionSetting/RemotePort","2404");
}

MainWindow::~MainWindow()
{
    _dbDataHandler->release();
    delete ui;
}

void MainWindow::setupConnections()
{
    connect(ui->testPageButton, &QAbstractButton::clicked, this, &MainWindow::onTestPageButtonClicked);
    connect(ui->settingPageButton, &QAbstractButton::clicked, this, &MainWindow::onSettingPageButtonClicked);
    connect(ui->deviceListButton, &QAbstractButton::clicked, this, &MainWindow::onDeviceListButtonClicked);
    connect(ui->newSettingButton, &QAbstractButton::clicked, this, &MainWindow::onNewSettingButtonClicked);
    connect(this, &MainWindow::refreshSettingComboBox, this, &MainWindow::onRefreshSettingComboBox);
    connect(ui->settingComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &MainWindow::onSettingComboBoxchanged);
}


void MainWindow::onSettingPageButtonClicked()
{
    qDebug() << "setting";
    ui->stackedWidget->setCurrentIndex(stackedPageIndex::settingPage);
    ui->testPageButton->setEnabled(true);
    ui->settingPageButton->setEnabled(false);
}

void MainWindow::onTestPageButtonClicked()
{
    qDebug() << "test";
    ui->stackedWidget->setCurrentIndex(stackedPageIndex::testPage);
    ui->testPageButton->setEnabled(false);
    ui->settingPageButton->setEnabled(true);
}

void MainWindow::onDeviceListButtonClicked()
{
    auto dialog = new QDialog(this);
    dialog->setMinimumSize(800, 600);
    dialog->show();
}

void MainWindow::onNewSettingButtonClicked()
{
    ui->newSettingButton->setEnabled(false);
    _dbDataHandler->addNewSettingToDB();
    emit refreshSettingComboBox();
    ui->newSettingButton->setEnabled(true);
}

void MainWindow::onRefreshSettingComboBox()
{
    ui->settingComboBox->clear();
    QList<QString> settingsNameList;
    _dbDataHandler->onRefreshSettingComboBox(settingsNameList);
    if(settingsNameList.isEmpty())
    {
        qDebug() << "暂无相关配置";
        return;
    }
    for(auto settingName : settingsNameList)
    {
        ui->settingComboBox->addItem(settingName);
    }
    qDebug() << "刷新 settingComboBox 成功";
}

void MainWindow::onSettingComboBoxchanged(int index)
{
    qDebug() << "index : " << index;
    settingComboCurrentIndex = index;
}
