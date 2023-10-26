#ifndef DEBUGUTILS_H
#define DEBUGUTILS_H

#include <QObject>
#include <QQmlApplicationEngine>


class DebugUtils : public QObject
{
    Q_OBJECT
public:
    explicit DebugUtils(QQmlApplicationEngine *engine, QObject *parent = nullptr);

private slots:
    void getLoaderItems(QObject *object, const QUrl &url);
    void getDrawer();
    void openDebugPage();

private:
    QList<QObject*> getChildByMetaName(QObject* parent, QString name, Qt::FindChildOptions recursive = Qt::FindChildrenRecursively);
    bool copyProperty(QObject* obj, QObject* obj2, QString property);

    QQmlApplicationEngine* m_engine;
    QObject* m_stackView;
    QObject* m_drawer;
};

#endif // DEBUGUTILS_H
