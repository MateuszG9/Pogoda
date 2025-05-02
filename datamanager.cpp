#include "datamanager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>
#include <QStandardPaths>
#include <qjsonarray.h>

DataManager::DataManager(QObject *parent) : QObject(parent)
{
}

void DataManager::saveMeasurementData(int stationId, int sensorId, const MeasurementData &data)
{
    QString filePath = getFilePath(stationId, sensorId);
    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Could not open file for writing:" << filePath;
        return;
    }

    QJsonObject jsonData;
    jsonData["stationId"] = stationId;
    jsonData["sensorId"] = sensorId;
    jsonData["parameterName"] = data.parameterName;
    jsonData["parameterCode"] = data.parameterCode;

    QJsonArray valuesArray;
    for (const auto &value : data.values) {
        QJsonObject valueObj;
        valueObj["date"] = value.first.toString(Qt::ISODate);
        valueObj["value"] = value.second;
        valuesArray.append(valueObj);
    }
    jsonData["values"] = valuesArray;

    file.write(QJsonDocument(jsonData).toJson());
    file.close();
}

QString DataManager::getFilePath(int stationId, int sensorId) const
{
    QString dirPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/pogoda_data";
    QDir().mkpath(dirPath);
    return QString("%1/station_%2_sensor_%3.json").arg(dirPath).arg(stationId).arg(sensorId);
}
