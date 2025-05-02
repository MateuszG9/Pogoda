/**
 * @file datamanager.h
 * @brief Definicja klasy DataManager do zarządzania danymi lokalnymi
 */
#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include <QString>
#include "measurement.h"

/**
 * @class DataManager
 * @brief Klasa do zarządzania lokalnym przechowywaniem danych
 *
 * Klasa zapewnia funkcjonalność zapisu i odczytu danych
 * w formacie JSON do plików na dysku.
 */
class DataManager : public QObject
{
    Q_OBJECT
public:

    /**
     * @brief Konstruktor klasy DataManager
     * @param parent Wskaźnik na obiekt rodzica
     */
    explicit DataManager(QObject *parent = nullptr);

    /**
     * @brief Zapisuje dane pomiarowe do pliku
     * @param stationId ID stacji
     * @param sensorId ID czujnika
     * @param data Dane pomiarowe do zapisania
     */
    void saveMeasurementData(int stationId, int sensorId, const MeasurementData &data);

private:
    /**
     * @brief Generuje ścieżkę do pliku danych
     * @param stationId ID stacji
     * @param sensorId ID czujnika
     * @return Ścieżka do pliku
     */
    QString getFilePath(int stationId, int sensorId) const;
};

#endif // DATAMANAGER_H
