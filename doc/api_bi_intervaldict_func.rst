.. _bi_intervaldict_func:

.. highlight:: cpp

Functions taking *BiBiIntervalDictExp* or *BiIntervalDict*
===========================================================

subtract()
-----------
.. doxygenfunction:: subtract(BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl> dict_1, const BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl> &dict_2)

merge()
-----------
.. doxygenfunction:: merge(BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl> dict_1, const BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl> &dict_2)

intervals()
-----------
.. doxygenfunction:: intervals(const BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl> &interval_dict, const Key &key, Interval query_interval = interval_extent<Interval>)

-----------

.. doxygenfunction:: intervals(const BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl> &interval_dict, std::vector<Key> keys, Interval query_interval = interval_extent<Interval>)

-----------

.. doxygenfunction:: intervals(const BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl> &interval_dict, Interval query_interval = interval_extent<Interval>)

disjoint_intervals()
----------------------
.. doxygenfunction:: disjoint_intervals(const BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl> &interval_dict, const Key &key, Interval query_interval = interval_extent<Interval>)

----------------------

.. doxygenfunction:: disjoint_intervals(const BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl> &interval_dict, std::vector<Key> keys, Interval query_interval = interval_extent<Interval>)

----------------------

.. doxygenfunction:: disjoint_intervals(const BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl> &interval_dict, Interval query_interval = interval_extent<Interval>)

operator -()
----------------------
.. doxygenfunction:: operator-(BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl> dict_1, const BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl> &dict_2)


operator +()
----------------------
.. doxygenfunction:: operator+(BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl> dict_1, const BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl> &dict_2)

operator <<()
----------------------
.. doxygenfunction:: operator<<(std::ostream &ostream, const BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl> &interval_dict)

Flatten
----------------------
.. doxygenfunction:: flattened(BiIntervalDictExp<Key, Val, Interval, Impl, InverseImpl> interval_dict, FlattenPolicy<typename detail::identity<Key>::type, typename detail::identity<Val>::type, typename detail::identity<Interval>::type> keep_one_value = flatten_policy_prefer_status_quo())

