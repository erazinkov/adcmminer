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
    m_settings = new Settings("settings.ini", this);
    m_path = m_settings->path();

    QMenu *fileMenu = new QMenu(tr("&File"), this);
    QAction *openAction = fileMenu->addAction(tr("&Open..."), this, &MainWindow::openFile);
    openAction->setShortcuts(QKeySequence::Open);
    QAction *quitAction = fileMenu->addAction(tr("E&xit"));
    quitAction->setShortcuts(QKeySequence::Quit);
    menuBar()->addMenu(fileMenu);

    connect(quitAction, &QAction::triggered, this, &MainWindow::close);


    m_mainWidget = new QWidget(this);
    m_mainLayout = new QGridLayout(m_mainWidget);
    setCentralWidget(m_mainWidget);

    QSplitter *splitterWidget = new QSplitter(m_mainWidget);
    m_mainLayout->addWidget(splitterWidget);

    m_widgetLeft = new QWidget(splitterWidget);

    m_gLleft = new QGridLayout(m_widgetLeft);
    m_pushButtonStartStop = new QPushButton("Start", m_widgetLeft);
    m_pushButtonStartStop->setCheckable(true);
    m_gLleft->addWidget(m_pushButtonStartStop);
    m_pushButtonReset = new QPushButton("Reset", m_widgetLeft);
    m_gLleft->addWidget(m_pushButtonReset);
    m_widgetLeft->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    m_widgetRight = new QWidget(splitterWidget);
    m_gLright = new QGridLayout(m_widgetRight);

    m_tabWidget = new QTabWidget(m_widgetRight);
    m_page_1 = new QWidget;
    m_tabWidget->addTab(m_page_1, "TimeByAlpha");
    m_page_2 = new QWidget;
    m_tabWidget->addTab(m_page_2, "AmpByGamma");
    m_gLright->addWidget(m_tabWidget);

    splitterWidget->addWidget(m_widgetLeft);
    splitterWidget->addWidget(m_widgetRight);


//    m_dialog = nullptr;

    m_statusMessageLabel = new QLabel(QString("<span style='color: yellow;'>%1</span>").arg(QChar(0x003F)), this);
    statusBar()->addWidget(m_statusMessageLabel);
    m_fileWatcherController = new FileWatcherController(m_path);
    m_processingController = new ProcessingController();


    m_pushButtonStartStop->setStyleSheet(
        "QPushButton {"
        "   color: white;"
        "}"
        "QPushButton:checked {"
        "   background-color: red;"
        "}"
        "QPushButton:checked:hover {"
        "   background-color: coral;"
        "}"
        "QPushButton:!checked {"
        "   background-color: green;"
        "}"
        "QPushButton:!checked:hover {"
        "   background-color: lime;"
        "}"
    );

    connect(m_fileWatcherController, &FileWatcherController::handleResultsReadyFileCheck, [this](const QString &message, const QString &path, const bool &isModified){
        m_statusMessageLabel->setText(message);
        if (isModified && m_pushButtonStartStop->isChecked()) {
            m_processingController->operateS(path);
        }
    });

    connect(m_processingController, &ProcessingController::handleResultsTimeCorrectedByAlpha, this, &MainWindow::newDataTimeCorrectedByAlpha);
    connect(m_processingController, &ProcessingController::handleResultsAmpByGamma, this, &MainWindow::newDataAmpByGamma);
    connect(m_pushButtonStartStop, &QPushButton::toggled, [this](bool checked){
        qDebug() << checked;
        m_pushButtonStartStop->setText(checked ? "Stop" : "Start");
    });
    connect(m_pushButtonReset, &QPushButton::clicked, m_processingController, &ProcessingController::operateR);

    setupTimeCorrectedByAlpha();
    setupAmpByGamma();
}

MainWindow::~MainWindow()
{
    m_settings->writeSettings();
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
    auto dataColor{QColorConstants::Blue};
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

    for (auto i{0}; i < std::min(m_seriesTimeCorrectesByAlpha.size(), m_chartWidgetsTimeCorrectedByAlpha.size()); ++i)
    {
        m_chartWidgetsTimeCorrectedByAlpha.at(i)->setTitle(m_seriesTimeCorrectesByAlpha.at(i)->name());
        m_chartWidgetsTimeCorrectedByAlpha.at(i)->process({m_seriesTimeCorrectesByAlpha.at(i)});
    }

//    for (auto i{0}; i < m_mainWidgetList.size(); ++i)
//    {
//        m_mainWidgetList.at(i)->setTitle(m_series.at(i)->name());
//        m_mainWidgetList.at(i)->process({m_series.at(i)});
//    }
}

