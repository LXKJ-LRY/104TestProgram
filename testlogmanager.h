#ifndef TESTLOGMANAGER_H
#define TESTLOGMANAGER_H

#include <QObject>
#include <QFile>
#include <QDateTime>
#include <QDir>
#include <QMutex>
#include <QMutexLocker>

class TestLogManager : public QObject
{
  Q_OBJECT
public:
  static TestLogManager* instance();
  void release();

  void addTestLogInFile(QString newLog);

  void onTestStop();
signals:


private:
  explicit TestLogManager(QObject *parent = nullptr);
  ~TestLogManager();
  void initialization();

private:
  static TestLogManager* _instance;

  QDir* _logFileDir;
  QFile* _logFile;
  QDateTime currentTime = QDateTime::currentDateTime();
  QString logFileName;

};

#endif // TESTLOGMANAGER_H
