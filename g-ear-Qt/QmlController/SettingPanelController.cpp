#include <QQuickItem>
#include <QQmlContext>
#include <QDebug>
#include <iostream>

#include "SettingPanelController.h"
#include "IApp.h"
#include "ServiceManager.h"
#include "IService.h"
#include "Preference.h"
#include "PreferenceGroup.h"
#include "IPreferences.h"
#include "IPreferencesPanel.h"

using namespace Gear;

SettingPanelController::SettingPanelController(QQmlEngine *engine,
                                               QObject *parent)
    : QmlController(engine, parent)
{
    qmlEngine()->rootContext()->setContextProperty("preferencesPanel", this);
}

void SettingPanelController::addSettingsTabs(QQuickWindow *window)
{
    auto preferenceGroupVector =
            IApp::instance()->preferencesPanel()->preferenceGroups();
    for (auto it = preferenceGroupVector.begin(); it != preferenceGroupVector.end(); ++it)
    {
        // Add tab
        PreferenceGroup group = (*it);
        QQmlComponent component(qmlEngine());
        component.loadUrl(QUrl("qrc:/qml/QML/SettingsTabItem.qml"));
        QQuickItem *tabItem = qobject_cast<QQuickItem *>(component.create());
        tabItem->setProperty("title", QString::fromStdString(group.title()));

        QQuickItem *stackItem =
                window->findChild<QQuickItem *>("settingsTabStack");
        tabItem->setParentItem(stackItem);

        // Add switch if type is OnOff
        auto preferenceVector = group.preferences();
        for (auto i = preferenceVector.begin(); i != preferenceVector.end(); ++i)
        {
            Preference preference = (*i);
            if (preference.type() == Preference::Type::OnOff)
            {
                QQuickItem *switchColumnItem =
                        tabItem->findChild<QQuickItem *>("switchColumn");

                component.loadUrl(QUrl("qrc:/qml/QML/ConnectionSwitch.qml"));
                QQuickItem *switchItem =
                        qobject_cast<QQuickItem *>(component.create());
                switchItem->setProperty("description",
                                   QString::fromStdString(preference.title()));
                switchItem->setParentItem(switchColumnItem);
                QObject::connect(switchItem, SIGNAL(checkedChanged(bool)),
                                 this, SLOT(connect(bool)));

                switch (preference.valueOnOff())
                {
                case Preference::OnOffState::Off:
                    switchItem->setProperty("isChecked", false);
                    break;
                case Preference::OnOffState::On:
                    switchItem->setProperty("isChecked", true);
                    break;
                case Preference::OnOffState::Disabled:
                    switchItem->setProperty("enabled", false);
                    break;
                 default:
                    break;
                }
            }
        }
    }
}

void SettingPanelController::connect(bool isToConnect)
{
    QObject *senderObject = sender();
    QQuickItem *senderSwitchItem = qobject_cast<QQuickItem *>(senderObject);

    auto preferenceGroupVector =
            IApp::instance()->preferencesPanel()->preferenceGroups();
    for (auto it = preferenceGroupVector.begin();
         it != preferenceGroupVector.end(); ++it)
    {
        PreferenceGroup group = (*it);
        auto preferenceVector = group.preferences();
        for (auto i = preferenceVector.begin(); i != preferenceVector.end(); ++i)
        {
            Preference preference = (*i);
            if (senderSwitchItem->property("description").toString()
                    == QString::fromStdString(preference.title()))
            {
                if (isToConnect)
                {
                    qDebug() << "Going to build connection";
                    preference.setValue(1); // Connect
                }
                else
                {
                    qDebug() << "Going to disconnect";
                    preference.setValue(0); // Disconnect
                }
                return;
            }
        }
    }
}