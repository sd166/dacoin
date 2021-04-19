#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-DA");
    BOOST_CHECK(GetBoolArg("-DA"));
    BOOST_CHECK(GetBoolArg("-DA", false));
    BOOST_CHECK(GetBoolArg("-DA", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-DAo"));
    BOOST_CHECK(!GetBoolArg("-DAo", false));
    BOOST_CHECK(GetBoolArg("-DAo", true));

    ResetArgs("-DA=0");
    BOOST_CHECK(!GetBoolArg("-DA"));
    BOOST_CHECK(!GetBoolArg("-DA", false));
    BOOST_CHECK(!GetBoolArg("-DA", true));

    ResetArgs("-DA=1");
    BOOST_CHECK(GetBoolArg("-DA"));
    BOOST_CHECK(GetBoolArg("-DA", false));
    BOOST_CHECK(GetBoolArg("-DA", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noDA");
    BOOST_CHECK(!GetBoolArg("-DA"));
    BOOST_CHECK(!GetBoolArg("-DA", false));
    BOOST_CHECK(!GetBoolArg("-DA", true));

    ResetArgs("-noDA=1");
    BOOST_CHECK(!GetBoolArg("-DA"));
    BOOST_CHECK(!GetBoolArg("-DA", false));
    BOOST_CHECK(!GetBoolArg("-DA", true));

    ResetArgs("-DA -noDA");  // -DA should win
    BOOST_CHECK(GetBoolArg("-DA"));
    BOOST_CHECK(GetBoolArg("-DA", false));
    BOOST_CHECK(GetBoolArg("-DA", true));

    ResetArgs("-DA=1 -noDA=1");  // -DA should win
    BOOST_CHECK(GetBoolArg("-DA"));
    BOOST_CHECK(GetBoolArg("-DA", false));
    BOOST_CHECK(GetBoolArg("-DA", true));

    ResetArgs("-DA=0 -noDA=0");  // -DA should win
    BOOST_CHECK(!GetBoolArg("-DA"));
    BOOST_CHECK(!GetBoolArg("-DA", false));
    BOOST_CHECK(!GetBoolArg("-DA", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--DA=1");
    BOOST_CHECK(GetBoolArg("-DA"));
    BOOST_CHECK(GetBoolArg("-DA", false));
    BOOST_CHECK(GetBoolArg("-DA", true));

    ResetArgs("--noDA=1");
    BOOST_CHECK(!GetBoolArg("-DA"));
    BOOST_CHECK(!GetBoolArg("-DA", false));
    BOOST_CHECK(!GetBoolArg("-DA", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-DA", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-DA", "eleven"), "eleven");

    ResetArgs("-DA -da");
    BOOST_CHECK_EQUAL(GetArg("-DA", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-DA", "eleven"), "");

    ResetArgs("-DA=");
    BOOST_CHECK_EQUAL(GetArg("-DA", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-DA", "eleven"), "");

    ResetArgs("-DA=11");
    BOOST_CHECK_EQUAL(GetArg("-DA", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-DA", "eleven"), "11");

    ResetArgs("-DA=eleven");
    BOOST_CHECK_EQUAL(GetArg("-DA", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-DA", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-DA", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-DA", 0), 0);

    ResetArgs("-DA -da");
    BOOST_CHECK_EQUAL(GetArg("-DA", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-da", 11), 0);

    ResetArgs("-DA=11 -da=12");
    BOOST_CHECK_EQUAL(GetArg("-DA", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-da", 11), 12);

    ResetArgs("-DA=NaN -da=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-DA", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-da", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--DA");
    BOOST_CHECK_EQUAL(GetBoolArg("-DA"), true);

    ResetArgs("--DA=verbose --da=1");
    BOOST_CHECK_EQUAL(GetArg("-DA", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-da", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noDA");
    BOOST_CHECK(!GetBoolArg("-DA"));
    BOOST_CHECK(!GetBoolArg("-DA", true));
    BOOST_CHECK(!GetBoolArg("-DA", false));

    ResetArgs("-noDA=1");
    BOOST_CHECK(!GetBoolArg("-DA"));
    BOOST_CHECK(!GetBoolArg("-DA", true));
    BOOST_CHECK(!GetBoolArg("-DA", false));

    ResetArgs("-noDA=0");
    BOOST_CHECK(GetBoolArg("-DA"));
    BOOST_CHECK(GetBoolArg("-DA", true));
    BOOST_CHECK(GetBoolArg("-DA", false));

    ResetArgs("-DA --noDA");
    BOOST_CHECK(GetBoolArg("-DA"));

    ResetArgs("-noDA -DA"); // DA always wins:
    BOOST_CHECK(GetBoolArg("-DA"));
}

BOOST_AUTO_TEST_SUITE_END()
