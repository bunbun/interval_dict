.. _ptime:

.. highlight:: cpp

Adapt boost::posix::time for IntervalDict
==============================================
.. doxygenfunction:: interval_dict::ptime_literals::operator""_pt

.. doxygenclass:: interval_dict::IntervalTraits< Interval, typename std::enable_if< std::is_same< typename boost::icl::interval_traits< Interval >::domain_type, boost::posix_time::ptime >::value >::type >
   :members:



