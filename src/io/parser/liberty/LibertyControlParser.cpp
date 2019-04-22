/* Copyright 2014-2018 Rsyn
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * LibertyControlParser.cpp
 *
 *  Created on: Feb 6, 2015
 *      Author: jucemar
 */

#include <math.h>
#include <limits>

#include "LibertyControlParser.h"

LibertyControlParser::LibertyControlParser() { print = false; }

LibertyControlParser::~LibertyControlParser() {
        // TODO Auto-generated destructor stub
}

// =============================================================================
// Liberty
// =============================================================================

void LibertyControlParser::parseLiberty_LookUpTable(si2drGroupIdT &gtiming2,
                                                    si2drErrorT &err,
                                                    ISPD13::LibParserLUT &lut,
                                                    const LutType &lutType) {
        liberty_value_data *ldata = liberty_get_values_data(gtiming2);

        si2drNamesIdT names;
        si2drStringT name;
        names = si2drGroupGetNames(gtiming2, &err);
        name = si2drIterNextName(names, &err);
        si2drIterQuit(names, &err);

        if (!strcmp(name, "scalar") || lutType == LUT_TYPE_HOLD ||
            lutType == LUT_TYPE_SETUP) {
                lut.loadIndices.resize(1);
                lut.transitionIndices.resize(1);
                lut.tableVals.initialize(1, 1);

                lut.loadIndices[0] = std::numeric_limits<double>::quiet_NaN();
                lut.transitionIndices[0] =
                    std::numeric_limits<double>::quiet_NaN();

                long double el = liberty_get_element(ldata, 0, 0);
                lut.tableVals(0, 0) = Rsyn::Units::convertToInternalUnits(
                    Rsyn::MEASURE_TIME, (double)el, unitPrefixForTime);

                lut.isScalar = true;
        } else {
                auto it = lutTemplates.find(name);
                if (it == lutTemplates.end()) {
                        std::cout << "LUT Template '" << name
                                  << "' not found.\n";
                        std::exit(1);
                }  // end if

                const int lutLoadIndex = it->second.loadIndex;
                const int lutSlewIndex = it->second.slewIndex;

                const int dimLoadSize = ldata->dim_sizes[lutLoadIndex];
                const int dimSlewSize = ldata->dim_sizes[lutSlewIndex];

                lut.loadIndices.resize(dimLoadSize);
                lut.transitionIndices.resize(max(1, dimSlewSize));
                lut.tableVals.initialize(dimLoadSize, std::max(1, dimSlewSize));
                for (int i = 0; i < dimLoadSize; ++i) {
                        const double load = Rsyn::Units::convertToInternalUnits(
                            Rsyn::MEASURE_CAPACITANCE,
                            (double)ldata->index_info[lutLoadIndex][i],
                            unitPrefixForCapacitance);
                        lut.loadIndices[i] = load;
                        for (int j = 0; j < dimSlewSize; ++j) {
                                const double slew =
                                    Rsyn::Units::convertToInternalUnits(
                                        Rsyn::MEASURE_TIME,
                                        (double)ldata->index_info[lutSlewIndex]
                                                                 [j],
                                        unitPrefixForTime);
                                lut.transitionIndices[j] = slew;
                                long double el =
                                    lutLoadIndex == 0
                                        ? liberty_get_element(ldata, i, j)
                                        : liberty_get_element(ldata, j, i);
                                lut.tableVals(i, j) =
                                    Rsyn::Units::convertToInternalUnits(
                                        Rsyn::MEASURE_TIME, (double)el,
                                        unitPrefixForTime);
                        }  // end for
                }          // end else

                lut.isScalar = false;
        }  // end else
}  // end method

// -----------------------------------------------------------------------------

