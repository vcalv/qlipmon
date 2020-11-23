# Qlipmon

## Description

Clipboard history saver with native rofi plugin and dbus interface.

There's two components:

 * A server that runs in the background (run from startx or via `systemctl --user (start/enable/disable) qlipmon-server.service`) and saves every clipboard selection on memory
 * A rofi plugin that allows you to select previous clipboard selections

Configuration for the server can be done either via the command line (`qlipmon --help`) or via ini file stored in `$HOME/.config/qlipmon/server.ini`

The rofi plugin can only be configured via ini file stored in `$HOME/.config/qlipmon/rofi.ini`

The rofi plugin can be selected by running `rofi -modi qlipmon -show qlipmon`.

After running each of the components the ini files should be populated with sensible default values.

There is also a dbus interface that allows you get get previous selections or even change the current one.

Every time a new selection is made, a dbus broadcast is also emitted allowing you to listen for these event and take any action you want.
Selection broadcast is disabled by default and can be enabled via command line, ini configuration or via dbus interface.

Selections are saved in memory only and for now no persistence is possible,


## Rationale

After searching for a rofi plugin for clipboard management I didn't find any that worked properly.

 * In some, the selections were truncated and distinct selections with equal truncations were considered to be identical.
 * In others there was no real integration with rofi, meaning you couldn't run it as part of combi for example.
 * Others were constantly polling the selection buffer and did not employ an asynchronous approach
 * I also wanted the ability to get notified of new selections and take any actions I deemed appropriate. A dbus interface seemed perfect for this.


Built using [QT](https://qt.io)
