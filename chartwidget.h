#ifndef CHARTWIDGET_H
#define CHARTWIDGET_H


#include <QtCharts/QChartGlobal>
#include <QtCharts/QtCharts>
#include <QtCharts/QChartView>
#include <QtWidgets/QWidget>
#include <QtWidgets/QGraphicsWidget>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGraphicsGridLayout>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGroupBox>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QAreaSeries>


#include "mousepresseater.h"
#include "chartview.h"

class ChartWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ChartWidget(QWidget *parent = nullptr);

    void setHeader(const QStringList &newData);

signals:
    void hovered(QString);

public slots:
    void process(QList<QAbstractSeries *>);
    void setTitle(const QString &);

private slots:
    void connectMarkers();
    void disconnectMarkers();
    void removeSeries();
    void handleMarkerClicked();
    void setSeries(QList<QAbstractSeries *>);

private:

    QChart *m_chart;
    ChartView *m_chartView;
//    QGridLayout *m_mainLayout;
    MousePressEater *m_mousePressEater;
    QStringList m_sL;
    QList<QLabel *> m_headerLabels;
//    QGraphicsDropShadowEffect *m_shadowEffect;
//    QLabel *m_label;
protected:
    bool eventFilter(QObject *obj, QEvent *event) override {
        if (event->type() == QEvent::HoverEnter) {
//            qDebug() << "Hovered";
//            m_shadowEffect->setBlurRadius(25);
            emit hovered(m_chart->title());
//            setStyleSheet("background-color: #e0e0e0;");
        }
        else if (event->type() == QEvent::HoverLeave) {
//            m_shadowEffect->setBlurRadius(0);
//            setStyleSheet("background-color: #f0f0f0;");
        }
        return QWidget::eventFilter(obj, event);
    }

};

#endif // CHARTWIDGET_H
