#include <QDebug>
#include <QQuickItem>
#include <QQmlContext>
#include <QQmlProperty>
#include <QFunctionPointer>
#include <QQmlExpression>
#include "debugutils.h"




DebugUtils::DebugUtils(QQmlApplicationEngine *engine, QObject *parent)
    : m_engine(engine), QObject{parent}
{
    qDebug() << "Debugging tools ON";
    connect(engine,&QQmlApplicationEngine::objectCreated,this,&DebugUtils::getLoaderItems,Qt::QueuedConnection);
}

QList<QObject *> DebugUtils::getChildByMetaName(QObject *parent, QString name, Qt::FindChildOptions recursive)
{
    QList<QObject*> children;
    for(QObject *obj : parent->children()){
        QString _name = obj->metaObject()->className();
        if(_name.contains(name)){
            children.push_back(obj);
        }
        if(recursive == Qt::FindChildrenRecursively){
            children.append(getChildByMetaName(obj,name,recursive));
        }
    }
    return children;
}

bool DebugUtils::copyProperty(QObject *obj, QObject *obj2, QString property)
{
    QVariant value = QQmlProperty::read(obj2, property);
    return QQmlProperty::write(obj,property,value);
}

void DebugUtils::getLoaderItems(QObject *object, const QUrl &url)
{

    QList<QObject*> loaders = getChildByMetaName(object, "Loader",Qt::FindDirectChildrenOnly);
    for(QObject *obj:loaders){
        connect(obj,SIGNAL(loaded()), this, SLOT(getDrawer()));
    }
    QList<QObject*> stacks = getChildByMetaName(object, "StackView",Qt::FindDirectChildrenOnly);
    if(!stacks.empty()){
        m_stackView = stacks[0];
    }
}

void DebugUtils::getDrawer()
{
    QList<QObject*> drawer = getChildByMetaName(sender(), "Drawer",Qt::FindDirectChildrenOnly);
    if(drawer.empty()){
        return;
    }
    m_drawer = drawer[0];
    QList<QObject*> children = getChildByMetaName(drawer[0],"Button");

    QQmlComponent debugButton(m_engine,"QtQuick.Controls","Button",this);
    QObject* button = debugButton.create();

    for (QObject *obj : children) {
        QString _name = obj->metaObject()->className();
        if(_name.compare("DrawerButton") == 0){
            copyProperty(button, obj, "display");
            copyProperty(button, obj, "icon.width");
            copyProperty(button, obj, "icon.height");
            copyProperty(button, obj, "font.pixelSize");
            copyProperty(button, obj, "padding");
            QQmlProperty::write(button,"text","Debug");
            QQmlProperty::write(button,"icon.source","qrc:/icons/settings");
            QQmlProperty::write(button,"implicitHeight",100);
            QQmlProperty::write(button,"implicitWidth",100);
            copyProperty(button, obj, "parent");
            break;
        }
    }
    connect(button,SIGNAL(clicked()), this, SLOT(openDebugPage()));

//

}

void DebugUtils::openDebugPage()
{
    if(m_stackView){
        QQmlExpression exp(m_engine->rootContext(), m_stackView, "push(\"qrc:/Pages/Debug/DebugPage.qml\");");
        exp.evaluate();
        QMetaObject::invokeMethod(m_drawer,"close");
    }
}

