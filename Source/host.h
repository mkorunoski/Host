#ifndef HOST_H
#define HOST_H

#include <QObject>
#include <QTcpSocket>
#include <QList>

#include "receiver.h"
#include "files.h"

class Host : public QObject
{
    Q_OBJECT

public:
    Host(const QString &, int, const QString &, int, QObject *parent = nullptr);
    void receive(const QString &);
    ~Host();

public slots:
    void onDataReady(QList<double>);
    void onConnected();
    void onDisconnected();

private:
    void initHostName();
    QString newFilename();

    QString mHostName;
    QString mDataFolderLocation;

    QTcpSocket *mSocketPtr;
    Receiver *mReceiverPtr;
    Files *mFilesPtr;

};

#endif // HOST_H
