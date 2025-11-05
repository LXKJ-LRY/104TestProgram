#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "memory"

MainWindow::MainWindow(QWidget *parent)   // there will be some error when merge into origin/main
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  setWindowTitle("104测试程序");
  setMinimumSize(800,600);

  qRegisterMetaType<QMap<int, bool>>("QMap<int,bool>");

  initialize();

  setupSelfConnections();
  setupOtherConnections();

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

  if (_logThread != nullptr)
  {
    _logThread->quit();
    _logThread->wait();
  }
  _logManager->release();

  delete ui;
}

void MainWindow::initialize()
{
  ui->stackedWidget->setCurrentIndex(0);
  ui->testPageButton->setEnabled(false);
  ui->disconnectButton->setEnabled(false);
  ui->testGroupBox->setEnabled(false);

  _dbThread = new QThread(this);
  _dbDataHandler = DBDataHandler::instance();
  _dbDataHandler->moveToThread(_dbThread);
  _dbThread->start();

  _104Thread = new QThread(this);
  _104Controller = Iec104Controller::instance();
  _104Controller->moveToThread(_104Thread);
  _104Thread->start();

  _logThread = new QThread(this);
  _logManager = TestLogManager::instance();
  _logManager->moveToThread(_logThread);
  _logThread->start();

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

  {
    auto va = new QIntValidator(this);
    va->setRange(0,100000);
    _testNumberValidator = va;
    ui->testNumberEdit->setText("10000");
    ui->testNumberEdit->setValidator(_testNumberValidator);

  }

  ui->localMasterAddrEdit->setValidator(_ipValidator);
  ui->remoteSlaveAddrEdit->setValidator(_ipValidator);
  ui->localPortEdit->setValidator(_portValidator);
  ui->remotePortEdit->setValidator(_portValidator);

}

void MainWindow::setupOtherConnections()
{

  connect(this, &MainWindow::addNewSettingToDB, _dbDataHandler, &DBDataHandler::onAddNewSettingToDB, Qt::QueuedConnection);
  connect(_dbDataHandler, &DBDataHandler::addNewSettingToDBFinished, this, &MainWindow::onAddNewSettingToDBFinished, Qt::QueuedConnection);

  connect(this, &MainWindow::deleteSettingFromDB, _dbDataHandler, &DBDataHandler::onDeleteSettingFromDB, Qt::QueuedConnection);
  connect(_dbDataHandler, &DBDataHandler::deleteSettingFromDBFinished, this, &MainWindow::onDeleteSettingFromDBFinished, Qt::QueuedConnection);

  connect(this, &MainWindow::saveSettingToDB, _dbDataHandler, &DBDataHandler::onSaveSettingToDB, Qt::QueuedConnection);
  connect(_dbDataHandler, &DBDataHandler::saveSettingToDBFinished, this, &MainWindow::onSaveSettingToDBFinished, Qt::QueuedConnection);

  connect(this, &MainWindow::refreshComboBoxFromDB, _dbDataHandler, &DBDataHandler::onRefreshSettingComboBox, Qt::QueuedConnection);
  connect(_dbDataHandler, &DBDataHandler::querySettingsNameFinished, this, &MainWindow::onQuerySettingsNameFinished, Qt::QueuedConnection);

  connect(this, &MainWindow::connectButtonClicked, _104Controller, &Iec104Controller::onConnection, Qt::QueuedConnection);
  connect(this, &MainWindow::disconnectbuttonClicked, _104Controller, &Iec104Controller::onDisConnection, Qt::QueuedConnection);

  connect(ui->sendTestCommandButton, &QAbstractButton::clicked, _104Controller, &Iec104Controller::onSendTestCommandButton, Qt::QueuedConnection);
  connect(ui->sendYKOpenButton, &QAbstractButton::clicked, _104Controller, &Iec104Controller::onSendYKOpenButtonClicked, Qt::QueuedConnection);
  connect(ui->sendYKCloseButton, &QAbstractButton::clicked, _104Controller, &Iec104Controller::onSendYKCloseButtonClicked, Qt::QueuedConnection);

  connect(this, &MainWindow::sendChoosedRelayYKOpen, _104Controller, &Iec104Controller::onSendChoosedRelayYKOpen, Qt::QueuedConnection);
  connect(this, &MainWindow::sendChoosedRelayYKClose, _104Controller, &Iec104Controller::onSendChoosedRelayYKClose, Qt::QueuedConnection);

  connect(_104Controller, &Iec104Controller::masterEstablishConnection, this, &MainWindow::onConnectionEstablished, Qt::QueuedConnection);
  connect(_104Controller, &Iec104Controller::masterConnectionClosed, this, &MainWindow::onConnectionClosed, Qt::QueuedConnection);
  connect(_104Controller, &Iec104Controller::masterReceiveCot20, this, &MainWindow::onMasterReceiveCot20, Qt::QueuedConnection);
  connect(_104Controller, &Iec104Controller::masterReceiveSinglePointStatus, this, &MainWindow::onMasterReceiveSinglePointStatus, Qt::QueuedConnection);

  connect(this, &MainWindow::startTest, _104Controller, &Iec104Controller::onStartTest, Qt::QueuedConnection);
  connect(this, &MainWindow::stopTest, _104Controller, &Iec104Controller::onStopTest, Qt::QueuedConnection);

  connect(this, &MainWindow::notifySetTestNumber, _104Controller, &Iec104Controller::updateTestNumber, Qt::QueuedConnection);

  connect(this, &MainWindow::appendTestLogToFile, _logManager, &TestLogManager::addTestLogInFile, Qt::QueuedConnection);

  connect(_104Controller, &Iec104Controller::notifyLogManagerTestStop, _logManager, &TestLogManager::onTestStop, Qt::QueuedConnection);

  connect(this, &MainWindow::clearTestHistoryCount, _104Controller, &Iec104Controller::setTestCountToZero, Qt::QueuedConnection);
}

