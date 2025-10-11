# Qlipmon

## Description

Clipboard history saver with native rofi plugin and dbus interface.

There's two components:

 * A server that runs in the background (run from startx or via `systemctl --user (start/enable/disable) qlipmon-server.service`) and saves every clipboard selection in memory
 * A rofi plugin that allows you to select previous clipboard selections

Configuration for the server can be done either via the command line (`qlipmon --help`) or via ini file stored in `$HOME/.config/qlipmon/server.ini`

The rofi plugin can only be configured via ini file stored in `$HOME/.config/qlipmon/rofi.ini`

The rofi plugin can be selected by running `rofi -modi qlipmon -show qlipmon`.

After running each of the components the ini files should be populated with sensible default values.

## Database Persistence

Qlipmon now supports persistent storage of clipboard history using SQLite database. By default, clipboard history is stored in memory only, but you can enable disk-based storage for persistence across reboots.

### Database Configuration Options

**Command Line Options:**
- `--disk-database true/false` - Enable or disable disk database (default: false)
- `--database-path <path>` - Set database file path (default: ~/.local/share/qlipmon/qlipmon.db)

**Configuration File Options:**
The server configuration file (`$HOME/.config/qlipmon/server.ini`) supports these database settings:
- `use_disk_database = false` - Enable disk database instead of in-memory storage
- `database_path = ~/.local/share/qlipmon/qlipmon.db` - Path to the SQLite database file

### Default Database Location

When disk database is enabled without specifying a custom path, the database file is created at:
`~/.local/share/qlipmon/qlipmon.db`

The directory `~/.local/share/qlipmon/` will be automatically created if it doesn't exist.

There is also a dbus interface that allows you get get previous selections or even change the current one.

Every time a new selection is made, a dbus broadcast is also emitted allowing you to listen for these event and take any action you want.
Selection broadcast is disabled by default and can be enabled via command line, ini configuration or via dbus interface.

Selections are saved in memory by default, but persistent disk storage is now supported via SQLite database.


## Rationale

After searching for a rofi plugin for clipboard management I didn't find any that worked properly.

 * In some, the selections were truncated and distinct selections with equal truncations were considered to be identical.
 * In others there was no real integration with rofi, meaning you couldn't run it as part of combi for example.
 * Others were constantly polling the selection buffer and did not employ an asynchronous approach
 * I also wanted the ability to get notified of new selections and take any actions I deemed appropriate. A dbus interface seemed perfect for this.


Built using [QT](https://qt.io)
