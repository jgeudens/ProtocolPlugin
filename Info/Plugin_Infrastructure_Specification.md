# Plugin Infrastructure Specification

**Project:** Qt C++ Data Polling Application
 **Plugin Types:** Native C++ and Embedded Python
 **Embedding Technology:** pybind11
 **Framework:** Qt

------

# 1. Purpose

This document specifies the design and implementation of a **language-agnostic plugin infrastructure** for a Qt-based C++ data polling application.

The infrastructure must:

- Support both C++ and Python protocol plugins
- Provide a unified configuration system
- Offer a shared runtime interface
- Be extensible and versioned
- Isolate language implementation details from the application core
- Enable future scalability and maintainability

------

# 2. Design Goals

## 2.1 Functional Goals

- Load protocol plugins dynamically at runtime
- Allow both C++ and Python plugin implementations
- Provide standardized lifecycle:
  - `connect()`
  - `poll()`
  - `disconnect()`
- Support declarative configuration schemas
- Enable dynamic UI generation from schema
- Validate user configuration before runtime use

## 2.2 Non-Functional Goals

- Language neutrality
- Strong separation of concerns
- Robust error handling
- Version compatibility management
- Extensibility without recompiling core
- Maintainable and testable architecture

------

# 3. Architectural Overview

## 3.1 High-Level Structure

```
Application Core
    ↓
PluginManager
    ↓
AbstractProtocolPlugin
    ↓
---------------------------------
|                               |
CppProtocolPlugin        PythonProtocolPlugin
```

At runtime:

```
AbstractProtocolInstance
    ↑
---------------------------------
|                               |
CppProtocolInstance      PythonProtocolInstance
```

The polling subsystem interacts only with `AbstractProtocolInstance`.

------

# 4. Core Interfaces

## 4.1 Plugin Metadata

```
struct PluginMetadata
{
    QString id;
    QString name;
    QString version;
    QString apiVersion;
};
```

Requirements:

- `id` must be globally unique.
- `apiVersion` must match supported application version.

------

## 4.2 Configuration Schema

### Field Definition

```
enum class FieldType {
    String,
    Integer,
    Double,
    Boolean,
    Enum
};

struct ConfigField
{
    QString name;
    FieldType type;
    QVariant defaultValue;
    bool required;
    QVariantMap constraints;
};
```

### Schema Container

```
using ConfigSchema = QVector<ConfigField>;
```

Constraints may include:

- min/max
- allowed enum values
- regex pattern
- step size

------

## 4.3 Plugin Factory Interface

```
class AbstractProtocolPlugin
{
public:
    virtual ~AbstractProtocolPlugin() = default;

    virtual PluginMetadata metadata() const = 0;
    virtual ConfigSchema configSchema() const = 0;
    virtual void validate(const QVariantMap& config) const = 0;

    virtual std::unique_ptr<AbstractProtocolInstance>
        create(const QVariantMap& config) = 0;
};
```

------

## 4.4 Runtime Instance Interface

```
class AbstractProtocolInstance
{
public:
    virtual ~AbstractProtocolInstance() = default;

    virtual void connect() = 0;
    virtual QVariantMap poll() = 0;
    virtual void disconnect() = 0;
};
```

The polling engine depends only on this interface.

------

# 5. C++ Plugin Implementation

## 5.1 Mechanism

Uses Qt plugin system via QPluginLoader.

Each C++ plugin:

- Implements `AbstractProtocolPlugin`
- Is compiled as shared library (.dll/.so/.dylib)
- Registers via Qt plugin metadata

## 5.2 Requirements

- Must provide metadata
- Must provide configuration schema
- Must validate configuration
- Must produce runtime instance

------

# 6. Python Plugin Implementation

## 6.1 Runtime

Python is embedded using pybind11.

The interpreter:

- Is initialized once at application startup
- Lives for entire application lifetime
- Uses scoped GIL management

------

## 6.2 Python Plugin Contract

Each Python plugin must expose:

```
API_VERSION = "1.0"

METADATA = {
    "id": "...",
    "name": "...",
    "version": "..."
}

CONFIG_SCHEMA = {
    ...
}

def validate_config(config: dict):
    pass

class Protocol:
    def __init__(self, config: dict):
        ...
    def connect(self):
        ...
    def poll(self) -> dict:
        ...
    def disconnect(self):
        ...
```

------

## 6.3 Adapter Layer

A `PythonProtocolPlugin` C++ class:

- Loads Python module
- Converts Python schema to C++ `ConfigSchema`
- Wraps Python `Protocol` instance
- Converts exceptions to C++ errors
- Manages GIL during calls

------

# 7. Plugin Manager

## 7.1 Responsibilities

- Discover C++ plugins
- Discover Python plugins
- Validate API versions
- Instantiate plugins
- Maintain registry by plugin ID

## 7.2 Loading Strategy

### C++ Plugins

Loaded via QPluginLoader from `/plugins/cpp`.

### Python Plugins

Loaded from `/plugins/python`.
 Modules must:

- Contain required attributes
- Match API_VERSION

------

# 8. Configuration Management

## 8.1 Ownership

Configuration is owned by the C++ core application.

Plugins must not persist configuration.

------

## 8.2 Storage Format

Configuration is stored in application-defined format (e.g., JSON).

Example:

```
{
  "pluginId": "modbus.tcp",
  "configVersion": 1,
  "settings": {
    "host": "192.168.1.10",
    "port": 502
  }
}
```

------

## 8.3 Validation Flow

1. UI collects user input
2. C++ validates using schema
3. Plugin-specific validation executed
4. Instance created only if validation succeeds

------

# 9. Threading Model

- Polling may run in worker threads.
- Python calls must:
  - Acquire GIL before execution
  - Release GIL immediately after
- No long-running operations while holding GIL.

------

# 10. Error Handling

Requirements:

- Python exceptions must never propagate uncaught into C++.
- All errors converted to structured C++ error types.
- Plugin failures must not crash application.

------

# 11. Versioning Strategy

## 11.1 API Version

- All plugins declare `apiVersion`.
- Core application declares supported versions.
- Incompatible plugins are rejected at load time.

## 11.2 Configuration Version (Optional)

Plugins may define internal configuration versioning.
 Core may implement migration support.

------

# 12. Directory Structure

```

```

------

# 13. Extensibility Roadmap

Future enhancements may include:

- Hot reload (Python plugins)
- Capability flags
- Plugin dependency declaration
- Sandboxed external process plugins
- Digital signature verification
- Plugin marketplace support

------

# 14. Acceptance Criteria

The implementation is considered complete when:

- Both C++ and Python plugins load successfully
- Configuration UI is generated from schema
- Polling engine operates without knowing plugin language
- Invalid plugins are rejected safely
- Python exceptions are contained
- Multi-threaded polling works correctly

------

# 15. Architectural Principles

- Language-neutral design
- Single source of configuration truth
- Clean interface boundaries
- No UI logic inside plugins
- No persistence logic inside plugins
- Core controls lifecycle and state

------

# Conclusion

This specification defines a unified, extensible plugin architecture supporting both C++ and Python protocol implementations within a Qt application.

The design ensures:

- Long-term maintainability
- Clean separation of responsibilities
- Controlled extensibility
- Scalable platform evolution
- Industrial-grade robustness