.. _intervaldict_func:

.. highlight:: cpp

Functions taking *IntervalDictExp* or *IntervalDict*
=======================================================

subtract()
-----------
.. doxygenfunction:: subtract(IntervalDictExp<Key, Value, Interval, Impl> dict_1, const IntervalDictExp<Key, Value, Interval, Impl> &dict_2)

merge()
-----------
.. doxygenfunction:: merge(IntervalDictExp<Key, Value, Interval, Impl> dict_1, const IntervalDictExp<Key, Value, Interval, Impl> &dict_2)

intervals()
-----------
.. doxygenfunction:: intervals(const IntervalDictExp<Key, Value, Interval, Impl> &interval_dict, const Key &key, Interval query_interval = interval_extent<Interval>)

-----------

.. doxygenfunction:: intervals(const IntervalDictExp<Key, Value, Interval, Impl> &interval_dict, std::vector<Key> keys, Interval query_interval = interval_extent<Interval>)

-----------

.. doxygenfunction:: intervals(const IntervalDictExp<Key, Value, Interval, Impl> &interval_dict, Interval query_interval = interval_extent<Interval>)

disjoint_intervals()
----------------------
.. doxygenfunction:: disjoint_intervals(const IntervalDictExp<Key, Value, Interval, Impl> &interval_dict, const Key &key, Interval query_interval = interval_extent<Interval>)

----------------------

.. doxygenfunction:: disjoint_intervals(const IntervalDictExp<Key, Value, Interval, Impl> &interval_dict, std::vector<Key> keys, Interval query_interval = interval_extent<Interval>)

----------------------

.. doxygenfunction:: disjoint_intervals(const IntervalDictExp<Key, Value, Interval, Impl> &interval_dict, Interval query_interval = interval_extent<Interval>)

operator -()
----------------------
.. doxygenfunction:: operator-(IntervalDictExp<Key, Value, Interval, Impl> dict_1, const IntervalDictExp<Key, Value, Interval, Impl> &dict_2)


operator +()
----------------------
.. doxygenfunction:: operator+(IntervalDictExp<Key, Value, Interval, Impl> dict_1, const IntervalDictExp<Key, Value, Interval, Impl> &dict_2)

operator <<()
----------------------
.. doxygenfunction:: operator<<(std::ostream &ostream, const IntervalDictExp<Key, Value, Interval, Impl> &interval_dict)

Flatten
----------------------
.. doxygentypedef:: FlattenPolicy

.. doxygenstruct:: interval_dict::FlattenPolicyDiscard
    :members:

.. doxygenfunction:: flatten_policy_discard

.. doxygenstruct:: interval_dict::FlattenPolicyPreferStatusQuo
    :members:

.. doxygenfunction:: flatten_policy_prefer_status_quo

.. doxygenfunction:: flattened(IntervalDictExp<Key, Value, Interval, Impl> interval_dict, FlattenPolicy<typename detail::identity<Key>::type, typename detail::identity<Value>::type, typename detail::identity<Interval>::type> keep_one_value = flatten_policy_prefer_status_quo())

