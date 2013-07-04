glib-test-source
================

Example use of glib main loop in thread alongside with 'main' main loop that is used in main thread.

Main thread has a timeout source added that triggers every so many seconds. Meanwhile 'other-thread' has another timeout source added that once triggerred stops the thread's main loop and pushes an idle call to the 'main' main loop.
The idle call in 'main' main loop quits the main loop.
