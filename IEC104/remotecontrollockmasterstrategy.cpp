#include "remotecontrollockmasterstrategy.h"

RemoteControlLockMasterStrategy::RemoteControlLockMasterStrategy()
{
  // {
  //   _progressDialog = std::make_shared<QDialog>();
  //   _progressDialog->setModal(true);

  //   auto layout = new QVBoxLayout(_progressDialog.get());
  //   auto label = new QLabel(tr("Quering Remote Control Device Status ..."));
  //   auto progressBar = new QProgressBar(_progressDialog.get());
  //   progressBar->setRange(0, 0);

  //   progressBar->setStyleSheet( "QProgressBar {" " border: 2px solid grey;" " border-radius: 5px;" " text-align: center;" " background: #f3f3f3;" "}" "QProgressBar::chunk {" " background: #05B8CC;" " width: 20px;" " margin: 0.5px;" "}" );

  //   layout->addWidget(label);
  //   layout->addWidget(progressBar);

  //   _progressDialog->setLayout(layout);
  //   _progressDialog->setMinimumSize(_progressDialog->sizeHint());
  //   _progressDialog->setMaximumSize(_progressDialog->sizeHint());
  //   _progressDialog->setWindowFlags(_progressDialog->windowFlags() & ~Qt::WindowCloseButtonHint);
  // }
}

void RemoteControlLockMasterStrategy::handleSPNA(CS101_ASDU asdu)
{

}

void RemoteControlLockMasterStrategy::handleSCNA(CS101_ASDU asdu)
{

}

bool RemoteControlLockMasterStrategy::sendYK(int deviceID, bool command)
{
  return false;
}

bool RemoteControlLockMasterStrategy::sendSyncYK(int deviceID, bool command, int ms)
{
  return false;
}

bool RemoteControlLockMasterStrategy::queryRemoteControlDeviceStatus()
{
  return false;
}

bool RemoteControlLockMasterStrategy::isDevStatusCotValid(int cot)
{
  return false;
}

bool RemoteControlLockMasterStrategy::isCheckSingleAmoStepCotValid(int cot)
{
  return false;
}

void RemoteControlLockMasterStrategy::devicesStatusRecved(QMap<int, std::tuple<bool, uint8_t> > devicePoint2Status)
{

}

void RemoteControlLockMasterStrategy::handleRelayYX(int point, bool state, uint8_t quality, int deviceID)
{

}

void RemoteControlLockMasterStrategy::handleGroundingWireYX(int point, bool state, uint8_t quality, int deviceID)
{

}

void RemoteControlLockMasterStrategy::handleStateDetectorYX(int point, bool state, uint8_t quality, int deviceID)
{

}