void LibertyControlParser::parseLiberty(const string &filename,
                                        ISPD13::LIBInfo &lib) {
        const bool debug = false;
        const bool infoSkipping = debug || false;

        si2drErrorT err;
        si2drGroupsIdT groups;
        si2drGroupIdT group;
        int count, write_out, print_stats;
        time_t startt, endt;
        /* extern liberty_parser_debug; */
        extern char *curr_file;

        count = 1;
        write_out = 0;
        print_stats = 1;

        bool hasTimingArcsWithDifferentWhenConditions = false;

        /* liberty_parser_debug =1; */

        /* handle the usual arguments */

        /* if the list of possible arguments gets any longer, use the options
         * package... */

        si2drPIInit(&err);
        if (print) printf("reading %s...\n", filename.c_str());
        time(&startt);
        curr_file = (char *)filename.c_str();

        si2drReadLibertyFile((char *)filename.c_str(), &err);
        if (err == SI2DR_INVALID_NAME) {
                printf("COULD NOT OPEN %s for parsing-- quitting...\n",
                       filename.c_str());
                exit(301);
        } else if (err == SI2DR_SYNTAX_ERROR) {
                printf("\nSyntax Errors were detected in the input file!\n\n");
                exit(401);
        }

        time(&endt);
        // printf("... Done. \nElapsed time= %ld seconds\n", endt-startt);
        groups = si2drPIGetGroups(&err);

        while (!si2drObjectIsNull((group = si2drIterNextGroup(groups, &err)),
                                  &err)) {
                if (print) printf("\n\nChecking the database...\n\n");

                time(&startt);
                si2drCheckLibertyLibrary(group, &err);
                time(&endt);

                if (err == SI2DR_NO_ERROR) {
                        if (print) printf("Passed\n\n");
                } else {
                        printf("Errors detected\n\n");
                }
                if (print)
                        printf("... Done. \nElapsed time= %ld seconds\n",
                               endt - startt);
        }
        si2drIterQuit(groups, &err);

        int cellcount = 0;

        // printf("\n\nStats:\n");
        groups = si2drPIGetGroups(&err);

        while (!si2drObjectIsNull((group = si2drIterNextGroup(groups, &err)),
                                  &err)) {
                si2drNamesIdT gnames;
                si2drStringT gname;
                si2drGroupsIdT gs2;
                si2drGroupIdT g2;

                gnames = si2drGroupGetNames(group, &err);
                gname = si2drIterNextName(gnames, &err);
                si2drIterQuit(gnames, &err);

                if (debug) printf("\n\nLibrary %s:\n", gname);

                unitPrefixForTime = Rsyn::PICO;
                unitPrefixForCapacitance = Rsyn::FEMTO;
                unitPrefixForLeakagePower = Rsyn::MILLI;

                {  // Library Attributes
                        si2drAttrsIdT groupAttributes;
                        si2drAttrIdT groupAttribute;
                        groupAttributes = si2drGroupGetAttrs(group, &err);
                        while (!si2drObjectIsNull(
                            (groupAttribute =
                                 si2drIterNextAttr(groupAttributes, &err)),
                            &err)) {
                                si2drStringT attributeName =
                                    si2drAttrGetName(groupAttribute, &err);
                                if (!strcmp(attributeName, "time_unit")) {
                                        si2drStringT value =
                                            si2drSimpleAttrGetStringValue(
                                                groupAttribute, &err);
                                        std::cout
                                            << "Time Unit  (Liberty): " << value
                                            << "\n";
                                        if (!strcmp(value, "1ps")) {
                                                unitPrefixForTime = Rsyn::PICO;
                                        } else if (!strcmp(value, "1ns")) {
                                                unitPrefixForTime = Rsyn::NANO;
                                        } else {
                                                std::cout
                                                    << "ERROR: Timing unit '"
                                                    << value
                                                    << "' invalid or not "
                                                       "supported.\n";
                                                std::exit(1);
                                        }  // end else
                                } else if (!strcmp(attributeName,
                                                   "voltage_unit")) {
                                        // TODO
                                } else if (!strcmp(attributeName,
                                                   "current_unit")) {
                                        // TODO
                                } else if (!strcmp(attributeName,
                                                   "leakage_power_unit")) {
                                        si2drStringT value =
                                            si2drSimpleAttrGetStringValue(
                                                groupAttribute, &err);
                                        cout << "Leakage Power Unit (Liberty): "
                                             << value << "\n";
                                        if (!strcmp(value, "1mW")) {
                                                unitPrefixForLeakagePower =
                                                    Rsyn::MILLI;
                                        } else if (!strcmp(value, "1uW")) {
                                                unitPrefixForLeakagePower =
                                                    Rsyn::MICRO;
                                        } else if (!strcmp(value, "1nW")) {
                                                unitPrefixForLeakagePower =
                                                    Rsyn::NANO;
                                        } else {
                                                std::cout << "ERROR: Leakage "
                                                             "power unit '"
                                                          << value
                                                          << "' invalid or not "
                                                             "supported.\n";
                                                std::exit(1);
                                        }
                                } else if (!strcmp(attributeName,
                                                   "capacitive_load_unit")) {
                                        si2drValueTypeT vtype;
                                        si2drInt32T intgr;
                                        si2drFloat64T float64;
                                        si2drStringT str;
                                        si2drBooleanT boolval;
                                        si2drExprT *exprp;
                                        si2drValuesIdT vals =
                                            si2drComplexAttrGetValues(
                                                groupAttribute, &err);

                                        int multiplier = 1;
                                        std::string unit;

                                        for (si2drIterNextComplexValue(
                                                 vals, &vtype, &intgr, &float64,
                                                 &str, &boolval, &exprp, &err);
                                             vtype != SI2DR_UNDEFINED_VALUETYPE;
                                             si2drIterNextComplexValue(
                                                 vals, &vtype, &intgr, &float64,
                                                 &str, &boolval, &exprp,
                                                 &err)) {
                                                switch (vtype) {
                                                        case SI2DR_STRING:
                                                                unit = str;
                                                                break;

                                                        case SI2DR_FLOAT64:
                                                                break;

                                                        case SI2DR_INT32:
                                                                multiplier =
                                                                    intgr;
                                                                break;

                                                        case SI2DR_BOOLEAN:
                                                                break;

                                                        default:
                                                                break;
                                                }  // end switch
                                        }          // end for

                                        if (multiplier != 1) {
                                                std::cout
                                                    << "ERROR: Multiplier "
                                                       "value in "
                                                       "'capacitive_load_unit' "
                                                       "not set or not. It "
                                                       "should be 1.\n";
                                                std::exit(1);
                                        }  // end if

                                        if (unit == "ff") {
                                                unitPrefixForCapacitance =
                                                    Rsyn::FEMTO;
                                        } else if (unit == "pf") {
                                                unitPrefixForCapacitance =
                                                    Rsyn::PICO;
                                        } else if (unit == "") {
                                                std::cout << "INFO: Assuming "
                                                             "'capacitive_load_"
                                                             "unit' as 1fF.\n";
                                        } else {
                                                std::cout
                                                    << "ERROR: Invalid or "
                                                       "unsupported value for "
                                                       "'capacitive_load_unit' "
                                                       "("
                                                    << unit << ").\n";
                                                std::exit(1);
                                        }  // end else

                                        si2drIterQuit(vals, &err);

                                } else if (!strcmp(attributeName,
                                                   "pulling_resistance_unit")) {
                                        // TODO
                                } else {
                                        if (infoSkipping) {
                                                std::cout << "INFO: Skipping "
                                                             "attribute '"
                                                          << attributeName
                                                          << "' of pin...\n";
                                        }  // end if
                                }          // end else
                        }                  // end while
                        si2drIterQuit(groupAttributes, &err);
                }  // end block

                // Added by Henrique -  22/02/2017
                // getting input pin default_max_transition attribute
                {
                        si2drAttrsIdT groupAttributes;
                        si2drAttrIdT groupAttribute;
                        groupAttributes = si2drGroupGetAttrs(group, &err);

                        while (!si2drObjectIsNull(
                            (groupAttribute =
                                 si2drIterNextAttr(groupAttributes, &err)),
                            &err)) {
                                si2drStringT attributeName =
                                    si2drAttrGetName(groupAttribute, &err);
                                if (!strcmp(attributeName,
                                            "default_max_transition")) {
                                        double defaultMaxTransition =
                                            si2drSimpleAttrGetFloat64Value(
                                                groupAttribute, &err);
                                        lib.default_max_transition =
                                            defaultMaxTransition;
                                }  // end if
                        }          // end while
                        si2drIterQuit(groupAttributes, &err);
                }  // end block

                gs2 = si2drGroupGetGroups(group, &err);

                while (!si2drObjectIsNull((g2 = si2drIterNextGroup(gs2, &err)),
                                          &err)) {
                        si2drNamesIdT gns;
                        si2drStringT gn;
                        si2drStringT gt = si2drGroupGetGroupType(g2, &err);

                        if (!strcmp(gt, "cell")) {
                                gns = si2drGroupGetNames(g2, &err);
                                gn = si2drIterNextName(gns, &err);

                                cellcount++;
                                if (debug) printf("Cell: %s\n", gn);
                                si2drIterQuit(gns, &err);

                                ISPD13::LibParserCellInfo cell;
                                cell.name = gn;
                                cell.isSequential = false;
                                cell.dontTouch = false;
                                cell.isTieLow = false;
                                cell.isTieHigh = false;

                                // Getting cell attributes
                                // Added by Henrique - 08/02/2017
                                si2drAttrsIdT groupAttributes;
                                si2drAttrIdT groupAttribute;
                                groupAttributes = si2drGroupGetAttrs(g2, &err);
                                while (!si2drObjectIsNull(
                                    (groupAttribute = si2drIterNextAttr(
                                         groupAttributes, &err)),
                                    &err)) {
                                        si2drStringT attributeName =
                                            si2drAttrGetName(groupAttribute,
                                                             &err);
                                        if (!strcmp(attributeName,
                                                    "cell_leakage_power")) {
                                                double leakage =
                                                    si2drSimpleAttrGetFloat64Value(
                                                        groupAttribute, &err);
                                                cell.leakagePower = Rsyn::Units::
                                                    convertToInternalUnits(
                                                        Rsyn::MEASURE_POWER,
                                                        leakage,
                                                        unitPrefixForLeakagePower);
                                        }  // end if
                                }          // end while
                                si2drIterQuit(groupAttributes, &err);

                                si2drGroupsIdT gpins;
                                si2drGroupIdT gpin;
                                gpins = si2drGroupGetGroups(g2, &err);

                                while (!si2drObjectIsNull(
                                    (gpin = si2drIterNextGroup(gpins, &err)),
                                    &err)) {
                                        si2drGroupsIdT gtimings;
                                        si2drGroupIdT gtiming;
                                        gtimings =
                                            si2drGroupGetGroups(gpin, &err);

                                        si2drStringT gts2 =
                                            si2drGroupGetGroupType(gpin, &err);
                                        if (debug)
                                                std::cout << "Group: " << gts2
                                                          << std::endl;

                                        if (!strcmp(gts2, "pin")) {
                                                cell.pins.push_back(
                                                    ISPD13::LibParserPinInfo());
                                                ISPD13::LibParserPinInfo &pin =
                                                    cell.pins.back();

                                                si2drNamesIdT pnames;
                                                si2drStringT pname;
                                                pnames = si2drGroupGetNames(
                                                    gpin, &err);
                                                pname = si2drIterNextName(
                                                    pnames, &err);
                                                if (debug)
                                                        std::cout
                                                            << "Pin: "
                                                            << (pname
                                                                    ? pname
                                                                    : std::string(
                                                                          "<nul"
                                                                          "l>"))
                                                            << endl;
                                                si2drIterQuit(pnames, &err);

                                                pin.name = pname;

                                                si2drAttrsIdT gpins2;
                                                si2drAttrIdT gpin2;
                                                gpins2 = si2drGroupGetAttrs(
                                                    gpin, &err);
                                                while (!si2drObjectIsNull(
                                                    (gpin2 = si2drIterNextAttr(
                                                         gpins2, &err)),
                                                    &err)) {
                                                        si2drStringT gts7 =
                                                            si2drAttrGetName(
                                                                gpin2, &err);
                                                        if (!strcmp(
                                                                gts7,
                                                                "direction")) {
                                                                si2drStringT gts8 =
                                                                    si2drSimpleAttrGetStringValue(
                                                                        gpin2,
                                                                        &err);
                                                                if (!strcmp(
                                                                        gts8,
                                                                        "inpu"
                                                                        "t"))
                                                                        pin.isInput =
                                                                            true;
                                                                else if (
                                                                    !strcmp(
                                                                        gts8,
                                                                        "outpu"
                                                                        "t"))
                                                                        pin.isInput =
                                                                            false;
                                                        } else if (!strcmp(
                                                                       gts7,
                                                                       "capacit"
                                                                       "anc"
                                                                       "e")) {
                                                                pin.capacitance =
                                                                    si2drSimpleAttrGetFloat64Value(
                                                                        gpin2,
                                                                        &err);
                                                        } else if (!strcmp(
                                                                       gts7,
                                                                       "max_"
                                                                       "capacit"
                                                                       "anc"
                                                                       "e")) {
                                                                pin.maxCapacitance =
                                                                    si2drSimpleAttrGetFloat64Value(
                                                                        gpin2,
                                                                        &err);
                                                        } else if (!strcmp(
                                                                       gts7,
                                                                       "max_"
                                                                       "transit"
                                                                       "ion")) {
                                                                pin.maxTransition =
                                                                    si2drSimpleAttrGetFloat64Value(
                                                                        gpin2,
                                                                        &err);
                                                        } else if (
                                                            !strcmp(gts7,
                                                                    "clock")) {
                                                                pin.isClock =
                                                                    true;
                                                                cell.isSequential =
                                                                    true;
                                                        } else if (
                                                            !strcmp(
                                                                gts7,
                                                                "function")) {
                                                                // ignore data
                                                        } else if (!strcmp(
                                                                       gts7,
                                                                       "min_"
                                                                       "capacit"
                                                                       "anc"
                                                                       "e")) {
                                                                // ignore data
                                                        } else if (!strcmp(
                                                                       gts7,
                                                                       "nextsta"
                                                                       "te_"
                                                                       "typ"
                                                                       "e")) {
                                                                // ignore data
                                                        } else if (!strcmp(
                                                                       gts7,
                                                                       "driver_"
                                                                       "typ"
                                                                       "e")) {
                                                                if (!strcmp(
                                                                        si2drSimpleAttrGetStringValue(
                                                                            gpin2,
                                                                            &err),
                                                                        "pull_"
                                                                        "up"))
                                                                        cell.isTieHigh =
                                                                            true;
                                                                if (!strcmp(
                                                                        si2drSimpleAttrGetStringValue(
                                                                            gpin2,
                                                                            &err),
                                                                        "pull_"
                                                                        "down"))
                                                                        cell.isTieLow =
                                                                            true;
                                                        } else {
                                                                if (infoSkipping)
                                                                        std::cout
                                                                            << "INFO: Skipping attribute '"
                                                                            << gts7
                                                                            << "' of pin...\n";
                                                        }
                                                }
                                                si2drIterQuit(gpins2, &err);

                                                // Hack to filter out multiple
                                                // descriptions for the
                                                // same timing arc with
                                                // different when conditions.
                                                // For now we just use the first
                                                // one and ignore the
                                                // remaining. Guilherme Flach -
                                                // 2016/08/21
                                                std::set<std::tuple<
                                                    std::string, std::string>>
                                                    hack;

                                                while (!si2drObjectIsNull(
                                                    (gtiming =
                                                         si2drIterNextGroup(
                                                             gtimings, &err)),
                                                    &err)) {
                                                        si2drStringT gts3 =
                                                            si2drGroupGetGroupType(
                                                                gtiming, &err);
                                                        // cout << " pin group:
                                                        // " << gts3 << endl;
                                                        si2drGroupsIdT
                                                            gtimings2;
                                                        si2drGroupIdT gtiming2;

                                                        if (!strcmp(gts3,
                                                                    "timing")) {
                                                                string
                                                                    toPin =
                                                                        pin.name,
                                                                    fromPin,
                                                                    timingSense,
                                                                    timingType;
                                                                bool setup =
                                                                    false;
                                                                bool
                                                                    createTimingArc =
                                                                        false;
                                                                si2drAttrsIdT
                                                                    gtimings3;
                                                                si2drAttrIdT
                                                                    gtiming3;
                                                                gtimings3 =
                                                                    si2drGroupGetAttrs(
                                                                        gtiming,
                                                                        &err);
                                                                while (!si2drObjectIsNull(
                                                                    (gtiming3 = si2drIterNextAttr(
                                                                         gtimings3,
                                                                         &err)),
                                                                    &err)) {
                                                                        si2drStringT
                                                                            gts5 = si2drAttrGetName(
                                                                                gtiming3,
                                                                                &err);
                                                                        si2drStringT
                                                                            gts6 = si2drSimpleAttrGetStringValue(
                                                                                gtiming3,
                                                                                &err);
                                                                        if (debug)
                                                                                std::cout
                                                                                    << "attribute: "
                                                                                    << gts5
                                                                                    << "   value "
                                                                                    << gts6
                                                                                    << endl;

                                                                        if (!strcmp(
                                                                                gts5,
                                                                                "timing_sense")) {
                                                                                timingSense =
                                                                                    gts6;
                                                                        } else if (
                                                                            !strcmp(
                                                                                gts5,
                                                                                "related_pin")) {
                                                                                fromPin =
                                                                                    gts6;
                                                                                pin.related =
                                                                                    gts6;
                                                                        } else if (
                                                                            !strcmp(
                                                                                gts5,
                                                                                "timing_type")) {
                                                                                timingType =
                                                                                    gts6;
                                                                                if (!strcmp(
                                                                                        gts6,
                                                                                        "rising_edge"))
                                                                                        pin.risingEdge =
                                                                                            true;
                                                                                if (!strcmp(
                                                                                        gts6,
                                                                                        "falling_edge"))
                                                                                        pin.risingEdge =
                                                                                            false;
                                                                                if (!strcmp(
                                                                                        gts6,
                                                                                        "setup_rising"))
                                                                                        setup =
                                                                                            true;
                                                                                if (!strcmp(
                                                                                        gts6,
                                                                                        "hold_rising"))
                                                                                        setup =
                                                                                            false;
                                                                        } else {
                                                                                if (infoSkipping)
                                                                                        std::cout
                                                                                            << "INFO: Skipping attribute '"
                                                                                            << gts5
                                                                                            << "' of timing group...\n";
                                                                        }  // end
                                                                        // else
                                                                }  // end while
                                                                   // (timing
                                                                   // groups)
                                                                si2drIterQuit(
                                                                    gtimings3,
                                                                    &err);

                                                                const std::string
                                                                    arcName =
                                                                        fromPin +
                                                                        "->" +
                                                                        toPin;
                                                                if (hack.count(std::make_tuple(
                                                                        arcName,
                                                                        timingType)) ==
                                                                    0) {
                                                                        hack.insert(
                                                                            std::make_tuple(
                                                                                arcName,
                                                                                timingType));

                                                                        ISPD13::LibParserTimingInfo
                                                                            arc;  // = cell.timingArcs.back();
                                                                        arc.toPin =
                                                                            toPin;
                                                                        arc.fromPin =
                                                                            fromPin;
                                                                        arc.timingSense =
                                                                            timingSense;
                                                                        arc.timingType =
                                                                            timingType;

                                                                        pin.isTimingEndpoint =
                                                                            false;
                                                                        gtimings2 = si2drGroupGetGroups(
                                                                            gtiming,
                                                                            &err);
                                                                        while (!si2drObjectIsNull(
                                                                            (gtiming2 = si2drIterNextGroup(
                                                                                 gtimings2,
                                                                                 &err)),
                                                                            &err)) {
                                                                                si2drStringT
                                                                                    gts4 = si2drGroupGetGroupType(
                                                                                        gtiming2,
                                                                                        &err);

                                                                                if (!strcmp(
                                                                                        gts4,
                                                                                        "fall_constraint")) {
                                                                                        if (setup) {
                                                                                                parseLiberty_LookUpTable(
                                                                                                    gtiming2,
                                                                                                    err,
                                                                                                    pin.fallSetup,
                                                                                                    LUT_TYPE_SETUP);
                                                                                        } else {
                                                                                                parseLiberty_LookUpTable(
                                                                                                    gtiming2,
                                                                                                    err,
                                                                                                    pin.fallHold,
                                                                                                    LUT_TYPE_HOLD);
                                                                                        }
                                                                                        pin.isTimingEndpoint =
                                                                                            true;

                                                                                } else if (
                                                                                    !strcmp(
                                                                                        gts4,
                                                                                        "rise_constraint")) {
                                                                                        if (setup) {
                                                                                                parseLiberty_LookUpTable(
                                                                                                    gtiming2,
                                                                                                    err,
                                                                                                    pin.riseSetup,
                                                                                                    LUT_TYPE_SETUP);
                                                                                        } else {
                                                                                                parseLiberty_LookUpTable(
                                                                                                    gtiming2,
                                                                                                    err,
                                                                                                    pin.riseHold,
                                                                                                    LUT_TYPE_HOLD);
                                                                                        }
                                                                                        pin.isTimingEndpoint =
                                                                                            true;

                                                                                } else if (
                                                                                    !strcmp(
                                                                                        gts4,
                                                                                        "cell_fall")) {
                                                                                        parseLiberty_LookUpTable(
                                                                                            gtiming2,
                                                                                            err,
                                                                                            arc.fallDelay,
                                                                                            LUT_TYPE_DELAY);
                                                                                        createTimingArc =
                                                                                            true;

                                                                                } else if (
                                                                                    !strcmp(
                                                                                        gts4,
                                                                                        "cell_rise")) {
                                                                                        parseLiberty_LookUpTable(
                                                                                            gtiming2,
                                                                                            err,
                                                                                            arc.riseDelay,
                                                                                            LUT_TYPE_DELAY);
                                                                                        createTimingArc =
                                                                                            true;

                                                                                } else if (
                                                                                    !strcmp(
                                                                                        gts4,
                                                                                        "fall_transition")) {
                                                                                        parseLiberty_LookUpTable(
                                                                                            gtiming2,
                                                                                            err,
                                                                                            arc.fallTransition,
                                                                                            LUT_TYPE_SLEW);
                                                                                        createTimingArc =
                                                                                            true;

                                                                                } else if (
                                                                                    !strcmp(
                                                                                        gts4,
                                                                                        "rise_transition")) {
                                                                                        parseLiberty_LookUpTable(
                                                                                            gtiming2,
                                                                                            err,
                                                                                            arc.riseTransition,
                                                                                            LUT_TYPE_SLEW);
                                                                                        createTimingArc =
                                                                                            true;

                                                                                } else {
                                                                                        if (infoSkipping)
                                                                                                std::cout
                                                                                                    << "INFO: Skipping group '"
                                                                                                    << gts4
                                                                                                    << "' of timing group...\n";
                                                                                }  // end if
                                                                        }
                                                                        si2drIterQuit(
                                                                            gtimings2,
                                                                            &err);
                                                                        if (createTimingArc)
                                                                                cell.timingArcs
                                                                                    .push_back(
                                                                                        arc);
                                                                } else {
                                                                        hasTimingArcsWithDifferentWhenConditions =
                                                                            true;
                                                                }  // end else
                                                        } else {
                                                                if (infoSkipping)
                                                                        std::cout
                                                                            << "INFO: Skipping group '"
                                                                            << gts3
                                                                            << "' of pin...\n";
                                                        }  // end if
                                                }  // end while (pin groups)
                                        } else {
                                                if (infoSkipping)
                                                        std::cout
                                                            << "INFO: Skipping "
                                                               "group '"
                                                            << gts2
                                                            << "' of cell...\n";
                                        }  // end else

                                        si2drIterQuit(gtimings, &err);

                                }  // end while (cell groups)
                                si2drIterQuit(gpins, &err);
                                lib.libCells.push_back(cell);
                        } else if (!strcmp(gt, "lu_table_template")) {
                                si2drNamesIdT gns =
                                    si2drGroupGetNames(g2, &err);
                                si2drStringT gn = si2drIterNextName(gns, &err);

                                LookUpTableTemplate &lutTemplate =
                                    lutTemplates[gn];

                                si2drIterQuit(gns, &err);

                                si2drAttrIdT attr;
                                si2drAttrsIdT attrs =
                                    si2drGroupGetAttrs(g2, &err);
                                while (!si2drObjectIsNull(
                                    (attr = si2drIterNextAttr(attrs, &err)),
                                    &err)) {
                                        si2drStringT key =
                                            si2drAttrGetName(attr, &err);
                                        si2drStringT value =
                                            si2drSimpleAttrGetStringValue(attr,
                                                                          &err);

                                        if (!strcmp(key, "variable_1")) {
                                                lutTemplate.var1 =
                                                    getLutVariableTypeFromString(
                                                        value);

                                                if (!strcmp(value,
                                                            "input_net_"
                                                            "transition")) {
                                                        lutTemplate.slewIndex =
                                                            0;
                                                } else if (!strcmp(value,
                                                                   "total_"
                                                                   "output_net_"
                                                                   "capacitanc"
                                                                   "e")) {
                                                        lutTemplate.loadIndex =
                                                            0;
                                                }  // end if
                                        } else if (!strcmp(key, "variable_2")) {
                                                lutTemplate.var2 =
                                                    getLutVariableTypeFromString(
                                                        value);

                                                if (!strcmp(value,
                                                            "input_net_"
                                                            "transition")) {
                                                        lutTemplate.slewIndex =
                                                            1;
                                                } else if (!strcmp(value,
                                                                   "total_"
                                                                   "output_net_"
                                                                   "capacitanc"
                                                                   "e")) {
                                                        lutTemplate.loadIndex =
                                                            1;
                                                }  // end if
                                        }          // end if
                                }                  // end while
                                si2drIterQuit(attrs, &err);
                        }  // end else-if

                }  // end while (library groups)
                si2drIterQuit(gs2, &err);
                if (print) printf("\tcells: %d\n", cellcount);
        }
        si2drIterQuit(groups, &err);

        if (print) printf("Quitting..\n");
        si2drPIQuit(&err);

        if (hasTimingArcsWithDifferentWhenConditions) {
                std::cout << "WARNING: At least one timing arc detected with "
                             "multiple "
                             "when conditions. Currently we use only the first "
                             "one and ignore "
                             "the rest.\n";
        }  // end if

}  // end method
