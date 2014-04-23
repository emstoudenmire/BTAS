#include "test.h"

#include "itensor/index.h"

using namespace itensor;

TEST_CASE("Index")
    {
    Index l1("l1",3),
          l2("l2",4),
          s1("s1",2,Site),
          s2("s2",2,Site);

    SECTION("Dimensions")
        {
        REQUIRE(l1.m() == 3);
        REQUIRE(l2.m() == 4);
        REQUIRE(s1.m() == 2);
        REQUIRE(s2.m() == 2);
        }

    SECTION("Index Types")
        {
        REQUIRE(l1.type() == Link);
        REQUIRE(l2.type() == Link);
        REQUIRE(s1.type() == Site);
        REQUIRE(s2.type() == Site);
        }

    SECTION("Prime Levels")
        {
        auto l1p = prime(l1);
        REQUIRE(l1p.primeLevel() == 1);
        REQUIRE(l1p != l1);
        auto l10 = noprime(l1p);
        REQUIRE(l10.primeLevel() == 0);
        REQUIRE(l10 == l1);
        auto l1m = mapprime(l10,0,3);
        REQUIRE(l1m.primeLevel() == 3);
        auto l1n = mapprime(l10,1,3);
        REQUIRE(l1n.primeLevel() == 0);
        l1m.mapprime(3,2);
        REQUIRE(l1m.primeLevel() == 2);
        l1m.mapprime(2,0,Site);
        REQUIRE(l1m.primeLevel() == 2);
        auto s1m = mapprime(s1,0,5,Link);
        REQUIRE(s1m.primeLevel() == 0);
        s1m = mapprime(s1,0,5,Site);
        REQUIRE(s1m.primeLevel() == 5);
        }

    SECTION("showm function")
        {
        REQUIRE(showm(l1) == "m=3");
        REQUIRE(showm(l2) == "m=4");
        REQUIRE(showm(s1) == "m=2");
        REQUIRE(showm(s2) == "m=2");
        auto ll = Index("ll",500);
        REQUIRE(showm(ll) == "m=500");
        }
    }
