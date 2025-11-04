#ifndef IEC1_4MASTERSTRATEGYFACTORY_H
#define IEC1_4MASTERSTRATEGYFACTORY_H

#include <memory>
#include "iec104masterstrategy.h"


class IEC104MasterStrategyFactory
{
public:
  IEC104MasterStrategyFactory();

public:
  enum MasterStrategyType
  {
    eDefaultStrategy,
    eSequenceControl,
    eRemoteControlLock,
  };
  static std::shared_ptr<IEC104MasterStrategy> getStrategy(MasterStrategyType type);
};

#endif // IEC1_4MASTERSTRATEGYFACTORY_H
