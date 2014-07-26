uManage
=======

_uManage_ is a time-monitoring application to operate under the X Window System.

Maybe I didn't look hard enough, but every time I try to monitor my computer usage, the best the software seems to be able to do is _ask me_ every few minutes.  The problems with this are many, but the biggies:

 - You just broke my train of thought.
 - You're making me do clerical work that a computer can do better.
 - If I really wanted to write down what I was doing, I don't need special software for that.
 - You're not catching windows I only have open for a couple of minutes.

In other words, I would rather the computer do the work for me, rather than having me do the work for the computer.  So, uManage is a simple application to do the tedious monitoring for you.

Two Builds
==========

Two versions of _uManage_ are built with this package.

 - `uManage` is purely a command-line program.

 - `uManageUi` uses the same code and operation described, but adds an application notification ("system tray icon") menu to configure options and exit the program.

Usage
=====

Run _uManage_ from the command line (for now) as follows:

    uManage [-b database] [-d loop_delay] [-f log_file] [-i idle_threshold] [-j jiggle_delay] [-n] [-s] [-t time_format]

The options are:

 - `-b`/`--database` _`database`_:  Name of an SQLite3 database file, which _uManage_ will create, if it doesn't already exist.  __Note__ that _uManage_ will automatically set the _time format_ (see `-t`, below) to `%Y-%m-%dT%T` ([ISO 8601](https://en.wikipedia.org/wiki/ISO_8601), which is required for SQLite's date and time functions) and block attempts to change the format.  Items in the CSV will also be quoted (including the entire date, regardless of format) for compatability with the database.

 - `-d`/`--delay` _`delay`_:  Delay (in seconds) between looks at the current window.  The default and minimum are both one second (1s).

 - `-f`/`--filename` _`filename`_:  The name of the file to log timing information.  If no file is specified, the program directs output to `stdout`.  If `--no-output` is set, then it overrides writing to both `stdout` and the specified file.

 - `-i`/`--idle` _`idle`_:  Time (in seconds) before idle time counts as idle, to limit racking up "idle" time looking away from the screen.  The default is three minutes (180s).

 - `-j`/`--jiggle` _`delay`_:  Time (in seconds) between automatic mouse movements, if active.  The default is one minute (60s).  The option has no effect in the non-GUI version of _uManage_.

 - `-n`/`--no-output`:  Block console/file output.

 - `-s`/`--save`:  Save current configuration options to the configuration file.  __Warning__:  This option will overwrite the existing options.

 - `-t`/`--time-format` _`format`_:  Time and date format for log entries.  Passed through directly to [`strftime()`](http://en.cppreference.com/w/c/chrono/strftime); quote if necessary.  If `--database` is used, an ISO 8601 format overrides this selection, for compatibility with SQLite.

 - `-x`/`--jiggle-distance` _`distance`_:  Distance to move the mouse pointer when the _Jiggle Mouse_ feature is active.

_uManageUi_ can be invoked the same way.  However, there is potential to also run it _without_ the command-line, provided the output is logged to a file, since the options can be configured and the user can exit graphically.

Alternate Configuration
-----------------------

You can also set options with a configuration file in your home directory.  At this time, there are equivalent options to the above, divided into two categories.

For obvious reasons, there is no equivalent to `-s`.  However, there is the menu configuration here that isn't available otherwise.

The following is an example `~/.uManage` file with all available options set:

    [File]
    log=/home/you/logs/useractivity.csv
    database=/home/you/logs/useractivity.db
    notext=1
    
    [Timing]
    idle=240
    poll=2
    format=%c
    jiggle=23

    [Menu]
    Cooking=
    Morning Exercise=

The aforementioned defaults are still in effect if they are not replaced by the file, and command-line options override the file's options.

Also, as suggested earlier, _uManageUi_ can also change the configuration with a graphical interface.

Note that `log` is unused if `notext` is set.

GUI Features
============

In addition to easier configuration and exit, _uManageUi_ also provides the following:

 - _About uManage..._:  Shows information on licensing and sources.

 - _Show Options..._:  Provides a simple graphical interface to change the program's configuration options.

 - _Quit_:  Terminates _uManage_ on the next polling cycle.

There are also two features that log information.

 - _Pause_:  Suspends data collection while active.

 - _Jiggle Mouse_:  Moves the mouse back and forth regularly, 25 pixels diagonally, to simulate input activity, thereby avoiding system time-outs.  (Use with care, if the jiggle period is low, since turning it off becomes challenging.)

Both log their durations to tables in the database, _only_ if available, the `pauses` and `keepalives` tables, respectively.  They each log the starting and ending time of use.

User Defined
------------

Through the use of the configuration file, _uManageUi_ also tracks the duration of any user-defined activities.

You can define each activity in the `.uManage` file, with a `[Menu]` section, followed by each desired item followed by an equal sign (`=`).  For example, the following might handle major daily activities, though I don't know many people with the discipline to fill it all in.

    [Menu]
    Wash Up=
    Exercise=
    Eat=
    Commute=
    Work=
    Watch Television=
    Go Out=

Notice that the items may be any string (that doesn't have an equal sign in it), including spaces.  Those titles (spaces included) become the `reason` listed in the log, as described below.

Failing to terminate with the equal sign causes the Glib key-file parser some sort of indigestive problem.  At best, it skips the name.  At worst, it may crash.  A this time, no value is read for the key, though future expansion may provide a use.

For the moment, all such activities are logged to the `pauses` table, even though they don't pause operations.

Output
======

_uManage_ generates data as comma-separated values (CSV), by default formatted as:

    YYYY,MM,DD,HH,MM,SS,Window ID,Window Title,Time Used,Time Idle

Breaking that down, we have:

 - YYYY:  A four digit year, as in __2014__.  Zero-padded, if for some reason you're using this in AD999 or earlier.

 - MM:  A two digit month, as in __05__.

 - DD:  A two digit day of the month, as in __08__.

 - HH:  A two digit hour of the day, on a twenty-four hour clock, __00__-__23__.

 - MM:  A two digit minute of the hour, __00__-__59__.

 - SS:  A two digit second of the minute, __00__-__59__.

 - Window ID:  An eight digit hexadecimal value representing the window that currently has focus, such as __0520008D__.

 - Window Title:  Title of the window that currently has focus, such as __*README.md (~/Documents/code/uManage.c) - gedit__.

 - Time Used:  Time (in seconds) that the current window has been in use.

 - Time Idle:  Time (in seconds) that the current window has been open but idle.

If, however, the time format is changed through configuration, that format replaces the first six fields.

Database
--------

_uManage_ will also log to an SQLite database (see the `-b` command-line option and the `[File]`/`database` configuration described above), which has similar fields except for the date, which is a single field by default in C's `%Y-%m-%dT%T` format (based on `%c`, which appears to be incompatible with SQLite, itself).

The table for this data is named **`activity`**, and has the following fields:

 - `start`, a `text` field.  Again, in `%Y-%m-%dT%T` format unless configured to do something different in the graphical options interface.

 - `window`, the Window ID, also `text`.  Technically, since it's an eight-digit hexadecimal number, it could obviously be an integer, but it's doubtful that anybody could rationally use it _as_ a number.

 - `title`, obviously `text`, storing the Window Title.

 - `used`, the Time Used, as an `integer`, in seconds.

 - `idle`, the Time Idle, as an `integer`, in seconds.

The difference in date format, of course, reflects SQLite's [Date and Time API](https://www.sqlite.org/lang_datefunc.html).

As mentioned, there are two other tables.

The use of the *Pause Recording* feature is recorded in the **`pauses`** table, with the following fields.  Again, all date strings are in `%Y-%m-%dT%T` format unless configured to do something different in the graphical options interface.

 - `start`, a `text` field representing when the user turned the feature on.

 - `end`, a `text` field representing when the user deactivated the feature.

 - `reason`, another `text` field showing what the duration represents.  Pauses are marked **`Pause`**, while any user-defined options will show under their own titles.

The use of the *Jiggle Mouse* feature, likewise, is recorded in the **`keepalives`** table, with fields mostly similar to the `pauses` table:

 - `start`, a `text` field representing when the user turned the feature on.

 - `end`, a `text` field representing when the user deactivated the feature.

Given that the features are not available without the user interface, no data will be logged to `pauses` or `keepalives` using _uManage_ rather than _uManageUi_.

Notes
-----

The time and date has been split up under the philosophy that it is easier to reassemble than to break apart later.

The _Window ID_ has been included to make it easier to group similar activities (browser tabs, multiple files being edited, etc.) for processing.  However, _uManage_ issues a message based on every change in _Window Title_ so that the activity can be separated along critically-different uses (reading versus writing, which web site in the browser, etc.).

The _Time Used_ for each window represents active usage (user input has occurred).  It does not include idle time, so that a window that is merely open does not accumulate time.

I should also note that `umenu.glade` is _not_ a Glade-produced file.  Or, rather, it was, but since Glade doesn't appear to support `GtkMenu`s, and certainly not as top-level objects, I did some hand-editing to make it fly.  So, don't go loading it up to make changes, or at least don't be surprised if you're attacked by monsters when you do.

Name
====

Think of __uManage__ as "micro-manage"---the 'u' is close enough to a Greek _mu_/&mu;---which is either a pun or accurate, depending on how you use it.

Or think of it as "You Manage," since you can, indeed, do that.

Or I'm running on Ubuntu, and I assume that my eventual goals for a GUI will end up integrating with or at least touching on Unity, somehow.  So, maybe it's an abbreviation for it.  Probably not, though.

Sources and Requirements
========================

The icon for the application notification menu is adapted from __[Clock](http://thenounproject.com/term/clock/6781/) by Olyn LeRoy from The Noun Project__, used under a [Creative Commons Attribution 3.0](https://creativecommons.org/licenses/by/3.0/us/) license.

Since idle time is important, we'll need [XCB](http://xcb.freedesktop.org/), the X protocol C-language Binding, especially its screensaver libraries.

    sudo apt-get install libxcb-screensaver0-dev

We'll also need to know what window we're looking at, and [xdotool](http://www.semicomplete.com/projects/xdotool/), or rather its library form, will also be useful.

    sudo apt-get install libxdo-dev

Glib provides its [key-value file parser](https://developer.gnome.org/glib/unstable/glib-Key-value-file-parser.html) to manage the configuration file.

    sudo apt-get install libglib2.0-dev

Database support is [SQLite](https://www.sqlite.org/).

    sudo apt-get install libsqlite3-dev

The graphical interface is handled with [GTK+](https://developer.gnome.org/gtk3/).

    sudo apt-get install libgtk-3-dev

The application menu also uses the [Application Indicator](https://wiki.ubuntu.com/DesktopExperienceTeam/ApplicationIndicators) library.

    sudo apt-get install appindicator-dev

More to come as the application advances, but this should be a good start.

The Future
==========

Some possible options that may eventually be in the works come largely from scripts I already use to monitor certain activity.

 - [X] User-defined start/stop states.

 - [ ] Trigger actions when state changes, for example opening a web page or start an application after a long time idle or paused.

 - [ ] Download and log/insert supplemental data such as weather.

 - [ ] Create installable package.

 - [ ] Show program state through application icon, especially _Pause_.

 - [ ] Allow filing notes from indicator menu.  (Possible with GTK+?)

 - [ ] Report on log entries.  (Separate application?)

