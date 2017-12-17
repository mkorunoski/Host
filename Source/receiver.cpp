#include "receiver.h"

#include <ctime>

#include <QAudioBuffer>
#include <QDateTime>
#include <QDebug>

#include "experiment.h"

const quint8 Receiver::sMaxNumData          = 32;
const quint16 Receiver::sMaxNumLinesPerFile = 512;

Receiver::Receiver(const QString &mediaLocation, QObject *parent)
    : QObject(parent),
      mCurrNumData(0),
      mCurrNumFileLines(0),
      mBufferSzMul(Experiment::instance()->bufferSzMul()),
      mSumData(0),
      mMediaPlayerPtr(new QMediaPlayer(this)),
      mAudioProbePtr(new QAudioProbe(this)),
      mArrayPtr(new QByteArray)
{
    mMediaPlayerPtr->setMedia(QUrl(mediaLocation));
    mMediaPlayerPtr->setVolume(0);

    mAudioProbePtr->setSource(mMediaPlayerPtr.data());

    connect(mAudioProbePtr.data(), SIGNAL(audioBufferProbed(QAudioBuffer)), this, SLOT(onAudioBufferProbed(QAudioBuffer)), Qt::QueuedConnection);
}

void Receiver::receive()
{
    mMediaPlayerPtr->play();
}

void Receiver::onAudioBufferProbed(QAudioBuffer audioBuffer)
{
    mArrayPtr->append(QByteArray(audioBuffer.constData<char>(), audioBuffer.byteCount()));

    if (mBufferSzMul-- < 0)
    {
        auto start = std::clock();
        calc();
        auto stop  = std::clock();

        mArrayPtr.reset(new QByteArray);

        auto execTime = (stop - start) / (double) CLOCKS_PER_SEC * 1000;
        Experiment::instance()->addExecTimeCurrTest(execTime);
        mBufferSzMul = Experiment::instance()->bufferSzMul();

        Experiment::instance()->nextRun();
        if (Experiment::instance()->isLastRun())
            Experiment::instance()->newTest();
    }
}

void Receiver::calc()
{
    for (auto it = mArrayPtr->cbegin(); it != mArrayPtr->cend(); ++it)
    {
        mSumData += (double) *it;

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
                break;
            }
        }
    }
}


