#include "iec104master.h"

IEC104Master::IEC104Master(QObject *parent)
    : QObject{parent}
{
  _strategy = IEC104MasterStrategyFactory::getStrategy(IEC104MasterStrategyFactory::eRemoteControlLock);
  qRegisterMetaType<QMap<int, bool>>("QMap<int,bool>");

  setupTimers();
}

void IEC104Master::setupTimers()
{
  _reconnectTimer = new QTimer(this);
  _reconnectTimer->setInterval(10000);
  _reconnectTimer->setSingleShot(true);
  connect(_reconnectTimer, &QTimer::timeout,
          this, &IEC104Master::onReconnectTimerTriggered);

  _interrogationTimer = new QTimer(this);
  _interrogationTimer->setInterval(10000);
  _interrogationTimer->setSingleShot(true);
  connect(_interrogationTimer, &QTimer::timeout,
          this, &IEC104Master::onInterrogationTimerTriggered);
}


void IEC104Master::start(QString localAddr, int localPort, QString remoteAddr, int remotePort)
{
  if (!_isEnabled.load())
  {
    qDebug() << "连接手动关闭,";
    return;
  }
  this->localAddr = localAddr;
  this->localPort = localPort;
  this->remoteAddr = remoteAddr;
  this->remotePort = remotePort;
  // stop old connection before start
  qDebug() << "hello4";
  if (_con)
  {
    if (stop() == false)
    {
      qDebug() << "IEC104 Master: Start failed, stop old connection failed.";
      return;
    }
  }


  _con = CS104_Connection_create(this->remoteAddr.toStdString().c_str(), this->remotePort);
  qDebug() << "---Connecting to: " << remoteAddr << ": " << remotePort;

  if (_con == nullptr)
  {
    qDebug() <<"IEC104 Master: Start failed, connection object create failed.";
    return;
  }

  // CS104_APCIParameters apciParameters = CS104_Connection_getAPCIParameters(_con);
  // CS101_AppLayerParameters alParams = CS104_Connection_getAppLayerParameters(_con);
  CS104_APCIParameters apciParams = CS104_Connection_getAPCIParameters(_con);
  apciParams->t0 = 30; // update by protocol

  CS104_Connection_setConnectionHandler(_con, connectionHandler, (void*)this);
  CS104_Connection_setASDUReceivedHandler(_con, asduReceivedHandler, (void*)this);
  CS104_Connection_setRawMessageHandler(_con, rawMessageHandler, (void*)this);

  CS104_Connection_connectAsync(_con);


}

bool IEC104Master::stop()
{
  qDebug() << "stop2";
  if (_con)
  {
    CS104_Connection_destroy(_con);
    _con = nullptr;
    qDebug() << "stop3";
  }

  if (_interrogationTimer && _interrogationTimer->isActive())
  {
    _interrogationTimer->stop();
  }

  if (_reconnectTimer && _reconnectTimer->isActive())
  {
    _reconnectTimer->stop();
  }
  qDebug() << "stop4";

  return true;
}

/* Connection event handler */
void IEC104Master::connectionHandler(void* parameter, CS104_Connection connection, CS104_ConnectionEvent event)
{

  IEC104Master *pThis = reinterpret_cast<IEC104Master*>(parameter);
  switch (event)
  {
  case CS104_CONNECTION_OPENED:
    qDebug() << "Connection established\n";
    CS104_Connection_sendStartDT(connection);
    pThis->handleConnectionOpened();
    break;

  case CS104_CONNECTION_CLOSED:
    qDebug() << "Connection closed\n";
    pThis->handleConnectionClosed();
    break;

  case CS104_CONNECTION_STARTDT_CON_RECEIVED:
    qDebug() << "send interrogation 发送总召";
    pThis->sendInterrogation();
    break;

  case CS104_CONNECTION_STOPDT_CON_RECEIVED:
    break;

  case CS104_CONNECTION_FAILED:
    qDebug() << "Connection failed\n";
    pThis->handleConnectionFailed();
    break;
  }

}

void IEC104Master::rawMessageHandler(void* parameter, uint8_t* msg, int msgSize, bool sent)
{
  QString printStr;
  if (sent)
    printStr += "M >> ";
  else
    printStr += "M << ";

  for (auto i = 0; i < msgSize; i++)
  {
    printStr += QString::number(msg[i], 16).toUpper() + " ";
  }
  qDebug() <<  printStr;
}

/*
 * CS101_ASDUReceivedHandler implementation
 *
 * For CS104 the address parameter has to be ignored
 */
