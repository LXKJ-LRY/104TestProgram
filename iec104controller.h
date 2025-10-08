#ifndef IEC1_4CONTROLLER_H
#define IEC1_4CONTROLLER_H

#include <QObject>

class Iec104Controller : public QObject
{
  Q_OBJECT
public:


signals:

private:
  explicit Iec104Controller(QObject *parent = nullptr);
  Iec104Controller(const Iec104Controller&) {}
  ~Iec104Controller();



};

#endif // IEC1_4CONTROLLER_H
