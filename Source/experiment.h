#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QChartView>

QT_CHARTS_USE_NAMESPACE

class Graph
{
public:
    Graph(const QString &title, const QList<double> &data);
    ~Graph();
    void saveAsImage(const QString &fileName) const;

private:
    QChartView *mChartViewPtr;

};

class Test
{
    friend class Experiment;

public:
    Test() = default;
    Test(int bufferSize);
    double averageExecTime() const;

private:
    void saveResults(const QString &fileName) const;

    int mBufferSize;
    QList<double> mExecTimes;
};

class Experiment : public QObject
{
    Q_OBJECT

public:
    static Experiment* instance();
    ~Experiment();

    int bufferSzMul();
    void newTest();
    void nextRun();
    bool isLastRun();
    void addExecTimeCurrTest(double execTime);
    void saveResults(const QString &location) const;

signals:
    void experimentEnds();

private:
    Experiment(QObject *parent = nullptr);

    static Experiment *mInstancePtr;

    QVector<int> mBufferSzMul;
    int mRun;
    QVector<Test> mTests;

};

#endif // EXPERIMENT_H
