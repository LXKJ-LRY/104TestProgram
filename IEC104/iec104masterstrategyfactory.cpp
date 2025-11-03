#include "iec104masterstrategyfactory.h"
#include "remotecontrollockmasterstrategy.h"

std::shared_ptr<IEC104MasterStrategy> IEC104MasterStrategyFactory::getStrategy(MasterStrategyType type)
{
  if (type == eSequenceControl)
  {
    return std::make_shared<IEC104MasterStrategy>() /*std::make_shared<SequenceControlMasterStrategy>()*/;
  }
  else if (type == eRemoteControlLock)
  {
    return std::make_shared<RemoteControlLockMasterStrategy>();
  }
  return std::make_shared<IEC104MasterStrategy>();
}
