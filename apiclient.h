/**
 * @file apiclient.h
 * @brief Definicja klasy ApiClient do komunikacji z API GIOS
 */
#ifndef APICLIENT_H
#define APICLIENT_H

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include "station.h"
#include "measurement.h"

/**
 * @class ApiClient
 * @brief Klasa odpowiedzialna za komunikacje z API GIOS
 *
 * Klasa wykorzystuje QNetworkAccessManager do wysyłania żądań HTTP
 * i zwraca dane w postaci obiektów QFuture dla asynchronicznego przetwarzania.
 */
class ApiClient : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief Konstruktor klasy ApiClient
     * @param parent Wskaźnik na obiekt rodzica (domyślnie nullptr)
     */
    explicit ApiClient(QObject *parent = nullptr);

    /**
     * @brief Pobiera listę wszystkich stacji pomiarowych
     * @return QFuture<QList<Station>> - przyszły wynik z listą stacji
     */
    QFuture<QList<Station>> fetchAllStations();

    /**
     * @brief Pobiera stanowiska pomiarowe dla danej stacji
     * @param stationId ID stacji
     * @return QFuture<QList<MeasurementStation>> - przyszły wynik z listą czujników
     */
    QFuture<QList<MeasurementStation>> fetchStationSensors(int stationId);

    /**
     * @brief Pobiera dane pomiarowe dla danego czujnika
     * @param sensorId ID czujnika
     * @return QFuture<MeasurementData> - przyszły wynik z danymi pomiarowymi
     */
    QFuture<MeasurementData> fetchSensorData(int sensorId);

    /**
     * @brief Pobiera indeks jakości powietrza dla stacji
     * @param stationId ID stacji
     * @return QFuture<AirQualityIndex> - przyszły wynik z indeksem jakości powietrza
     */
    QFuture<AirQualityIndex> fetchAirQualityIndex(int stationId);

signals:

    /**
     * @brief Sygnał emitowany w przypadku błędu
     * @param message Komunikat błędu
     */
    void errorOccurred(const QString &message);

private:
    QNetworkAccessManager *manager; // Menadżer połączeń sieciowych
    const QString baseUrl = "https://api.gios.gov.pl/pjp-api/rest"; // Bazowy URL API

    /**
     * @brief Wykonuje żądanie do API
     * @param endpoint Endpoint API
     * @return QFuture<QJsonDocument> - przyszły wynik z odpowiedzią JSON
     */
    QFuture<QJsonDocument> makeApiRequest(const QString &endpoint);
};

#endif // APICLIENT_H
