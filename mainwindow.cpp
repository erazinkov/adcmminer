#include <QtConcurrent/QtConcurrent>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "calibration.h"
#include "datadelegate.h"

#include <Q3DSurface>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QWidget *widget = new QWidget;
    m_mainLayout = new QHBoxLayout(widget);
    setCentralWidget(widget);
//    m_path = "/home/egor/shares/tmp/c12_2kg_mask_1";
//    m_path = "/home/egor/shares/tmp/adcm.dat.test_0_180";
//    m_path = "/misc/agpk_std/adcm.dat";
    m_path = "/home/egor/build-adcmemulate-Desktop-Debug/adcm.dat";
    m_fileWatcher = new FileWatcher(m_path);
    m_fileWatcher->moveToThread(&workerThread);
    m_fileWatcherTimer = new QTimer(this);
    m_fileWatcherTimer->setInterval(1000);

    m_controller = new Controller(m_path);


    m_pushButton = new QPushButton("Button", this);
    m_pushButton->setCheckable(true);

    m_pushButton1 = new QPushButton("Button1", this);

    m_comboBox = new QComboBox(this);
    m_comboBox->insertItems(0, {"4", "16", "32", "64"});
    m_comboBox->setCurrentIndex(0);

    m_label = new QLabel("Channels:", this);

    m_widgetLeft = new QWidget(widget);
    m_widgetRight = new QWidget(widget);

    m_gLleft = new QGridLayout(m_widgetLeft);
//    m_gLright = new QGridLayout(m_widgetRight);

    m_gLleft->addWidget(m_pushButton, 0, 0);
    m_gLleft->addWidget(m_pushButton1, 0, 1);
    m_gLleft->addWidget(m_label, 1, 0);
    m_gLleft->addWidget(m_comboBox, 1, 1);

    m_tabWidget = new QTabWidget(m_widgetRight);
    m_page_1 = new QWidget;
    m_tabWidget->addTab(m_page_1, "Tab &One");
    m_mainLayout->addWidget(m_widgetLeft);
    m_mainLayout->addWidget(m_tabWidget);

//    m_flowLayout = new FlowLayout(m_page_1);

//    QVBoxLayout *layout = new QVBoxLayout;
//    layout->setContentsMargins(5, 5, 5, 5);
//    layout->addWidget(topFiller);
//    layout->addLayout(m_mainLayout, 1);
//    layout->addWidget(bottomFiller);
//    widget->setLayout(m_mainLayout);

    connect(m_comboBox, &QComboBox::currentTextChanged, this, &MainWindow::onCurrentTextChanged);
    connect(m_controller, &Controller::handleResults, this, &MainWindow::newData);
    connect(&workerThread, &QThread::finished, m_fileWatcher, &QObject::deleteLater);
    connect(m_fileWatcherTimer, &QTimer::timeout, m_fileWatcher, &FileWatcher::operate);

    connect(m_fileWatcher, &FileWatcher::onFileChanged, [this](const QString &path){
        qInfo() << path;
        m_controller->operateS();
    });


    connect(m_pushButton, &QPushButton::toggled, [this](bool checked){
        if (checked)
        {
            m_fileWatcherTimer->start();
        }
        else
        {
            m_fileWatcherTimer->stop();
        }
    });

    connect(m_pushButton, &QPushButton::clicked, [this](){

    });


    workerThread.start();

    onCurrentTextChanged(m_comboBox->currentText());
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

void MainWindow::newData(const QMap<QString, QList<QPointF>> &data)
{
    qDebug() << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
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

    for (auto i{0}; i < std::min(m_series.size(), m_mainWidgetList.size()); ++i)
    {
        m_mainWidgetList.at(i)->setTitle(m_series.at(i)->name());
        m_mainWidgetList.at(i)->process({m_series.at(i)});
    }

//    for (auto i{0}; i < m_mainWidgetList.size(); ++i)
//    {
//        m_mainWidgetList.at(i)->setTitle(m_series.at(i)->name());
//        m_mainWidgetList.at(i)->process({m_series.at(i)});
//    }
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
    for (auto i{0}; i < std::min(m_series.size(), m_mainWidgetList.size()); ++i)
    {
        m_mainWidgetList.at(i)->setTitle(m_series.at(i)->name());
        m_mainWidgetList.at(i)->process({m_series.at(i)});
    }
}

void MainWindow::onCurrentTextChanged(const QString &currentText)
{
//    for (auto i{0}; i < m_mainWidgetList.size(); ++i)
//    {
//        m_flowLayout->removeWidget(m_mainWidgetList.at(i));
//        m_mainWidgetList.at(i)->deleteLater();
//    }

//    m_mainWidgetList.resize(static_cast<qsizetype>(currentText.toInt()));

//    for (auto i{0}; i < m_mainWidgetList.size(); ++i)
//    {
//        m_mainWidgetList[i] = new MainWidget;
//        m_flowLayout->addWidget(m_mainWidgetList.at(i));
//    }


    if (m_page_1->layout())
    {
        delete m_page_1->layout();
    }
    for (auto i{0}; i < m_mainWidgetList.size(); ++i)
    {
        m_mainWidgetList.at(i)->deleteLater();
    }

    m_page_1->setLayout(new QGridLayout(m_page_1));
    static_cast<QGridLayout*>(m_page_1->layout())->setSpacing(0);
    static_cast<QGridLayout*>(m_page_1->layout())->setContentsMargins(0, 0, 0, 0);

    m_mainWidgetList.resize(static_cast<qsizetype>(currentText.toInt()));
    auto cd{static_cast<qsizetype>(std::ceil(std::sqrt(static_cast<qsizetype>(currentText.toInt()))))};
    auto index{0};
    for (auto ir{0}; ir < cd; ++ir)
    {
        for (auto ic{0}; ic < cd; ++ic)
        {
            if (index < m_mainWidgetList.size())
            {
                m_mainWidgetList[index] = new MainWidget;
                static_cast<QGridLayout*>(m_page_1->layout())->addWidget(m_mainWidgetList.at(index), ir, ic);
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

