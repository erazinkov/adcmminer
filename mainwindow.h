#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>

#include "filewatcher.h"
#include "mainwidget.h"
#include "controller.h"
#include "constants.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QThread workerThread;
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;


private slots:
    void newDataTimeCorrectedByAlpha(const QMap<QString, QList<QPointF>> &);
    void newDataAmpByGamma(const QMap<QString, QList<QPointF>> &);

    void setupTimeCorrectedByAlpha();
    void setupAmpByGamma(const int gammaNumber = AppConstants::MAX_GAMMA_NUMBER);

private:
    Ui::MainWindow *ui;
    FileWatcher *m_fileWatcher;
    QString m_path;

    QList<MainWidget *> m_mainWidgetsTimeCorrectedByAlpha;
    QList<QAbstractSeries *> m_seriesTimeCorrectesByAlpha;
    QMap<QString, QList<QPointF>> m_dataTimeCorrectedByAlpha;

    QList<MainWidget *> m_mainWidgetsAmpByGamma;
    QList<QAbstractSeries *> m_seriesAmpByGamma;
    QMap<QString, QList<QPointF>> m_dataAmpByGamma;

    QTimer *m_fileWatcherTimer;
    QPushButton *m_pushButton;
    QPushButton *m_pushButton1;
    Controller *m_controller;

    QTabWidget *m_tabWidget;

    QWidget *m_page_1;
    QWidget *m_page_2;

    QHBoxLayout *m_mainLayout;

    QWidget *m_widgetLeft;
    QWidget *m_widgetRight;

    QGridLayout *m_gLleft;
//    QGridLayout *m_gLright;


};
#endif // MAINWINDOW_H
