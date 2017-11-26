#ifndef RECEIVER_H
#define RECEIVER_H

#include <QObject>
#include <QMediaPlayer>
#include <QAudioProbe>
#include <QList>

class Receiver : public QObject
{
    Q_OBJECT

public:
    Receiver(const QString &, QObject *parent = nullptr);
    ~Receiver();

    void receive();

public slots:
    void onAudioBufferProbed(QAudioBuffer);

signals:
    void dataReady(QList<double>);

private:
    static const quint8 sMaxNumData;
    static const quint16 sMaxNumLinesPerFile;

    quint8 mCurrNumData;
    quint16 mCurrNumFileLines;

    double mSumData;
    QList<double> mData;

    QMediaPlayer *mMediaPlayerPtr;
    QAudioProbe *mAudioProbePtr;

};

#endif // RECEIVER_H
