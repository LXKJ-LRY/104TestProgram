#ifndef IEC1_4CONTROLLER_H
#define IEC1_4CONTROLLER_H

#include "IEC104/iec104master.h"

#include <QObject>
#include <QVariant>
#include <memory>
#include <QMetaType>
#include <QMap>


class Iec104Controller : public QObject
{
  Q_OBJECT
public:
  static Iec104Controller* instance() { return _104controller; }
  void release();


signals:
  void masterEstablishConnection(bool isEnabled);
  void masterConnectionClosed(bool isEnabled);

  void masterReceiveCot20(const QMap<int, bool> relayStatus);
  void masterReceiveSinglePointStatus(int ioa, bool newStatus, int receiveNO, int testNO, int testFaileNO);

  void updateTestNumber(int defaultNumber);

  void notifyLogManagerTestStop();

public slots:
  void onConnection(QString localAddr, int localPort, QString remoteAddr, int remotePort);
  void onDisConnection();
  void onSendTestCommandButton();

  void onSendYKOpenButtonClicked();
  void onSendYKCloseButtonClicked();

  void onSendChoosedRelayYKClose(int ioa);
  void onSendChoosedRelayYKOpen(int ioa);

  void onStartTest(int ioa);
  void onStopTest(int ioa);

private:
  explicit Iec104Controller(QObject *parent = nullptr);
  Iec104Controller(const Iec104Controller&) {}
  ~Iec104Controller();

  static Iec104Controller* _104controller;

  IEC104Master* master;

};

#endif // IEC1_4CONTROLLER_H