void MainWindow::setupSelfConnections()
{
  connect(ui->testPageButton, &QAbstractButton::clicked, this, &MainWindow::onTestPageButtonClicked);
  connect(ui->settingPageButton, &QAbstractButton::clicked, this, &MainWindow::onSettingPageButtonClicked);
  connect(ui->settingComboBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &MainWindow::onSettingComboBoxchanged);
  connect(ui->clearTestInfoButton, &QAbstractButton::clicked, this, &MainWindow::onClearTestInfoButtonClicked);

  connect(ui->newSettingButton, &QAbstractButton::clicked, this, &MainWindow::onNewSettingButtonClicked);
  connect(ui->deleteSettingButton, &QAbstractButton::clicked, this, &MainWindow::onDeleteSettingButtonClicked);
  connect(ui->saveSettingButton, &QAbstractButton::clicked, this, &MainWindow::onSaveSettingButtonClicked);

  connect(ui->connectButton, &QAbstractButton::clicked, this, &MainWindow::onConnectButtonClicked);
  connect(ui->disconnectButton, &QAbstractButton::clicked, this, &MainWindow::onDisconnectButtonClicked);

  connect(ui->choose1CheckBox, &QAbstractButton::clicked, this, &MainWindow::onChoose1CheckBoxClicked);
  connect(ui->choose2CheckBox, &QAbstractButton::clicked, this, &MainWindow::onChoose2CheckBoxClicked);
  connect(ui->choose3CheckBox, &QAbstractButton::clicked, this, &MainWindow::onChoose3CheckBoxClicked);
  connect(ui->choose4CheckBox, &QAbstractButton::clicked, this, &MainWindow::onChoose4CheckBoxClicked);

  connect(ui->test1CheckBox, &QAbstractButton::clicked, this, &MainWindow::onTest1CheckBoxClicked);
  connect(ui->test2CheckBox, &QAbstractButton::clicked, this, &MainWindow::onTest2CheckBoxClicked);
  connect(ui->test3CheckBox, &QAbstractButton::clicked, this, &MainWindow::onTest3CheckBoxClicked);
  connect(ui->test4CheckBox, &QAbstractButton::clicked, this, &MainWindow::onTest4CheckBoxClicked);

  connect(ui->startTestButton, &QAbstractButton::clicked, this, &MainWindow::onStartTestButtonClicked);
  connect(ui->stopTestButton, &QAbstractButton::clicked, this, &MainWindow::onStopTestButtonClicked);

  connect(ui->testNumberEdit, &QLineEdit::editingFinished, this, &MainWindow::onTestNumberEditCommitted);
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

void MainWindow::onClearTestInfoButtonClicked()
{
  ui->TestBrowser->clear();
  ui->testCounterLabel->setText(QString("<span style='color:black'>测试次数: [0]--收到结果: [0]</span>--失败次数: [0]"));
  emit clearTestHistoryCount();
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
  if (ui->localMasterAddrEdit->text().isEmpty() || ui->localPortEdit->text().isEmpty()
      || ui->remoteSlaveAddrEdit->text().isEmpty() || ui->remotePortEdit->text().isEmpty() )
  {
    QMessageBox::warning(this, tr("connect failed"), tr("Host / Port is empty"));
    return;
  }
  enableEditAndButton(false);

  ui->connectionSettingNameLabel->setText("连接配置: " + ui->nameEdit->text());
  qDebug() << "hello1";
  emit connectButtonClicked(ui->localMasterAddrEdit->text(), ui->localPortEdit->text().toInt(),
                            ui->remoteSlaveAddrEdit->text(), ui->remotePortEdit->text().toInt());
}

void MainWindow::onDisconnectButtonClicked()
{
  enableEditAndButton(true);
  ui->connectionSettingNameLabel->setText("停止服务");
  emit disconnectbuttonClicked();
}

void MainWindow::onConnectionEstablished(bool isEnabled)
{
  ui->testGroupBox->setEnabled(true);
  ui->connectionSettingNameLabel->setText(QString("连接建立: ") + ui->nameEdit->text());
}

void MainWindow::onConnectionClosed(bool isEnabled)
{
  ui->testGroupBox->setEnabled(false);
  if (isEnabled)
    ui->connectionSettingNameLabel->setText(QString("连接中断: 尝试重连: ") + ui->nameEdit->text());
  else
    ui->connectionSettingNameLabel->setText(QString("停止服务: ") + ui->nameEdit->text());
}

void MainWindow::onMasterReceiveCot20(const QMap<int, bool> relayStatus)
{
  this->relayStatus = relayStatus;
  ui->choose1CheckBox->setChecked(relayStatus[2]);
  ui->choose2CheckBox->setChecked(relayStatus[6]);
  ui->choose3CheckBox->setChecked(relayStatus[10]);
  ui->choose4CheckBox->setChecked(relayStatus[14]);
}

void MainWindow::onMasterReceiveSinglePointStatus(int ioa, bool newStatus, int receiveNO, int testNO, int testFailedNO)
{
  QString logInfo;
  ui->testCounterLabel->setText(QString("<span style='color:black'>测试次数: [%1]--收到结果: [%2]</span>--失败次数: [%3]")
                                        .arg(testNO).arg(receiveNO).arg(testFailedNO));
  switch (ioa)
  {
  case 2:
    if (relayStatus[ioa] == newStatus)
    {
      logInfo = tr("[%1]NO.%2 -- test failed -- 1#relay: expected status: %3 (0: open 1:close)")
                              .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(receiveNO).arg(newStatus);
      ui->relayStatusLabel->setText(tr("NO.%1 -- test failed -- 1#relay: expected status: %2").arg(receiveNO).arg(newStatus));
      ui->TestBrowser->append(QString("<span style='color:red;'>%1</span>").arg(logInfo));

      break;
    }
    if (newStatus)
    {
      logInfo = "[" + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "]"
                                        + tr("NO.%1 - 1#relay: close || old status: open").arg(receiveNO);
      ui->relayStatusLabel->setText(tr("NO.%1 - 1#relay: close || old status: open").arg(receiveNO));
      ui->TestBrowser->append(QString("<span style='color:black;'>%1</span>").arg(logInfo));

    }
    else
    {
      logInfo = "[" + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "]"
                                        + tr("NO.%1 - 1#relay: open  || old status: close").arg(receiveNO);
      ui->relayStatusLabel->setText(tr("NO.%1 - 1#relay: open  || old status: close").arg(receiveNO));
      ui->TestBrowser->append(QString("<span style='color:black;'>%1</span>").arg(logInfo));

    }
    break;
  case 6:
    if (relayStatus[ioa] == newStatus)
    {
      logInfo = tr("[%1]NO.%2 -- test failed -- 2#relay: expected status: %3 (0: open 1:close)")
                                    .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")).arg(receiveNO).arg(newStatus);
      ui->relayStatusLabel->setText(tr("NO.%1 -- test failed -- 2#relay: expected status: %2").arg(receiveNO).arg(newStatus));
      ui->TestBrowser->append(QString("<span style='color:red;'>%1</span>").arg(logInfo));

      break;
    }
    if (newStatus)
    {
      logInfo = "[" + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "]"
                                        + tr("NO.%1 - 2#relay: close || old status: open").arg(receiveNO);
      ui->relayStatusLabel->setText(tr("NO.%1 - 2#relay: close || old status: open").arg(receiveNO));
      ui->TestBrowser->append(QString("<span style='color:black;'>%1</span>").arg(logInfo));

    }
    else
    {
      logInfo = "[" + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "]"
                                        + tr("NO.%1 - 2#relay: open  || old status: close").arg(receiveNO);
      ui->relayStatusLabel->setText(tr("NO.%1 - 2#relay: open  || old status: close").arg(receiveNO));
      ui->TestBrowser->append(QString("<span style='color:black;'>%1</span>").arg(logInfo));

    }
    break;
  case 10:
    if (relayStatus[ioa] == newStatus)
    {
      logInfo = tr("[%1]NO.%2 -- test failed -- 3#relay: expected status: %3 (0: open 1:close)")
                                      .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").arg(receiveNO).arg(newStatus));
      ui->relayStatusLabel->setText(tr("NO.%1 -- test failed -- 3#relay: expected status: %2").arg(receiveNO).arg(newStatus));
      ui->TestBrowser->append(QString("<span style='color:red;'>%1</span>").arg(logInfo));
      break;
    }
    if (newStatus)
    {
      logInfo = "[" + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "]"
                                        + tr("NO.%1 - 3#relay: close || old status: open").arg(receiveNO);
      ui->relayStatusLabel->setText(tr("NO.%1 - 3#relay: close || old status: open").arg(receiveNO));
      ui->TestBrowser->append(QString("<span style='color:black;'>%1</span>").arg(logInfo));
    }
    else
    {
      logInfo = "[" + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "]"
                                        + tr("NO.%1 - 3#relay: open  || old status: close").arg(receiveNO);
      ui->relayStatusLabel->setText(tr("NO.%1 - 3#relay: open  || old status: close").arg(receiveNO));
      ui->TestBrowser->append(QString("<span style='color:black;'>%1</span>").arg(logInfo));
    }
    break;
  case 14:
    if (relayStatus[ioa] == newStatus)
    {
      logInfo = tr("[%1]NO.%2 -- test failed -- 4#relay: expected status: %3 (0: open 1:close)")
                                      .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").arg(receiveNO).arg(newStatus));
      ui->relayStatusLabel->setText(tr("NO.%1 -- test failed -- 4#relay: expected status: %2").arg(receiveNO).arg(newStatus));
      ui->TestBrowser->append(QString("<span style='color:red;'>%1</span>").arg(logInfo));
      break;
    }
    if (newStatus)
    {
      logInfo = "[" + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "]"
                                        + tr("NO.%1 - 4#relay: close || old status: open").arg(receiveNO);
      ui->relayStatusLabel->setText(tr("NO.%1 - 4#relay: close || old status: open").arg(receiveNO));
      ui->TestBrowser->append(QString("<span style='color:black;'>%1</span>").arg(logInfo));
    }
    else
    {
      logInfo = "[" + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "]"
                                        + tr("NO.%1 - 4#relay: open  || old status: close").arg(receiveNO);
      ui->relayStatusLabel->setText(tr("NO.%1 - 4#relay: open  || old status: close").arg(receiveNO));
      ui->TestBrowser->append(QString("<span style='color:black;'>%1</span>").arg(logInfo));
    }
    break;
  default:
    qDebug() << "收到的单点数据非信息对象地址2, 6, 10, 14";
    break;
  }

  emit appendTestLogToFile(logInfo);
  logInfo.clear();

  relayStatus[ioa] = newStatus;
}

void MainWindow::onChoose1CheckBoxClicked()
{
  if (ui->choose1CheckBox->isChecked())
  {
    ui->test1CheckBox->setEnabled(false);
    emit sendChoosedRelayYKClose(0x6001);
  }
  else
  {
    ui->test1CheckBox->setEnabled(true);
    emit sendChoosedRelayYKOpen(0x6001);
  }

}

void MainWindow::onChoose2CheckBoxClicked()
{
  if (ui->choose2CheckBox->isChecked())
  {
    ui->test2CheckBox->setEnabled(false);
    emit sendChoosedRelayYKClose(0x6002);
  }
  else
  {
    ui->test2CheckBox->setEnabled(true);
    emit sendChoosedRelayYKOpen(0x6002);
  }
}

void MainWindow::onChoose3CheckBoxClicked()
{
  if (ui->choose3CheckBox->isChecked())
  {
    ui->test3CheckBox->setEnabled(false);
    emit sendChoosedRelayYKClose(0x6003);
  }
  else
  {
    ui->test3CheckBox->setEnabled(true);
    emit sendChoosedRelayYKOpen(0x6003);

  }
}

void MainWindow::onChoose4CheckBoxClicked()
{
  if (ui->choose4CheckBox->isChecked())
  {
    ui->test4CheckBox->setEnabled(false);
    emit sendChoosedRelayYKClose(0x6004);
  }
  else
  {
    ui->test4CheckBox->setEnabled(true);
    emit sendChoosedRelayYKOpen(0x6004);
  }
}

void MainWindow::onTest1CheckBoxClicked()
{
  if (ui->test1CheckBox->isChecked())
  {
    TestIOA = 0x6001;
    ui->test2CheckBox->setEnabled(false);
    ui->test3CheckBox->setEnabled(false);
    ui->test4CheckBox->setEnabled(false);
  }
  else
  {
    TestIOA = -1;
    ui->test2CheckBox->setEnabled(true);
    ui->test3CheckBox->setEnabled(true);
    ui->test4CheckBox->setEnabled(true);
  }
}

void MainWindow::onTest2CheckBoxClicked()
{
  if (ui->test2CheckBox->isChecked())
  {
    TestIOA = 0x6002;
    ui->test1CheckBox->setEnabled(false);
    ui->test3CheckBox->setEnabled(false);
    ui->test4CheckBox->setEnabled(false);
  }
  else
  {
    TestIOA = -1;
    ui->test1CheckBox->setEnabled(true);
    ui->test3CheckBox->setEnabled(true);
    ui->test4CheckBox->setEnabled(true);
  }
}

void MainWindow::onTest3CheckBoxClicked()
{
  if (ui->test3CheckBox->isChecked())
  {
    TestIOA = 0x6003;
    ui->test2CheckBox->setEnabled(false);
    ui->test1CheckBox->setEnabled(false);
    ui->test4CheckBox->setEnabled(false);
  }
  else
  {
    TestIOA = -1;
    ui->test2CheckBox->setEnabled(true);
    ui->test1CheckBox->setEnabled(true);
    ui->test4CheckBox->setEnabled(true);
  }
}

void MainWindow::onTest4CheckBoxClicked()
{
  if (ui->test4CheckBox->isChecked())
  {
    TestIOA = 0x6004;
    ui->test2CheckBox->setEnabled(false);
    ui->test3CheckBox->setEnabled(false);
    ui->test1CheckBox->setEnabled(false);
  }
  else
  {
    TestIOA = -1;
    ui->test2CheckBox->setEnabled(true);
    ui->test3CheckBox->setEnabled(true);
    ui->test1CheckBox->setEnabled(true);
  }
}

void MainWindow::onStartTestButtonClicked()
{
  emit startTest(TestIOA);
}

void MainWindow::onStopTestButtonClicked()
{
  emit stopTest(TestIOA);
}

void MainWindow::onTestNumberEditCommitted()
{
  auto testNumber = ui->testNumberEdit->text().toInt();
  if (testNumber > 0)
  {
    emit notifySetTestNumber(testNumber);
  }
  else
  {
    QMessageBox::information(this, tr("Invalid Number"), tr("Set the test number as default"));
    emit notifySetTestNumber();
    ui->testNumberEdit->setText("10000");
  }
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

