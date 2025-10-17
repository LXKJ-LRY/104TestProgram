#ifndef REMOTECONTROLLOCKMASTERSTRATEGY_H
#define REMOTECONTROLLOCKMASTERSTRATEGY_H

#include "iec104masterstrategy.h"
#include <QDialog>
#include <memory>
#include <QObject>

#include <QSet>
#include <QMap>
#include <QVBoxLayout>
#include <QLabel>
#include <QProgressBar>

class RemoteControlLockMasterStrategy : public IEC104MasterStrategy
{
  Q_OBJECT
  public:
    RemoteControlLockMasterStrategy();

    virtual void handleSPNA(CS101_ASDU asdu) override;
    virtual void handleSCNA(CS101_ASDU asdu) override;
    virtual bool sendYK(int deviceID, bool command) override;
    virtual bool sendSyncYK(int deviceID, bool command, int ms = 3000) override;
    virtual bool queryRemoteControlDeviceStatus() override;

  signals:
    void exitLoop(bool result);
    void sendYKResult(long devID, bool command, bool result);

  private:

    static bool isDevStatusCotValid(int cot);
    static bool isCheckSingleAmoStepCotValid(int cot);

    void devicesStatusRecved(QMap<int, std::tuple<bool, uint8_t>> devicePoint2Status);

    void handleRelayYX(int point, bool state, uint8_t quality, int deviceID);
    void handleGroundingWireYX(int point, bool state, uint8_t quality, int deviceID);
    void handleStateDetectorYX(int point, bool state, uint8_t quality, int deviceID);

  private:
    std::unordered_map<int, bool> _deviceAddrHash;
    std::shared_ptr<QDialog> _progressDialog;
};

#endif // REMOTECONTROLLOCKMASTERSTRATEGY_H