void MainWindow::newDataAmpByGamma(const QMap<QString, QList<QPointF>> &data, const QMap<QString, QStringList> &text)
{
    qDebug() << "newDataAmpByGamma - received";
    if (data.size() != m_dataAmpByGamma.size()) {
        setupAmpByGamma(data.size());
    }
    m_dataAmpByGamma.clear();
    QList<QStringList> t;
    for (auto i = data.cbegin(), end = data.cend(); i != end; ++i)
    {
        m_dataAmpByGamma.insert(i.key(), i.value());
        t.append(text[i.key()]);
    }
    m_seriesAmpByGamma.clear();

    auto dataColor{QColorConstants::Green};
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

    for (auto i{0}; i < std::min(m_seriesAmpByGamma.size(), m_chartWidgetsAmpByGamma.size()); ++i)
    {
//        m_chartWidgetsAmpByGamma.at(i)->setTitle(m_seriesAmpByGamma.at(i)->name());
//        m_chartWidgetsAmpByGamma.at(i)->setTitle("");

        m_chartWidgetsAmpByGamma.at(i)->process({m_seriesAmpByGamma.at(i)});
        m_chartWidgetsAmpByGamma.at(i)->setHeader(t.at(i));
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
    m_chartWidgetsTimeCorrectedByAlpha.resize(AppConstants::MAX_ALPHA_NUMBER);
    auto cd{static_cast<qsizetype>(std::ceil(std::sqrt(AppConstants::MAX_ALPHA_NUMBER)))};
    auto index{0};
    for (auto ir{0}; ir < cd; ++ir)
    {
        for (auto ic{0}; ic < cd; ++ic)
        {
            if (index < m_chartWidgetsTimeCorrectedByAlpha.size())
            {
                m_chartWidgetsTimeCorrectedByAlpha[index] = new ChartWidget;
                static_cast<QGridLayout*>(m_page_1->layout())->addWidget(m_chartWidgetsTimeCorrectedByAlpha.at(index), ir, ic);
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

void MainWindow::setupAmpByGamma(const int gammaNumber)
{
    if (m_page_2->layout()) {
        delete m_page_2->layout();
    }
    for (auto i{0}; i < m_chartWidgetsAmpByGamma.size(); ++i) {
        m_chartWidgetsAmpByGamma.at(i)->deleteLater();
    }
    m_page_2->setLayout(new QGridLayout(m_page_2));
    static_cast<QGridLayout*>(m_page_2->layout())->setSpacing(0);
    static_cast<QGridLayout*>(m_page_2->layout())->setContentsMargins(0, 0, 0, 0);
    m_chartWidgetsAmpByGamma.resize(gammaNumber);
    auto cd{static_cast<qsizetype>(std::ceil(std::sqrt(gammaNumber)))};
    auto index{0};
    for (auto ir{0}; ir < cd; ++ir) {
        for (auto ic{0}; ic < cd; ++ic) {
            if (index < m_chartWidgetsAmpByGamma.size()) {
                m_chartWidgetsAmpByGamma[index] = new ChartWidget;
//                connect(m_chartWidgetsAmpByGamma[index], &ChartWidget::hovered, this, &MainWindow::showDialog);
                static_cast<QGridLayout*>(m_page_2->layout())->addWidget(m_chartWidgetsAmpByGamma.at(index), ir, ic);
                index++;
            }
        }
    }
    for (auto i{0}; i < cd; ++i) {
        static_cast<QGridLayout*>(m_page_2->layout())->setRowStretch(i, 1);
        static_cast<QGridLayout*>(m_page_2->layout())->setColumnStretch(i, 1);
    }
}


void MainWindow::showDialog(QString title) {
            qDebug() << "showDialog" << title;
            if (m_dialog) return;  // Dialog already exists
            qDebug() << "showDialog 1" << title;
            m_dialog = new QDialog(this);
            m_dialog->setWindowTitle("Information");
            m_dialog->setModal(false);
            m_dialog->setWindowFlags(Qt::ToolTip);
            m_dialog->setAttribute(Qt::WA_DeleteOnClose);

            QVBoxLayout *layout = new QVBoxLayout(m_dialog);
            QLabel *label = new QLabel("You hovered long enough!");
            label->setAlignment(Qt::AlignCenter);

            QPushButton *closeBtn = new QPushButton("Close");
            connect(closeBtn, &QPushButton::clicked, m_dialog, &QDialog::close);

            layout->addWidget(label);
            layout->addWidget(closeBtn);

            m_dialog->show();

            // Connect close signal to cleanup
            connect(m_dialog, &QDialog::finished, this, [this]() {
                m_dialog = nullptr;
            });
}

void MainWindow::openFile() {
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open File"),
        "",
        tr("ADCM Files (*.dat);;All Files (*.*)"));

    if (fileName.isEmpty()) {
        return;
    }
    m_path = fileName;
    m_fileWatcherController->operateP(m_path);
    m_settings->setPath(m_path);
}


