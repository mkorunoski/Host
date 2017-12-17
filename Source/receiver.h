#ifndef RECEIVER_H
#define RECEIVER_H

#include <QObject>
#include <QMediaPlayer>
#include <QAudioProbe>
#include <QList>
#include <QByteArray>

class Receiver : public QObject
{
    Q_OBJECT

public:
    Receiver(const QString &, QObject *parent = nullptr);

    void receive();

public slots:
    void onAudioBufferProbed(QAudioBuffer);

signals:
    void dataReady(QList<double>);

private:
    void calc();

    static const quint8 sMaxNumData;
    static const quint16 sMaxNumLinesPerFile;

    quint8 mCurrNumData;
    quint16 mCurrNumFileLines;

    int mBufferSzMul;

    double mSumData;
    QList<double> mData;

    QScopedPointer<QMediaPlayer> mMediaPlayerPtr;
    QScopedPointer<QAudioProbe> mAudioProbePtr;
    QScopedPointer<QByteArray> mArrayPtr;
};

#endif // RECEIVER_H
