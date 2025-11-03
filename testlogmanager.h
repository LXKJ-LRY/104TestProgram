#ifndef TESTLOGMANAGER_H
#define TESTLOGMANAGER_H

#include <QObject>

class TestLogManager : public QObject
{
  Q_OBJECT
public:
  explicit TestLogManager(QObject *parent = nullptr);

signals:
};

#endif // TESTLOGMANAGER_H
