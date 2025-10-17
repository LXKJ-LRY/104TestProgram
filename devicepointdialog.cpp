#include "devicepointdialog.h"
#include "ui_devicepointdialog.h"

DevicePointDialog::DevicePointDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DevicePointDialog)
{
    ui->setupUi(this);
}

DevicePointDialog::~DevicePointDialog()
{
    delete ui;
}
