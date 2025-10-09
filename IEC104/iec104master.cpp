#include "iec104master.h"

IEC104Master::IEC104Master(QObject *parent)
    : QObject{parent}
{

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
    CS104_Connection_sendStartDT(connection);
    //pThis->handleConnectionOpened();
    break;

  case CS104_CONNECTION_CLOSED:
    //pThis->handleConnectionClosed();
    break;

  case CS104_CONNECTION_STARTDT_CON_RECEIVED:
    //pThis->sendInterrogation();
    break;

  case CS104_CONNECTION_STOPDT_CON_RECEIVED:
    break;

  case CS104_CONNECTION_FAILED:
    //pThis->handleConnectionFailed();
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
  qDebug() << "--" + printStr + "--";
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

  auto ca = CS101_ASDU_getCA(asdu);
  if (ca != 1)
  {

    return false;
  }

  switch (asduType)
  {
  case M_ME_TE_1:
  {
    printf("  measured scaled values with CP56Time2a timestamp:\n");

    int i;

    for (i = 0; i < CS101_ASDU_getNumberOfElements(asdu); i++)
    {

      MeasuredValueScaledWithCP56Time2a io =
          (MeasuredValueScaledWithCP56Time2a) CS101_ASDU_getElement(asdu, i);

      printf("    IOA: %i value: %i\n",
             InformationObject_getObjectAddress((InformationObject) io),
             MeasuredValueScaled_getValue((MeasuredValueScaled) io)
             );

      MeasuredValueScaledWithCP56Time2a_destroy(io);
    }
  } break;
  case M_SP_NA_1: // single point YX
    //pThis->handleRecvRequestUpdateDeviceStatus(asdu);
    break;
  case M_DP_NA_1: // double point YX
    //pThis->handleRecvRequestUpdateDevStatusDP(asdu);
    break;
  case M_BO_NA_1: // multi point YC
    //pThis->handleRecvRequestUpdateMultiDevicesStatus(asdu);
    break;
  case C_TS_TA_1: // test with timestamp
    printf("  test command with timestamp\n");
    break;
  case C_SC_NA_1: // amo single step
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

}

void IEC104Master::onInterrogationTimerTriggered()
{

}

void IEC104Master::sentTestCommand()
{

}
