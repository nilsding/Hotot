# Hotot

Hotot is a "Lightweight, Flexible Microblogging Client" supporting
Twitter and Identi.ca at this point of development. You may visit our
official website at http://hotot.org/ . Check it out for any updates
on Hotot.

## Building from source:
Since Hotot core is largely based on HTML5, JavaScript and Webkit technology,
It can be run under many Webkit implementations.  Hotot officially supports Qt.

Dependencies:

Common Requirements:
* cmake
* intltool

Qt Wrapper:
* Qt6

On Ubuntu 11.10 all of these resources are available in the standard repositories.

    # apt-get install libqt4-dev cmake intltool

    $ cd {source-directory}
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make

Install as root:

    # make install

This will install Hotot in the default prefix: `/usr/local`, in order to change
to a different prefix, use:
`-DCMAKE_INSTALL_PREFIX=/prefix/you/want`

By default qt will be built.

The following options can be applied, with On/Off as value.

* `-DWITH_QT` build qt version (program name: `hotot-qt`)
