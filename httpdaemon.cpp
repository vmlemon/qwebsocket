#include "httpdaemon.h"

#include <QTcpSocket>
#include <QStringList>
#include <QDateTime>
#include <QFile>

#include <QDebug>

HttpDaemon::HttpDaemon(): QTcpServer()
{
    qDebug() << "HttpDaemon : New instance was created";

    listen(QHostAddress::Any);
}

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
        QRegExp pathPattern("^/websocket\\.(html|js)$");
        if (pathPattern.exactMatch(tokens[1]))
        {

            qDebug() << "HttpDaemon : WebSockets proxy requested";

            QFile file (":" + pathPattern.capturedTexts()[0]);
            file.open(QFile::ReadOnly);
            os << file.readAll()
               << "\n\n";
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

void HttpDaemon::discardClient()
{
    QTcpSocket* socket = (QTcpSocket*)sender();
    socket->deleteLater();
}
