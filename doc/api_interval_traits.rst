.. _interval_traits:

.. highlight:: cpp

Traits for intervals
=====================
.. doxygenclass:: interval_dict::IntervalTraits< Interval, typename std::enable_if< std::numeric_limits< typename boost::icl::interval_traits< Interval >::domain_type >::is_specialized, void >::type >
   :members:

Maximal extent of any interval
================================
.. doxygenvariable:: interval_dict::interval_extent


