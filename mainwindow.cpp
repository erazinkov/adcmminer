#include <QtConcurrent/QtConcurrent>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "calibration.h"
#include "datadelegate.h"

#include "piechartwidget.h"

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
    m_pushButtonStartStop = new QPushButton(tr("Start"), m_widgetLeft);
    m_pushButtonStartStop->setCheckable(true);
    m_gLleft->addWidget(m_pushButtonStartStop);
    m_pushButtonReset = new QPushButton(tr("Reset"), m_widgetLeft);
    m_gLleft->addWidget(m_pushButtonReset);
    QWidget *timeWidget = new QWidget(m_widgetLeft);
    QVBoxLayout *timeLayout = new QVBoxLayout(timeWidget);
    m_timeLabel = new QLabel(tr("Time, s"), m_widgetLeft);
    m_timeLabel->setAlignment(Qt::AlignCenter);
    timeLayout->addWidget(m_timeLabel);
    m_timeLineEdit = new QLineEdit(m_widgetLeft);
    m_timeLineEdit->setReadOnly(true);
    m_timeLineEdit->setAlignment(Qt::AlignCenter);
    timeLayout->addWidget(m_timeLineEdit);
    timeLayout->setAlignment(Qt::AlignBottom);
    m_gLleft->setRowStretch(2, 1);
    m_gLleft->addWidget(timeWidget);

    m_widgetLeft->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    m_widgetRight = new QWidget(splitterWidget);
    m_gLright = new QGridLayout(m_widgetRight);

    m_tabWidget = new QTabWidget(m_widgetRight);

    m_page_1 = new QWidget;
    m_tabWidget->addTab(m_page_1, tr("Time"));
    m_page_2 = new QWidget;
    m_tabWidget->addTab(m_page_2, tr("Energy"));
    m_page_3 = new QWidget;
    m_tabWidget->addTab(m_page_3, tr("Processing"));
    m_gLright->addWidget(m_tabWidget);

    splitterWidget->addWidget(m_widgetLeft);
    splitterWidget->addWidget(m_widgetRight);


//    m_dialog = nullptr;

    m_statusMessageLabel = new QLabel(QString("<span style='color: yellow;'>%1</span>").arg(QChar(0x003F)));
    statusBar()->addWidget(m_statusMessageLabel);


    m_controller = new Controller(m_path);


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

    connect(m_controller, &Controller::handleResultsReadyCheck, [this](const QString &message){
        m_statusMessageLabel->setText(message);
    });

    connect(m_controller, &Controller::handleResultsTimeCorrectedByAlpha, this, &MainWindow::newDataTimeCorrectedByAlpha);
    connect(m_controller, &Controller::handleResultsEnergyByAlpha, this, &MainWindow::newDataEnergyByAlpha);
    connect(m_controller, &Controller::handleResultsProcessing, this, &MainWindow::newDataProcessing);
    connect(m_pushButtonStartStop, &QPushButton::toggled, m_controller, &Controller::operateTimer);
    connect(m_pushButtonStartStop, &QPushButton::toggled, [this](bool checked){
        m_pushButtonStartStop->setText(checked ? tr("Stop") : tr("Start"));
    });
    connect(m_pushButtonReset, &QPushButton::clicked, m_controller, &Controller::operateReset);

    setupTimeCorrectedByAlpha();
    setupEnergyByAlpha();
    setupProcessing();
}

MainWindow::~MainWindow()
{
    m_settings->writeSettings();
    delete ui;
}

void MainWindow::newDataTimeCorrectedByAlpha(const QMap<QString, QList<QPointF>> &data, const QMap<QString, QStringList> &text)
{
    if (data.size() != m_histChartWidgetsTimeCorrectedByAlpha.size()) {
        // TODO ?
    }
    auto j{0};
    for (auto i = data.cbegin(), end = data.cend(); i != end; ++i) {
        if (j < m_histChartWidgetsTimeCorrectedByAlpha.size()) {
            m_histChartWidgetsTimeCorrectedByAlpha.at(j)->setHeader(text[i.key()]);
            m_histChartWidgetsTimeCorrectedByAlpha.at(j)->setData(i.key(), i.value());
        }
        j++;
    }
}

