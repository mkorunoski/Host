#include "experiment.h"

#include <algorithm>

#include <QDebug>
#include <QLineSeries>
#include <QValueAxis>
#include <QPainter>

Graph::Graph(const QString &title, const QList<double> &data)
    : mChartViewPtr(nullptr)
{
    QLineSeries *seriesPtr = new QLineSeries;
    int i = 1;
    for (auto it = data.cbegin(); it != data.cend(); ++it) {
        seriesPtr->append(i++, *it);
    }

    QChart *chartPtr = new QChart;
    chartPtr->legend()->hide();
    chartPtr->addSeries(seriesPtr);
    chartPtr->setTitle(title);

    QValueAxis *axisX = new QValueAxis;
    axisX->setTitleText("Run");
    axisX->setLabelFormat("%d");
    axisX->setTickCount(seriesPtr->count());
    chartPtr->addAxis(axisX, Qt::AlignBottom);
    seriesPtr->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis;
    axisY->setTitleText("Execution time");
    axisY->setLabelFormat("%.2f");
    axisY->setTickCount(seriesPtr->count());
    chartPtr->addAxis(axisY, Qt::AlignLeft);
    seriesPtr->attachAxis(axisY);

    mChartViewPtr = new QChartView(chartPtr);
    mChartViewPtr->resize(800, 600);
    mChartViewPtr->setRenderHint(QPainter::Antialiasing);
}

Graph::~Graph()
{
    delete mChartViewPtr;
}

void Graph::saveAsImage(const QString &fileName) const
{
    QPixmap pixmap = mChartViewPtr->grab();

    QFile file(fileName);
    file.open(QFile::WriteOnly);
    pixmap.save(&file, "png");
}

Test::Test(int bufferSize)
    : mBufferSize(bufferSize)
{

}

double Test::averageExecTime() const
{
    return std::accumulate(mExecTimes.begin(), mExecTimes.end(), 0.0) / mExecTimes.length();
}

void Test::saveResults(const QString &fileName) const
{
    QFile file(fileName);
    file.open(QFile::WriteOnly);
    QTextStream stream(&file);
    for (double val : mExecTimes)
        stream << val << "\n";

//    Graph graph(QString("Execution time for %1-tuple buffer size").arg(mBufferSize), mExecTimes);
//    graph.saveAsImage(fileName);
}

Experiment* Experiment::mInstancePtr = new Experiment;

Experiment* Experiment::instance()
{
    return mInstancePtr;
}

Experiment::~Experiment()
{
    delete mInstancePtr;
}

int Experiment::bufferSzMul()
{
    if (!mBufferSzMul.isEmpty())
        return mBufferSzMul.first();
    return -1;
}

void Experiment::newTest()
{
    mBufferSzMul.removeFirst();

    if (!mBufferSzMul.isEmpty())
    {
        mRun = 8;
        auto first = mBufferSzMul.first();
        mTests.append(Test(first));
    }
    else
    {
        emit experimentEnds();
    }
}

void Experiment::nextRun()
{
    mRun--;
}

bool Experiment::isLastRun()
{
    return mRun == 0;
}

void Experiment::addExecTimeCurrTest(double execTime)
{
    mTests.last().mExecTimes.append(execTime);
}

void Experiment::saveResults(const QString &location) const
{
    QFile file(location + QString("/graph_average.txt"));
    file.open(QFile::WriteOnly);
    QTextStream stream(&file);
    int i = 1;
    for (auto it = mTests.cbegin(); it != mTests.cend(); ++it)
    {
        it->saveResults(location + QString("/graph_%1.txt").arg(i++));
        stream << it->averageExecTime() << "\n";
    }

//    QList<double> averageTimes;
//    int i = 1;
//    for (auto it = mTests.cbegin(); it != mTests.cend(); ++it)
//    {
//        it->saveResults(location + QString("/graph_%1.png").arg(i++));
//        averageTimes.append(it->averageExecTime());
//    }
//    Graph graph(QString("Average execution time for variable buffer length"), averageTimes);
//    graph.saveAsImage(location + QString("/graph_average.png"));
}

Experiment::Experiment(QObject *parent)
    : QObject(parent),
      mRun(8)
{
    for (int i = 1; i <= mRun; ++i)
         mBufferSzMul << i;

    auto first = mBufferSzMul.first();
    mTests.append(Test(first));
}

