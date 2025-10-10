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
  master->sentTestCommand();
}
