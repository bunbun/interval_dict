.. _icl_interval_map_adaptor:

.. highlight:: cpp


These are the functions and types need to implement an `IntervalDict`.

In other words, this is the minimum abstract API that is required to support storing and query values and intervals in order to support `IntervalDict`.

Any alternative implementations just need to follow this model and provide:
    - (opaque) data structures to store intervals and values in whichever way
    - function calls with the same signature.


Trait to derive the "inverse" implementation for boost::icl::interval_map
=============================================================================================
.. doxygenstruct:: interval_dict::rebind< OldVal_, NewVal_, Interval_, Impl_, typename std::enable_if< std::is_same< Impl_, implementation::IntervalDictICLSubMap< OldVal_, Interval_ > >::value, void >::type >
   :members:

Implement IdentifierMap using boost::icl::interval_map
========================================================
.. doxygenfunction:: interval_dict::implementation::gaps

.. doxygenfunction:: interval_dict::implementation::sandwiched_gaps

.. doxygenfunction:: interval_dict::implementation::erase(IntervalDictICLSubMap<Value, Interval> &interval_values, const Interval &query_interval)

.. doxygenfunction:: interval_dict::implementation::erase(IntervalDictICLSubMap<Value, Interval> &interval_values, const Interval &query_interval, const Value &value)

.. doxygenfunction:: interval_dict::implementation::insert

.. doxygenfunction:: interval_dict::implementation::intervals

.. doxygenfunction:: interval_dict::implementation::disjoint_intervals

.. doxygenfunction:: interval_dict::implementation::empty

.. doxygenfunction:: interval_dict::implementation::merged_with

.. doxygenfunction:: interval_dict::implementation::subtract_by

.. doxygenfunction:: interval_dict::implementation::first_disjoint_interval

.. doxygenfunction:: interval_dict::implementation::last_disjoint_interval

