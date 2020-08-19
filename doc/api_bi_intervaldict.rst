.. _bi_intervaldict:

.. highlight:: cpp

*BiIntervalDict* API
==========================================

The bidirectional IntervalDict ``BiIntervalDict`` comes in two flavours:


1. *BiIntervalDict* template on Date / Time etc.
----------------------------------------------------

This is templated on ``BaseType`` = Date / Time etc.
The default ICL Interval allows you to mix and match open / closed intervals at run time.

Example:
**********

*Allow mixed Interval types*

.. literalinclude:: ../examples/simple_bi.cpp
   :language: c++
   :emphasize-lines: 5
   :lines: 1-4,14-30

Output:

.. literalinclude:: ../examples/simple_bi.cpp
   :language: text
   :lines: 35-39

Signature:
************
.. doxygentypedef:: interval_dict::BiIntervalDict

-----------

2. *BiIntervalDict* allows open | closed interval types to be specified
-------------------------------------------------------------------------

Example:
**********

*Only allow closed Intervals*

.. literalinclude:: ../examples/simple_bi_exp.cpp
   :language: c++
   :emphasize-lines: 5
   :lines: 1-4,14-29

Output:

.. literalinclude:: ../examples/simple_bi_exp.cpp
   :language: text
   :lines: 34-38

Signature:
************
.. doxygenclass:: interval_dict::BiIntervalDictExp
   :members:
