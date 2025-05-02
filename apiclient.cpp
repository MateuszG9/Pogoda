#include "apiclient.h"
#include "dataparser.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QUrl>
#include <QNetworkRequest>
#include <QtConcurrentRun>
#include <QEventLoop>
#include <QTimer>
#include <QDebug>

ApiClient::ApiClient(QObject *parent) : QObject(parent),
    manager(new QNetworkAccessManager(this)),
    baseUrl("https://api.gios.gov.pl/pjp-api/rest")
{
}

QFuture<QList<Station>> ApiClient::fetchAllStations()
{
    return QtConcurrent::run([this]() -> QList<Station> {
        try {
            QFuture<QJsonDocument> future = makeApiRequest("/station/findAll");
            future.waitForFinished();
            return DataParser::parseStations(future.result());
        } catch (const std::exception& e) {
            qWarning() << "Error fetching stations:" << e.what();
            return QList<Station>();
        }
    });
}

QFuture<QList<MeasurementStation>> ApiClient::fetchStationSensors(int stationId)
{
    return QtConcurrent::run([this, stationId]() {
        auto response = makeApiRequest(QString("/station/sensors/%1").arg(stationId)).result();
        qDebug() << "Odpowiedź API czujników:" << response;
        return DataParser::parseSensors(response);
    });
}

QFuture<MeasurementData> ApiClient::fetchSensorData(int sensorId)
{
    return QtConcurrent::run([this, sensorId]() -> MeasurementData {
        try {
            QFuture<QJsonDocument> future = makeApiRequest(QString("/data/getData/%1").arg(sensorId));
            future.waitForFinished();
            return DataParser::parseSensorData(future.result());
        } catch (const std::exception& e) {
            qWarning() << "Error fetching sensor data:" << e.what();
            return MeasurementData();
        }
    });
}

QFuture<AirQualityIndex> ApiClient::fetchAirQualityIndex(int stationId)
{
    return QtConcurrent::run([this, stationId]() -> AirQualityIndex {
        try {
            QFuture<QJsonDocument> future = makeApiRequest(QString("/aqindex/getIndex/%1").arg(stationId));
            future.waitForFinished();
            return DataParser::parseAirQualityIndex(future.result());
        } catch (const std::exception& e) {
            qWarning() << "Error fetching air quality index:" << e.what();
            return AirQualityIndex();
        }
    });
}

QFuture<QJsonDocument> ApiClient::makeApiRequest(const QString &endpoint)
{
    return QtConcurrent::run([this, endpoint]() -> QJsonDocument {
        QEventLoop loop;
        QNetworkReply* reply = nullptr;
        QJsonDocument jsonResponse;
        QTimer timer;

        timer.setSingleShot(true);
        QObject::connect(&timer, &QTimer::timeout, [&loop, &reply]() {
            if (reply && reply->isRunning()) {
                reply->abort();
                loop.quit();
            }
        });

        try {
            QUrl url(baseUrl + endpoint);
            QNetworkRequest request(url);
            request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

            reply = manager->get(request);
            timer.start(10000); // 10 sekund timeout

            QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
            QObject::connect(reply, &QNetworkReply::errorOccurred, [this](QNetworkReply::NetworkError code) {
                emit errorOccurred(tr("Network error: ") + QString::number(code));
            });

            loop.exec();

            if (reply->error() == QNetworkReply::NoError) {
                QByteArray responseData = reply->readAll();
                jsonResponse = QJsonDocument::fromJson(responseData);
                if (jsonResponse.isNull()) {
                    throw std::runtime_error("Invalid JSON response");
                }
            } else {
                throw std::runtime_error(reply->errorString().toStdString());
            }
        } catch (const std::exception& e) {
            emit errorOccurred(QString("API request failed: %1").arg(e.what()));
            jsonResponse = QJsonDocument();
        }

        if (reply) {
            reply->deleteLater();
        }
        return jsonResponse;
    });
}
