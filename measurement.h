/**
 * @file measurement.h
 * @brief Definicje struktur danych pomiarowych
 */
#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include <QDateTime>
#include <QList>
#include <QPair>

/**
 * @struct MeasurementData
 * @brief Struktura przechowująca dane pomiarowe
 */
struct MeasurementData {
    QString parameterName; //Nazwa parametru
    QString parameterCode; //Kod parametru
    QList<QPair<QDateTime, double>> values; // Lista wartości pomiarowych z datami
};

/**
 * @struct AirQualityIndex
 * @brief Struktura przechowująca indeks jakości powietrza
 */
struct AirQualityIndex {
    int stationId; // ID stacji
    QDateTime calculationDate; //Data obliczenia
    int indexLevel; //Poziom indeksu
    QString indexLevelName; // Nazwa poziomu indeksu
};

/**
 * @struct MeasurementStation
 * @brief Struktura opisująca stanowisko pomiarowe
 */
struct MeasurementStation {
    int id; //ID stanowiska
    int stationId; //ID stacji
    QString parameterName; //Nazwa mierzonego parametru
    QString parameterCode; // Kod parametru
};

#endif // MEASUREMENT_H
