/*
 Copyright_License {

 Copyright (C) 2016 VirtualFlightRadar-Backend
 A detailed list of copyright holders can be found in the file "AUTHORS".

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License version 3
 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 }
 */

#include <cstdint>
#include <string>
#include <boost/regex.hpp>

#include "../../src/aircraft/Aircraft.h"
#include "../../src/aircraft/AircraftProcessor.h"
#include "../../src/config/Configuration.h"
#include "../../src/aircraft/AircraftContainer.h"
#include "../../src/VFRB.h"
#include "../../src/util/Math.hpp"
#include "../framework/src/testsuite/TestSuite.hpp"
#include "../framework/src/testsuite/TestSuitesRunner.hpp"
#include "../framework/src/util/assert.hpp"
#include "../Helper.hpp"

#ifdef assert
#undef assert
#endif

using namespace util;
using namespace testsuite;
using namespace comparator;
using namespace aircraft;

void test_aircraft(TestSuitesRunner& runner)
{
    describe<aircraft::AircraftProcessor>("Process Aircrafts", runner)->test("Aircraft at,above base pos", []()
    {
        config::Configuration::mMaxDistance = INT32_MAX;
        config::Configuration::mMaxHeight = INT32_MAX;
        struct GpsPosition pos =
        {   49.0, 8.0, math::dToI(math::FEET_2_M * 3281)};
        double press = 1013.25;
        struct GpsPosition base = {49.0,8.0,0};
        AircraftContainer container;
        std::string id("BBBBBB");
        Aircraft ac(id, pos);
        ac.setFullInfo(false);
        ac.setTargetType(Aircraft::TargetType::TRANSPONDER);
        container.upsert(ac, 0);
        std::string proc = container.processAircrafts(base, press);
        boost::smatch match;

        bool matched = boost::regex_search(proc, match, helper::pflauRe);
        assert(matched, true, helper::eqb);

        assert(match.str(1), std::string("0"), helper::eqs);
        assert(match.str(2), std::string("1000"), helper::eqs);
        assert(match.str(3), std::string("0"), helper::eqs);
        assert(match.str(4), std::string("BBBBBB"), helper::eqs);

        matched = boost::regex_search(proc, match, helper::pflaaRe);
        assert(matched, true, helper::eqb);

        assert(match.str(1), std::string("0"), helper::eqs);
        assert(match.str(2), std::string("0"), helper::eqs);
        assert(match.str(3), std::string("1000"), helper::eqs);
        assert(match.str(5), std::string("BBBBBB"), helper::eqs);
        assert(match.str(9), std::string("8"), helper::eqs);
    })->test("filter distance", []()
    {
        config::Configuration::mMaxDistance = 10000;
        config::Configuration::mMaxHeight = INT32_MAX;
        struct GpsPosition pos =
        {   49.1, 8.1, math::dToI(math::FEET_2_M * 3281)};
        double press = 1013.25;
        struct GpsPosition base = {49.0,8.0,0};
        AircraftContainer container;
        std::string id("BBBBBB");
        Aircraft ac(id, pos);
        ac.setFullInfo(false);
        ac.setTargetType(Aircraft::TargetType::TRANSPONDER);
        container.upsert(ac, 0);
        assert(container.processAircrafts(base, press), std::string(""), helper::eqs);
        config::Configuration::mMaxDistance = INT32_MAX;
    });

    describeParallel<aircraft::AircraftProcessor>("process relative positions", runner)->test("Cross Equator S to N", []()
    {
        struct GpsPosition base = {-0.1,0.0,0};
        struct GpsPosition pos =
        {   0.1, 0.0, math::dToI(math::FEET_2_M * 3281)};
        std::string id("BBBBBB");
        AircraftContainer container;
        double press = 1013.25;
        Aircraft ac(id, pos);
        ac.setFullInfo(false);
        ac.setTargetType(Aircraft::TargetType::TRANSPONDER);
        container.upsert(ac, 0);
        std::string proc = container.processAircrafts(base, press);
        boost::smatch match;

        bool matched = boost::regex_search(proc, match, helper::pflauRe);
        assert(matched, true, helper::eqb);
        assert(match.str(1), std::string("0"), helper::eqs);
        assert(match.str(3), std::string("22239"), helper::eqs);

        matched = boost::regex_search(proc, match, helper::pflaaRe);
        assert(matched, true, helper::eqb);
        assert(match.str(1), std::string("22239"), helper::eqs);
        assert(match.str(2), std::string("0"), helper::eqs);
        assert(match.str(3), std::string("1000"), helper::eqs);
    })->test("Cross Equator N to S", []()
    {
        struct GpsPosition base = {0.1,0.0,0};
        AircraftContainer container;
        double press = 1013.25;
        struct GpsPosition pos =
        {   -0.1, 0.0, math::dToI(math::FEET_2_M * 3281)};
        std::string id("BBBBBB");
        Aircraft ac(id, pos);
        ac.setFullInfo(false);
        ac.setTargetType(Aircraft::TargetType::TRANSPONDER);
        container.upsert(ac, 0);
        std::string proc = container.processAircrafts(base, press);
        boost::smatch match;

        bool matched = boost::regex_search(proc, match, helper::pflauRe);
        assert(matched, true, helper::eqb);
        assert(match.str(1), std::string("180"), helper::eqs);
        assert(match.str(3), std::string("22239"), helper::eqs);

        matched = boost::regex_search(proc, match, helper::pflaaRe);
        assert(matched, true, helper::eqb);
        assert(match.str(1), std::string("-22239"), helper::eqs);
        assert(match.str(2), std::string("0"), helper::eqs);
        assert(match.str(3), std::string("1000"), helper::eqs);
    })->test("Cross Northpole", []()
    {
        struct GpsPosition base = {89.9,180.0,0};
        AircraftContainer container;
        double press = 1013.25;
        struct GpsPosition pos =
        {   89.9, 0.0, math::dToI(math::FEET_2_M * 3281)};
        std::string id("BBBBBB");
        Aircraft ac(id, pos);
        ac.setFullInfo(false);
        ac.setTargetType(Aircraft::TargetType::TRANSPONDER);
        container.upsert(ac, 0);
        std::string proc = container.processAircrafts(base, press);
        boost::smatch match;

        bool matched = boost::regex_search(proc, match, helper::pflauRe);
        assert(matched, true, helper::eqb);
        assert(match.str(1), std::string("0"), helper::eqs);
        assert(match.str(3), std::string("22239"), helper::eqs);

        matched = boost::regex_search(proc, match, helper::pflaaRe);
        assert(matched, true, helper::eqb);
        assert(match.str(1), std::string("22239"), helper::eqs);
        assert(match.str(2), std::string("0"), helper::eqs);
        assert(match.str(3), std::string("1000"), helper::eqs);
    })->test("Cross Southpole", []()
    {
        struct GpsPosition base = {-89.9,180.0,0};
        AircraftContainer container;
        double press = 1013.25;
        struct GpsPosition pos =
        {   -89.9, 0.0, math::dToI(math::FEET_2_M * 3281)};
        std::string id("BBBBBB");
        Aircraft ac(id, pos);
        ac.setFullInfo(false);
        ac.setTargetType(Aircraft::TargetType::TRANSPONDER);
        container.upsert(ac, 0);
        std::string proc = container.processAircrafts(base, press);
        boost::smatch match;

        bool matched = boost::regex_search(proc, match, helper::pflauRe);
        assert(matched, true, helper::eqb);
        assert(match.str(1), std::string("-180"), helper::eqs);
        assert(match.str(3), std::string("22239"), helper::eqs);

        matched = boost::regex_search(proc, match, helper::pflaaRe);
        assert(matched, true, helper::eqb);
        assert(match.str(1), std::string("-22239"), helper::eqs);
        assert(match.str(2), std::string("0"), helper::eqs);
        assert(match.str(3), std::string("1000"), helper::eqs);
    })->test("Cross 0-Meridian on Equator E to W", []()
    {
        struct GpsPosition base = {0.0,0.1,0};
        AircraftContainer container;
        double press = 1013.25;
        struct GpsPosition pos =
        {   0.0, -0.1, math::dToI(math::FEET_2_M * 3281)};
        std::string id("BBBBBB");
        Aircraft ac(id, pos);
        ac.setFullInfo(false);
        ac.setTargetType(Aircraft::TargetType::TRANSPONDER);
        container.upsert(ac, 0);
        std::string proc = container.processAircrafts(base, press);
        boost::smatch match;

        bool matched = boost::regex_search(proc, match, helper::pflauRe);
        assert(matched, true, helper::eqb);
        assert(match.str(1), std::string("-90"), helper::eqs);
        assert(match.str(3), std::string("22239"), helper::eqs);

        matched = boost::regex_search(proc, match, helper::pflaaRe);
        assert(matched, true, helper::eqb);
        assert(match.str(1), std::string("0"), helper::eqs);
        assert(match.str(2), std::string("-22239"), helper::eqs);
        assert(match.str(3), std::string("1000"), helper::eqs);
    })->test("Cross 0-Meridian on Equator W to E", []()
    {
        struct GpsPosition base = {0.0,-0.1,0};
        AircraftContainer container;
        double press = 1013.25;
        struct GpsPosition pos =
        {   0.0, 0.1, math::dToI(math::FEET_2_M * 3281)};
        std::string id("BBBBBB");
        Aircraft ac(id, pos);
        ac.setFullInfo(false);
        ac.setTargetType(Aircraft::TargetType::TRANSPONDER);
        container.upsert(ac, 0);
        std::string proc = container.processAircrafts(base, press);
        boost::smatch match;

        bool matched = boost::regex_search(proc, match, helper::pflauRe);
        assert(matched, true, helper::eqb);
        assert(match.str(1), std::string("90"), helper::eqs);
        assert(match.str(3), std::string("22239"), helper::eqs);

        matched = boost::regex_search(proc, match, helper::pflaaRe);
        assert(matched, true, helper::eqb);
        assert(match.str(1), std::string("0"), helper::eqs);
        assert(match.str(2), std::string("22239"), helper::eqs);
        assert(match.str(3), std::string("1000"), helper::eqs);
    })->test("Cross 0-Meridian on LAT(60) E to W", []()
    {
        struct GpsPosition base = {60.0,0.1,0};
        AircraftContainer container;
        double press = 1013.25;
        struct GpsPosition pos =
        {   60.0, -0.1, math::dToI(math::FEET_2_M * 3281)};
        std::string id("BBBBBB");
        Aircraft ac(id, pos);
        ac.setFullInfo(false);
        ac.setTargetType(Aircraft::TargetType::TRANSPONDER);
        container.upsert(ac, 0);
        std::string proc = container.processAircrafts(base, press);
        boost::smatch match;

        bool matched = boost::regex_search(proc, match, helper::pflauRe);
        assert(matched, true, helper::eqb);
        assert(match.str(1), std::string("-90"), helper::eqs);
        assert(match.str(3), std::string("11119"), helper::eqs);

        matched = boost::regex_search(proc, match, helper::pflaaRe);
        assert(matched, true, helper::eqb);
        assert(match.str(1), std::string("17"), helper::eqs);
        assert(match.str(2), std::string("-11119"), helper::eqs);
        assert(match.str(3), std::string("1000"), helper::eqs);
    })->test("Cross 0-Meridian on LAT(-60) W to E", []()
    {
        struct GpsPosition base = {-60.0,-0.1,0};
        AircraftContainer container;
        double press = 1013.25;
        struct GpsPosition pos =
        {   -60.0, 0.1, math::dToI(math::FEET_2_M * 3281)};
        std::string id("BBBBBB");
        Aircraft ac(id, pos);
        ac.setFullInfo(false);
        ac.setTargetType(Aircraft::TargetType::TRANSPONDER);
        container.upsert(ac, 0);
        std::string proc = container.processAircrafts(base, press);
        boost::smatch match;

        bool matched = boost::regex_search(proc, match, helper::pflauRe);
        assert(matched, true, helper::eqb);
        assert(match.str(1), std::string("90"), helper::eqs);
        assert(match.str(3), std::string("11119"), helper::eqs);

        matched = boost::regex_search(proc, match, helper::pflaaRe);
        assert(matched, true, helper::eqb);
        assert(match.str(1), std::string("-17"), helper::eqs);
        assert(match.str(2), std::string("11119"), helper::eqs);
        assert(match.str(3), std::string("1000"), helper::eqs);
    })->test("Cross 180-Meridian on Equator E to W", []()
    {
        struct GpsPosition base = {0.0,179.9,0};
        AircraftContainer container;
        double press = 1013.25;
        struct GpsPosition pos =
        {   0.0, -179.9, math::dToI(math::FEET_2_M * 3281)};
        std::string id("BBBBBB");
        Aircraft ac(id, pos);
        ac.setFullInfo(false);
        ac.setTargetType(Aircraft::TargetType::TRANSPONDER);
        container.upsert(ac, 0);
        std::string proc = container.processAircrafts(base, press);
        boost::smatch match;

        bool matched = boost::regex_search(proc, match, helper::pflauRe);
        assert(matched, true, helper::eqb);
        assert(match.str(1), std::string("90"), helper::eqs);
        assert(match.str(3), std::string("22239"), helper::eqs);

        matched = boost::regex_search(proc, match, helper::pflaaRe);
        assert(matched, true, helper::eqb);
        assert(match.str(1), std::string("0"), helper::eqs);
        assert(match.str(2), std::string("22239"), helper::eqs);
        assert(match.str(3), std::string("1000"), helper::eqs);
    })->test("Cross 180-Meridian on Equator W to E", []()
    {
        struct GpsPosition base = {0.0,-179.9,0};
        AircraftContainer container;
        double press = 1013.25;
        struct GpsPosition pos =
        {   0.0, 179.9, math::dToI(math::FEET_2_M * 3281)};
        std::string id("BBBBBB");
        Aircraft ac(id, pos);
        ac.setFullInfo(false);
        ac.setTargetType(Aircraft::TargetType::TRANSPONDER);
        container.upsert(ac, 0);
        std::string proc = container.processAircrafts(base, press);
        boost::smatch match;

        bool matched = boost::regex_search(proc, match, helper::pflauRe);
        assert(matched, true, helper::eqb);
        assert(match.str(1), std::string("-90"), helper::eqs);
        assert(match.str(3), std::string("22239"), helper::eqs);

        matched = boost::regex_search(proc, match, helper::pflaaRe);
        assert(matched, true, helper::eqb);
        assert(match.str(1), std::string("0"), helper::eqs);
        assert(match.str(2), std::string("-22239"), helper::eqs);
        assert(match.str(3), std::string("1000"), helper::eqs);
    })->test("North America", []()
    {
        struct GpsPosition base = {33.653124,-112.692253,0};
        AircraftContainer container;
        double press = 1013.25;
        struct GpsPosition pos =
        {   33.825808, -112.219232, math::dToI(math::FEET_2_M * 3281)};
        std::string id("BBBBBB");
        Aircraft ac(id, pos);
        ac.setFullInfo(false);
        ac.setTargetType(Aircraft::TargetType::TRANSPONDER);
        container.upsert(ac, 0);
        std::string proc = container.processAircrafts(base, press);
        boost::smatch match;

        bool matched = boost::regex_search(proc, match, helper::pflauRe);
        assert(matched, true, helper::eqb);
        assert(match.str(1), std::string("66"), helper::eqs);
        assert(match.str(3), std::string("47768"), helper::eqs);

        matched = boost::regex_search(proc, match, helper::pflaaRe);
        assert(matched, true, helper::eqb);
        assert(match.str(1), std::string("19302"), helper::eqs);
        assert(match.str(2), std::string("43695"), helper::eqs);
        assert(match.str(3), std::string("1000"), helper::eqs);
    })->test("South America", []()
    {
        struct GpsPosition base = {-34.680059,-58.818111,0};
        AircraftContainer container;
        double press = 1013.25;
        struct GpsPosition pos =
        {   -34.699833,-58.791788, math::dToI(math::FEET_2_M * 3281)};
        std::string id("BBBBBB");
        Aircraft ac(id, pos);
        ac.setFullInfo(false);
        ac.setTargetType(Aircraft::TargetType::TRANSPONDER);
        container.upsert(ac, 0);
        std::string proc = container.processAircrafts(base, press);
        boost::smatch match;

        bool matched = boost::regex_search(proc, match, helper::pflauRe);
        assert(matched, true, helper::eqb);
        assert(match.str(1), std::string("132"), helper::eqs);
        assert(match.str(3), std::string("3260"), helper::eqs);

        matched = boost::regex_search(proc, match, helper::pflaaRe);
        assert(matched, true, helper::eqb);
        assert(match.str(1), std::string("-2199"), helper::eqs);
        assert(match.str(2), std::string("2407"), helper::eqs);
        assert(match.str(3), std::string("1000"), helper::eqs);
    })->test("North Africa", []()
    {
        struct GpsPosition base = {5.392435,-5.748392,0};
        AircraftContainer container;
        double press = 1013.25;
        struct GpsPosition pos =
        {   5.386705,-5.750365, math::dToI(math::FEET_2_M * 3281)};
        std::string id("BBBBBB");
        Aircraft ac(id, pos);
        ac.setFullInfo(false);
        ac.setTargetType(Aircraft::TargetType::TRANSPONDER);
        container.upsert(ac, 0);
        std::string proc = container.processAircrafts(base, press);
        boost::smatch match;

        bool matched = boost::regex_search(proc, match, helper::pflauRe);
        assert(matched, true, helper::eqb);
        assert(match.str(1), std::string("-161"), helper::eqs);
        assert(match.str(3), std::string("674"), helper::eqs);

        matched = boost::regex_search(proc, match, helper::pflaaRe);
        assert(matched, true, helper::eqb);
        assert(match.str(1), std::string("-638"), helper::eqs);
        assert(match.str(2), std::string("-219"), helper::eqs);
        assert(match.str(3), std::string("1000"), helper::eqs);
    })->test("South Africa", []()
    {
        struct GpsPosition base = {-26.069244,15.484389,0};
        AircraftContainer container;
        double press = 1013.25;
        struct GpsPosition pos =
        {   -23.229517,15.049683 , math::dToI(math::FEET_2_M * 3281)};
        std::string id("BBBBBB");
        Aircraft ac(id, pos);
        ac.setFullInfo(false);
        ac.setTargetType(Aircraft::TargetType::TRANSPONDER);
        container.upsert(ac, 0);
        std::string proc = container.processAircrafts(base, press);
        boost::smatch match;

        bool matched = boost::regex_search(proc, match, helper::pflauRe);
        assert(matched, true, helper::eqb);
        assert(match.str(1), std::string("-8"), helper::eqs);
        assert(match.str(3), std::string("318804"), helper::eqs);

        matched = boost::regex_search(proc, match, helper::pflaaRe);
        assert(matched, true, helper::eqb);
        assert(match.str(1), std::string("315692"), helper::eqs);
        assert(match.str(2), std::string("-44437"), helper::eqs);
        assert(match.str(3), std::string("1000"), helper::eqs);
    })->test("Australia", []()
    {
        struct GpsPosition base = {-25.278208,133.366885,0};
        AircraftContainer container;
        double press = 1013.25;
        struct GpsPosition pos =
        {   -26.152199,133.376684 , math::dToI(math::FEET_2_M * 3281)};
        std::string id("BBBBBB");
        Aircraft ac(id, pos);
        ac.setFullInfo(false);
        ac.setTargetType(Aircraft::TargetType::TRANSPONDER);
        container.upsert(ac, 0);
        std::string proc = container.processAircrafts(base, press);
        boost::smatch match;

        bool matched = boost::regex_search(proc, match, helper::pflauRe);
        assert(matched, true, helper::eqb);
        assert(match.str(1), std::string("179"), helper::eqs);
        assert(match.str(3), std::string("97188"), helper::eqs);

        matched = boost::regex_search(proc, match, helper::pflaaRe);
        assert(matched, true, helper::eqb);
        assert(match.str(1), std::string("-97183"), helper::eqs);
        assert(match.str(2), std::string("978"), helper::eqs);
        assert(match.str(3), std::string("1000"), helper::eqs);
    })->test("Central Europe", []()
    {
        struct GpsPosition base = {49.719521,9.083279,0};
        AircraftContainer container;
        double press = 1013.25;
        struct GpsPosition pos =
        {   49.719445,9.087646 , math::dToI(math::FEET_2_M * 3281)};
        std::string id("BBBBBB");
        Aircraft ac(id, pos);
        ac.setFullInfo(false);
        ac.setTargetType(Aircraft::TargetType::TRANSPONDER);
        container.upsert(ac, 0);
        std::string proc = container.processAircrafts(base, press);
        boost::smatch match;

        bool matched = boost::regex_search(proc, match, helper::pflauRe);
        assert(matched, true, helper::eqb);
        assert(match.str(1), std::string("92"), helper::eqs);
        assert(match.str(3), std::string("314"), helper::eqs);

        matched = boost::regex_search(proc, match, helper::pflaaRe);
        assert(matched, true, helper::eqb);
        assert(match.str(1), std::string("-8"), helper::eqs);
        assert(match.str(2), std::string("314"), helper::eqs);
        assert(match.str(3), std::string("1000"), helper::eqs);
    })->test("Asia", []()
    {
        struct GpsPosition base = {65.900837,101.570680,0};
        AircraftContainer container;
        double press = 1013.25;
        struct GpsPosition pos =
        {   32.896360,103.855837 , math::dToI(math::FEET_2_M * 3281)};
        std::string id("BBBBBB");
        Aircraft ac(id, pos);
        ac.setFullInfo(false);
        ac.setTargetType(Aircraft::TargetType::TRANSPONDER);
        container.upsert(ac, 0);
        std::string proc = container.processAircrafts(base, press);
        boost::smatch match;

        bool matched = boost::regex_search(proc, match, helper::pflauRe);
        assert(matched, true, helper::eqb);
        assert(match.str(1), std::string("176"), helper::eqs);
        assert(match.str(3), std::string("3673118"), helper::eqs);

        matched = boost::regex_search(proc, match, helper::pflaaRe);
        assert(matched, true, helper::eqb);
        assert(match.str(1), std::string("-3666184"), helper::eqs);
        assert(match.str(2), std::string("225589"), helper::eqs);
        assert(match.str(3), std::string("1000"), helper::eqs);
    });
}
