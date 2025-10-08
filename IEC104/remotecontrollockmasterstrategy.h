#ifndef REMOTECONTROLLOCKMASTERSTRATEGY_H
#define REMOTECONTROLLOCKMASTERSTRATEGY_H

#include "iec104masterstrategy.h"

#include <QObject>

class RemoteControlLockMasterStrategy : public IEC104MasterStrategy
{
  Q_OBJECT
public:
  explicit RemoteControlLockMasterStrategy();

signals:
};

#endif // REMOTECONTROLLOCKMASTERSTRATEGY_H
