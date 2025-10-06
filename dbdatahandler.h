#ifndef DBDATAHANDLER_H
#define DBDATAHANDLER_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

class DBDataHandler
{
public:
    DBDataHandler* instance();

private:
    DBDataHandler();
    DBDataHandler(const DBDataHandler& );
    ~DBDataHandler();

    static DBDataHandler* _instance;
    QSqlDatabase dataBase;
    QSqlQuery query;
};

#endif // DBDATAHANDLER_H
