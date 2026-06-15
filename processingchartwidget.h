#ifndef PROCESSINGCHARTWIDGET_H
#define PROCESSINGCHARTWIDGET_H

#include <QObject>
#include <QWidget>

#include <QChart>
#include <QChartView>
#include <QPieSeries>
#include <QVBoxLayout>

class ProcessingChartWidget : public QWidget
{
    Q_OBJECT
public:
    ProcessingChartWidget(QWidget *parent = nullptr);
public slots:
    void setData(const QMap<QString, double> &);
private:
    QChart *m_chart;
    QChartView *m_chartView;
    QPieSeries *m_series;
    QVBoxLayout *m_mainLayout;
};

#endif // PROCESSINGCHARTWIDGET_H
