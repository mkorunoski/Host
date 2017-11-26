#include "receiver.h"

#include <QAudioBuffer>
#include <QDateTime>
#include <QDebug>

const quint8 Receiver::sMaxNumData          = 32;
const quint16 Receiver::sMaxNumLinesPerFile = 512;

Receiver::Receiver(const QString &mediaLocation, QObject *parent)
    : QObject(parent),
      mCurrNumData(0),
      mCurrNumFileLines(0),
      mSumData(0)
{
    mMediaPlayerPtr = new QMediaPlayer(this);
    mMediaPlayerPtr->setMedia(QUrl(mediaLocation));
    mMediaPlayerPtr->setVolume(0);

    mAudioProbePtr = new QAudioProbe(this);
    mAudioProbePtr->setSource(mMediaPlayerPtr);

    connect(mAudioProbePtr, SIGNAL(audioBufferProbed(QAudioBuffer)), this, SLOT(onAudioBufferProbed(QAudioBuffer)), Qt::QueuedConnection);
}

Receiver::~Receiver()
{
    delete mAudioProbePtr;
    delete mMediaPlayerPtr;
}

void Receiver::receive()
{
    mMediaPlayerPtr->play();
}

void Receiver::onAudioBufferProbed(QAudioBuffer audioBuffer)
{
    const qint32 *data = audioBuffer.constData<qint32>();
    const int size = sizeof(qint32);

    for (int byte = 0; byte < audioBuffer.byteCount() / size; byte += size)
    {
        mSumData += (double) data[byte];

        ++mCurrNumData %= sMaxNumData;
        if (mCurrNumData == 0)
        {
            mData.append((mSumData / sMaxNumData));
            mSumData = 0;

            ++mCurrNumFileLines %= sMaxNumLinesPerFile;
            if (mCurrNumFileLines == 0)
            {
                emit dataReady(mData);
                mData.clear();
            }
        }
    }
}


