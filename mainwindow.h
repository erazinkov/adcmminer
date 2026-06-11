#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>

#include "filewatcher.h"
#include "chartwidget.h"
#include "processingcontroller.h"
#include "constants.h"
#include "settings.h"
#include "filewatchercontroller.h"


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
    void newDataAmpByGamma(const QMap<QString, QList<QPointF>> &data, const QMap<QString, QStringList> &text);

    void setupTimeCorrectedByAlpha();
    void setupAmpByGamma(const int gammaNumber = AppConstants::MAX_GAMMA_NUMBER);

    void showDialog(QString);

    void openFile();

private:
    Ui::MainWindow *ui;
    Settings *m_settings;
    FileWatcher *m_fileWatcher;
    QString m_path;
    QLabel *m_statusMessageLabel;


    QList<ChartWidget *> m_chartWidgetsTimeCorrectedByAlpha;
    QList<QAbstractSeries *> m_seriesTimeCorrectesByAlpha;
    QMap<QString, QList<QPointF>> m_dataTimeCorrectedByAlpha;

    QList<ChartWidget *> m_chartWidgetsAmpByGamma;
    QList<QAbstractSeries *> m_seriesAmpByGamma;
    QMap<QString, QList<QPointF>> m_dataAmpByGamma;

    QTimer *m_fileWatcherTimer;
    QPushButton *m_pushButtonStartStop;
    QPushButton *m_pushButtonReset;
    ProcessingController *m_controller;
    FileWatcherController *m_fileWatcherController;

    QTabWidget *m_tabWidget;

    QWidget *m_page_1;
    QWidget *m_page_2;

    QHBoxLayout *m_mainLayout;

    QWidget *m_widgetLeft;
    QWidget *m_widgetRight;

    QGridLayout *m_gLleft;
//    QGridLayout *m_gLright;
    QDialog *m_dialog;

};
#endif // MAINWINDOW_H
