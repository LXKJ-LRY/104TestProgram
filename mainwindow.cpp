#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "memory"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  initialize();

  setupConnections();

  initialEdits();

  RefreshSettingComboBox();
}

MainWindow::~MainWindow()
{
  if (_104Thread != nullptr)
  {
    _104Thread->quit();
    _104Thread->wait();
  }
  _104Controller->release();
  if (_dbThread != nullptr)
  {
    _dbThread->quit();
    _dbThread->wait();
  }
  _dbDataHandler->release();
  delete ui;
}

void MainWindow::initialize()
{
  ui->stackedWidget->setCurrentIndex(0);
  ui->testPageButton->setEnabled(false);
  ui->disconnectButton->setEnabled(false);

  _dbThread = new QThread(this);
  _dbDataHandler = DBDataHandler::instance();
  _dbDataHandler->moveToThread(_dbThread);
  _dbThread->start();

  _104Thread = new QThread(this);
  _104Controller = Iec104Controller::instance();
  _104Controller->moveToThread(_104Thread);
  _104Thread->start();

  _setting = new QSettings(QApplication::applicationDirPath() + "/ConnectionSetting.ini", QSettings::Format::IniFormat, this);
  _setting->setValue("/ConnectionSetting/SettingName","新的配置");
  _setting->setValue("/ConnectionSetting/LocalMasterAddr","127.0.0.1");
  _setting->setValue("/ConnectionSetting/LocalPort","2404");
  _setting->setValue("/ConnectionSetting/RemoteSlaveAddr","127.0.0.1");
  _setting->setValue("/ConnectionSetting/RemotePort","2404");
}

