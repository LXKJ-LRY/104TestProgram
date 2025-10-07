#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "memory"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);
    ui->testPageButton->setEnabled(false);

    _dbThread = new QThread(this);
    _dbDataHandler = DBDataHandler::instance();
    _dbDataHandler->moveToThread(_dbThread);
    _dbThread->start();

    setupConnections();

    RefreshSettingComboBox();

    _setting = new QSettings(QApplication::applicationDirPath() + "/ConnectionSetting.ini", QSettings::Format::IniFormat, this);
    _setting->setValue("/ConnectionSetting/SettingName","新的配置");
    _setting->setValue("/ConnectionSetting/LocalMasterAddr","127.0.0.1");
    _setting->setValue("/ConnectionSetting/LocalPort","2404");
    _setting->setValue("/ConnectionSetting/RemoteSlaveAddr","127.0.0.1");
    _setting->setValue("/ConnectionSetting/RemotePort","2404");
}

MainWindow::~MainWindow()
{
    if (_dbThread != nullptr)
    {
        _dbThread->quit();
        _dbThread->wait();
    }
    _dbDataHandler->release();
    delete ui;
}

void MainWindow::setupConnections()
{
    connect(ui->testPageButton, &QAbstractButton::clicked, this, &MainWindow::onTestPageButtonClicked);
    connect(ui->settingPageButton, &QAbstractButton::clicked, this, &MainWindow::onSettingPageButtonClicked);
    connect(ui->deviceListButton, &QAbstractButton::clicked, this, &MainWindow::onDeviceListButtonClicked);
    connect(ui->settingComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &MainWindow::onSettingComboBoxchanged);

    connect(ui->newSettingButton, &QAbstractButton::clicked, this, &MainWindow::onNewSettingButtonClicked);
    connect(this, &MainWindow::addNewSettingToDB, _dbDataHandler, &DBDataHandler::onAddNewSettingToDB, Qt::QueuedConnection);
    connect(_dbDataHandler, &DBDataHandler::addNewSettingToDBFinished, this, &MainWindow::onAddNewSettingToDBFinished, Qt::QueuedConnection);

    connect(ui->deleteSettingButton, &QAbstractButton::clicked, this, &MainWindow::onDeleteSettingButtonClicked);
    connect(ui->saveSettingButton, &QAbstractButton::clicked, this, &MainWindow::onSaveSettingButtonClicked);

    connect(this, &MainWindow::refreshComboBoxFromDB, _dbDataHandler, &DBDataHandler::onRefreshSettingComboBox, Qt::QueuedConnection);
    connect(_dbDataHandler, &DBDataHandler::querySettingsNameFinished, this, &MainWindow::onQuerySettingsNameFinished, Qt::QueuedConnection);
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
    std::unique_ptr<QDialog> dialog(new QDialog(this));
    dialog->setMinimumSize(800, 600);
    dialog->exec();
}

void MainWindow::onNewSettingButtonClicked()
{
    ui->newSettingButton->setEnabled(false);
    ui->saveSettingButton->setEnabled(false);
    ui->deleteSettingButton->setEnabled(false);
    qDebug() << "hello1";
    emit addNewSettingToDB();

}

void MainWindow::onAddNewSettingToDBFinished()
{
    qDebug() << "hello4";
    RefreshSettingComboBox();
    ui->newSettingButton->setEnabled(true);
    ui->saveSettingButton->setEnabled(true);
    ui->deleteSettingButton->setEnabled(true);
}

void MainWindow::RefreshSettingComboBox()
{
    ui->settingComboBox->clear();
    emit refreshComboBoxFromDB(settingsNameList);
}



void MainWindow::onSettingComboBoxchanged(int index)
{
    if(index == -1)
    {
        qDebug() << " 刷新 settingCombo 时的清空阶段,index = -1，跳过此 onSettingComboBoxchanged ";
        return;
    }
    qDebug() << "index : " << index;
    settingComboCurrentIndex = index;
    QString singleSettingName = ui->settingComboBox->currentText();
    static QList<QString> singleSettingInfo;
    _dbDataHandler->querySingleSettingInfo(singleSettingName, singleSettingInfo);

    if (singleSettingInfo.size() >= 5)
    {
        ui->nameEdit->setText(singleSettingInfo[0]);
        ui->localMasterAddrEdit->setText(singleSettingInfo[1]);
        ui->localMasterPortEdit->setText(singleSettingInfo[2]);
        ui->remoteSlaveAddrEdit->setText(singleSettingInfo[3]);
        ui->remoteSlavePortEdit->setText(singleSettingInfo[4]);
    }
    else
    {
        ui->localMasterAddrEdit->clear();
        ui->localMasterPortEdit->clear();
        ui->remoteSlaveAddrEdit->clear();
        ui->remoteSlavePortEdit->clear();
        qDebug() << "配置查询结果不足 4 个字段，已清空输入框";
    }

    singleSettingName.clear();
    singleSettingInfo.clear();

}

void MainWindow::onDeleteSettingButtonClicked()
{
    ui->newSettingButton->setEnabled(false);
    ui->saveSettingButton->setEnabled(false);
    ui->deleteSettingButton->setEnabled(false);

    RefreshSettingComboBox();
    ui->newSettingButton->setEnabled(true);
    ui->saveSettingButton->setEnabled(true);
    ui->deleteSettingButton->setEnabled(true);
}

void MainWindow::onSaveSettingButtonClicked()
{
    ui->newSettingButton->setEnabled(false);
    ui->saveSettingButton->setEnabled(false);
    ui->deleteSettingButton->setEnabled(false);

    RefreshSettingComboBox();
    ui->newSettingButton->setEnabled(true);
    ui->saveSettingButton->setEnabled(true);
    ui->deleteSettingButton->setEnabled(true);
}

void MainWindow::onQuerySettingsNameFinished(QList<QString> settingsNameList)
{
    if(settingsNameList.isEmpty())
    {
        QMessageBox::warning(this, tr("dont have config"), tr("no settings"));
        return;
    }
    for(auto settingName : settingsNameList)
    {
        ui->settingComboBox->addItem(settingName);
    }
    qDebug() << "刷新 settingComboBox 成功";
    settingsNameList.clear();
}