bool IEC104Master::asduReceivedHandler(void* parameter, int address, CS101_ASDU asdu)
{
  Q_UNUSED(address) // ignored for CS104
  IEC104Master *pThis = reinterpret_cast<IEC104Master*>(parameter);

  auto asduType = CS101_ASDU_getTypeID(asdu);
  auto asduElemCount = CS101_ASDU_getNumberOfElements(asdu);
  auto cot = CS101_ASDU_getCOT(asdu);
  auto ca = CS101_ASDU_getCA(asdu);

  qDebug() << "---RECVD ASDU type: " << asduType << " || elements: " << asduElemCount << " || ca: " << ca << " || cot: " << cot;

  if (ca != 1)
  {

    return false;
  }

  switch (asduType)
  {
  case M_ME_TE_1:
  {
    qDebug() << "  measured scaled values with CP56Time2a timestamp:\n";

    int i;

    for (i = 0; i < CS101_ASDU_getNumberOfElements(asdu); i++)
    {

      MeasuredValueScaledWithCP56Time2a io =
          (MeasuredValueScaledWithCP56Time2a) CS101_ASDU_getElement(asdu, i);

      qDebug() << QString("    IOA: %1 value: %2\n").arg(
                                                        InformationObject_getObjectAddress((InformationObject) io)).arg(
                          MeasuredValueScaled_getValue((MeasuredValueScaled) io));

      MeasuredValueScaledWithCP56Time2a_destroy(io);
    }
  } break;
  case M_SP_NA_1: // single point YX
    pThis->handleRecvRequestUpdateDeviceStatus(asdu);
    break;
  case M_DP_NA_1: // double point YX
    //pThis->handleRecvRequestUpdateDevStatusDP(asdu);
    break;
  case M_BO_NA_1: // multi point YC
    //pThis->handleRecvRequestUpdateMultiDevicesStatus(asdu);
    break;
  case C_TS_TA_1: // test with timestamp
    qDebug() << ("  test command with timestamp\n");
    break;
  case C_SC_NA_1: // amo single step
    for (int i = 0; i < asduElemCount; i++)
    {

      auto object = CS101_ASDU_getElement(asdu, i);

      qDebug() << QString("---IOA: %1 cot: %2 asduType: %3").arg(
                          InformationObject_getObjectAddress(object)).arg(cot).arg(asduType);

      InformationObject_destroy(object);
    }
    //pThis->handleRecvRequestCheckSingleAmoStep(asdu);
    break;
  case 43: // type of operate ticket
    //pThis->handleRecvRequestCheckOperationTicket(asdu);
    break;
  case C_IC_NA_1: // handled in other place
    break;
  default:
    // log invalid type
    qDebug() << QString("IEC-104 Master: Asdu dropped with type not handled, type: %1").arg(asduType);
    return false;
  }
  return true;
}

void IEC104Master::onReconnectTimerTriggered()
{
  if (this->localAddr.isEmpty() || this->remoteAddr.isEmpty())
  {
    qDebug() << "地址信息不全,请补充后再次手动连接";
    stop();
    _isEnabled.store(false);
    return;
  }

  start(this->localAddr, this->localPort, this->remoteAddr, this->remotePort);
}

void IEC104Master::onInterrogationTimerTriggered()
{
  sendInterrogation();
}

void IEC104Master::sendTestCommand()
{
  struct sCP56Time2a testTimestamp;
  CP56Time2a_createFromMsTimestamp(&testTimestamp, Hal_getTimeInMs());

  CS104_Connection_sendTestCommandWithTimestamp(_con, 1, 0x01, &testTimestamp);

}


bool IEC104Master::sendYKOpen()
{
  if (_con == nullptr)
  {
    qDebug() << "   no create connection";
    return false;
  }

  if (!_isConnected.load())
  {
    qDebug() << "   connection closed";
    return false;
  }

  InformationObject openRelay = (InformationObject)SingleCommand_create(NULL, 0x6001, false, false, 0);
  if (CS104_Connection_sendProcessCommandEx(_con, CS101_COT_ACTIVATION, 1, openRelay) == false)
  {
    qDebug() << "IEC104 Master: Send Open Relay failed.";
    return false;
  }
  return true;
}

bool IEC104Master::sendYKClose()
{
  if (_con == nullptr)
  {
    qDebug() << "   no create connection";
    return false;
  }

  if (!_isConnected.load())
  {
    qDebug() << "   connection closed";
    return false;
  }

  InformationObject closeRelay = (InformationObject)SingleCommand_create(NULL, 0x6001, true, false, 0);
  if (CS104_Connection_sendProcessCommandEx(_con, CS101_COT_ACTIVATION, 1, closeRelay) == false)
  {
    qDebug() << "IEC104 Master: Send Close Relay failed.";
    return false;
  }
  return true;

}

