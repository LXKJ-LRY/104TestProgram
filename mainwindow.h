#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QSettings>
#include <QCoreApplication>

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

private slots:
    void onSettingPageButtonClicked();
    void onTestPageButtonClicked();

private:
    Ui::MainWindow *ui;
    QSettings* _setting;


};
#endif // MAINWINDOW_H
