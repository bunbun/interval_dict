.. _intervaldict:

.. highlight:: cpp

IntervalDict API
================

``IntervalDict`` comes in two flavours:


1. *IntervalDict* template on Date / Time etc.
----------------------------------------------

This is templated on ``BaseType`` = Date / Time etc.
The default ICL Interval allows you to mix and match open / closed intervals at run time.

Example:
**********

*Allow mixed Interval types*

.. literalinclude:: ../examples/simple.cpp
   :language: c++
   :emphasize-lines: 5
   :lines: 1-4,14-29

Output:

.. literalinclude:: ../examples/simple.cpp
   :language: text
   :lines: 34-38

Signature:
************
.. doxygentypedef:: interval_dict::IntervalDict

-----------

2. *IntervalDictExp* allows open | closed interval types to be specified
-------------------------------------------------------------------------

Example:
**********

*Only allow closed Intervals*

.. literalinclude:: ../examples/simple_exp.cpp
   :language: c++
   :emphasize-lines: 5
   :lines: 1-4,14-29

Output:

.. literalinclude:: ../examples/simple_exp.cpp
   :language: text
   :lines: 34-38

Signature:
************
.. doxygenclass:: interval_dict::IntervalDictExp
   :members:

