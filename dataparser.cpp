#include "dataparser.h"
#include <QJsonObject>
#include <QJsonArray>

namespace DataParser {

QList<Station> parseStations(const QJsonDocument &json) {
    QList<Station> stations;
    if (!json.isArray()) return stations;

    QJsonArray array = json.array();
    for (const QJsonValue &value : array) {
        Station station;
        QJsonObject obj = value.toObject();

        station.id = obj["id"].toInt();
        station.stationName = obj["stationName"].toString();
        station.gegrLat = obj["gegrLat"].toString().toDouble();
        station.gegrLon = obj["gegrLon"].toString().toDouble();

        QJsonObject cityObj = obj["city"].toObject();
        station.city.id = cityObj["id"].toInt();
        station.city.name = cityObj["name"].toString();

        QJsonObject communeObj = cityObj["commune"].toObject();
        station.city.commune = communeObj["communeName"].toString();
        station.city.district = communeObj["districtName"].toString();
        station.city.province = communeObj["provinceName"].toString();
        station.city.address = cityObj["addressStreet"].toString();

        stations.append(station);
    }
    return stations;
}

QList<MeasurementStation> parseSensors(const QJsonDocument &json) {
    QList<MeasurementStation> sensors;
    if (!json.isArray()) return sensors;

    QJsonArray array = json.array();
    for (const QJsonValue &value : array) {
        MeasurementStation sensor;
        QJsonObject obj = value.toObject();

        sensor.id = obj["id"].toInt();
        sensor.stationId = obj["stationId"].toInt();

        QJsonObject paramObj = obj["param"].toObject();
        sensor.parameterName = paramObj["paramName"].toString();
        sensor.parameterCode = paramObj["paramCode"].toString();

        sensors.append(sensor);
    }
    return sensors;
}

MeasurementData parseSensorData(const QJsonDocument &json) {
    MeasurementData data;
    QJsonObject obj = json.object();

    data.parameterName = obj["key"].toString();

    QJsonArray valuesArray = obj["values"].toArray();
    for (const QJsonValue &value : valuesArray) {
        QJsonObject valueObj = value.toObject();
        QDateTime date = QDateTime::fromString(valueObj["date"].toString(), Qt::ISODate);
        double val = valueObj["value"].isNull() ? 0.0 : valueObj["value"].toDouble();
        data.values.append(qMakePair(date, val));
    }

    return data;
}

AirQualityIndex parseAirQualityIndex(const QJsonDocument &json) {
    AirQualityIndex index;
    QJsonObject obj = json.object();

    index.stationId = obj["id"].toInt();
    index.calculationDate = QDateTime::fromString(obj["stCalcDate"].toString(), Qt::ISODate);

    QJsonObject indexObj = obj["stIndexLevel"].toObject();
    index.indexLevel = indexObj["id"].toInt();
    index.indexLevelName = indexObj["indexLevelName"].toString();

    return index;
}

} // namespace DataParser
