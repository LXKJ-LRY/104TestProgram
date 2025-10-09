#include "devicetablewidget.h"
#include "ui_devicetablewidget.h"

DeviceTableWidget::DeviceTableWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DeviceTableWidget)
{
    ui->setupUi(this);
}

DeviceTableWidget::~DeviceTableWidget()
{
    delete ui;
}
