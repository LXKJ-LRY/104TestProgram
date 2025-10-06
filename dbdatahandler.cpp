#include "dbdatahandler.h"

DBDataHandler* DBDataHandler::_instance = new DBDataHandler;

DBDataHandler::DBDataHandler()
{
    dataBase = QSqlDatabase::addDatabase("QSQLITE");
    dataBase.setDatabaseName("TestProgram.sqlite");
    qDebug() << (bool)dataBase.open();

    query = (QSqlQuery)dataBase;
    sql = "create table if not exists t_ConnectionSettings (SettingID int primary key, "
                  "SettingName text not NULL, LocalMasterAddr varchar(15), LocalPort int,"
                  " RemoteSlaveAddr varchar(15), RemotePort int);";
    qDebug() << "创建104连接设置表：" << (bool)query.exec(sql);
    sql.clear();

    sql = "create table if not exists t_HistoryTests "
          "(TestNO integer primary key autoincrement, DeviceName text, ActualState bool, ExpectState bool);";
    qDebug() << "创建历史测试记录表：" << (bool)query.exec(sql);
    sql.clear();

    sql = "create table if not exists t_Devices "
          "(DeviceNO int primary key, DeviceName text, Address int, YXpoint int, YKpoint int);";
    qDebug() << "创建设备表：" << (bool)query.exec(sql);
    sql.clear();

}

DBDataHandler::~DBDataHandler()
{
    dataBase.close();
}

DBDataHandler* DBDataHandler::instance()
{
    return _instance;
}

void DBDataHandler::release()
{
    if( _instance != nullptr)
    {
        delete _instance;
        _instance = nullptr;
    }
}

void DBDataHandler::addNewSettingToDB()
{
    static QString baseName = "新的配置";

    sql = "SELECT SettingName FROM t_ConnectionSettings WHERE SettingName LIKE :baseName;";
    query.prepare(sql);
    query.bindValue(":baseName", baseName + "%");

    if (!query.exec()) {
        qDebug() << "查询配置失败";
        return;
    }

    // 3. 解析查询结果，提取最大数字后缀
    int maxNum = -1;  // 初始值为-1（表示无数字后缀的记录）
    bool hasBaseNameRecord = false;  // 是否存在“新的配置”（无后缀）

    while (query.next())
    {
        QString name = query.value("SettingName").toString();

        if (name == baseName) {
            hasBaseNameRecord = true;
            continue;
        }

        // 情况2：名称是“新的配置X”（带数字后缀），用正则提取X
        // 正则表达式：匹配“新的配置”后面跟1个或多个数字
        QRegularExpression regex(baseName + "(\\d+)");
        QRegularExpressionMatch match = regex.match(name);

        if (match.hasMatch())
        {
            // 提取数字部分（如“3”）并转换为整数
            int num = match.captured(1).toInt();
            // 更新最大数字
            if (num > maxNum)
            {
                maxNum = num;
            }
        }
    }

    QString newName;
    if (!hasBaseNameRecord && maxNum == -1)
    {
        newName = baseName;
    }
    else if (hasBaseNameRecord && maxNum == -1)
    {
        newName = baseName + "1";
    }
    else
    {
        newName = baseName + QString::number(maxNum + 1);
    }

    query.prepare("INSERT INTO t_ConnectionSettings (SettingName) VALUES (:name);");
    query.bindValue(":name", newName);

    if (query.exec())
    {
        qDebug() << "新增配置成功";
    }
    else
    {
        qDebug() << "新增配置失败";
    }
}

void DBDataHandler::onRefreshSettingComboBox(QList<QString>& settingsNameList)
{
    sql.clear();
    sql = "select SettingName from t_ConnectionSettings";
    if(!query.exec(sql))
    {
        qDebug() << "查询已有设置名称失败";
        return;
    }
    while(query.next())
    {
        settingsNameList.append(query.value("SettingName").toString());
    }
    sql.clear();
}
