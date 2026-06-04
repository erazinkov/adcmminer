#include <QtConcurrent/QtConcurrent>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "calibration.h"
#include "datadelegate.h"

#include <Q3DSurface>

#include "constants.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QWidget *widget = new QWidget;
    m_mainLayout = new QHBoxLayout(widget);
    setCentralWidget(widget);

    m_path = "/home/egor/build-adcmemulate-Desktop-Debug/adcm.dat";
    m_fileWatcher = new FileWatcher(m_path);
    m_fileWatcher->moveToThread(&workerThread);
    m_fileWatcherTimer = new QTimer(this);
    m_fileWatcherTimer->setInterval(1000);

    m_controller = new Controller(m_path);

    m_pushButton = new QPushButton("Start", this);
    m_pushButton->setCheckable(true);

    m_pushButton1 = new QPushButton("Button1", this);



    m_widgetLeft = new QWidget(widget);
    m_widgetRight = new QWidget(widget);

    m_gLleft = new QGridLayout(m_widgetLeft);
//    m_gLright = new QGridLayout(m_widgetRight);

    m_gLleft->addWidget(m_pushButton, 0, 0);
    m_gLleft->addWidget(m_pushButton1, 0, 1);

    m_tabWidget = new QTabWidget(m_widgetRight);
    m_page_1 = new QWidget;
    m_tabWidget->addTab(m_page_1, "TimeByAlpha");
    m_page_2 = new QWidget;
    m_tabWidget->addTab(m_page_2, "AmpByGamma");
    m_mainLayout->addWidget(m_widgetLeft);
    m_mainLayout->addWidget(m_tabWidget);


    connect(m_controller, &Controller::handleResultsTimeCorrectedByAlpha, this, &MainWindow::newDataTimeCorrectedByAlpha);
    connect(m_controller, &Controller::handleResultsAmpByGamma, this, &MainWindow::newDataAmpByGamma);
    connect(&workerThread, &QThread::finished, m_fileWatcher, &QObject::deleteLater);
    connect(m_fileWatcherTimer, &QTimer::timeout, m_fileWatcher, &FileWatcher::operate);

    connect(m_fileWatcher, &FileWatcher::onFileChanged, [this](const QString &path){
        qInfo() << path;
        m_controller->operateS();
    });


    connect(m_pushButton, &QPushButton::toggled, [this](bool checked){
        if (checked) {
            m_pushButton->setText("Stop");
            m_fileWatcherTimer->start();
        } else {
            m_pushButton->setText("Start");
            m_fileWatcherTimer->stop();
        }
    });

    connect(m_pushButton, &QPushButton::clicked, [this](){

    });


    workerThread.start();

    setupTimeCorrectedByAlpha();
    setupAmpByGamma();

//    onCurrentTextChanged(m_comboBox->currentText());
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

void MainWindow::newDataTimeCorrectedByAlpha(const QMap<QString, QList<QPointF>> &data)
{
    qDebug() << "newDataTimeCorrectedByAlpha - received";
    m_dataTimeCorrectedByAlpha.clear();
    for (auto i = data.cbegin(), end = data.cend(); i != end; ++i)
    {
        m_dataTimeCorrectedByAlpha.insert(i.key(), i.value());
    }
    m_seriesTimeCorrectesByAlpha.clear();
    auto dataColor{QColor::fromRgb(0, 255, 255)};
    for (auto i = m_dataTimeCorrectedByAlpha.cbegin(), end = m_dataTimeCorrectedByAlpha.cend(); i != end; ++i)
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
        m_seriesTimeCorrectesByAlpha.append(areaSeries);
    }

    for (auto i{0}; i < std::min(m_seriesTimeCorrectesByAlpha.size(), m_mainWidgetsTimeCorrectedByAlpha.size()); ++i)
    {
        m_mainWidgetsTimeCorrectedByAlpha.at(i)->setTitle(m_seriesTimeCorrectesByAlpha.at(i)->name());
        m_mainWidgetsTimeCorrectedByAlpha.at(i)->process({m_seriesTimeCorrectesByAlpha.at(i)});
    }

