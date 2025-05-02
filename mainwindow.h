/**
 * @file mainwindow.h
 * @brief Definicja klasy głównego okna aplikacji
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include "apiclient.h"
#include "datamanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/**
 * @class MainWindow
 * @brief Główne okno aplikacji monitorującej jakość powietrza
 *
 * Klasa zarządza interfejsem użytkownika i koordynuje działanie
 * poszczególnych komponentów aplikacji.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    /**
     * @brief Konstruktor klasy MainWindow
     * @param parent Wskaźnik na obiekt rodzica
     */
    explicit MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destruktor klasy MainWindow
     */
    ~MainWindow();

private slots:

    /// Slot obsługujący kliknięcie przycisku pobierania stacji
    void on_fetchStationsButton_clicked();

    /// Slot obsługujący wybór stacji z listy
    void onStationSelected(int index);

    /// Slot obsługujący wybór czujnika z listy
    void onSensorSelected(int index);

    /// Slot obsługujący kliknięcie przycisku pobierania danych
    void onFetchDataClicked();

    /// Slot obsługujący kliknięcie przycisku zapisu danych
    void onSaveDataClicked();

    /// Slot obsługujący kliknięcie przycisku wyświetlania wykresu
    void onShowChartClicked();

    /// Slot obsługujący kliknięcie przycisku analizy danych
    void onAnalyzeDataClicked();

    /// Slot obsługujący błędy z ApiClient
    void onApiError(const QString &message);

private:
    Ui::MainWindow *ui; // Wskaźnik na interfejs użytkownika
    ApiClient *apiClient; // Wskaźnik na klienta API
    DataManager *dataManager; //Wskaźnik na menadżera danych
    QList<Station> currentStations; //Lista aktualnie załadowanych stacji
    QList<MeasurementStation> currentSensors; //Lista aktualnie załadowanych czujników
    MeasurementData currentData; // Aktualnie załadowane dane pomiarowe

    /**
     * @brief Wyświetla wykres danych
     * @param data Dane do wyświetlenia
     * @param title Tytuł wykresu
     */
    void showChart(const QList<QPair<QDateTime, double>> &data, const QString &title);

    /**
     * @brief Wyświetla analizę danych
     * @param data Dane do analizy
     */
    void showAnalysis(const MeasurementData &data);
};

#endif // MAINWINDOW_H
