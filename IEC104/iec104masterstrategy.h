#ifndef IEC1_4MASTERSTRATEGY_H
#define IEC1_4MASTERSTRATEGY_H

#include "lib60870/cs104_connection.h"
#include "lib60870/iec60870_common.h"

#include <QObject>
#include <QMap>

class IEC104MasterStrategy : public QObject
{
  Q_OBJECT
public:
  virtual void handleSPNA(CS101_ASDU asdu){}
  virtual void handleDPNA(QMap<int, std::tuple<int, uint8_t>> devicePoint2Status){}
  virtual void handleBONA(CS101_ASDU asdu){}
  virtual void handleSCNA(CS101_ASDU asdu){}
  virtual void handle43(CS101_ASDU asdu){}

  virtual bool sendYK(int deviceID, bool command){return false;}
  virtual bool sendSyncYK(int deviceID, bool command, int ms = 3000){return false;}

  virtual bool queryRemoteControlDeviceStatus(){return false;};

  void setPointName(const QString& pointName)
  {
    _point = pointName;
  }

  void setConnection(CS104_Connection con)
  {
    _con = con;
  }

  virtual ~IEC104MasterStrategy() = default;

protected:
  QString _point;
  CS104_Connection _con = nullptr;

signals:
};

#endif // IEC1_4MASTERSTRATEGY_H