//    for (auto i{0}; i < m_mainWidgetList.size(); ++i)
//    {
//        m_mainWidgetList.at(i)->setTitle(m_series.at(i)->name());
//        m_mainWidgetList.at(i)->process({m_series.at(i)});
//    }
}

void MainWindow::newDataAmpByGamma(const QMap<QString, QList<QPointF>> &data)
{
    qDebug() << "newDataAmpByGamma - received";
    m_dataAmpByGamma.clear();
    for (auto i = data.cbegin(), end = data.cend(); i != end; ++i)
    {
        m_dataAmpByGamma.insert(i.key(), i.value());
    }
    m_seriesAmpByGamma.clear();
    auto dataColor{QColor::fromRgb(0, 255, 255)};
    for (auto i = m_dataAmpByGamma.cbegin(), end = m_dataAmpByGamma.cend(); i != end; ++i)
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
        m_seriesAmpByGamma.append(areaSeries);
    }

    for (auto i{0}; i < std::min(m_seriesAmpByGamma.size(), m_mainWidgetsAmpByGamma.size()); ++i)
    {
        m_mainWidgetsAmpByGamma.at(i)->setTitle(m_seriesAmpByGamma.at(i)->name());
        m_mainWidgetsAmpByGamma.at(i)->process({m_seriesAmpByGamma.at(i)});
    }

//    for (auto i{0}; i < m_mainWidgetList.size(); ++i)
//    {
//        m_mainWidgetList.at(i)->setTitle(m_series.at(i)->name());
//        m_mainWidgetList.at(i)->process({m_series.at(i)});
//    }
}

void MainWindow::setupTimeCorrectedByAlpha()
{
    m_page_1->setLayout(new QGridLayout(m_page_1));
    static_cast<QGridLayout*>(m_page_1->layout())->setSpacing(0);
    static_cast<QGridLayout*>(m_page_1->layout())->setContentsMargins(0, 0, 0, 0);
    m_mainWidgetsTimeCorrectedByAlpha.resize(AppConstants::MAX_ALPHA_NUMBER);
    auto cd{static_cast<qsizetype>(std::ceil(std::sqrt(AppConstants::MAX_ALPHA_NUMBER)))};
    auto index{0};
    for (auto ir{0}; ir < cd; ++ir)
    {
        for (auto ic{0}; ic < cd; ++ic)
        {
            if (index < m_mainWidgetsTimeCorrectedByAlpha.size())
            {
                m_mainWidgetsTimeCorrectedByAlpha[index] = new MainWidget;
                static_cast<QGridLayout*>(m_page_1->layout())->addWidget(m_mainWidgetsTimeCorrectedByAlpha.at(index), ir, ic);
                index++;
            }
        }
    }
    for (auto i{0}; i < cd; ++i)
    {
        static_cast<QGridLayout*>(m_page_1->layout())->setRowStretch(i, 1);
        static_cast<QGridLayout*>(m_page_1->layout())->setColumnStretch(i, 1);
    }
}

void MainWindow::setupAmpByGamma()
{
    m_page_2->setLayout(new QGridLayout(m_page_2));
    static_cast<QGridLayout*>(m_page_2->layout())->setSpacing(0);
    static_cast<QGridLayout*>(m_page_2->layout())->setContentsMargins(0, 0, 0, 0);
    m_mainWidgetsAmpByGamma.resize(AppConstants::MAX_GAMMA_NUMBER);
    auto cd{static_cast<qsizetype>(std::ceil(std::sqrt(AppConstants::MAX_GAMMA_NUMBER)))};
    auto index{0};
    for (auto ir{0}; ir < cd; ++ir)
    {
        for (auto ic{0}; ic < cd; ++ic)
        {
            if (index < m_mainWidgetsAmpByGamma.size())
            {
                m_mainWidgetsAmpByGamma[index] = new MainWidget;
                static_cast<QGridLayout*>(m_page_2->layout())->addWidget(m_mainWidgetsAmpByGamma.at(index), ir, ic);
                index++;
            }
        }
    }
    for (auto i{0}; i < cd; ++i)
    {
        static_cast<QGridLayout*>(m_page_2->layout())->setRowStretch(i, 1);
        static_cast<QGridLayout*>(m_page_2->layout())->setColumnStretch(i, 1);
    }
}

