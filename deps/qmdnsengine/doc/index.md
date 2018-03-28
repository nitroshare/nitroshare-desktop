QMdnsEngine provides an implementation of multicast DNS as per RFC 6762.

Some of QMdnsEngine's features include:

- Supports Windows, macOS, Linux, and most other platforms supported by Qt
- Requires only QtCore and QtNetwork - no other dependencies
- Includes an exhaustive set of unit tests

## Build Requirements

QMdnsEngine requires the following in order to build the library:

- CMake 3.2+
- Qt 5.4+
- C++ compiler with C++11 support

## Build Instructions

QMdnsEngine uses CMake for building the library. The options shown below allow the build to be customized:

- Installation:
  - `BIN_INSTALL_DIR` - binary installation directory relative to the install prefix
  - `LIB_INSTALL_DIR` - library installation directory relative to the install prefix
  - `INCLUDE_INSTALL_DIR` - header installation directory relative to the install prefix
- Customization:
  - `BUILD_DOC` - build the documentation for the library with Doxygen
  - `BUILD_EXAMPLES` - build example applications that use the library
  - `BUILD_TESTS` - build the test suite

## Basic Provider Usage

To provide a service on the local network, begin by creating a [Server](@ref QMdnsEngine::Server), a [Hostname](@ref QMdnsEngine::Hostname), and a [Provider](@ref QMdnsEngine::Provider):

@code
QMdnsEngine::Server server;
QMdnsEngine::Hostname hostname(&server);
QMdnsEngine::Provider provider(&server, &hostname);
@endcode

The server sends and receives raw DNS packets. The hostname finds a unique hostname that is not in use to identify the device. Lastly, the provider manages the records for a service.

The next step is to create the service and update the provider:

@code
QMdnsEngine::Service service;
service.setType("_http._tcp.local.");
service.setName("My Service");
service.setPort(1234);
provider.update(service);
@endcode

That's it! As long as the provider remains in scope, the service will be available on the local network and other devices will be able to find it.

## Basic Browser Usage

To find services on the local network, begin by creating a [Server](@ref QMdnsEngine::Server) and a [Browser](@ref QMdnsEngine::Browser):

@code
QMdnsEngine::Server server;
QMdnsEngine::Cache cache;
QMdnsEngine::Browser browser(&server, "_http._tcp.local.", &cache);
@endcode

The cache is optional but helps save time later when resolving services. The browser is provided with a service type which is used to filter services.

To receive a notification when services are added, connect to the [Browser::serviceAdded()](@ref QMdnsEngine::Browser::serviceAdded) signal:

@code
QObject::connect(&browser, &QMdnsEngine::Browser::serviceAdded,
    [](const QMdnsEngine::Service &service) {
        qDebug() << service.name() << "discovered!";
    }
);
@endcode

To resolve the service, use a [Resolver](@ref QMdnsEngine::Resolver):

@code
QMdnsEngine::Resolver resolver(&server, service.name(), &cache);
QObject::connect(&resolver, &QMdnsEngine::Resolver::resolved,
    [](const QHostAddress &address) {
        qDebug() << "resolved to" << address;
    }
);
@endcode

Note that [Resolver::resolved()](@ref QMdnsEngine::Resolver::resolved) may be emitted once for each address provided by the service.
