    #include <interval_dict/icl_interval_map_adaptor.h>
    #include <interval_dict/intervaldict.h>
    #include <interval_dict/gregorian.h>

    #include <iostream>

int main ()
{
    using namespace interval_dict;
    using namespace interval_dict::date_literals;
    using namespace std::string_literals;

    //! [Simple Example]
    // Mix and match open and closed intervals
    using Interval = boost::icl::interval<boost::gregorian::date>::type;
    using DateDict = IntervalDict<std::string, int,
                                  boost::gregorian::date,
                                  implementation::IntervalDictICLSubMap<int, Interval>>;

    DateDict interval_dict(DateDict::KeyValueIntervals
    {
        {"aa"s, 0, {20200110_dt, 20200120_dt}},
        {"aa"s, 1, {20200115_dt, 20200125_dt}},
        {"bb"s, 2, {20200201_dt, 20200205_dt}},
        // Specify an inclusive/closed interval
        {"bb"s, 3, DateDict::IntervalType::closed(20200228_dt, 20200228_dt)},
    });

    std::cout << interval_dict << "\n";
    //! [Simple Example]

    /*
       //! [Output]
        aa	[0]	[2020-Jan-10,2020-Jan-15)
        aa	[0, 1]	[2020-Jan-15,2020-Jan-20)
        aa	[1]	[2020-Jan-20,2020-Jan-25)
        bb	[2]	[2020-Feb-01,2020-Feb-05)
        bb	[3]	[2020-Feb-28,2020-Feb-28]
       //! [Output]
    */


    return 0;
}