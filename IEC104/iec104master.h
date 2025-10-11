#ifndef IEC1_4MASTER_H
#define IEC1_4MASTER_H

#include "lib60870/cs104_connection.h"
#include "lib60870/hal_time.h"
#include "lib60870/hal_thread.h"
#include "iec104masterstrategy.h"
#include "iec104masterstrategyfactory.h"

#include <QObject>
#include <QTimer>
#include <QDebug>
#include <QMessageBox>
#include <QThread>
#include <QMetaType>
#include <QMap>


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

  void sendTestCommand();
  bool sendYKOpen();
  bool sendYKClose();

  bool sendChoosedYKOpen(int ioa);
  bool sendChoosedYKClose(int ioa);

  void onConnectButtonClicked();
  void onDisconnectButtonClicked();

private:
  void setupTimers();

  void handleConnectionOpened();
  void handleConnectionClosed();
  void handleConnectionFailed();

  void sendInterrogation();

  // M_SP_NA_1
  void handleRecvRequestUpdateDeviceStatus(CS101_ASDU asdu);
  // M_DP_NA_1
  void handleRecvRequestUpdateDevStatusDP(CS101_ASDU asdu);
  // M_BO_NA_1
  void handleRecvRequestUpdateMultiDevicesStatus(CS101_ASDU asdu);
  // C_SC_NA_1
  void handleRecvRequestCheckSingleAmoStep(CS101_ASDU asdu);
  // TYPE: 43
  void handleRecvRequestCheckOperationTicket(CS101_ASDU asdu);

signals:
  void connectionEstablished(bool isEnabled);
  void connectionClosed(bool isEnabled);

  void receiveCot20(const QMap<int, bool> relayStatus);
  void receiveSinglePointStatus(int ioa, bool isClose);

private slots:
  void onReconnectTimerTriggered();
  void onInterrogationTimerTriggered();

private:
  QMap<int, bool> relayStatus;
  QTimer* _reconnectTimer = nullptr;
  QTimer* _interrogationTimer = nullptr;

  CS104_Connection _con = nullptr;

  std::atomic_bool _isConnected;

  std::atomic_bool _isEnabled;

  std::atomic_bool _isUnderTest;

  // Strategy
  std::shared_ptr<IEC104MasterStrategy> _strategy;

  QString localAddr;
  int localPort;
  QString remoteAddr;
  int remotePort;

};

#endif // IEC1_4MASTER_H
