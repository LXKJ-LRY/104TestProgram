#ifndef DBDATAHANDLER_H
#define DBDATAHANDLER_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QCoreApplication>
#include <QDebug>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QSqlError>
#include <QStringList>

class DBDataHandler : public QObject
{
  Q_OBJECT

  enum ConnectionSettingsColum
  {
    SettingID = 0,
    SettingName,
    LocalMasterAddr,
    LocalPort,
    RemoteSlaveAddr,
    Remoteport
  };

public:
  static DBDataHandler* instance();

  static void release();

  void addNewSettingToDB();
  void onRefreshSettingComboBox();
  void querySingleSettingInfo(QString& settingName, QList<QString>& singleSettingInfo);

signals:
  void addNewSettingToDBFinished();
  void querySettingsNameFinished(const QList<QString> settingsNameList);
  void deleteSettingFromDBFinished();
  void saveSettingToDBFinished();

public slots:
  void onAddNewSettingToDB();
  void onDeleteSettingFromDB(const QString singleSettingName);
  void onSaveSettingToDB(const QList<QString> singleSettinginfo);

private:
  DBDataHandler(QObject *parent = nullptr);
  DBDataHandler(const DBDataHandler& ){}
  ~DBDataHandler();

  static DBDataHandler* _instance;
  QSqlDatabase dataBase;
  QSqlQuery query;
  QString sql;

};

#endif // DBDATAHANDLER_H
