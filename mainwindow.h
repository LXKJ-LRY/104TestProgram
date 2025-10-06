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
    void refreshSettingComboBox();

private slots:
    void onSettingPageButtonClicked();
    void onTestPageButtonClicked();
    void onDeviceListButtonClicked();
    void onNewSettingButtonClicked();
    void onRefreshSettingComboBox();
    void onSettingComboBoxchanged(int index);

private:
    Ui::MainWindow *ui;
    QSettings* _setting;

    SettingManager* _settingManager;

    DBDataHandler* _dbDataHandler = nullptr;
    QThread* _dbThread = nullptr;

    int settingComboCurrentIndex = 0;
    int settingComboLastIndex = -1;
};
#endif // MAINWINDOW_H
