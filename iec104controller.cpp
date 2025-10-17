#include "iec104controller.h"

Iec104Controller* Iec104Controller::_104controller = new Iec104Controller;

void Iec104Controller::release()
{
  if (_104controller != nullptr)
  {
    delete _104controller;
    _104controller = nullptr;
  }
}

Iec104Controller::Iec104Controller(QObject *parent)
    : QObject{parent}
{
  master = new IEC104Master(this);
  qRegisterMetaType<QMap<int, bool>>("QMap<int,bool>");

  connect(master, &IEC104Master::connectionEstablished, this, &Iec104Controller::masterEstablishConnection);
  connect(master, &IEC104Master::connectionClosed, this, &Iec104Controller::masterConnectionClosed);
  connect(master, &IEC104Master::receiveCot20, this, &Iec104Controller::masterReceiveCot20);
  connect(master, &IEC104Master::receiveSinglePointStatus, this, &Iec104Controller::masterReceiveSinglePointStatus);
}

Iec104Controller::~Iec104Controller()
{

}


void Iec104Controller::onConnection(QString localAddr, int localPort, QString remoteAddr, int remotePort)
{
  qDebug() << "hello2";
  master->onConnectButtonClicked();
  master->start(localAddr, localPort, remoteAddr, remotePort);
}

void Iec104Controller::onDisConnection()
{
  qDebug() << "stop1";
  master->onDisconnectButtonClicked();
  master->stop();
}

void Iec104Controller::onSendTestCommandButton()
{
  master->sendTestCommand();
}

void Iec104Controller::onSendYKOpenButtonClicked()
{
  master->sendYKOpen();
}

void Iec104Controller::onSendYKCloseButtonClicked()
{
  master->sendYKClose();
}

void Iec104Controller::onSendChoosedRelayYKClose(int ioa)
{
  master->sendChoosedYKClose(ioa);
}

void Iec104Controller::onSendChoosedRelayYKOpen(int ioa)
{
  master->sendChoosedYKOpen(ioa);
}

void Iec104Controller::onStartTest(int ioa)
{
  master->startTestTo10000(ioa);
}

void Iec104Controller::onStopTest(int ioa)
{
  master->stopTestTo10000(ioa);
}
