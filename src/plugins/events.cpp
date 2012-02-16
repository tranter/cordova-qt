#include "events.h"

#include "../pluginregistry.h"
#include "../cordova.h"

#include <QDebug>

// Create static instance of ourself
Events *Events::m_events = new Events();

/**
 * Constructor - NOTE: Never do anything except registering the plugin
 */
Events::Events() :
    CPlugin()
{
    PluginRegistry::getRegistry()->registerPlugin( "com.phonegap.Events", this );
}

/**
 * Initialize the pugin
 */
void Events::init()
{
    m_previousPercent = 100;
    qDebug() << Q_FUNC_INFO;
#if QT_VERSION < 0x050000
    m_batteryInfo = new QSystemBatteryInfo(this);
    connect(m_batteryInfo, SIGNAL(remainingCapacityChanged(int)), this, SLOT(remainingCapacityChanged(int)));
    connect(m_batteryInfo, SIGNAL(chargerTypeChanged(QSystemBatteryInfo::ChargerType)), this, SLOT(chargerTypeChanged(QSystemBatteryInfo::ChargerType)));
#else
    m_batteryInfo = new QBatteryInfo(this);
    connect(m_batteryInfo, SIGNAL(remainingCapacityChanged(int,int)), this, SLOT(remainingCapacityChanged(int,int)));
    connect(m_batteryInfo, SIGNAL(chargerTypeChanged(QBatteryInfo::ChargerType)), this, SLOT(chargerTypeChanged(QBatteryInfo::ChargerType)));
#endif
}

#if QT_VERSION < 0x050000
void Events::remainingCapacityChanged(int capacity)
#else
void Events::remainingCapacityChanged(int battery, int capacity)
#endif
{
    qDebug() << Q_FUNC_INFO;
    int newPercent;

#if QT_VERSION < 0x050000
    if (m_batteryInfo->nominalCapacity() < 1)
        newPercent = 0;
    newPercent = capacity/((double)m_batteryInfo->nominalCapacity()/100);
#else
    if (m_batteryInfo->maximumCapacity(battery) < 1)
        newPercent = 0;
    newPercent = capacity/((double)m_batteryInfo->maximumCapacity(battery)/100);
#endif
    if (m_previousPercent == newPercent)
        return;
    m_previousPercent = newPercent;
#if QT_VERSION < 0x050000
    bool isPlugged = m_batteryInfo->chargerType() == QSystemBatteryInfo::UnknownCharger ||
            m_batteryInfo->chargerType() == QSystemBatteryInfo::NoCharger ||
            m_batteryInfo->chargerType() == QSystemBatteryInfo::VariableCurrentCharger;
#else
    bool isPlugged = m_batteryInfo->chargerType() == QBatteryInfo::UnknownCharger ||
            m_batteryInfo->chargerType() == QBatteryInfo::VariableCurrentCharger;
#endif
    Cordova::instance()->execJS( QString("PhoneGap.batteryStatusChanged(%1, %2);")
                                  .arg(m_previousPercent)
                                  .arg(isPlugged));
}

#if QT_VERSION < 0x050000
void Events::chargerTypeChanged(QSystemBatteryInfo::ChargerType type)
#else
void Events::chargerTypeChanged(QBatteryInfo::ChargerType type)
#endif
{
    Q_UNUSED(type);
    qDebug() << Q_FUNC_INFO;
#if QT_VERSION < 0x050000
    bool isPlugged = m_batteryInfo->chargerType() == QSystemBatteryInfo::UnknownCharger ||
            m_batteryInfo->chargerType() == QSystemBatteryInfo::NoCharger ||
            m_batteryInfo->chargerType() == QSystemBatteryInfo::VariableCurrentCharger;
#else
    bool isPlugged = m_batteryInfo->chargerType() == QBatteryInfo::UnknownCharger ||
            m_batteryInfo->chargerType() == QBatteryInfo::VariableCurrentCharger;
#endif
    Cordova::instance()->execJS( QString("PhoneGap.batteryStatusChanged(%1, %2);")
                                    .arg(m_previousPercent)
                                    .arg(isPlugged));
}
