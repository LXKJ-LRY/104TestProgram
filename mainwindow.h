#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "dbdatahandler.h"
#include "iec104controller.h"
#include "testlogmanager.h"

#include <QMainWindow>
#include <QSettings>
#include <QCoreApplication>
#include <QDebug>
#include <QDialog>
#include <QThread>
#include <QMessageBox>
#include <QValidator>
#include <QMetaType>
#include <QMap>
#include <QDateTime>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
  Q_OBJECT

  enum stackedPageIndex
  {
    testPage = 0,
    settingPage,
  };

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  void setupSelfConnections();
  void setupOtherConnections();

signals:
  void refreshComboBoxFromDB();
  void addNewSettingToDB();
  void saveSettingToDB(const QList<QString> singleSettinginfo);
  void deleteSettingFromDB(const QString singleSettingName);

  void connectButtonClicked(QString localAddr, int localPort, QString remoteAddr, int remotePort);
  void disconnectbuttonClicked();

  void sendChoosedRelayYKClose(int ioa);
  void sendChoosedRelayYKOpen(int ioa);

  void startTest(int ioa);
  void stopTest(int ioa);

  void notifySetTestNumber(int defaultNumber = 10000);

  void appendTestLogToFile(QString text);

private slots:
  void onSettingPageButtonClicked();
  void onTestPageButtonClicked();
  void onClearTestInfoButtonClicked();

  void onSettingComboBoxchanged(int index);

  void onNewSettingButtonClicked();
  void onAddNewSettingToDBFinished();

  void onDeleteSettingButtonClicked();
  void onDeleteSettingFromDBFinished();

  void onSaveSettingButtonClicked();
  void onSaveSettingToDBFinished();

  void onQuerySettingsNameFinished(const QList<QString> settingsNameList);

  void onConnectButtonClicked();
  void onDisconnectButtonClicked();

  void onConnectionEstablished(bool isEnabled);
  void onConnectionClosed(bool isEnabled);

  void onMasterReceiveCot20(const QMap<int, bool> relayStatus);
  void onMasterReceiveSinglePointStatus(int ioa, bool newStatus, int receiveNO, int testNO, int testFailedNO);

  void onChoose1CheckBoxClicked();
  void onChoose2CheckBoxClicked();
  void onChoose3CheckBoxClicked();
  void onChoose4CheckBoxClicked();

  void onTest1CheckBoxClicked();
  void onTest2CheckBoxClicked();
  void onTest3CheckBoxClicked();
  void onTest4CheckBoxClicked();

  void onStartTestButtonClicked();
  void onStopTestButtonClicked();

  void onTestNumberEditCommitted();

private:
  void initialize();
  void initialEdits();
  void RefreshSettingComboBox();
  void clearAllEdit();

  void enableEditAndButton(bool enable);

  Ui::MainWindow *ui;
  QSettings* _setting;

  TestLogManager* _logManager = nullptr;
  QThread* _logThread = nullptr;

  DBDataHandler* _dbDataHandler = nullptr;
  QThread* _dbThread = nullptr;

  Iec104Controller* _104Controller;
  QThread* _104Thread;

  QValidator* _ipValidator = nullptr;
  QValidator* _portValidator = nullptr;
  QValidator* _testNumberValidator = nullptr;

  QMap<int, bool> relayStatus;

  int TestIOA = -1;

  int settingComboCurrentIndex = 0;
  int settingComboLastIndex = -1;
};
#endif // MAINWINDOW_H