void MainWindow::initialEdits()
{
  {
    auto va = new QRegularExpressionValidator(this);
    QRegularExpression reg("^((25[0-5]|2[0-4][0-9]|[0-1]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[0-1]?[0-9][0-9]?)$");
    va->setRegularExpression(reg);
    _ipValidator = va;
  }
  {
    auto va = new QIntValidator(this);
    va->setRange(0, 65535);
    _portValidator = va;
  }

  ui->localMasterAddrEdit->setValidator(_ipValidator);
  ui->remoteSlaveAddrEdit->setValidator(_ipValidator);
  ui->localPortEdit->setValidator(_portValidator);
  ui->remotePortEdit->setValidator(_portValidator);

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
  connect(this, &MainWindow::deleteSettingFromDB, _dbDataHandler, &DBDataHandler::onDeleteSettingFromDB, Qt::QueuedConnection);
  connect(_dbDataHandler, &DBDataHandler::deleteSettingFromDBFinished, this, &MainWindow::onDeleteSettingFromDBFinished, Qt::QueuedConnection);

  connect(ui->saveSettingButton, &QAbstractButton::clicked, this, &MainWindow::onSaveSettingButtonClicked);
  connect(this, &MainWindow::saveSettingToDB, _dbDataHandler, &DBDataHandler::onSaveSettingToDB, Qt::QueuedConnection);
  connect(_dbDataHandler, &DBDataHandler::saveSettingToDBFinished, this, &MainWindow::onSaveSettingToDBFinished, Qt::QueuedConnection);

  connect(this, &MainWindow::refreshComboBoxFromDB, _dbDataHandler, &DBDataHandler::onRefreshSettingComboBox, Qt::QueuedConnection);
  connect(_dbDataHandler, &DBDataHandler::querySettingsNameFinished, this, &MainWindow::onQuerySettingsNameFinished, Qt::QueuedConnection);

  connect(ui->connectButton, &QAbstractButton::clicked, this, &MainWindow::onConnectButtonClicked);
  connect(ui->disconnectButton, &QAbstractButton::clicked, this, &MainWindow::onDisconnectButtonClicked);

  connect(this, &MainWindow::connectButtonClicked, _104Controller, &Iec104Controller::onConnection, Qt::QueuedConnection);
  connect(this, &MainWindow::disconnectbuttonClicked, _104Controller, &Iec104Controller::onDisConnection, Qt::QueuedConnection);

  connect(ui->sendTestOrderButton, &QAbstractButton::clicked, _104Controller, &Iec104Controller::onSendTestOrderButton, Qt::QueuedConnection);
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

void MainWindow::onDeviceListButtonClicked()
{
    qDebug() << "设备点表按钮被点击，准备显示窗口";
    // 创建DeviceTableWidget窗口实例，父窗口设为当前主窗口
    DeviceTableWidget *deviceTableWindow = new DeviceTableWidget(this);
    // 设置窗口最小尺寸（可选，根据需要调整）
    deviceTableWindow->setMinimumSize(800, 600);
    // 显示窗口
    deviceTableWindow->show();
}

void MainWindow::onNewSettingButtonClicked()
{
  ui->newSettingButton->setEnabled(false);
  ui->saveSettingButton->setEnabled(false);
  ui->deleteSettingButton->setEnabled(false);
  emit addNewSettingToDB();

}

void MainWindow::onAddNewSettingToDBFinished()
{
  RefreshSettingComboBox();
  ui->newSettingButton->setEnabled(true);
  ui->saveSettingButton->setEnabled(true);
  ui->deleteSettingButton->setEnabled(true);
}

void MainWindow::RefreshSettingComboBox()
{
  ui->settingComboBox->clear();
  emit refreshComboBoxFromDB();
}

void MainWindow::clearAllEdit()
{
  ui->nameEdit->clear();
  ui->localMasterAddrEdit->clear();
  ui->localPortEdit->clear();
  ui->remoteSlaveAddrEdit->clear();
  ui->remotePortEdit->clear();
  ui->saveSettingButton->setEnabled(false);
}

void MainWindow::onSettingComboBoxchanged(int index)
{
  if (index == -1)
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
    ui->localPortEdit->setText(singleSettingInfo[2]);
    ui->remoteSlaveAddrEdit->setText(singleSettingInfo[3]);
    ui->remotePortEdit->setText(singleSettingInfo[4]);
  }
  else
  {
    ui->localMasterAddrEdit->clear();
    ui->localPortEdit->clear();
    ui->remoteSlaveAddrEdit->clear();
    ui->remotePortEdit->clear();
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
  emit deleteSettingFromDB(ui->settingComboBox->currentText());
}

void MainWindow::onDeleteSettingFromDBFinished()
{
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
  QList<QString> singleSettinginfo;
  singleSettinginfo.append(ui->settingComboBox->currentText());
  singleSettinginfo.append(ui->nameEdit->text());
  singleSettinginfo.append(ui->localMasterAddrEdit->text());
  singleSettinginfo.append(ui->localPortEdit->text());
  singleSettinginfo.append(ui->remoteSlaveAddrEdit->text());
  singleSettinginfo.append(ui->remotePortEdit->text());
  emit saveSettingToDB(singleSettinginfo);


}

void MainWindow::onSaveSettingToDBFinished()
{
  RefreshSettingComboBox();
  ui->newSettingButton->setEnabled(true);
  ui->saveSettingButton->setEnabled(true);
  ui->deleteSettingButton->setEnabled(true);
}

void MainWindow::onQuerySettingsNameFinished(QList<QString> settingsNameList)
{
  if (settingsNameList.isEmpty())
  {
    clearAllEdit();
    QMessageBox::warning(this, tr("dont have config"), tr("no settings"));
    return;
  }
  for (auto& settingName : settingsNameList)
  {
    ui->settingComboBox->addItem(settingName);
  }
  qDebug() << "刷新 settingComboBox 成功";
  ui->saveSettingButton->setEnabled(true);
  settingsNameList.clear();
}

void MainWindow::onConnectButtonClicked()
{
  if (ui->localMasterAddrEdit->text().isEmpty() || ui->localPortEdit->text().isEmpty() || ui->remoteSlaveAddrEdit->text().isEmpty() || ui->remotePortEdit->text().isEmpty() )
  {
    QMessageBox::warning(this, "connect failed", "Host/ Port is empty");
    return;
  }
  enableEditAndButton(false);

  ui->connectionSettingNameLabel->setText("连接配置: " + ui->nameEdit->text());
  qDebug() << "hello1";
  emit connectButtonClicked(ui->localMasterAddrEdit->text(), ui->localPortEdit->text().toInt(), ui->remoteSlaveAddrEdit->text(), ui->remotePortEdit->text().toInt());
}

void MainWindow::onDisconnectButtonClicked()
{
  enableEditAndButton(true);

  ui->connectionSettingNameLabel->setText("停止服务");
  emit disconnectbuttonClicked();
}

void MainWindow::enableEditAndButton(bool enable)
{
  ui->connectButton->setEnabled(enable);
  ui->disconnectButton->setEnabled(!enable);
  ui->settingComboBox->setEnabled(enable);
  ui->nameEdit->setEnabled(enable);
  ui->localMasterAddrEdit->setEnabled(enable);
  ui->localPortEdit->setEnabled(enable);
  ui->remoteSlaveAddrEdit->setEnabled(enable);
  ui->remotePortEdit->setEnabled(enable);
  ui->newSettingButton->setEnabled(enable);
  ui->deleteSettingButton->setEnabled(enable);
  ui->saveSettingButton->setEnabled(enable);
}

