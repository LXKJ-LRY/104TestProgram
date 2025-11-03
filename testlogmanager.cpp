#include "testlogmanager.h"

#include <QApplication>
#include <QDebug>

TestLogManager* TestLogManager::_instance = nullptr;

static QMutex logMutex;

TestLogManager::TestLogManager(QObject *parent)
    : QObject{parent}
{
  initialization();

}

TestLogManager::~TestLogManager()
{
  if (_logFileDir)
  {
    delete _logFileDir;
    _logFileDir = nullptr;
  }

  if (_logFile && _logFile->isOpen())
  {
    _logFile->close();
    delete _logFile;
    _logFile = nullptr;
  }
}

void TestLogManager::initialization()
{
  {
    QString LogFilesPath = QApplication::applicationDirPath() + "/TestLogs";
    _logFileDir = new QDir(LogFilesPath);
    if (!_logFileDir->exists())
    {
      bool createSuccess = _logFileDir->mkpath(".");
      if (!createSuccess)
      {
        qDebug() << "日志目录创建失败: " << LogFilesPath;
      }
      else
      {
        qDebug() << "日志目录创建成功: " << LogFilesPath;
      }
    }
  }

  {
    logFileName = QString("TestHistory_" + QDateTime::currentDateTime().toString("yyyy-MM-dd_hh") + ".log");  // "yyyy-MM-dd_hh-mm-ss"
    auto logFilePath = _logFileDir->filePath(logFileName);
    _logFile = new QFile(logFilePath);
    if (!_logFile->open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text))
    {
      qDebug() << "log file open failed : " << _logFile->errorString();
      delete _logFile;
      _logFile = nullptr;
    }
    else
    {
      qDebug() << "log file opened";
    }
  }
}


TestLogManager *TestLogManager::instance()
{
  if (!_instance)
  {
    QMutexLocker locker(&logMutex);
    if (!_instance)
    {
      _instance = new TestLogManager();
    }
  }
  return _instance;
}

void TestLogManager::release()
{
  if (_instance != nullptr)
  {
    delete _instance;
    _instance = nullptr;
  }
}

void TestLogManager::addTestLogInFile(QString newLog)
{
  QTextStream appendLogs(_logFile);
  appendLogs << newLog << "\n";
  appendLogs.flush();
}
