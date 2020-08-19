.. _gregorian:

.. highlight:: cpp

Adapt boost::gregorian::date for IntervalDict
==============================================
.. doxygenfunction:: interval_dict::date_literals::operator""_dt

.. doxygenclass:: interval_dict::IntervalTraits< Interval, typename std::enable_if< std::is_same< typename boost::icl::interval_traits< Interval >::domain_type, boost::gregorian::date >::value >::type >
   :members:

