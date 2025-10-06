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
public:
    static DBDataHandler* instance();

    static void release();

    void addNewSettingToDB();
    void onRefreshSettingComboBox(QList<QString>& settingsNameList);

private:
    DBDataHandler();
    DBDataHandler(const DBDataHandler& ){}
    ~DBDataHandler();

    static DBDataHandler* _instance;
    QSqlDatabase dataBase;
    QSqlQuery query;
    QString sql;

};

#endif // DBDATAHANDLER_H
