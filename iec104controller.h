#ifndef IEC1_4CONTROLLER_H
#define IEC1_4CONTROLLER_H

#include <QObject>

class Iec104Controller : public QObject
{
    Q_OBJECT
public:
    explicit Iec104Controller(QObject *parent = nullptr);

signals:
};

#endif // IEC1_4CONTROLLER_H
