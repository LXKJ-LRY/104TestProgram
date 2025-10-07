#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "dbdatahandler.h"
#include "settingManager/settingmanager.h"

#include <QMainWindow>
#include <QSettings>
#include <QCoreApplication>
#include <QDebug>
#include <QDialog>
#include <QThread>
#include <QMessageBox>


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
    void refreshComboBoxFromDB(QList<QString> settingsNameList);
    void addNewSettingToDB();
    void saveSettingToDB();
    void deleteSettingFromDB();


private slots:
    void onSettingPageButtonClicked();
    void onTestPageButtonClicked();
    void onDeviceListButtonClicked();

    void onSettingComboBoxchanged(int index);

    void onNewSettingButtonClicked();
    void onAddNewSettingToDBFinished();

    void onDeleteSettingButtonClicked();
    void onSaveSettingButtonClicked();
    void onQuerySettingsNameFinished(QList<QString> settingsNameList);


private:
    void RefreshSettingComboBox();

    Ui::MainWindow *ui;
    QSettings* _setting;

    SettingManager* _settingManager;

    DBDataHandler* _dbDataHandler = nullptr;
    QThread* _dbThread = nullptr;

    QList<QString> settingsNameList;

    int settingComboCurrentIndex = 0;
    int settingComboLastIndex = -1;
};
#endif // MAINWINDOW_H
