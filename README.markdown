# High-water mark buffers

This library implements “high-water mark” buffers.  A brief overview
of what they are and why you might want to use them can be found
[here](http://dcreager.net/2009/12/23/high-water-mark-buffers/).


## Building libhwm

To build libhwm, you need a recent C compiler and
[SCons](http://www.scons.org/).  There are no library prerequisites.
To build the source, just run

    $ scons

To build and run the test suite, use

    $ scons test

To install the library, use

    $ sudo scons prefix=/usr/local install

The `prefix` parameter is optional; if left out, we default to _/usr_.


## Documentation

The library is documented using
[Doxygen](http://www.stack.nl/~dimitri/doxygen/index.html).  To build
the documentation, run

    $ scons doc

(You must have doxygen installed for this to work.)  This will create
HTML documentation in the _doc/html_ subdirectory.  To install the
documentation, use

    $ sudo scons prefix=/usr/local install-doc

This will copy the documentation into _/usr/local/share/doc/libhwm_.
Note that `scons install` does not install the documentation.
