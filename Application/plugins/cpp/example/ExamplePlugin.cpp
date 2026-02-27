#include "ExamplePlugin.h"

namespace ExamplePlugin {

ProtocolCore::PluginMetadata ExamplePluginImpl::metadata() const
{
    return ProtocolCore::PluginMetadata{ "example.simple", "Example Simple Plugin", "0.1", "1.0" };
}

ProtocolCore::ConfigSchema ExamplePluginImpl::configSchema() const
{
    ProtocolCore::ConfigField f;
    f.name = "dummy";
    f.type = ProtocolCore::FieldType::Integer;
    f.defaultValue = 0;
    f.required = false;

    return ProtocolCore::ConfigSchema{ f };
}

void ExamplePluginImpl::validate(const QVariantMap& /*config*/) const
{
    // no-op for example
}

std::unique_ptr<ProtocolCore::AbstractProtocolInstance> ExamplePluginImpl::create(const QVariantMap& /*config*/)
{
    return std::make_unique<ExampleInstance>();
}

} // namespace ExamplePlugin
