.. _bi_intervaldicticl:

.. highlight:: cpp

*BiIntervalDict* boost ICL implementation
===========================================

``interval_dict/bi_intervaldicticl.h`` provides an bidirectional interval associative map ``IntervalDict`` with an underlying implementation
based on ``boost::icl::interval_map``.

This stores each disjoint interval separately.

This is very efficient only if key-value associations do not fluctuation rapidly
over time (leading to a profusion of disjoint intervals).

``BiIntervalDictICL`` Comes in two flavours:

1. *BiIntervalDictICL* template on Date / Time etc.
-----------------------------------------------------

Example:
**********

*Allow mixed Interval types*

.. literalinclude:: ../examples/simple_bi_icl.cpp
   :language: c++
   :emphasize-lines: 4
   :lines: 1-3,11-24

Output:

.. literalinclude:: ../examples/simple_bi_icl.cpp
   :language: text
   :lines: 29-33

Signature:
************
.. doxygentypedef:: interval_dict::BiIntervalDictICL

-----------

2. *BiIntervalDictICLExp* templated on open | closed interval types
-------------------------------------------------------------------------

Example:
**********

*Only allow closed Intervals*

.. literalinclude:: ../examples/simple_bi_iclexp.cpp
   :language: c++
   :emphasize-lines: 4
   :lines: 1-3,11-24

Output:

.. literalinclude:: ../examples/simple_bi_iclexp.cpp
   :language: text
   :lines: 29-33

Signature:
************
.. doxygentypedef:: interval_dict::BiIntervalDictICLExp
