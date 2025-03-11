#include <QtConcurrent/QtConcurrent>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "calibration.h"
#include "datadelegate.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    createActions();
    fileMenu = ui->menubar->addMenu(tr("&File"));
    fileMenu->addAction(openAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);


    QWidget *widget = new QWidget;
    setCentralWidget(widget);

    QWidget *topFiller = new QWidget;
    topFiller->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    QWidget *bottomFiller = new QWidget;
    bottomFiller->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);


    QString message = tr("A context menu is available by right-clicking");
    statusBar()->showMessage(message);


    m_path = "/home/egor/build-adcmemulate-Desktop-Debug/adcm.dat";
//    m_path = "/misc/agpk_std/adcm.dat";
    m_fileWatcher = new FileWatcher(m_path);
    m_fileWatcher->moveToThread(&workerThread);

    m_fileWatcherTimer = new QTimer(this);
    m_fileWatcherTimer->setInterval(1000);

    m_controller = new Controller(m_path, m_pre);
    m_pushButton = new QPushButton("Button", this);
    m_comboBox = new QComboBox(this);
    m_label = new QLabel("Gamma:", this);

    m_groupBox = new QGroupBox(this);
    QHBoxLayout *rBLayout = new QHBoxLayout;
    m_rBtime = new QRadioButton("Time");
    m_rBtime->setChecked(true);
    m_rBamp = new QRadioButton("Amp");
    rBLayout->addWidget(m_rBtime);
    rBLayout->addWidget(m_rBamp);

    m_groupBox->setLayout(rBLayout);

    for (uint i{0}; i < m_pre.numberOfChannelsGamma(); ++i)
    {
        m_comboBox->addItem(QString("%1").arg(i));
    }

    QGridLayout *gLleft = new QGridLayout;
    QGridLayout *gLright = new QGridLayout;

    gLleft->addWidget(m_pushButton, 0, 0, 1, 2);
    gLleft->addWidget(m_label, 1, 0);
    gLleft->addWidget(m_comboBox, 1, 1);
    gLleft->addWidget(m_groupBox, 2, 0, 1, 2);
    gLleft->setAlignment(Qt::AlignTop);
    gLright->setContentsMargins(0, 0, 0, 0);
    gLright->setSpacing(0);
    m_mainWidgetList.resize(static_cast<qsizetype>(m_pre.numberOfChannelsAlpha()));
    for (qsizetype i{0}, j{0}; i < m_mainWidgetList.size(); ++i, ++j)
    {
        m_mainWidgetList[i] = new MainWidget(this);
        gLright->addWidget(m_mainWidgetList[i], static_cast<int>((j - (i % 3)) / 3), static_cast<int>(i % 3));
    }

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addLayout(gLleft);
    mainLayout->addLayout(gLright);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(5, 5, 5, 5);
    layout->addWidget(topFiller);
    layout->addLayout(mainLayout, 1);
    layout->addWidget(bottomFiller);
    widget->setLayout(layout);

    connect(m_comboBox, &QComboBox::currentTextChanged, [this](const QString &str){
        m_selected = str;
        m_controller->operate1(m_selected, m_type);
    });
    connect(m_controller, &Controller::handleResults, this, &MainWindow::newData);
    connect(&workerThread, &QThread::finished, m_fileWatcher, &QObject::deleteLater);
    connect(m_fileWatcherTimer, &QTimer::timeout, m_fileWatcher, &FileWatcher::operate);

    connect(m_fileWatcher, &FileWatcher::onFileChanged, [this](const QString &path){
        qInfo() << path;
        m_controller->operate(m_selected, m_type);
    });

    connect(m_rBtime, &QRadioButton::clicked, this, &MainWindow::onRadioClicked);
    connect(m_rBamp, &QRadioButton::clicked, this, &MainWindow::onRadioClicked);


    workerThread.start();
    m_fileWatcherTimer->start();

}

