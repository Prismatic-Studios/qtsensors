/****************************************************************************
**
** Copyright (C) 2013 BogDan Vatra <bogdan@kde.org>
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtSensors module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <qplugin.h>
#include <qsensorplugin.h>
#include <qsensorbackend.h>
#include <qsensormanager.h>
#include <qaccelerometer.h>
#include "androidaccelerometer.h"
#include "androidgyroscope.h"
#include "androidlight.h"
#include "androidmagnetometer.h"
#include "androidpressure.h"
#include "androidproximity.h"
#include "androidrotation.h"
#include "androidtemperature.h"

using namespace AndroidSensors;

class AndroidSensorPlugin : public QObject, public QSensorPluginInterface, public QSensorBackendFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QSensorPluginInterface/1.0" FILE "plugin.json")
    Q_INTERFACES(QSensorPluginInterface)
public:
    void registerSensors()
    {
        foreach (AndroidSensorType sensor, availableSensors()) {
            switch (sensor) {
            case TYPE_ACCELEROMETER:
                QSensorManager::registerBackend(QAccelerometer::type, QByteArray::number(sensor), this);
                break;
            case TYPE_AMBIENT_TEMPERATURE:
            case TYPE_TEMPERATURE:
                QSensorManager::registerBackend(QAmbientTemperatureSensor::type, QByteArray::number(sensor), this);
                break;
            case TYPE_GRAVITY:
                break; // add the gravity sensor backend
            case TYPE_GYROSCOPE:
                QSensorManager::registerBackend(QGyroscope::type, QByteArray::number(sensor), this);
                break;
            case TYPE_LIGHT:
                QSensorManager::registerBackend(QLightSensor::type, QByteArray::number(sensor), this);
                break; // add the light sensor backend
            case TYPE_LINEAR_ACCELERATION:
                break; // add the linear acceleration sensor backend
            case TYPE_MAGNETIC_FIELD:
                QSensorManager::registerBackend(QMagnetometer::type, QByteArray::number(sensor), this);
                break;
            case TYPE_ORIENTATION:
                break; // add the orientation sensor backend
            case TYPE_PRESSURE:
                QSensorManager::registerBackend(QPressureSensor::type, QByteArray::number(sensor), this);
                break;
            case TYPE_PROXIMITY:
                QSensorManager::registerBackend(QProximitySensor::type, QByteArray::number(sensor), this);
                break;
            case TYPE_RELATIVE_HUMIDITY:
                break; // add the relative humidity sensor backend
            case TYPE_ROTATION_VECTOR:
                QSensorManager::registerBackend(QRotationSensor::type, QByteArray::number(sensor), this);
                break;

            case TYPE_GAME_ROTATION_VECTOR:
            case TYPE_GYROSCOPE_UNCALIBRATED:
            case TYPE_MAGNETIC_FIELD_UNCALIBRATED:
            case TYPE_SIGNIFICANT_MOTION:
                break; // add backends for API level 18 sensors
            }
        }
    }

    QSensorBackend *createBackend(QSensor *sensor)
    {
        AndroidSensorType type = static_cast<AndroidSensorType>(sensor->identifier().toInt());
        switch (type) {
        case TYPE_ACCELEROMETER:
        {
            QAccelerometer * const accelerometer = qobject_cast<QAccelerometer *>(sensor);
            AndroidSensors::AndroidSensorType type = AndroidAccelerometer::modeToSensor(accelerometer->accelerationMode());
            return new AndroidAccelerometer(type, sensor);
        }
        case TYPE_AMBIENT_TEMPERATURE:
        case TYPE_TEMPERATURE:
            return new AndroidTemperature(type, sensor);
        case TYPE_GRAVITY:
            break; // add the gravity sensor backend
        case TYPE_GYROSCOPE:
            return new AndroidGyroscope(type, sensor);
        case TYPE_LIGHT:
            return new AndroidLight(type, sensor);
        case TYPE_LINEAR_ACCELERATION:
            break; // add the linear acceleration sensor backend
        case TYPE_MAGNETIC_FIELD:
            return new AndroidMagnetometer(type, sensor);
        case TYPE_ORIENTATION:
            break; // add the orientation sensor backend
        case TYPE_PRESSURE:
            return new AndroidPressure(type, sensor);
        case TYPE_PROXIMITY:
            return new AndroidProximity(type, sensor);
        case TYPE_RELATIVE_HUMIDITY:
            break; // add the relative humidity sensor backend
        case TYPE_ROTATION_VECTOR:
            return new AndroidRotation(type, sensor);

        case TYPE_GAME_ROTATION_VECTOR:
        case TYPE_GYROSCOPE_UNCALIBRATED:
        case TYPE_MAGNETIC_FIELD_UNCALIBRATED:
        case TYPE_SIGNIFICANT_MOTION:
            break; // add backends for API level 18 sensors
        }
        return 0;
    }
};

Q_IMPORT_PLUGIN (AndroidSensorPlugin) // automatically register the plugin

#include "main.moc"

