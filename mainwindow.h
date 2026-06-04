#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>

#include "filewatcher.h"
#include "mainwidget.h"
#include "controller.h"

#include "flowlayout.h"

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
    void newData(const QMap<QString, QList<QPointF>> &);
    void newDataShow();
    void onCurrentTextChanged(const QString &);

    void setupTimeCorrectedByAlpha();

private:
    Ui::MainWindow *ui;
    FileWatcher *m_fileWatcher;
    QString m_path;

    QList<MainWidget *> m_mainWidgetsTimeCorrectedByAlpha;

    QList<MainWidget *> m_mainWidgetList;
    QList<QAbstractSeries *> m_series;
    QMap<QString, QList<QPointF>> m_data;
    QLabel *m_label;
    QTimer *m_fileWatcherTimer;
    QPushButton *m_pushButton;
    QPushButton *m_pushButton1;
    Controller *m_controller;
    QComboBox *m_comboBox;

    QTabWidget *m_tabWidget;

    QWidget *m_page_1;

    QHBoxLayout *m_mainLayout;

    QWidget *m_widgetLeft;
    QWidget *m_widgetRight;

    QGridLayout *m_gLleft;
//    QGridLayout *m_gLright;

    FlowLayout *m_flowLayout;


};
#endif // MAINWINDOW_H
