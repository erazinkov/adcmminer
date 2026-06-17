#ifndef BARCHARTWIDGET_H
#define BARCHARTWIDGET_H

#include <QWidget>
#include <QChart>
#include <QChartView>
#include <QBarSeries>
#include <QVBoxLayout>
#include <QValueAxis>
#include <QBarCategoryAxis>
#include <QBarSet>

class BarChartWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BarChartWidget(const QString &title, QWidget *parent = nullptr);

signals:

private:
    QChart *m_chart;
    QChartView *m_chartView;
    QBarSeries *m_series;
    QVBoxLayout *m_mainLayout;
    QBarSet *m_set;

    QBarCategoryAxis *m_axisX;
    QValueAxis *m_axisY;
    QStringList m_categories;
};

#endif // BARCHARTWIDGET_H