void MainWindow::newDataEnergyByAlpha(const QMap<QString, QList<QPointF>> &data, const QMap<QString, QStringList> &text)
{
    if (data.size() != m_histChartWidgetsEnergyByAlpha.size()) {
        // TODO ?
    }
    auto j{0};
    for (auto i = data.cbegin(), end = data.cend(); i != end; ++i) {
        if (j < m_histChartWidgetsEnergyByAlpha.size()) {
            m_histChartWidgetsEnergyByAlpha.at(j)->setHeader(text[i.key()]);
            m_histChartWidgetsEnergyByAlpha.at(j)->setData(i.key(), i.value());
        }
        j++;
    }
}

void MainWindow::newDataProcessing(const QMap<QString, double> &data, double s)
{
    m_timeLineEdit->setText(QString::number(s, 'f', 2));
    m_pieChartWidget->setData(data);
}

void MainWindow::setupTimeCorrectedByAlpha()
{
    m_page_1->setLayout(new QGridLayout());
    static_cast<QGridLayout*>(m_page_1->layout())->setSpacing(0);
    static_cast<QGridLayout*>(m_page_1->layout())->setContentsMargins(0, 0, 0, 0);
    m_histChartWidgetsTimeCorrectedByAlpha.resize(AppConstants::MAX_ALPHA_NUMBER);
    auto cd{static_cast<qsizetype>(std::ceil(std::sqrt(AppConstants::MAX_ALPHA_NUMBER)))};
    auto index{0};
    for (auto ir{0}; ir < cd; ++ir)
    {
        for (auto ic{0}; ic < cd; ++ic)
        {
            if (index < m_histChartWidgetsTimeCorrectedByAlpha.size())
            {
                m_histChartWidgetsTimeCorrectedByAlpha[index] = new HistChartWidget(-100.0, 100.0);
                static_cast<QGridLayout*>(m_page_1->layout())->addWidget(m_histChartWidgetsTimeCorrectedByAlpha.at(index), ir, ic);
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

void MainWindow::setupEnergyByAlpha()
{
    if (m_page_2->layout()) {
        delete m_page_2->layout();
    }
    for (auto i{0}; i < m_histChartWidgetsEnergyByAlpha.size(); ++i) {
        m_histChartWidgetsEnergyByAlpha.at(i)->deleteLater();
    }
    m_page_2->setLayout(new QGridLayout());
    static_cast<QGridLayout*>(m_page_2->layout())->setSpacing(0);
    static_cast<QGridLayout*>(m_page_2->layout())->setContentsMargins(0, 0, 0, 0);
    m_histChartWidgetsEnergyByAlpha.resize(AppConstants::MAX_ALPHA_NUMBER);
    auto cd{static_cast<qsizetype>(std::ceil(std::sqrt(AppConstants::MAX_ALPHA_NUMBER)))};
    auto index{0};
    for (auto ir{0}; ir < cd; ++ir) {
        for (auto ic{0}; ic < cd; ++ic) {
            if (index < m_histChartWidgetsEnergyByAlpha.size()) {
                m_histChartWidgetsEnergyByAlpha[index] = new HistChartWidget(0.0, 8'000.0);
//                connect(m_chartWidgetsAmpByGamma[index], &ChartWidget::hovered, this, &MainWindow::showDialog);
                static_cast<QGridLayout*>(m_page_2->layout())->addWidget(m_histChartWidgetsEnergyByAlpha.at(index), ir, ic);
                index++;
            }
        }
    }
    for (auto i{0}; i < cd; ++i) {
        static_cast<QGridLayout*>(m_page_2->layout())->setRowStretch(i, 1);
        static_cast<QGridLayout*>(m_page_2->layout())->setColumnStretch(i, 1);
    }
}

void MainWindow::setupProcessing()
{
    QGridLayout *gl = new QGridLayout(m_page_3);
    m_page_3->setLayout(gl);
    m_pieChartWidget = new PieChartWidget(m_page_3);
    gl->addWidget(m_pieChartWidget, 0, 0);
    m_barChartWidget = new BarChartWidget(tr("Counters"), m_page_3);
    gl->addWidget(m_barChartWidget, 0, 1);
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
    m_controller->operatePath(m_path);
    m_settings->setPath(m_path);
}
