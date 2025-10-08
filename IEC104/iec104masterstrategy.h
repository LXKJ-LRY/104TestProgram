#ifndef IEC1_4MASTERSTRATEGY_H
#define IEC1_4MASTERSTRATEGY_H

#include <QObject>

class IEC104MasterStrategy : public QObject
{
  Q_OBJECT
public:
  explicit IEC104MasterStrategy(QObject *parent = nullptr);

signals:
};

#endif // IEC1_4MASTERSTRATEGY_H
