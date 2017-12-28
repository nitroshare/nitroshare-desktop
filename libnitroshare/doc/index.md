libnitroshare provides a shared library that contains the core functionality for the NitroShare binaries.

Third-party plugins that extend NitroShare's functionality must link against this library. The classes included in the library provide access to NitroShare's internals, enabling the following:

- Register new actions (used by the CLI, API plugin, and tray plugin)
- Provide a new method for discovering / registering devices
- Create handlers for new item types
- Enumerate and modify settings
- Invoke actions provided by other plugins
- Create and manage transfers
