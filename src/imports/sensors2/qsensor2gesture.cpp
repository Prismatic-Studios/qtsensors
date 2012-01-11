/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtSensors module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qsensor2gesture.h"
#include <qsensorgesture.h>
#include <qsensorgesturemanager.h>

QT_BEGIN_NAMESPACE

//#define LOGGESTURQMLAPI

/*!
    \qmlclass SensorGesture QSensor2Gesture
    \inqmlmodule QtSensors 5
    \since QtSensors 5.0
    \brief The SensorGesture element provide notification when sensor gestures are triggered.

    This element is part of the \bold{QtSensors 5} module.

    The following QML code creates a "shake" and "template" SensorGesture QML element.

    \qml
    Item {
       SensorGesture {
           id: sensorGesture
           enabled: false
           gestures : ["QtSensors.shake", "QtSensors.SecondCounter"]
           onDetected:{
               detectedText.text = gesture
           }
       }
       Text {
           id: detectedText
           x:5
           y:160
           text: ""
       }
    }
    \endqml
*/
QSensor2Gesture::QSensor2Gesture(QObject* parent)
    : QObject(parent)
    , _enabled(false)
    , _oldEnabled(false)
    , _init(true)
    , _sensorGesture(0)
    , _sensorGestureManager(0)
{
    _sensorGestureManager = new QSensorGestureManager(this);
    connect(_sensorGestureManager, SIGNAL(newSensorGestureAvailable()), SIGNAL(availableGesturesChanged()));
}

QSensor2Gesture::~QSensor2Gesture()
{
}

/*
  QDeclarativeParserStatus interface implementation
*/
void QSensor2Gesture::classBegin()
{
}

void QSensor2Gesture::componentComplete()
{
    /*
      this is needed in the case the customer defines the type(s) and set it enabled = true
    */
    _init = false;
    setEnabled(_enabled);
}
/*
  End of QDeclarativeParserStatus interface implementation
*/

/*!
    \qmlproperty QStringList QtSensors5::SensorGesture::availableGestures
    This property can be used to determine all available gestures on the system.
*/
QStringList QSensor2Gesture::availableGestures()
{
    return _sensorGestureManager->gestureIds();
}

/*!
    \qmlproperty QString QtSensors5::SensorGesture::gestures
    Set this property to the gestures the application is interested in detecting.
    The properties validGestures and invalidGestures will be set as appropriate immediately.
    The list of available gestures can be found in the availableGestures property.
    This property cannot be changed while the element is enabled.
    To determine all available getures on the system please use the
    \l {QtSensors5::SensorGesture::availableGestures} {availableGestures} property.
*/
QStringList QSensor2Gesture::gestures() const
{
    return _gestures;
}

void QSensor2Gesture::setGestures(const QStringList& value)
{
    if (_gestures == value)
        return;

    if (!_init && enabled()){
        qWarning() << "Cannot change gestures while running.";
        return;
    }
    _gestures.clear();
    _gestures = value;
    createGesture();
    emit gesturesChanged();
}


/*!
    \qmlproperty QStringList QtSensors5::SensorGesture::validGestures
    This property holds the requested gestures that were found on the system.
*/
QStringList QSensor2Gesture::validGestures() const
{
    if (_sensorGesture)
        return _sensorGesture->validIds();
    return QStringList();
}

/*!
    \qmlproperty QStringList QtSensors5::SensorGesture::invalidGestures
    This property holds the requested gestures that were not found on the system.
*/
QStringList QSensor2Gesture::invalidGestures() const
{
    if (_sensorGesture)
        return _sensorGesture->invalidIds();
    return QStringList();
}

/*!
    \qmlproperty bool QtSensors5::SensorGesture::enabled
    This property can be used to activate or deactivate the sensor gesture.
    \sa {QtSensors5::SensorGesture::detected} {detected}
*/
bool QSensor2Gesture::enabled() const
{
    return _enabled;
}

void QSensor2Gesture::setEnabled(bool value)
{
    _enabled = value;
    if (_init)
        return;

    if (_enabled != _oldEnabled){
        _oldEnabled = _enabled;
        if (_sensorGesture){
            if (_enabled){
#ifdef LOGGESTURQMLAPI
                qDebug() << "start detection" << _gestureIds;
#endif
                _sensorGesture->startDetection();
            }
            else {
#ifdef LOGGESTURQMLAPI
                qDebug() << "stop detection" << _gestureIds;
#endif
                _sensorGesture->stopDetection();
            }
        }
        emit enabledChanged();
    }
}

/*!
    \qmlsignal QtSensors5::SensorGesture::detected(QString gesture)
    This signal is emitted whenever a gesture is detected.
    The gesture parameter contains the gesture that was detected.
*/

/*
  private funtion implementation
*/
void QSensor2Gesture::deleteGesture()
{
    if (_sensorGesture){
        bool emitinvalidchange = !invalidGestures().isEmpty();
        bool emitvalidchange = !validGestures().isEmpty();

        if (_sensorGesture->isActive()) {
            _sensorGesture->stopDetection();
        }
        delete _sensorGesture;
        _sensorGesture = 0;

        if (emitinvalidchange){
            emit invalidGesturesChanged();
        }
        if (emitvalidchange){
            emit validGesturesChanged();
        }
    }
}

void QSensor2Gesture::createGesture()
{
    deleteGesture();
#ifdef LOGGESTURQMLAPI
    qDebug() << "Create Gestrues:";
    for (int i = 0; i < _gestures.count(); i++){
        qDebug() << " -" << _gestures[i];
    }
#endif
    _sensorGesture = new QSensorGesture(_gestures, this);
    if (!validGestures().isEmpty()){
        QObject::connect(_sensorGesture
                         , SIGNAL(detected(QString))
                         , this
                         , SIGNAL(detected(QString)));
        emit validGesturesChanged();
    }
    if (!invalidGestures().isEmpty())
        emit invalidGesturesChanged();
}

/*
  End of private funtion implementation
*/

QT_END_NAMESPACE