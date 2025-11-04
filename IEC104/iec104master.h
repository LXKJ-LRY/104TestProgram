#ifndef IEC1_4MASTER_H
#define IEC1_4MASTER_H

#include "lib60870/cs104_connection.h"
#include "lib60870/hal_time.h"
#include "lib60870/hal_thread.h"
// #include "iec104masterstrategy.h"
// #include "iec104masterstrategyfactory.h"

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

  bool sendTestYK(int ioa, bool status);

  void onConnectButtonClicked();
  void onDisconnectButtonClicked();

  void startTestTo10000(int ioa);
  void stopTestTo10000(int ioa);

  void setNewTestNumber(int newTestNumber);

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

  void testTo10000(int testIoa, int receiveIoa);


signals:
  void connectionEstablished(bool isEnabled);
  void connectionClosed(bool isEnabled);

  void receiveCot20(const QMap<int, bool> relayStatus);
  void receiveSinglePointStatus(int ioa, bool newStatus, int receiveNO = -1, int testNO = 0, int testFailedNO = 0);

  void underTestReceiveSinglePoint(int receiveNO, int ioa, bool status);

  void initialTestRelay();


private slots:
  void onReconnectTimerTriggered();
  void onInterrogationTimerTriggered();

  void onUnderTestReceiveSinglePoint(int receiveNO, int ioa, bool status);
  void initialRelaysAfterStopTest();

  void initialTestRelayAfterStop();

private:
  QMap<int, bool> relayStatus;
  QTimer* _reconnectTimer = nullptr;
  QTimer* _interrogationTimer = nullptr;
  QTimer* _closeRelayAfterStopTest =nullptr;


  CS104_Connection _con = nullptr;

  std::atomic_bool _isConnected;

  std::atomic_bool _isEnabled;

  std::atomic_bool _isUnderTest;

  std::atomic_bool _isInitializeAfterStop;

  // Strategy
  //std::shared_ptr<IEC104MasterStrategy> _strategy;

  QString localAddr;
  int localPort;
  QString remoteAddr;
  int remotePort;

  int testNO = 0;
  int receivceSingleNO = 0;
  int testFailedNO = 0;

  int closedRelayIOA = -1;
  int testIOA = -1;
  int receiveIOA = -1;

  int singleTestTwice = 0;
  int oldTestNO = 0;

  std::atomic_int testNumber;

};

#endif // IEC1_4MASTER_H
