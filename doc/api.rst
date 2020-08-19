.. _interval_dict-api:

*************
Reference
*************

The *interval_dict* library consists of the following parts:

* :ref:`interval_dict/intervaldicticl.h <intervaldicticl>`: Implements
  `IntervalDictICL` / `IntervalDictICLExp` using `boost::icl::interval_map`
  (partial specialisation of `IntervalDict`)
* :ref:`interval_dict/intervaldict.h <intervaldict>`: `IntervalDict` /
  `IntervalDictExp` dictionaries templated for different implementations as well
  as key, value, interval
* :ref:`interval_dict/intervaldict.h functions <intervaldict_func>`: Functions taking `IntervalDict` / `IntervalDictExp`
* :ref:`interval_dict/bi_intervaldicticl.h <bi_intervaldicticl>`: Implements
  `BiIntervalDictICL` / `BiIntervalDictICLExp` using `boost::icl::interval_map`
  (partial specialisation of `BiIntervalDict`)
* :ref:`interval_dict/bi_intervaldict.h <bi_intervaldict>`: `BiIntervalDict` /
  `BiIntervalDictExp` bidirectional dictionaries
* :ref:`interval_dict/bi_intervaldict.h functions <bi_intervaldict_func>`: Functions taking
  `BiIntervalDict` / `BiIntervalDictExp` bidirectional dictionaries
* :ref:`interval_dict/ptime.h <ptime>`: Adapts Posix time
  (`boost::posix_time::ptime`) for `IntervalDict`
* :ref:`interval_dict/gregorian.h <gregorian>`: Adapts Gregorian dates
  (`boost::gregorian::date`) for `IntervalDict`
* :ref:`interval_dict/interval_traits.h <interval_traits>`: Traits to build intervals for `IntervalDict`.
* :ref:`interval_dict/icl_interval_map_adaptor.h <icl_interval_map_adaptor>`:
  Functions adapting `boost::icl::interval_map` to implement `IntervalDict` and
  `BiIntervalDict`
* :ref:`interval_dict/rebase_implementation.h <rebase_implementation>`: Template
  metaprogramming to get the type of the underlying implementation
  but in the other (inverse) direction

All functions and types provided by the library reside in namespace *interval_dict*.

