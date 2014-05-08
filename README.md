uManage
=======

_uManage_ is a time-monitoring application to operate under the X Window System.

Maybe I didn't look hard enough, but every time I try to monitor my computer usage, the best the software seems to be able to do is _ask me_ every few minutes.  The problems with this are many, but the biggies:

 - You just broke my train of thought.
 - You're making me do clerical work that a computer can do better.
 - If I really wanted to write down what I was doing, I don't need special software for that.
 - You're not catching windows I only have open for a couple of minutes.

So, uManage is a simple application to do the tedious work for you.

Name
====

Think of __uManage__ as "micro-manage"---the 'u' is close enough to a Greek _mu_---which is either a pun or accurate, depending on how you use it.

Or think of it as "You Manage," since you can, indeed, do that.

Or I'm running on Ubuntu, and I assume that my eventual goals for a GUI will end up integrating with or at least touching on Unity, somehow.  So, maybe it's an abbreviation for it.  Probably not, though.

Requirements
============

Since idle time is important, we'll need [XCB](http://xcb.freedesktop.org/), the X protocol C-language Binding, especially its screensaver libraries.

    sudo apt-get install libxcb-screensaver0-dev

We'll also need to know what window we're looking at, and [xdotool](http://www.semicomplete.com/projects/xdotool/), or rather its library form, will also be useful.

    sudo apt-get install libxdo-dev

More to come as the application advances, but this should be a good start.

License
=======

I am releasing uManage under the GPL v3.

