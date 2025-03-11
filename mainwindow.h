#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>

#include "filewatcher.h"
#include "mainwidget.h"
#include "controller.h"

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
    void onFileChanged(const QString &);
    void newData(const QMap<QString, QList<QPointF>> &);
    void newDataShow();
    void onClicked();
    void onRadioClicked();

    void open();


private:
    Ui::MainWindow *ui;
    FileWatcher *m_fileWatcher;
    QString m_path;
    QList<MainWidget *> m_mainWidgetList;
    QList<QAbstractSeries *> m_series;
    QMap<QString, QList<QPointF>> m_data;
    QString m_selected{"0"};
    Enums::Type m_type{Enums::Type::TIME};
    QLabel *m_label;
    QTimer *m_fileWatcherTimer;

    ChannelMap m_pre{ChannelMap::mapSTD()};
    QPushButton *m_pushButton;
    Controller *m_controller;
    QComboBox *m_comboBox;
    QGroupBox *m_groupBox;
    QRadioButton *m_rBtime;
    QRadioButton *m_rBamp;

    void createMenus();
    void createActions();
    QMenu *fileMenu;
    QAction *openAct;
    QAction *exitAct;

};
#endif // MAINWINDOW_H
