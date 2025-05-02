/**
 * @file station.h
 * @brief Definicje struktur danych stacji pomiarowych
 */
#ifndef STATION_H
#define STATION_H

#include <QString>

/**
 * @struct City
 * @brief Struktura opisująca miasto
 */
struct City {
    int id; //ID miasta
    QString name; // Nazwa miasta
    QString commune; //Gmina
    QString district; // Powiat
    QString province; // Województwo
    QString address; // Adres
};

/**
 * @struct Station
 * @brief Struktura opisująca stację pomiarową
 */
struct Station {
    int id; // ID stacji
    QString stationName; //Nazwa stacji
    double gegrLat; // Szerokość geograficzna
    double gegrLon; // Długość geograficzna
    City city; // Informacje o lokalizacji
};

#endif // STATION_H