bool IEC104Master::sendChoosedYKOpen(int ioa)
{
  if (_con == nullptr)
  {
    qDebug() << "   no create connection";
    return false;
  }

  if (!_isConnected.load())
  {
    qDebug() << "   connection closed";
    return false;
  }

  InformationObject openRelay = (InformationObject)SingleCommand_create(NULL, ioa, false, false, 0);
  if (CS104_Connection_sendProcessCommandEx(_con, CS101_COT_ACTIVATION, 1, openRelay) == false)
  {
    qDebug() << "IEC104 Master: Send Open Relay failed.";
    return false;
  }
  return true;
}

bool IEC104Master::sendChoosedYKClose(int ioa)
{
  if (_con == nullptr)
  {
    qDebug() << "   no create connection";
    return false;
  }

  if (!_isConnected.load())
  {
    qDebug() << "   connection closed";
    return false;
  }

  InformationObject closeRelay = (InformationObject)SingleCommand_create(NULL, ioa, true, false, 0);
  if (CS104_Connection_sendProcessCommandEx(_con, CS101_COT_ACTIVATION, 1, closeRelay) == false)
  {
    qDebug() << "IEC104 Master: Send Close Relay failed.";
    return false;
  }
  return true;
}

void IEC104Master::handleConnectionOpened()
{
  _isConnected.store(true);
  emit connectionEstablished(_isEnabled.load());
}

void IEC104Master::handleConnectionClosed()
{
  _isConnected.store(false);
  emit connectionClosed(_isEnabled.load());
  QMetaObject::invokeMethod(this, [this]()
                            {
                              stop();
                              if(_isEnabled.load())
                              {
                                _reconnectTimer->start();
                              }
                            }, Qt::QueuedConnection);
}

void IEC104Master::handleConnectionFailed()
{
  handleConnectionClosed();
}

void IEC104Master::sendInterrogation()
{
  if (_con == nullptr) // stopped
  {
    qDebug() << "IEC104 Master: Connection stopped while ready to send interrogation.";
    return;
  }

  if (CS104_Connection_sendInterrogationCommand(_con, CS101_COT_ACTIVATION, 1, IEC60870_QOI_STATION) == false)
  {
    qDebug() << "IEC104 Master: Send interrogation failed!.";
    QMetaObject::invokeMethod(this, [this]()
                              {
                                _interrogationTimer->start();
                              }, Qt::QueuedConnection);
  }


}

void IEC104Master::handleRecvRequestUpdateDeviceStatus(CS101_ASDU asdu)
{
  auto asduElemCount = CS101_ASDU_getNumberOfElements(asdu);
  auto cot = CS101_ASDU_getCOT(asdu);

  if (cot == 20)
  {
    for (int i = 0; i < asduElemCount; i++)
    {
      if(i % 4 != 1) continue;

      auto object = CS101_ASDU_getElement(asdu, i);

      qDebug() << QString(" --IOA: %1 value: %2 quality: %3").arg(
                                                                  InformationObject_getObjectAddress(object)).arg(
                          SinglePointInformation_getValue((SinglePointInformation) object)).arg(
                          SinglePointInformation_getQuality((SinglePointInformation) object));

      relayStatus[InformationObject_getObjectAddress(object)] = SinglePointInformation_getValue((SinglePointInformation) object);
      qDebug() << "'    QMap relayStatus test' || IOA: " << \
          InformationObject_getObjectAddress(object) << " sta"\
          "us: " << SinglePointInformation_getValue((SinglePointInformation) object);

      InformationObject_destroy(object);
    }
    emit receiveCot20(relayStatus);
  }
  else if (cot == 3)
  {
    auto object = CS101_ASDU_getElement(asdu, 0);

    qDebug() << QString("  -IOA: %1 value: %2 ").arg(
                        InformationObject_getObjectAddress(object)).arg(
                        SinglePointInformation_getValue((SinglePointInformation) object));

    relayStatus[InformationObject_getObjectAddress(object)] = SinglePointInformation_getValue((SinglePointInformation) object);
    qDebug() << "'    QMap relayStatus test' || IOA: " << \
        InformationObject_getObjectAddress(object) << " sta"\
        "us: " << SinglePointInformation_getValue((SinglePointInformation) object);

    InformationObject_destroy(object);
  }
  else
  {

  }

}


void IEC104Master::onConnectButtonClicked()
{
  _isEnabled.store(true);
}

void IEC104Master::onDisconnectButtonClicked()
{
  _isEnabled.store(false);
}


