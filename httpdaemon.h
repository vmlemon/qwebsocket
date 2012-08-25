#ifndef _HTTPDAEMON_H
#define _HTTPDAEMON_H

#include <QTcpServer>
#include <QSettings>

class HttpDaemon: public QTcpServer
{
    Q_OBJECT

public:
    HttpDaemon();

    void incomingConnection(int socket);

    /* Persistent storage of settings */

#ifndef Q_OS_SYMBIAN
    QVariant ReadSetting(QString aUnifiedKey, QVariant aDefaultValue);
    void WriteSetting(QString aSubsystem, QString aKeyName, QString aKeyValue);
    void WriteSetting(QString aSubsystem, QString aKeyName, bool aKeyValue);
#endif

private slots:
    void readClient();
    void discardClient();
};

#endif
