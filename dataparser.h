/**
 * @file dataparser.h
 * @brief Definicja przestrzeni nazw DataParser do przetwarzania danych JSON
 */
#ifndef DATAPARSER_H
#define DATAPARSER_H

#include <QJsonDocument>
#include <QList>
#include "station.h"
#include "measurement.h"

/**
 * @namespace DataParser
 * @brief Przestrzeń nazw zawierająca funkcje do parsowania danych JSON
 */
namespace DataParser {

    /**
     * @brief Parsuje listę stacji z dokumentu JSON
     * @param json Dokument JSON
     * @return Lista stacji
     */
    QList<Station> parseStations(const QJsonDocument &json);

/**
     * @brief Parsuje listę czujników z dokumentu JSON
     * @param json Dokument JSON
     * @return Lista czujników
     */
    QList<MeasurementStation> parseSensors(const QJsonDocument &json);

    /**
     * @brief Parsuje dane pomiarowe z dokumentu JSON
     * @param json Dokument JSON
     * @return Dane pomiarowe
     */
    MeasurementData parseSensorData(const QJsonDocument &json);

    /**
     * @brief Parsuje indeks jakości powietrza z dokumentu JSON
     * @param json Dokument JSON
     * @return Indeks jakości powietrza
     */
    AirQualityIndex parseAirQualityIndex(const QJsonDocument &json);
}

#endif // DATAPARSER_H