MainWindow::~MainWindow()
{
    if (m_fileWatcherTimer != nullptr) {
        m_fileWatcherTimer->stop();
    }
    workerThread.quit();
    workerThread.wait();
    delete ui;
}

void MainWindow::onFileChanged(const QString &pathAndTime)
{

}

void MainWindow::newData(const QMap<QString, QList<QPointF>> &data)
{

    m_data.clear();
    for (auto i = data.cbegin(), end = data.cend(); i != end; ++i)
    {
        m_data.insert(i.key(), i.value());
    }
    m_series.clear();
    auto dataColor{QColor::fromRgb(0, 255, 255)};
    for (auto i = m_data.cbegin(), end = m_data.cend(); i != end; ++i)
    {
        QLineSeries *series = new QLineSeries();
        series->append(i.value());
        series->setName(i.key());
        QAreaSeries *areaSeries = new QAreaSeries(series);
        QPen pen;
        pen.setWidth(1);
        pen.setColor(dataColor);
        areaSeries->setPen(pen);
        areaSeries->setColor(dataColor);
        QLinearGradient dataGradient(QPointF(0, 0), QPointF(0, 1));
        dataGradient.setColorAt(0.0, dataColor);
        dataGradient.setColorAt(1.0, dataColor.lighter());
        dataGradient.setCoordinateMode(QGradient::ObjectBoundingMode);
        areaSeries->setOpacity(0.75);
        areaSeries->setBrush(dataGradient);
        areaSeries->setColor(dataColor);
        areaSeries->setName(i.key());
        m_series.append(areaSeries);
    }
    for (auto i{0}; i < m_series.size(); ++i)
    {
        m_mainWidgetList.at(i)->setTitle(m_series.at(i)->name());
        m_mainWidgetList.at(i)->process({m_series.at(i)});
    }
}

void MainWindow::newDataShow()
{
    m_series.clear();
    auto dataColor{QColor::fromRgb(0, 255, 255)};
    for (auto i = m_data.cbegin(), end = m_data.cend(); i != end; ++i)
    {
        QLineSeries *series = new QLineSeries();
        series->append(i.value());
        series->setName(i.key());
        QAreaSeries *areaSeries = new QAreaSeries(series);
        QPen pen;
        pen.setWidth(1);
        pen.setColor(dataColor);
        areaSeries->setPen(pen);
        areaSeries->setColor(dataColor);
        QLinearGradient dataGradient(QPointF(0, 0), QPointF(0, 1));
        dataGradient.setColorAt(0.0, dataColor);
        dataGradient.setColorAt(1.0, dataColor.lighter());
        dataGradient.setCoordinateMode(QGradient::ObjectBoundingMode);
        areaSeries->setOpacity(0.75);
        areaSeries->setBrush(dataGradient);
        areaSeries->setColor(dataColor);
        areaSeries->setName(i.key());
        m_series.append(areaSeries);
    }
//    for (auto i = m_data.cbegin(), end = m_data.cend(); i != end; ++i)
//    {
//        QSplineSeries *series = new QSplineSeries();
//        series->append(i.value());
//        series->setMarkerSize(2.5);
//        series->setColor(Qt::black);
//        series->setName(i.key());
//        m_series.append(series);
//    }
    for (auto i{0}; i < m_series.size(); ++i)
    {
        m_mainWidgetList.at(i)->setTitle(m_series.at(i)->name());
        m_mainWidgetList.at(i)->process({m_series.at(i)});
    }
}

void MainWindow::onClicked()
{

}

void MainWindow::onRadioClicked()
{
    m_type = m_rBtime->isChecked() ? Enums::Type::TIME : Enums::Type::AMP;
    m_controller->operate1(m_selected, m_type);
}

void MainWindow::open()
{

}

void MainWindow::createActions()
{
    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
}

void MainWindow::createMenus()
{
     fileMenu = ui->menubar->addMenu(tr("&File"));
     fileMenu->addAction(openAct);
     fileMenu->addSeparator();
     fileMenu->addAction(exitAct);
}


