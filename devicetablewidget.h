#ifndef DEVICETABLEWIDGET_H
#define DEVICETABLEWIDGET_H

#include <QWidget>

namespace Ui {
class DeviceTableWidget;
}

class DeviceTableWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DeviceTableWidget(QWidget *parent = nullptr);
    ~DeviceTableWidget();

private:
    Ui::DeviceTableWidget *ui;
};

#endif // DEVICETABLEWIDGET_H
