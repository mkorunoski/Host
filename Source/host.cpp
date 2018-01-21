#include "host.h"

#include <QApplication>
#include <QDateTime>
#include <QDir>
#include <QStringRef>

#include "experiment.h"

Host::Host(const QString &address, int port, const QString &dataFolderLocation, int maxNumFiles, QObject *parent)
    : QObject(parent),
      mDataFolderLocation(dataFolderLocation)
{
    initHostName();

    mSocketPtr = new QTcpSocket(this);

    connect(mSocketPtr, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(mSocketPtr, SIGNAL(disconnected()), this, SLOT(onDisconnected()));

    mSocketPtr->connectToHost(address, port);

    if (!mSocketPtr->waitForConnected())
    {
        qCritical() << "Can't connect to server! Error:" << mSocketPtr->errorString();
    }

    mFilesPtr = new Files(maxNumFiles);
    mFilesPtr->newFile(newFilename().toStdString());

    connect(Experiment::instance(), SIGNAL(experimentEnds()), this, SLOT(onExperimentEnds()));
}

Host::~Host()
{
    mSocketPtr->disconnectFromHost();
    delete mFilesPtr;
    delete mReceiverPtr;
    delete mSocketPtr;
}

void Host::receive(const QString &mediaLocation)
{
    mReceiverPtr = new Receiver(mediaLocation, this);
    connect(mReceiverPtr, SIGNAL(dataReady(QList<double>)), this, SLOT(onDataReady(QList<double>)), Qt::QueuedConnection);

    mReceiverPtr->receive();
}

void Host::onDataReady(QList<double> data)
{
    for (auto d : data)
    {
       mFilesPtr->write(d);
    }

    if (mSocketPtr->isOpen())
    {
        mSocketPtr->write(mFilesPtr->currFilename().c_str());
        mSocketPtr->flush();
    }

    mFilesPtr->newFile(newFilename().toStdString());
}

void Host::onConnected()
{
    qInfo() << "Connected!";
}

void Host::onDisconnected()
{
    qInfo() << "Disconnected!";
}

void Host::onExperimentEnds()
{
    Experiment::instance()->saveResults(mDataFolderLocation);
    QApplication::exit();
}

void Host::initHostName()
{
    mHostName = "";
    srand(time(0));

    QString letters = "abcdefghijklmnopqrstuvwxyz";
    const size_t len = 10;
    for (size_t i = 0; i < len; ++i)
    {
        auto r = rand() % letters.length();
        Q_ASSERT(r < letters.length());
        mHostName.append(letters.at(r));
    }
}

QString Host::newFilename()
{
    QString currDateTime    = QDateTime::currentDateTime().toString("ddMMyyyyhhmmsszzz");
    QString filename        = mHostName + currDateTime + ".txt";
    QString newFilelocation = mDataFolderLocation + filename;

    return newFilelocation;
}
