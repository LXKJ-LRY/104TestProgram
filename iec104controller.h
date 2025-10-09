#ifndef IEC1_4CONTROLLER_H
#define IEC1_4CONTROLLER_H

#include "IEC104/iec104master.h"

#include <QObject>
#include <QVariant>
#include <memory>

class Iec104Controller : public QObject
{
  Q_OBJECT
public:
  static Iec104Controller* instance() { return _104controller; }
  void release();


signals:

public slots:
  void onConnection(QString localAddr, int localPort, QString remoteAddr, int remotePort);
  void onDisConnection();

private:
  explicit Iec104Controller(QObject *parent = nullptr);
  Iec104Controller(const Iec104Controller&) {}
  ~Iec104Controller();

  static Iec104Controller* _104controller;

  IEC104Master* master;

};

#endif // IEC1_4CONTROLLER_H
