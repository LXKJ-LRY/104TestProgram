#ifndef IEC1_4MASTER_H
#define IEC1_4MASTER_H

#include "lib60870/cs104_connection.h"
#include "iec104masterstrategy.h"

#include <QObject>
#include <QTimer>
#include <QDebug>
#include <QMessageBox>
#include <QThread>

class IEC104Master : public QObject
{
  Q_OBJECT
public:
  explicit IEC104Master(QObject *parent = nullptr);

  void start(QString localAddr, int localPort, QString remoteAddr, int remotePort);
  bool stop();

  // handlers for 104
  static bool asduReceivedHandler(void* parameter, int address, CS101_ASDU asdu);
  static void connectionHandler(void* parameter, CS104_Connection connection, CS104_ConnectionEvent event);
  static void rawMessageHandler(void* parameter, uint8_t* msg, int msgSize, bool sent);

  void sentTestCommand();

private:
  void setupTimers();

signals:


private slots:
  void onReconnectTimerTriggered();
  void onInterrogationTimerTriggered();


private:
  QTimer* _reconnectTimer = nullptr;
  QTimer* _interrogationTimer = nullptr;

  CS104_Connection _con = nullptr;

  std::atomic_bool _isConnected;

  // Strategy
  std::shared_ptr<IEC104MasterStrategy> _strategy;

  QString localAddr;
  int localPort;
  QString remoteAddr;
  int remotePort;
};

#endif // IEC1_4MASTER_H
