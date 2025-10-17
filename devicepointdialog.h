#ifndef DEVICEPOINTDIALOG_H
#define DEVICEPOINTDIALOG_H

#include <QDialog>

namespace Ui {
class DevicePointDialog;
}

class DevicePointDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DevicePointDialog(QWidget *parent = nullptr);
    ~DevicePointDialog();

private:
    Ui::DevicePointDialog *ui;
};

#endif // DEVICEPOINTDIALOG_H
