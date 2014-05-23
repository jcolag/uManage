uManage
=======

_uManage_ is a time-monitoring application to operate under the X Window System.

Maybe I didn't look hard enough, but every time I try to monitor my computer usage, the best the software seems to be able to do is _ask me_ every few minutes.  The problems with this are many, but the biggies:

 - You just broke my train of thought.
 - You're making me do clerical work that a computer can do better.
 - If I really wanted to write down what I was doing, I don't need special software for that.
 - You're not catching windows I only have open for a couple of minutes.

So, uManage is a simple application to do the tedious work for you.

Two Builds
==========

Two versions of _uManage_ are built with this package.

 - `uManage` is purely a command-line program.

 - `uManageUi` uses the same code and operation described, but adds an application notification ("system tray") menu to configure options and exit the program.

Usage
=====

Run _uManage_ from the command line (for now) as follows:

    uManage [-d loop_delay] [-f log_file] [-i idle_threshold] [-s]

The options are:

 - `-d` _`delay`_:  Delay (in seconds) between looks at the current window.  The default is one second (1s).

 - `-f` _`filename`_:  The name of the file to log timing information.  If no file is specified, the program directs output to `stdout`.

 - `-i` _`idle`_:  Time (in seconds) before idle time counts as idle, to limit racking up "idle" time looking away from the screen.  The default is three minutes (180s).

 - `-s`:  Save current configuration options to the configuration file.  __Warning__:  This option will overwrite the existing options.

_uManageUi_ can be invoked the same way.  However, there is potential to also run it _without_ the command-line, provided the output is logged to a file, since the options can be configured and the user can exit graphically.

Alternate Configuration
-----------------------

You can also set options with a configuration file in your home directory.  At this time, there are equivalent options to the above, divided into two categories.

For obvious reasons, there is no equivalent to `-s`.

The following is an example `~/.uManage` file with all available options set:

    [File]
    log=~/logs/useractivity.csv
    
    [Timing]
    idle=240
    poll=2

The aforementioned defaults are still in effect if they are not replaced by the file, and command-line options override the file's options.

Also, as suggested earlier, _uManageUi_ can also change the configuration with a graphical interface.

Output
======

_uManage_ generates data as comma-separated values (CSV), formatted as:

    YYYY,MM,DD,HH,MM,SS,Window ID,Window Title,Time Used,Time Idle

Breaking that down, we have:

 - YYYY:  A four digit year, as in __2014__.  Zero-padded, if for some reason you're using this in AD999 or earlier.

 - MM:  A two digit month, as in __05__.

 - DD:  A two digit day of the month, as in __08__.

 - HH:  A two digit hour of the day, on a twenty-four hour clock, __00__-__23__.

 - MM:  A two digit minute of the hour, __00__-__59__.

 - SS:  A two digit second of the minute, __00__-__59__.

 - Window ID:  An eight digit hexadecimal value representing the window that currently has focus, such as __0520008D__.

 - Window Title:  Title of the window that currently has focus, such as __*README.md (~/Documents/code/uManage) - gedit__.

 - Time Used:  Time (in seconds) that the current window has been in use.

 - Time Idle:  Time (in seconds) that the current window has been open but idle.

Notes
-----

The time and date has been split up under the philosophy that it is easier to reassemble than to break apart later.

The _Window ID_ has been included to make it easier to group similar activities (browser tabs, multiple files being edited, etc.) for processing.  However, _uManage_ issues a message based on every change in _Window Title_ so that the activity can be separated along critically-different uses (reading versus writing, which web site in the browser, etc.).

The _Time Used_ for each window represents active usage (user input has occurred).  It does not include idle time, so that a window that is merely open does not accumulate time.

I should also note that `umenu.glade` is _not_ a Glade-produced file.  Or, rather, it was, but since Glade doesn't appear to support `GtkMenu`s, and certainly not as top-level objects, I did some hand-editing to make it fly.  So, don't go loading it up to make changes, or at least don't be surprised if you're attacked by monsters when you do.

Name
====

Think of __uManage__ as "micro-manage"---the 'u' is close enough to a Greek _mu_---which is either a pun or accurate, depending on how you use it.

Or think of it as "You Manage," since you can, indeed, do that.

Or I'm running on Ubuntu, and I assume that my eventual goals for a GUI will end up integrating with or at least touching on Unity, somehow.  So, maybe it's an abbreviation for it.  Probably not, though.

Sources and Requirements
========================

The icon for the application notification menu is adapted from __[Clock](http://thenounproject.com/term/clock/6781/) by Olyn LeRoy from The Noun Project__, used under a [Creative Commons Attribution 3.0](https://creativecommons.org/licenses/by/3.0/us/) license.

Since idle time is important, we'll need [XCB](http://xcb.freedesktop.org/), the X protocol C-language Binding, especially its screensaver libraries.

    sudo apt-get install libxcb-screensaver0-dev

We'll also need to know what window we're looking at, and [xdotool](http://www.semicomplete.com/projects/xdotool/), or rather its library form, will also be useful.

    sudo apt-get install libxdo-dev

Glib provides its "key-value file parser" to manage the configuration file.

    sudo apt-get install libglib2.0-dev

The graphical interface is handled with GTK+.

    sudo apt-get install libgtk-3-dev

The application menu also uses the Application Indicator library.

    sudo apt-get install appindicator-dev

More to come as the application advances, but this should be a good start.

