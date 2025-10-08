#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "dbdatahandler.h"
#include "settingManager/settingmanager.h"
#include "iec104controller.h"

#include <QMainWindow>
#include <QSettings>
#include <QCoreApplication>
#include <QDebug>
#include <QDialog>
#include <QThread>
#include <QMessageBox>
#include <QValidator>


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

  void setupConnections();

signals:
  void refreshComboBoxFromDB();
  void addNewSettingToDB();
  void saveSettingToDB(const QList<QString> singleSettinginfo);
  void deleteSettingFromDB(const QString singleSettingName);

  void connectButtonClicked(QString localAddr, int localPort, QString remoteAddr, int remotePort);
  void disconnectbuttonClicked();

private slots:
  void onSettingPageButtonClicked();
  void onTestPageButtonClicked();
  void onDeviceListButtonClicked();

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

private:
  void initialize();
  void initialEdits();
  void RefreshSettingComboBox();
  void clearAllEdit();

  void enableEditAndButton(bool enable);

  Ui::MainWindow *ui;
  QSettings* _setting;

  SettingManager* _settingManager;

  DBDataHandler* _dbDataHandler = nullptr;
  QThread* _dbThread = nullptr;

  Iec104Controller* _104Controller;
  QThread* _104Thread;

  QValidator* _ipValidator = nullptr;
  QValidator* _portValidator = nullptr;

  int settingComboCurrentIndex = 0;
  int settingComboLastIndex = -1;
};
#endif // MAINWINDOW_H
