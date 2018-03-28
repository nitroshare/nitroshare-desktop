QHttpEngine provides a simple set of classes for developing HTTP server applications in Qt.

The design goals of QHttpEngine include:

- **Reliability** - extremely exhaustive test suite ensures that bugs are caught as soon as they are introduced
- **Flexibility** - classes are designed for easy integration with existing Qt code
- **Simplicity** - everything is intentionally kept as simple as possible
- **Usability** - documentation is provided for all classes

All of QHttpEngine's functionality is included in a single monolithic library.

## Build Requirements

QHttpEngine has been tested on the following combinations of compiler and operating system:

- Visual C++ 2013 on Windows 7, 10, and Server 2012 R2
- g++ and Clang on Mac OS X
- g++ on i386, amd64, and ARM builds of Linux

QHttpEngine is designed in a portable way, so it may run on other compilers and operating systems than the ones listed above. However, the list represents the combinations that are actively tested and officially supported.

## Build Instructions

QHttpEngine uses CMake for building the library. The library recognizes three options during configuration, all of which are disabled by default (the library is built as a shared library):

- `BUILD_DOC` - (requires Doxygen) generates documentation from the comments in the source code
- `BUILD_EXAMPLES` - builds the sample applications that demonstrate how to use QHttpEngine
- `BUILD_TESTS` - build the test suite

It is also possible to override installation directories by customizing the `BIN_INSTALL_DIR`, `LIB_INSTALL_DIR`, `INCLUDE_INSTALL_DIR`, `DOC_INSTALL_DIR`, and `EXAMPLES_INSTALL_DIR` variables.

## Basic Usage

QHttpEngine includes all of the classes you will need to build your HTTP server application.

### Socket

In order to create an HTTP socket, create an instance of [Socket](@ref QHttpEngine::Socket) and pass a QTcpSocket* in the constructor:

@code
QTcpSocket *tcpSocket = ...
QHttpEngine::Socket httpSocket(tcpSocket);
@endcode

Once the [headersParsed()](@ref QHttpEngine::Socket::headersParsed) signal is emitted (and [isHeadersParsed()](@ref QHttpEngine::Socket::isHeadersParsed) returns true), information about the request can easily be retrieved:

@code
// Check if the method is GET
bool isGet = httpSocket.method() == QHttpEngine::Socket::GET;

// Retrieve the path
QString path = httpSocket.path();

// Lookup the value of the "User-Agent" header
QByteArray userAgent = httpSocket.headers().value("User-Agent");
@endcode

Because [Socket](@ref QHttpEngine::Socket) derives from QIODevice, writing a response to the client is very simple:

@code
httpSocket.setStatusCode(QHttpEngine::Socket::OK);
httpSocket.setHeader("Content-Type", "text/plain");
httpSocket.writeHeaders();
httpSocket.write("This is a sample message.");
@endcode

Writing a local file to the socket can be done with little effort by using the [QIODeviceCopier](@ref QHttpEngine::QIODeviceCopier) class:

@code
QFile file("somefile.txt");
file.open(QIODevice::ReadOnly);

QHttpEngine::QIODeviceCopier copier(&file, &httpSocket);
copier.start();

// Wait for the finished() signal from copier
@endcode

### Server

To create an HTTP server, simply create an instance of the [Server](@ref QHttpEngine::Server) class:

@code
QHttpEngine::Server server;
server.listen();
@endcode

In order to route requests based on their path, a handler must be used. Handlers derive from the [Handler](@ref QHttpEngine::Handler) class. The simplest of these is the [FilesystemHandler](@ref QHttpEngine::FilesystemHandler) class:

@code
QHttpEngine::FilesystemHandler handler("/var/www");
server.setHandler(&handler);
@endcode

A request to `/path` will cause the server to respond with the contents of `/var/www/path`.

### Slot Methods

Although it is possible to create a handler that manually routes requests, it is far easier to use the [QObjectHandler](@ref QHttpEngine::QObjectHandler) class and register slots for each path - you can even use the new connection syntax:

@code
class Api : public QObject
{
    Q_OBJECT
public slots:
    void doSomething(QHttpEngine::Socket *socket);
    void doSomethingElse(QHttpEngine::Socket *socket);
};

Api api;

QHttpEngine::QObjectHandler handler;
handler.registerMethod("something", &api, &Api::doSomething);
@endcode

A request to `/something` will cause the `doSomething()` slot to be invoked.

## Where to Go From Here

- Middleware can be used to process requests before final routing: [Middleware](@ref QHttpEngine::Middleware)
- Authentication middleware can be used to restrict access: [BasicAuthMiddleware](@ref QHttpEngine::BasicAuthMiddleware), [LocalAuthMiddleware](@ref QHttpEngine::LocalAuthMiddleware)
