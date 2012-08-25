#include "httpdaemon.h"

#include <QTcpSocket>
#include <QStringList>
#include <QDateTime>
#include <QFile>
//#include <QDir>
#include <QString>

#include <QTextStream>

#include <QDebug>

HttpDaemon::HttpDaemon(): QTcpServer()
{
    qDebug() << "HttpDaemon : New instance was created";

    listen(QHostAddress::Any);

    //QDir example = QDir(":/");
    //qDebug() << example.;
}

QSettings iPersistentStore("QWebSocket", "HttpDaemon");

void HttpDaemon::incomingConnection(int socket)
{

    qDebug() << "HttpDaemon : Incoming connection on socket" << socket;

    QTcpSocket* s = new QTcpSocket(this);
    connect(s, SIGNAL(readyRead()), this, SLOT(readClient()));
    connect(s, SIGNAL(disconnected()), this, SLOT(discardClient()));
    s->setSocketDescriptor(socket);
}

void HttpDaemon::readClient()
{
    QTcpSocket* socket = (QTcpSocket*)sender();

    if (socket->canReadLine())
    {
        QTextStream os(socket);
        os.setAutoDetectUnicode(true);
        os << "HTTP/1.0 200 Ok\r\n"
            "Content-Type: text/html; charset=\"utf-8\"\r\n"
            "\r\n";

        QString clientData = QString(socket->readLine());
        QStringList tokens = clientData.split(QRegExp("[ \r\n][ \r\n]*"));

        QString tokenSize = "(" + QString::number(tokens.size()) + ")";

        qDebug() << "HttpDaemon : Client request body elements:" << tokens
                 << tokenSize;

        qDebug() << "HttpDaemon : Client data line(s): \n\r" << QString(socket->readAll());;

        /* The client requested a WebSocket proxy file*/
        QRegExp wsPathPattern("^/websocket\\.(html|js)$");
        QRegExp formsPathPattern("^/formstest\\.(html|js)$");
        QRegExp settingsPattern("^/allsettings$");

        if (wsPathPattern.exactMatch(tokens[1]))
        {

            qDebug() << "HttpDaemon : WebSockets proxy requested";

            QFile file (":" + wsPathPattern.capturedTexts()[0]);
            file.open(QFile::ReadOnly);
            os << file.readAll()
               << "\n\n";
        }

        /* The client requested the forms test page */

        else if (formsPathPattern.exactMatch(tokens[1]))
        {

            qDebug() << "HttpDaemon : Forms test page requested";

            QFile file (":" + formsPathPattern.capturedTexts()[0]);
            file.open(QFile::ReadOnly);
            os << file.readAll()
               << "\n\n";
        }

        /* The client requested a list of settings */

        else if (settingsPattern.exactMatch(tokens[1]))
        {

            qDebug() << "HttpDaemon : Settings page requested";

            os << "<b>Server settings:</b> <br> \r\n";
               os << "Currently storing " << iPersistentStore.allKeys().size()
                  << " keys. " << "\n\n";
        }
        else

        /* The client wanted the home directory, or something else */
        {
            qDebug() << "HttpDaemon : HTTP root directory requested";

            os << "<h1>Nothing to see here</h1>\n\n";
        }

        socket->close();

        if (socket->state() == QTcpSocket::UnconnectedState)
            delete socket;
    }
}

/* Persistent storage of settings */

#ifndef Q_OS_SYMBIAN
QVariant HttpDaemon::ReadSetting(QString aUnifiedKey, QVariant aDefaultValue) {
    return iPersistentStore.value(aUnifiedKey, aDefaultValue);

    qDebug() << "Read: " << iPersistentStore.value(aUnifiedKey, "").toString();
}

void HttpDaemon::WriteSetting(QString aSubsystem, QString aKeyName, QString aKeyValue) {
    QString unifiedKey = aSubsystem + "/" + aKeyName;
    iPersistentStore.setValue(unifiedKey, aKeyValue);

    qDebug() << "Stored: " << iPersistentStore.applicationName() << unifiedKey + ", " + aKeyValue;
}

/* Overload WriteSetting() to support storage of boolean values */

void HttpDaemon::WriteSetting(QString aSubsystem, QString aKeyName, bool aKeyValue) {
    QString unifiedKey = aSubsystem + "/" + aKeyName;
    iPersistentStore.setValue(unifiedKey, aKeyValue);

    qDebug() << "Stored: " << iPersistentStore.applicationName() << unifiedKey + ", " + QString::number(aKeyValue);
}

#endif


void HttpDaemon::discardClient()
{
    QTcpSocket* socket = (QTcpSocket*)sender();
    socket->deleteLater();
}
