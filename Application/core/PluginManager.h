#pragma once

#include "ProtocolCore/ProtocolCore.h"
#include "ProtocolCore/QtPluginInterface.h"
#include <QObject>
#include <QString>
#include <QVector>

namespace ProtocolCore {

class PluginManager : public QObject
{
    Q_OBJECT

public:
    explicit PluginManager(QObject* parent = nullptr);
    ~PluginManager() override;

    // Load plugins from directory (shared libs). Returns true if any plugins loaded.
    bool loadPluginsFromDir(const QString& dirPath);

    QVector<AbstractProtocolPlugin*> plugins() const;
    QString lastError() const;

private:
    QVector<QObject*> loaderOwners_; // keep plugin instances alive
    QVector<AbstractProtocolPlugin*> plugins_;
    QString lastError_;
};

} // namespace ProtocolCore
