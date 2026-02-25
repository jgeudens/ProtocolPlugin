#pragma once

#include "ProtocolCore/ProtocolCore.h"
#include "ProtocolCore/QtPluginInterface.h"
#include <QObject>
#include <QVariantMap>

namespace ExamplePlugin {

class ExampleInstance : public ProtocolCore::AbstractProtocolInstance
{
public:
    void connect() override
    {
    }
    QVariantMap poll() override
    {
        return QVariantMap{ { "value", 42 } };
    }
    void disconnect() override
    {
    }
};

class ExamplePluginImpl : public QObject, public ProtocolCore::AbstractProtocolPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.yourcompany.ProtocolPlugin")
    Q_INTERFACES(ProtocolCore::AbstractProtocolPlugin)

public:
    ProtocolCore::PluginMetadata metadata() const override;
    ProtocolCore::ConfigSchema configSchema() const override;
    void validate(const QVariantMap& config) const override;
    std::unique_ptr<ProtocolCore::AbstractProtocolInstance> create(const QVariantMap& config) override;
};

} // namespace ExamplePlugin
