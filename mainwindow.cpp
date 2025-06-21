#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "datamanager.h"
#include <QMessageBox>
#include <QtCharts>
#include <QDateTimeAxis>
#include <QValueAxis>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , apiClient(new ApiClient(this))
    , dataManager(new DataManager(this))
{
    ui->setupUi(this);

    connect(apiClient, &ApiClient::errorOccurred,
            this, &MainWindow::onApiError);

    // Połączenia przycisków z metodami
    connect(ui->fetchStationsButton, &QPushButton::clicked,
            this, &MainWindow::on_fetchStationsButton_clicked);
    connect(ui->stationComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onStationSelected);
    connect(ui->sensorComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onSensorSelected);
    connect(ui->fetchDataButton, &QPushButton::clicked,
            this, &MainWindow::onFetchDataClicked);
    connect(ui->saveDataButton, &QPushButton::clicked,
            this, &MainWindow::onSaveDataClicked);
    connect(ui->showChartButton, &QPushButton::clicked,
            this, &MainWindow::onShowChartClicked);
    connect(ui->analyzeButton, &QPushButton::clicked,
            this, &MainWindow::onAnalyzeDataClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_fetchStationsButton_clicked()
{
    ui->statusLabel->setText(tr("Pobieranie stacji..."));
    ui->fetchStationsButton->setEnabled(false);

    auto future = apiClient->fetchAllStations();
    future.then([this](QList<Station> stations) {
        currentStations = stations;
        ui->stationComboBox->clear();

        for (const Station &station : stations) {
            ui->stationComboBox->addItem(
                QString("%1 (%2)").arg(station.stationName, station.city.name),
                station.id
                );
        }

        ui->statusLabel->setText(tr("Pobrano %1 stacji").arg(stations.size()));
        ui->fetchStationsButton->setEnabled(true);
    });
    qDebug() << "Fetch Stations przycisk kliknięty!";
}

void MainWindow::onStationSelected(int index)
{
    if (index < 0) {
        ui->sensorComboBox->clear();
        ui->sensorComboBox->setEnabled(false);
        return;
    }

    int stationId = ui->stationComboBox->currentData().toInt();
    ui->statusLabel->setText(tr("Pobieranie czujników dla stacji ID: %1...").arg(stationId));
    ui->sensorComboBox->setEnabled(false);

    auto future = apiClient->fetchStationSensors(stationId);
    future.then([this](QList<MeasurementStation> sensors) {
              currentSensors = sensors;
              ui->sensorComboBox->clear();

              for (const MeasurementStation &sensor : sensors) {
                  ui->sensorComboBox->addItem(
                      QString("%1 (%2)").arg(sensor.parameterName).arg(sensor.parameterCode),
                      sensor.id
                      );
              }

              ui->statusLabel->setText(tr("Znaleziono %1 czujników").arg(sensors.size()));
              ui->sensorComboBox->setEnabled(!sensors.isEmpty());

              if (!sensors.isEmpty()) {
                  ui->sensorComboBox->setCurrentIndex(0); // Automatycznie wybierz pierwszy czujnik
              }
          }).onFailed([this]() {
            ui->sensorComboBox->setEnabled(false);
        });
}

void MainWindow::onSensorSelected(int index)
{
    if (index < 0 || index >= currentSensors.size()) return;
    ui->fetchDataButton->setEnabled(true);
}

void MainWindow::onFetchDataClicked()
{
    int sensorIndex = ui->sensorComboBox->currentIndex();
    if (sensorIndex < 0 || sensorIndex >= currentSensors.size()) return;

    int sensorId = currentSensors[sensorIndex].id;
    ui->statusLabel->setText(tr("Pobieranie danych dla czujnika ID: %1...").arg(sensorId));

    auto future = apiClient->fetchSensorData(sensorId);
    future.then([this](MeasurementData data) {
        currentData = data;
        ui->statusLabel->setText(
            tr("Pobrano %1 pomiarów dla %2").arg(data.values.size()).arg(data.parameterName)
            );
        ui->showChartButton->setEnabled(true);
        ui->analyzeButton->setEnabled(true);
        ui->saveDataButton->setEnabled(true);
    });
}

void MainWindow::onSaveDataClicked()
{
    if (currentData.values.isEmpty()) {
        QMessageBox::warning(this, tr("Błąd"), tr("Brak danych do zapisania"));
        return;
    }

    int stationIndex = ui->stationComboBox->currentIndex();
    int sensorIndex = ui->sensorComboBox->currentIndex();

    if (stationIndex >= 0 && sensorIndex >= 0) {
        int stationId = currentStations[stationIndex].id;
        int sensorId = currentSensors[sensorIndex].id;

        dataManager->saveMeasurementData(stationId, sensorId, currentData);
        ui->statusLabel->setText(tr("Dane zapisane do bazy"));
    }
}

void MainWindow::onShowChartClicked()
{
    if (currentData.values.isEmpty()) {
        QMessageBox::warning(this, tr("Błąd"), tr("Brak danych do wyświetlenia"));
        return;
    }

    showChart(currentData.values,
              tr("Wykres dla %1 (%2)").arg(currentData.parameterName, currentData.parameterCode));
}

void MainWindow::onAnalyzeDataClicked()
{
    if (currentData.values.isEmpty()) {
        QMessageBox::warning(this, tr("Błąd"), tr("Brak danych do analizy"));
        return;
    }

    showAnalysis(currentData);
}

void MainWindow::onApiError(const QString &message)
{
    QMessageBox::critical(this, tr("Błąd"), message);
    ui->statusLabel->setText(tr("Błąd: %1").arg(message));
}

void MainWindow::showChart(const QList<QPair<QDateTime, double>> &data, const QString &title)
{
    QChart *chart = new QChart();
    chart->setTitle(title);

    QLineSeries *series = new QLineSeries();
    for (const auto &point : data) {
        series->append(point.first.toMSecsSinceEpoch(), point.second);
    }

    chart->addSeries(series);

    QDateTimeAxis *axisX = new QDateTimeAxis();
    axisX->setFormat("dd.MM.yyyy hh:mm");
    axisX->setTitleText(tr("Data"));
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText(tr("Wartość"));
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    QDialog *chartDialog = new QDialog(this);
    chartDialog->setWindowTitle(title);
    chartDialog->resize(800, 600);

    QVBoxLayout *layout = new QVBoxLayout(chartDialog);
    layout->addWidget(chartView);
    chartDialog->setLayout(layout);

    chartDialog->exec();
}

void MainWindow::showAnalysis(const MeasurementData &data)
{
    if (data.values.isEmpty()) {
        QMessageBox::information(this, tr("Analiza"), tr("Brak danych do analizy"));
        return;
    }

    double min = std::numeric_limits<double>::max();
    double max = std::numeric_limits<double>::lowest();
    double sum = 0.0;
    QDateTime minTime, maxTime;

    for (const auto &point : data.values) {
        double value = point.second;
        if (value < min) {
            min = value;
            minTime = point.first;
        }
        if (value > max) {
            max = value;
            maxTime = point.first;
        }
        sum += value;
    }

    double avg = sum / data.values.size();

    QString analysis = tr("<h2>Analiza danych: %1</h2>").arg(data.parameterName);
    analysis += tr("<p><b>Liczba pomiarów:</b> %1</p>").arg(data.values.size());
    analysis += tr("<p><b>Wartość minimalna:</b> %1 (%2)</p>")
                    .arg(min).arg(minTime.toString("dd.MM.yyyy hh:mm"));
    analysis += tr("<p><b>Wartość maksymalna:</b> %1 (%2)</p>")
                    .arg(max).arg(maxTime.toString("dd.MM.yyyy hh:mm"));
    analysis += tr("<p><b>Średnia wartość:</b> %1</p>").arg(avg);

    QMessageBox::information(this, tr("Analiza danych"), analysis);
}
