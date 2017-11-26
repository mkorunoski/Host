#include "host.h"

#include <QDateTime>
#include <QDir>

#ifdef unix
#include "was/storage_account.h"
#include "was/file.h"
#endif

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

#ifdef unix
    // Define the connection-string with your values.
    const utility::string_t storage_connection_string(U("DefaultEndpointsProtocol=https;AccountName=your_storage_account;AccountKey=your_storage_account_key"));
    // Retrieve storage account from connection string.
    azure::storage::cloud_storage_account storage_account = azure::storage::cloud_storage_account::parse(storage_connection_string);
    // Create the Azure Files client.
    azure::storage::cloud_file_client file_client = storage_account.create_cloud_file_client();
    // Get a reference to the file share
    azure::storage::cloud_file_share share = file_client.get_share_reference(_XPLATSTR("my-sample-share"));
    if (share.create_if_not_exists())
    {
        std::wcout << U("New share created") << std::endl;
    }
#endif
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
    connect(mReceiverPtr, SIGNAL(dataReady(QList<double>)), this, SLOT(onDataReady(QList<double>)));

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
    QString currDateTime    = QDateTime::currentDateTime().toString("ddMMyyyyhhmmss");
    QString filename        = mHostName + currDateTime + ".txt";
    QString newFilelocation = mDataFolderLocation + filename;

    return newFilelocation;
}
