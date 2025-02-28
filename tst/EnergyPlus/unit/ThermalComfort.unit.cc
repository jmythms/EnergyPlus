// EnergyPlus, Copyright (c) 1996-2021, The Board of Trustees of the University of Illinois,
// The Regents of the University of California, through Lawrence Berkeley National Laboratory
// (subject to receipt of any required approvals from the U.S. Dept. of Energy), Oak Ridge
// National Laboratory, managed by UT-Battelle, Alliance for Sustainable Energy, LLC, and other
// contributors. All rights reserved.
//
// NOTICE: This Software was developed under funding from the U.S. Department of Energy and the
// U.S. Government consequently retains certain rights. As such, the U.S. Government has been
// granted for itself and others acting on its behalf a paid-up, nonexclusive, irrevocable,
// worldwide license in the Software to reproduce, distribute copies to the public, prepare
// derivative works, and perform publicly and display publicly, and to permit others to do so.
//
// Redistribution and use in source and binary forms, with or without modification, are permitted
// provided that the following conditions are met:
//
// (1) Redistributions of source code must retain the above copyright notice, this list of
//     conditions and the following disclaimer.
//
// (2) Redistributions in binary form must reproduce the above copyright notice, this list of
//     conditions and the following disclaimer in the documentation and/or other materials
//     provided with the distribution.
//
// (3) Neither the name of the University of California, Lawrence Berkeley National Laboratory,
//     the University of Illinois, U.S. Dept. of Energy nor the names of its contributors may be
//     used to endorse or promote products derived from this software without specific prior
//     written permission.
//
// (4) Use of EnergyPlus(TM) Name. If Licensee (i) distributes the software in stand-alone form
//     without changes from the version obtained under this License, or (ii) Licensee makes a
//     reference solely to the software portion of its product, Licensee must refer to the
//     software as "EnergyPlus version X" software, where "X" is the version number Licensee
//     obtained under this License and may not use a different name for the software. Except as
//     specifically required in this Section (4), Licensee shall not use in a company name, a
//     product name, in advertising, publicity, or other promotional activities any name, trade
//     name, trademark, logo, or other designation of "EnergyPlus", "E+", "e+" or confusingly
//     similar designation, without the U.S. Department of Energy's prior written consent.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

// EnergyPlus::ThermalComfort Unit Tests

// Google Test Headers
#include <gtest/gtest.h>

// EnergyPlus Headers
#include <EnergyPlus/Construction.hh>
#include <EnergyPlus/Data/EnergyPlusData.hh>
#include <EnergyPlus/DataEnvironment.hh>
#include <EnergyPlus/DataGlobals.hh>
#include <EnergyPlus/DataHVACGlobals.hh>
#include <EnergyPlus/DataHeatBalFanSys.hh>
#include <EnergyPlus/DataHeatBalSurface.hh>
#include <EnergyPlus/DataHeatBalance.hh>
#include <EnergyPlus/DataRoomAirModel.hh>
#include <EnergyPlus/DataSurfaces.hh>
#include <EnergyPlus/DataZoneEnergyDemands.hh>
#include <EnergyPlus/IOFiles.hh>
#include <EnergyPlus/SimulationManager.hh>
#include <EnergyPlus/ThermalComfort.hh>
#include <EnergyPlus/ZoneTempPredictorCorrector.hh>
#include <EnergyPlus/ScheduleManager.hh>
#include <EnergyPlus/Psychrometrics.hh>
#include "Fixtures/EnergyPlusFixture.hh"

using namespace EnergyPlus;
using namespace EnergyPlus::ThermalComfort;
using namespace EnergyPlus::DataEnvironment;
using namespace EnergyPlus::DataHeatBalance;
using namespace EnergyPlus::DataHVACGlobals;
using namespace EnergyPlus::DataRoomAirModel;
using namespace EnergyPlus::DataHeatBalFanSys;
using namespace EnergyPlus::DataSurfaces;
using namespace EnergyPlus::DataHeatBalSurface;
//using namespace EnergyPlus::ScheduleManager;
using namespace SimulationManager;
using namespace ObjexxFCL;

TEST_F(EnergyPlusFixture, ThermalComfort_CalcIfSetPointMetTest1)
{
    state->dataGlobal->NumOfZones = 1;
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand.allocate(state->dataGlobal->NumOfZones);
    state->dataThermalComforts->ThermalComfortSetPoint.allocate(state->dataGlobal->NumOfZones);
    state->dataHeatBalFanSys->TempControlType.allocate(1);
    state->dataRoomAirMod->AirModel.allocate(state->dataGlobal->NumOfZones);
    state->dataRoomAirMod->AirModel(1).AirModelType = DataRoomAirModel::RoomAirModel::Mixing;
    state->dataHeatBalFanSys->ZTAV.allocate(state->dataGlobal->NumOfZones);
    state->dataHeatBalFanSys->ZoneThermostatSetPointLo.allocate(state->dataGlobal->NumOfZones);
    state->dataHeatBalFanSys->ZoneThermostatSetPointHi.allocate(state->dataGlobal->NumOfZones);
    state->dataGlobal->TimeStepZone = 0.25;
    state->dataThermalComforts->ThermalComfortInASH55.allocate(state->dataGlobal->NumOfZones);
    state->dataThermalComforts->ThermalComfortInASH55(1).ZoneIsOccupied = true;
    state->dataHeatBal->Zone.allocate(state->dataGlobal->NumOfZones);

    // SingleHeatingSetPoint thermostat

    state->dataHeatBalFanSys->TempControlType(1) = SingleHeatingSetPoint;

    // heating
    state->dataHeatBalFanSys->ZTAV(1) = 21.1;                                     // 70F
    state->dataHeatBalFanSys->ZoneThermostatSetPointLo(1) = 22.2;                 // 72F
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).TotalOutputRequired = 500.0; // must be greater than zero
    CalcIfSetPointMet(*state);
    EXPECT_EQ(state->dataGlobal->TimeStepZone, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetHeating);
    EXPECT_EQ(state->dataGlobal->TimeStepZone, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetHeatingOccupied);
    EXPECT_EQ(0., state->dataThermalComforts->ThermalComfortSetPoint(1).notMetCooling);
    EXPECT_EQ(0., state->dataThermalComforts->ThermalComfortSetPoint(1).notMetCoolingOccupied);

    // cooling
    state->dataHeatBalFanSys->ZTAV(1) = 25.0;                                      // 77F
    state->dataHeatBalFanSys->ZoneThermostatSetPointHi(1) = 23.9;                  // 75F
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).TotalOutputRequired = -500.0; // must be less than zero
    CalcIfSetPointMet(*state);
    EXPECT_EQ(0, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetHeating);
    EXPECT_EQ(0, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetHeatingOccupied);
    EXPECT_EQ(0., state->dataThermalComforts->ThermalComfortSetPoint(1).notMetCooling);
    EXPECT_EQ(0., state->dataThermalComforts->ThermalComfortSetPoint(1).notMetCoolingOccupied);

    // SingleCoolingSetPoint thermostat

    state->dataHeatBalFanSys->TempControlType(1) = SingleCoolingSetPoint;

    // heating
    state->dataHeatBalFanSys->ZTAV(1) = 21.1;                                     // 70F
    state->dataHeatBalFanSys->ZoneThermostatSetPointLo(1) = 22.2;                 // 72F
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).TotalOutputRequired = 500.0; // must be greater than zero
    CalcIfSetPointMet(*state);
    EXPECT_EQ(0, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetHeating);
    EXPECT_EQ(0, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetHeatingOccupied);
    EXPECT_EQ(0., state->dataThermalComforts->ThermalComfortSetPoint(1).notMetCooling);
    EXPECT_EQ(0., state->dataThermalComforts->ThermalComfortSetPoint(1).notMetCoolingOccupied);

    // cooling
    state->dataHeatBalFanSys->ZTAV(1) = 25.0;                                      // 77F
    state->dataHeatBalFanSys->ZoneThermostatSetPointHi(1) = 23.9;                  // 75F
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).TotalOutputRequired = -500.0; // must be less than zero
    CalcIfSetPointMet(*state);
    EXPECT_EQ(0, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetHeating);
    EXPECT_EQ(0, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetHeatingOccupied);
    EXPECT_EQ(state->dataGlobal->TimeStepZone, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetCooling);
    EXPECT_EQ(state->dataGlobal->TimeStepZone, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetCoolingOccupied);

    // SingleHeatCoolSetPoint thermostat

    state->dataHeatBalFanSys->TempControlType(1) = SingleHeatCoolSetPoint;

    // heating
    state->dataHeatBalFanSys->ZTAV(1) = 21.1;                                     // 70F
    state->dataHeatBalFanSys->ZoneThermostatSetPointLo(1) = 22.2;                 // 72F
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).TotalOutputRequired = 500.0; // must be greater than zero
    CalcIfSetPointMet(*state);
    EXPECT_EQ(state->dataGlobal->TimeStepZone, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetHeating);
    EXPECT_EQ(state->dataGlobal->TimeStepZone, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetHeatingOccupied);
    EXPECT_EQ(0., state->dataThermalComforts->ThermalComfortSetPoint(1).notMetCooling);
    EXPECT_EQ(0., state->dataThermalComforts->ThermalComfortSetPoint(1).notMetCoolingOccupied);

    // cooling
    state->dataHeatBalFanSys->ZTAV(1) = 25.0;                                      // 77F
    state->dataHeatBalFanSys->ZoneThermostatSetPointHi(1) = 23.9;                  // 75F
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).TotalOutputRequired = -500.0; // must be less than zero
    CalcIfSetPointMet(*state);
    EXPECT_EQ(0, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetHeating);
    EXPECT_EQ(0, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetHeatingOccupied);
    EXPECT_EQ(state->dataGlobal->TimeStepZone, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetCooling);
    EXPECT_EQ(state->dataGlobal->TimeStepZone, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetCoolingOccupied);

    // DualSetPointWithDeadBand thermostat

    state->dataHeatBalFanSys->TempControlType(1) = DualSetPointWithDeadBand;

    // heating
    state->dataHeatBalFanSys->ZTAV(1) = 21.1;                                     // 70F
    state->dataHeatBalFanSys->ZoneThermostatSetPointLo(1) = 22.2;                 // 72F
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).TotalOutputRequired = 500.0; // must be greater than zero
    CalcIfSetPointMet(*state);
    EXPECT_EQ(state->dataGlobal->TimeStepZone, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetHeating);
    EXPECT_EQ(state->dataGlobal->TimeStepZone, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetHeatingOccupied);
    EXPECT_EQ(0., state->dataThermalComforts->ThermalComfortSetPoint(1).notMetCooling);
    EXPECT_EQ(0., state->dataThermalComforts->ThermalComfortSetPoint(1).notMetCoolingOccupied);

    // cooling
    state->dataHeatBalFanSys->ZTAV(1) = 25.0;                                      // 77F
    state->dataHeatBalFanSys->ZoneThermostatSetPointHi(1) = 23.9;                  // 75F
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).TotalOutputRequired = -500.0; // must be less than zero
    CalcIfSetPointMet(*state);
    EXPECT_EQ(0, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetHeating);
    EXPECT_EQ(0, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetHeatingOccupied);
    EXPECT_EQ(state->dataGlobal->TimeStepZone, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetCooling);
    EXPECT_EQ(state->dataGlobal->TimeStepZone, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetCoolingOccupied);
}

TEST_F(EnergyPlusFixture, ThermalComfort_CalcThermalComfortFanger)
{

    std::string const idf_objects = delimited_string({
        "  People,                                                                 ",
        "    Space People,   !- Name                                      ",
        "    Space,     !- Zone or ZoneList Name                     ",
        "    PeopleSchedule,          !- Number of People Schedule Name            ",
        "    People,                  !- Number of People Calculation Method       ",
        "    5.0,                     !- Number of People                          ",
        "    ,                        !- People per Zone Floor Area {person/m2}    ",
        "    ,                        !- Zone Floor Area per Person {m2/person}    ",
        "    0.3,                     !- Fraction Radiant                          ",
        "    AUTOCALCULATE,           !- Sensible Heat Fraction                    ",
        "    Activity Schedule,       !- Activity Level Schedule Name              ",
        "    ,                        !- Carbon Dioxide Generation Rate {m3/s-W}   ",
        "    Yes,                     !- Enable ASHRAE 55 Comfort Warnings         ",
        "    ZoneAveraged,            !- Mean Radiant Temperature Calculation Type ",
        "    ,                        !- Surface Name/Angle Factor List Name       ",
        "    Work efficiency,         !- Work Efficiency Schedule Name             ",
        "    ClothingInsulationSchedule,  !- Clothing Insulation Calculation Method",
        "    ,                        !- Clothing Insulation Calculation Method Sch",
        "    Clothing Schedule,       !- Clothing Insulation Schedule Name         ",
        "    AirVelocitySchedule,     !- Air Velocity Schedule Name                ",
        "    Fanger;                  !- Thermal Comfort Model 1 Type              ",
        "                                                                          ",
        "  Schedule:Compact,                                                       ",
        "    PeopleSchedule,          !- Name                                      ",
        "    Any Number,              !- Schedule Type Limits Name                 ",
        "    Through: 12/30,          !- Field 1                                   ",
        "    For: AllDays,            !- Field 2                                   ",
        "    Until: 24:00,1.0,        !- Field 3                                   ",
        "    Through: 12/31,          !- Field 1                                   ",
        "    For: AllDays,            !- Field 2                                   ",
        "    Until: 24:00,0.3;        !- Field 3                                   ",
        "                                                                          ",
        "  Schedule:Compact,                                                       ",
        "    Activity Schedule,       !- Name                                      ",
        "    Any Number,              !- Schedule Type Limits Name                 ",
        "    Through: 12/31,          !- Field 1                                   ",
        "    For: AllDays,            !- Field 2                                   ",
        "    Until: 24:00,70;         !- Field 3                                   ",
        "                                                                          ",
        "  Schedule:Compact,                                                       ",
        "    Clothing Schedule,       !- Name                                      ",
        "    Any Number,              !- Schedule Type Limits Name                 ",
        "    Through: 12/31,          !- Field 9                                   ",
        "    For: AllDays,            !- Field 10                                  ",
        "    Until: 24:00,1.0;         !- Field 11                                 ",
        "                                                                          ",
        "  Schedule:Compact,                                                       ",
        "    AirVelocitySchedule,     !- Name                                      ",
        "    Any Number,              !- Schedule Type Limits Name                 ",
        "    Through: 12/31,          !- Field 1                                   ",
        "    For: AllDays,            !- Field 2                                   ",
        "    Until: 24:00,0.0;        !- Field 3                                   ",
        "                                                                          ",
        "  Schedule:Compact,                                                       ",
        "    Work efficiency,         !- Name                                      ",
        "    Any Number,              !- Schedule Type Limits Name                 ",
        "    Through: 12/31,          !- Field 9                                   ",
        "    For: AllDays,            !- Field 10                                  ",
        "    Until: 24:00,0.0;         !- Field 11                                 ",
        "                                                                          ",
        " Output:Diagnostics, DisplayExtraWarnings;",
        " Timestep, 4;",
        " BUILDING, AirloopHVAC_VentilationRateProcedure, 0.0, Suburbs, .04, .4, FullExterior, 25, 6;",
        " SimulationControl, NO, NO, NO, YES, NO;",
        "ScheduleTypeLimits,",
        "  Any Number;              !- Name",
        "  Site:Location,",
        "    Miami Intl Ap FL USA TMY3 WMO=722020E,    !- Name",
        "    25.82,                 !- Latitude {deg}",
        "    -80.30,                !- Longitude {deg}",
        "    -5.00,                 !- Time Zone {hr}",
        "    11;                    !- Elevation {m}",

        "SizingPeriod:DesignDay,",
        " Miami Intl Ap Ann Clg .4% Condns DB/MCWB, !- Name",
        " 7,                        !- Month",
        " 21,                       !- Day of Month",
        " SummerDesignDay,          !- Day Type",
        " 31.7,                     !- Maximum Dry - Bulb Temperature{ C }",
        " 10.0,                      !- Daily Dry - Bulb Temperature Range{ deltaC }",
        " ,                         !- Dry - Bulb Temperature Range Modifier Type",
        " ,                         !- Dry - Bulb Temperature Range Modifier Day Schedule Name",
        " Wetbulb,                  !- Humidity Condition Type",
        " 22.7,                     !- Wetbulb or DewPoint at Maximum Dry - Bulb{ C }",
        " ,                         !- Humidity Condition Day Schedule Name",
        " ,                         !- Humidity Ratio at Maximum Dry - Bulb{ kgWater / kgDryAir }",
        " ,                         !- Enthalpy at Maximum Dry - Bulb{ J / kg }",
        " ,                         !- Daily Wet - Bulb Temperature Range{ deltaC }",
        " 101217.,                  !- Barometric Pressure{ Pa }",
        " 3.8,                      !- Wind Speed{ m / s }",
        " 340,                      !- Wind Direction{ deg }",
        " No,                       !- Rain Indicator",
        " No,                       !- Snow Indicator",
        " No,                       !- Daylight Saving Time Indicator",
        " ASHRAEClearSky,           !- Solar Model Indicator",
        " ,                         !- Beam Solar Day Schedule Name",
        " ,                         !- Diffuse Solar Day Schedule Name",
        " ,                         !- ASHRAE Clear Sky Optical Depth for Beam Irradiance( taub ) { dimensionless }",
        " ,                         !- ASHRAE Clear Sky Optical Depth for Diffuse Irradiance( taud ) { dimensionless }",
        " 1.00;                     !- Sky Clearness",

        "SizingPeriod:DesignDay,",
        " Miami Intl Ap Ann Htg 99.6% Condns DB, !- Name",
        " 1,                        !- Month",
        " 21,                       !- Day of Month",
        " WinterDesignDay,          !- Day Type",
        " 8.7,                      !- Maximum Dry - Bulb Temperature{ C }",
        " 0.0,                      !- Daily Dry - Bulb Temperature Range{ deltaC }",
        " ,                         !- Dry - Bulb Temperature Range Modifier Type",
        " ,                         !- Dry - Bulb Temperature Range Modifier Day Schedule Name",
        " Wetbulb,                  !- Humidity Condition Type",
        " 8.7,                      !- Wetbulb or DewPoint at Maximum Dry - Bulb{ C }",
        " ,                         !- Humidity Condition Day Schedule Name",
        " ,                         !- Humidity Ratio at Maximum Dry - Bulb{ kgWater / kgDryAir }",
        " ,                         !- Enthalpy at Maximum Dry - Bulb{ J / kg }",
        " ,                         !- Daily Wet - Bulb Temperature Range{ deltaC }",
        " 101217.,                  !- Barometric Pressure{ Pa }",
        " 3.8,                      !- Wind Speed{ m / s }",
        " 340,                      !- Wind Direction{ deg }",
        " No,                       !- Rain Indicator",
        " No,                       !- Snow Indicator",
        " No,                       !- Daylight Saving Time Indicator",
        " ASHRAEClearSky,           !- Solar Model Indicator",
        " ,                         !- Beam Solar Day Schedule Name",
        " ,                         !- Diffuse Solar Day Schedule Name",
        " ,                         !- ASHRAE Clear Sky Optical Depth for Beam Irradiance( taub ) { dimensionless }",
        " ,                         !- ASHRAE Clear Sky Optical Depth for Diffuse Irradiance( taud ) { dimensionless }",
        " 0.00;                     !- Sky Clearness",

        "Zone,",
        "  Space,                   !- Name",
        "  0.0000,                  !- Direction of Relative North {deg}",
        "  0.0000,                  !- X Origin {m}",
        "  0.0000,                  !- Y Origin {m}",
        "  0.0000,                  !- Z Origin {m}",
        "  1,                       !- Type",
        "  1,                       !- Multiplier",
        "  2.4,                     !- Ceiling Height {m}",
        "  ,                        !- Volume {m3}",
        "  autocalculate,           !- Floor Area {m2}",
        "  ,                        !- Zone Inside Convection Algorithm",
        "  ,                        !- Zone Outside Convection Algorithm",
        "  Yes;                     !- Part of Total Floor Area",

        "ZoneGroup,",
        " Zone Group,               !- Name",
        " Zone List,                !- Zone List Name",
        " 10;                       !- Zone List Multiplier",

        "ZoneList,",
        " Zone List,                !- Name",
        " Spacex10;                 !- Zone 1 Name",

        "Zone,",
        "  Spacex10,                !- Name",
        "  0.0000,                  !- Direction of Relative North {deg}",
        "  0.0000,                  !- X Origin {m}",
        "  0.0000,                  !- Y Origin {m}",
        "  0.0000,                  !- Z Origin {m}",
        "  1,                       !- Type",
        "  1,                       !- Multiplier",
        "  2.4,                     !- Ceiling Height {m}",
        "  ,                        !- Volume {m3}",
        "  autocalculate,           !- Floor Area {m2}",
        "  ,                        !- Zone Inside Convection Algorithm",
        "  ,                        !- Zone Outside Convection Algorithm",
        "  Yes;                     !- Part of Total Floor Area",

        "People,",
        " Spacex10 People,          !- Name",
        " Spacex10,                 !- Zone or ZoneList Name",
        " OnSched,                  !- Number of People Schedule Name",
        " people,                   !- Number of People Calculation Method",
        " 11,                       !- Number of People",
        " ,                         !- People per Zone Floor Area{ person / m2 }",
        " ,                         !- Zone Floor Area per Person{ m2 / person }",
        " 0.3,                      !- Fraction Radiant",
        " AutoCalculate,            !- Sensible Heat Fraction",
        " Activity Schedule;        !- Activity Level Schedule Name",

        "Lights,",
        " Space Lights,             !- Name",
        " Space,                    !- Zone or ZoneList Name",
        " OnSched,                  !- Schedule Name",
        " Watts/Area,               !- Design Level Calculation Method",
        " ,                         !- Lighting Level{ W }",
        " 10.0,                     !- Watts per Zone Floor Area{ W / m2 }",
        " ,                         !- Watts per Person{ W / person }",
        " 0.0,                      !- Return Air Fraction",
        " 0.59,                     !- Fraction Radiant",
        " 0.2,                      !- Fraction Visible",
        " 0,                        !- Fraction Replaceable",
        " GeneralLights;            !- End - Use Subcategory",

        "Lights,",
        " Space Lights x10,         !- Name",
        " Spacex10,                 !- Zone or ZoneList Name",
        " OnSched,                  !- Schedule Name",
        " Watts/Area,               !- Design Level Calculation Method",
        " ,                         !- Lighting Level{ W }",
        " 10.0,                     !- Watts per Zone Floor Area{ W / m2 }",
        " ,                         !- Watts per Person{ W / person }",
        " 0.0,                      !- Return Air Fraction",
        " 0.59,                     !- Fraction Radiant",
        " 0.2,                      !- Fraction Visible",
        " 0,                        !- Fraction Replaceable",
        " GeneralLights;            !- End - Use Subcategory",

        "ElectricEquipment,",
        " Space ElecEq,             !- Name",
        " Space,                    !- Zone or ZoneList Name",
        " OnSched,                  !- Schedule Name",
        " Watts/Area,               !- Design Level Calculation Method",
        " ,                         !- Design Level{ W }",
        " 20.0,                     !- Watts per Zone Floor Area{ W / m2 }",
        " ,                         !- Watts per Person{ W / person }",
        " 0.1,                      !- Fraction Latent",
        " 0.3,                      !- Fraction Radiant",
        " 0.1;                      !- Fraction Lost",

        "ElectricEquipment,",
        " Space ElecEq x10,         !- Name",
        " Spacex10,                 !- Zone or ZoneList Name",
        " OnSched,                  !- Schedule Name",
        " Watts/Area,               !- Design Level Calculation Method",
        " ,                         !- Design Level{ W }",
        " 20.0,                     !- Watts per Zone Floor Area{ W / m2 }",
        " ,                         !- Watts per Person{ W / person }",
        " 0.1,                      !- Fraction Latent",
        " 0.3,                      !- Fraction Radiant",
        " 0.1;                      !- Fraction Lost",

        "Schedule:Compact,",
        " OnSched,                  !- Name",
        " Fraction,                 !- Schedule Type Limits Name",
        " Through: 12/31,           !- Field 1",
        " For: AllDays,             !- Field 2",
        " Until: 24:00, 1.0;        !- Field 26",

        "ScheduleTypeLimits,",
        " Fraction,                 !- Name",
        " 0.0,                      !- Lower Limit Value",
        " 1.0,                      !- Upper Limit Value",
        " CONTINUOUS;               !- Numeric Type",

        "Construction,",
        " INT-WALL-1,               !- Name",
        " GP02,                     !- Outside Layer",
        " AL21,                     !- Layer 2",
        " GP02;                     !- Layer 3",

        "Material,",
        " GP02,                     !- Name",
        " MediumSmooth,             !- Roughness",
        " 1.5900001E-02,            !- Thickness{ m }",
        " 0.1600000,                !- Conductivity{ W / m - K }",
        " 801.0000,                 !- Density{ kg / m3 }",
        " 837.0000,                 !- Specific Heat{ J / kg - K }",
        " 0.9000000,                !- Thermal Absorptance",
        " 0.7500000,                !- Solar Absorptance",
        " 0.7500000;                !- Visible Absorptance",

        "Material:AirGap,",
        " AL21,                     !- Name",
        " 0.1570000;                !- Thermal Resistance{ m2 - K / W }",

        "Construction,",
        "FLOOR-SLAB-1,              !- Name",
        "CC03,                      !- Outside Layer",
        "CP01;                      !- Layer 2",

        "Material,",
        " CC03,                     !- Name",
        " MediumRough,              !- Roughness",
        " 0.1016000,                !- Thickness{ m }",
        " 1.310000,                 !- Conductivity{ W / m - K }",
        " 2243.000,                 !- Density{ kg / m3 }",
        " 837.0000,                 !- Specific Heat{ J / kg - K }",
        " 0.9000000,                !- Thermal Absorptance",
        " 0.6500000,                !- Solar Absorptance",
        " 0.6500000;                !- Visible Absorptance",

        "Material:NoMass,",
        " CP01,                     !- Name",
        " Rough,                    !- Roughness",
        " 0.3670000,                !- Thermal Resistance{ m2 - K / W }",
        " 0.9000000,                !- Thermal Absorptance",
        " 0.7500000,                !- Solar Absorptance",
        " 0.7500000;                !- Visible Absorptance",

        "Construction,",
        " CLNG-1,                   !- Name",
        " MAT-CLNG-1;               !- Outside Layer",

        "Material:NoMass,",
        " MAT-CLNG-1,               !- Name",
        " Rough,                    !- Roughness",
        " 0.652259290,              !- Thermal Resistance{ m2 - K / W }",
        " 0.65,                     !- Thermal Absorptance",
        " 0.65,                     !- Solar Absorptance",
        " 0.65;                     !- Visible Absorptance",

        "BuildingSurface:Detailed,",
        " FRONT-1,                  !- Name",
        " WALL,                     !- Surface Type",
        " INT-WALL-1,               !- Construction Name",
        " Space,                    !- Zone Name",
        " Outdoors,                 !- Outside Boundary Condition",
        " ,                         !- Outside Boundary Condition Object",
        " SunExposed,               !- Sun Exposure",
        " WindExposed,              !- Wind Exposure",
        " 0.50000,                  !- View Factor to Ground",
        " 4,                        !- Number of Vertices",
        " 0.0, 0.0, 2.4,            !- X, Y, Z == > Vertex 1 {m}",
        " 0.0, 0.0, 0.0,            !- X, Y, Z == > Vertex 2 {m}",
        " 30.5, 0.0, 0.0,           !- X, Y, Z == > Vertex 3 {m}",
        " 30.5, 0.0, 2.4;           !- X, Y, Z == > Vertex 4 {m}",

        "BuildingSurface:Detailed,",
        " C1-1,                     !- Name",
        " CEILING,                  !- Surface Type",
        " CLNG-1,                   !- Construction Name",
        " Space,                    !- Zone Name",
        " Outdoors,                 !- Outside Boundary Condition",
        " ,                         !- Outside Boundary Condition Object",
        " NoSun,                    !- Sun Exposure",
        " NoWind,                   !- Wind Exposure",
        " 0.0,                      !- View Factor to Ground",
        " 4,                        !- Number of Vertices",
        " 3.7, 3.7, 2.4,            !- X, Y, Z == > Vertex 1 {m}",
        " 0.0, 0.0, 2.4,            !- X, Y, Z == > Vertex 2 {m}",
        " 30.5, 0.0, 2.4,           !- X, Y, Z == > Vertex 3 {m}",
        " 26.8, 3.7, 2.4;           !- X, Y, Z == > Vertex 4 {m}",

        "BuildingSurface:Detailed,",
        " F1-1,                     !- Name",
        " FLOOR,                    !- Surface Type",
        " FLOOR-SLAB-1,             !- Construction Name",
        " Space,                    !- Zone Name",
        " Outdoors,                   !- Outside Boundary Condition",
        " ,                         !- Outside Boundary Condition Object",
        " NoSun,                    !- Sun Exposure",
        " NoWind,                   !- Wind Exposure",
        " 0.0,                      !- View Factor to Ground",
        " 4,                        !- Number of Vertices",
        " 26.8, 3.7, 0.0,           !- X, Y, Z == > Vertex 1 {m}",
        " 30.5, 0.0, 0.0,           !- X, Y, Z == > Vertex 2 {m}",
        " 0.0, 0.0, 0.0,            !- X, Y, Z == > Vertex 3 {m}",
        " 3.7, 3.7, 0.0;            !- X, Y, Z == > Vertex 4 {m}",

        "BuildingSurface:Detailed,",
        " SB12,                     !- Name",
        " WALL,                     !- Surface Type",
        " INT-WALL-1,               !- Construction Name",
        " Space,                    !- Zone Name",
        " Adiabatic,                !- Outside Boundary Condition",
        " ,                         !- Outside Boundary Condition Object",
        " NoSun,                    !- Sun Exposure",
        " NoWind,                   !- Wind Exposure",
        " 0.0,                      !- View Factor to Ground",
        " 4,                        !- Number of Vertices",
        " 30.5, 0.0, 2.4,           !- X, Y, Z == > Vertex 1 {m}",
        " 30.5, 0.0, 0.0,           !- X, Y, Z == > Vertex 2 {m}",
        " 26.8, 3.7, 0.0,           !- X, Y, Z == > Vertex 3 {m}",
        " 26.8, 3.7, 2.4;           !- X, Y, Z == > Vertex 4 {m}",

        "BuildingSurface:Detailed,",
        " SB14,                     !- Name",
        " WALL,                     !- Surface Type",
        " INT-WALL-1,               !- Construction Name",
        " Space,                    !- Zone Name",
        " Adiabatic,                !- Outside Boundary Condition",
        " ,                         !- Outside Boundary Condition Object",
        " NoSun,                    !- Sun Exposure",
        " NoWind,                   !- Wind Exposure",
        " 0.0,                      !- View Factor to Ground",
        " 4,                        !- Number of Vertices",
        " 3.7, 3.7, 2.4,            !- X, Y, Z == > Vertex 1 {m}",
        " 3.7, 3.7, 0.0,            !- X, Y, Z == > Vertex 2 {m}",
        " 0.0, 0.0, 0.0,            !- X, Y, Z == > Vertex 3 {m}",
        " 0.0, 0.0, 2.4;            !- X, Y, Z == > Vertex 4 {m}",

        "BuildingSurface:Detailed,",
        " SB15,                     !- Name",
        " WALL,                     !- Surface Type",
        " INT-WALL-1,               !- Construction Name",
        " Space,                    !- Zone Name",
        " Adiabatic,                !- Outside Boundary Condition",
        " ,                         !- Outside Boundary Condition Object",
        " NoSun,                    !- Sun Exposure",
        " NoWind,                   !- Wind Exposure",
        " 0.0,                      !- View Factor to Ground",
        " 4,                        !- Number of Vertices",
        " 26.8, 3.7, 2.4,           !- X, Y, Z == > Vertex 1 {m}",
        " 26.8, 3.7, 0.0,           !- X, Y, Z == > Vertex 2 {m}",
        " 3.7, 3.7, 0.0,            !- X, Y, Z == > Vertex 3 {m}",
        " 3.7, 3.7, 2.4;            !- X, Y, Z == > Vertex 4 {m}",

        "BuildingSurface:Detailed,",
        " FRONT-1x10,               !- Name",
        " WALL,                     !- Surface Type",
        " INT-WALL-1,               !- Construction Name",
        " Spacex10,                 !- Zone Name",
        " Outdoors,                 !- Outside Boundary Condition",
        " ,                         !- Outside Boundary Condition Object",
        " SunExposed,               !- Sun Exposure",
        " WindExposed,              !- Wind Exposure",
        " 0.50000,                  !- View Factor to Ground",
        " 4,                        !- Number of Vertices",
        " 0.0, 0.0, 2.4,            !- X, Y, Z == > Vertex 1 {m}",
        " 0.0, 0.0, 0.0,            !- X, Y, Z == > Vertex 2 {m}",
        " 30.5, 0.0, 0.0,           !- X, Y, Z == > Vertex 3 {m}",
        " 30.5, 0.0, 2.4;           !- X, Y, Z == > Vertex 4 {m}",

        "BuildingSurface:Detailed,",
        " C1-1x10,                  !- Name",
        " CEILING,                  !- Surface Type",
        " CLNG-1,                   !- Construction Name",
        " Spacex10,                 !- Zone Name",
        " Outdoors,                 !- Outside Boundary Condition",
        " ,                         !- Outside Boundary Condition Object",
        " NoSun,                    !- Sun Exposure",
        " NoWind,                   !- Wind Exposure",
        " 0.0,                      !- View Factor to Ground",
        " 4,                        !- Number of Vertices",
        " 3.7, 3.7, 2.4,            !- X, Y, Z == > Vertex 1 {m}",
        " 0.0, 0.0, 2.4,            !- X, Y, Z == > Vertex 2 {m}",
        " 30.5, 0.0, 2.4,           !- X, Y, Z == > Vertex 3 {m}",
        " 26.8, 3.7, 2.4;           !- X, Y, Z == > Vertex 4 {m}",

        "BuildingSurface:Detailed,",
        " F1-1x10,                  !- Name",
        " FLOOR,                    !- Surface Type",
        " FLOOR-SLAB-1,             !- Construction Name",
        " Spacex10,                 !- Zone Name",
        " Outdoors,                   !- Outside Boundary Condition",
        " ,                         !- Outside Boundary Condition Object",
        " NoSun,                    !- Sun Exposure",
        " NoWind,                   !- Wind Exposure",
        " 0.0,                      !- View Factor to Ground",
        " 4,                        !- Number of Vertices",
        " 26.8, 3.7, 0.0,           !- X, Y, Z == > Vertex 1 {m}",
        " 30.5, 0.0, 0.0,           !- X, Y, Z == > Vertex 2 {m}",
        " 0.0, 0.0, 0.0,            !- X, Y, Z == > Vertex 3 {m}",
        " 3.7, 3.7, 0.0;            !- X, Y, Z == > Vertex 4 {m}",

        "BuildingSurface:Detailed,",
        " SB12x10,                  !- Name",
        " WALL,                     !- Surface Type",
        " INT-WALL-1,               !- Construction Name",
        " Spacex10,                 !- Zone Name",
        " Adiabatic,                !- Outside Boundary Condition",
        " ,                         !- Outside Boundary Condition Object",
        " NoSun,                    !- Sun Exposure",
        " NoWind,                   !- Wind Exposure",
        " 0.0,                      !- View Factor to Ground",
        " 4,                        !- Number of Vertices",
        " 30.5, 0.0, 2.4,           !- X, Y, Z == > Vertex 1 {m}",
        " 30.5, 0.0, 0.0,           !- X, Y, Z == > Vertex 2 {m}",
        " 26.8, 3.7, 0.0,           !- X, Y, Z == > Vertex 3 {m}",
        " 26.8, 3.7, 2.4;           !- X, Y, Z == > Vertex 4 {m}",

        "BuildingSurface:Detailed,",
        " SB14x10,                  !- Name",
        " WALL,                     !- Surface Type",
        " INT-WALL-1,               !- Construction Name",
        " Spacex10,                 !- Zone Name",
        " Adiabatic,                !- Outside Boundary Condition",
        " ,                         !- Outside Boundary Condition Object",
        " NoSun,                    !- Sun Exposure",
        " NoWind,                   !- Wind Exposure",
        " 0.0,                      !- View Factor to Ground",
        " 4,                        !- Number of Vertices",
        " 3.7, 3.7, 2.4,            !- X, Y, Z == > Vertex 1 {m}",
        " 3.7, 3.7, 0.0,            !- X, Y, Z == > Vertex 2 {m}",
        " 0.0, 0.0, 0.0,            !- X, Y, Z == > Vertex 3 {m}",
        " 0.0, 0.0, 2.4;            !- X, Y, Z == > Vertex 4 {m}",

        "BuildingSurface:Detailed,",
        " SB15x10,                  !- Name",
        " WALL,                     !- Surface Type",
        " INT-WALL-1,               !- Construction Name",
        " Spacex10,                 !- Zone Name",
        " Adiabatic,                !- Outside Boundary Condition",
        " ,                         !- Outside Boundary Condition Object",
        " NoSun,                    !- Sun Exposure",
        " NoWind,                   !- Wind Exposure",
        " 0.0,                      !- View Factor to Ground",
        " 4,                        !- Number of Vertices",
        " 26.8, 3.7, 2.4,           !- X, Y, Z == > Vertex 1 {m}",
        " 26.8, 3.7, 0.0,           !- X, Y, Z == > Vertex 2 {m}",
        " 3.7, 3.7, 0.0,            !- X, Y, Z == > Vertex 3 {m}",
        " 3.7, 3.7, 2.4;            !- X, Y, Z == > Vertex 4 {m}",

        "Output:Table:SummaryReports,",
        "  AllSummary; !- Report 1 Name",
        " ",

    });

    ASSERT_TRUE(process_idf(idf_objects));

    // OutputProcessor::TimeValue.allocate(2);
    state->dataGlobal->DDOnlySimulation = true;

    ManageSimulation(*state);

    //	compare_err_stream( "" );

    state->dataHeatBalFanSys->ZTAVComf(1) = 25.0;
    state->dataHeatBal->MRT(1) = 26.0;
    state->dataHeatBalFanSys->ZoneAirHumRatAvgComf(1) = 0.00529; // 0.002 to 0.006

    CalcThermalComfortFanger(*state);

    EXPECT_NEAR(state->dataThermalComforts->ThermalComfortData(1).FangerPMV, -1.262, 0.005);
    EXPECT_NEAR(state->dataThermalComforts->ThermalComfortData(1).FangerPPD, 38.3, 0.1);

    state->dataHeatBalFanSys->ZTAVComf(1) = 26.0;
    state->dataHeatBal->MRT(1) = 27.0;
    state->dataHeatBalFanSys->ZoneAirHumRatAvgComf(1) = 0.00529; // 0.002 to 0.006

    CalcThermalComfortFanger(*state);

    EXPECT_NEAR(state->dataThermalComforts->ThermalComfortData(1).FangerPMV, -0.860, 0.005);
    EXPECT_NEAR(state->dataThermalComforts->ThermalComfortData(1).FangerPPD, 20.6, 0.1);

    state->dataHeatBalFanSys->ZTAVComf(1) = 27.0;
    state->dataHeatBal->MRT(1) = 28.0;
    state->dataHeatBalFanSys->ZoneAirHumRatAvgComf(1) = 0.00529; // 0.002 to 0.006

    CalcThermalComfortFanger(*state);

    EXPECT_NEAR(state->dataThermalComforts->ThermalComfortData(1).FangerPMV, -0.460, 0.005);
    EXPECT_NEAR(state->dataThermalComforts->ThermalComfortData(1).FangerPPD, 9.4, 0.1);

    state->dataHeatBalFanSys->ZTAVComf(1) = 25.0;
    state->dataHeatBal->MRT(1) = 26.0;
    state->dataHeatBalFanSys->ZoneAirHumRatAvgComf(1) = 0.00629; // 0.002 to 0.006

    CalcThermalComfortFanger(*state);

    EXPECT_NEAR(state->dataThermalComforts->ThermalComfortData(1).FangerPMV, -1.201, 0.005);
    EXPECT_NEAR(state->dataThermalComforts->ThermalComfortData(1).FangerPPD, 35.3, 0.1);
}

TEST_F(EnergyPlusFixture, ThermalComfort_CalcSurfaceWeightedMRT)
{

    int ZoneNum(1);
    int SurfNum(1);
    Real64 RadTemp;

    state->dataHeatBalSurf->TH.deallocate();
    state->dataSurface->Surface.deallocate();
    state->dataHeatBal->Zone.deallocate();
    state->dataThermalComforts->AngleFactorList.allocate(1);
    state->dataSurface->TotSurfaces = 3;
    state->dataGlobal->NumOfZones = 1;
    state->dataHeatBalSurf->TH.allocate(2, 2, state->dataSurface->TotSurfaces);
    state->dataSurface->Surface.allocate(state->dataSurface->TotSurfaces);
    state->dataConstruction->Construct.allocate(state->dataSurface->TotSurfaces);
    state->dataHeatBal->Zone.allocate(1);

    state->dataSurface->Surface(1).Area = 20.0;
    state->dataSurface->Surface(2).Area = 15.0;
    state->dataSurface->Surface(3).Area = 10.0;
    state->dataSurface->Surface(1).HeatTransSurf = true;
    state->dataSurface->Surface(2).HeatTransSurf = true;
    state->dataSurface->Surface(3).HeatTransSurf = true;
    state->dataSurface->Surface(1).Construction = 1;
    state->dataSurface->Surface(2).Construction = 2;
    state->dataSurface->Surface(3).Construction = 3;
    state->dataConstruction->Construct(1).InsideAbsorpThermal = 1.0;
    state->dataConstruction->Construct(2).InsideAbsorpThermal = 0.9;
    state->dataConstruction->Construct(3).InsideAbsorpThermal = 0.8;
    state->dataSurface->Surface(1).Zone = 1;
    state->dataSurface->Surface(2).Zone = 1;
    state->dataSurface->Surface(3).Zone = 1;
    state->dataHeatBal->Zone(1).HTSurfaceFirst = 1;
    state->dataHeatBal->Zone(1).HTSurfaceLast = 3;
    state->dataHeatBalSurf->TH(2, 1, 1) = 20.0;
    state->dataHeatBalSurf->TH(2, 1, 2) = 15.0;
    state->dataHeatBalSurf->TH(2, 1, 3) = 10.0;

    SurfNum = 1;
    state->dataThermalComforts->clear_state();
    RadTemp = CalcSurfaceWeightedMRT(*state, ZoneNum, SurfNum);
    EXPECT_NEAR(RadTemp, 16.6, 0.1);

    SurfNum = 2;
    state->dataThermalComforts->clear_state();
    RadTemp = CalcSurfaceWeightedMRT(*state, ZoneNum, SurfNum);
    EXPECT_NEAR(RadTemp, 16.1, 0.1);

    SurfNum = 3;
    state->dataThermalComforts->clear_state();
    RadTemp = CalcSurfaceWeightedMRT(*state, ZoneNum, SurfNum);
    EXPECT_NEAR(RadTemp, 14.0, 0.1);
}

TEST_F(EnergyPlusFixture, ThermalComfort_CalcAngleFactorMRT)
{

    Real64 RadTemp;

    state->dataThermalComforts->AngleFactorList.allocate(1);
    state->dataThermalComforts->AngleFactorList(1).TotAngleFacSurfaces = 3;
    state->dataThermalComforts->AngleFactorList(1).SurfacePtr.allocate(state->dataThermalComforts->AngleFactorList(1).TotAngleFacSurfaces);
    state->dataThermalComforts->AngleFactorList(1).AngleFactor.allocate(state->dataThermalComforts->AngleFactorList(1).TotAngleFacSurfaces);

    state->dataThermalComforts->AngleFactorList(1).SurfacePtr(1) = 1;
    state->dataThermalComforts->AngleFactorList(1).SurfacePtr(2) = 2;
    state->dataThermalComforts->AngleFactorList(1).SurfacePtr(3) = 3;
    state->dataThermalComforts->AngleFactorList(1).AngleFactor(1) = 0.5;
    state->dataThermalComforts->AngleFactorList(1).AngleFactor(2) = 0.3;
    state->dataThermalComforts->AngleFactorList(1).AngleFactor(3) = 0.2;

    state->dataHeatBalSurf->TH.deallocate();
    state->dataSurface->TotSurfaces = state->dataThermalComforts->AngleFactorList(1).TotAngleFacSurfaces;
    state->dataHeatBalSurf->TH.allocate(2, 2, state->dataSurface->TotSurfaces);
    state->dataSurface->Surface.deallocate();
    state->dataConstruction->Construct.deallocate();
    state->dataSurface->Surface.allocate(state->dataSurface->TotSurfaces);
    state->dataConstruction->Construct.allocate(state->dataSurface->TotSurfaces);

    state->dataHeatBalSurf->TH(2, 1, 1) = 20.0;
    state->dataHeatBalSurf->TH(2, 1, 2) = 15.0;
    state->dataHeatBalSurf->TH(2, 1, 3) = 10.0;
    state->dataSurface->Surface(1).Construction = 1;
    state->dataSurface->Surface(2).Construction = 2;
    state->dataSurface->Surface(3).Construction = 3;
    state->dataConstruction->Construct(1).InsideAbsorpThermal = 1.0;
    state->dataConstruction->Construct(2).InsideAbsorpThermal = 0.9;
    state->dataConstruction->Construct(3).InsideAbsorpThermal = 0.8;

    RadTemp = CalcAngleFactorMRT(*state, 1);
    EXPECT_NEAR(RadTemp, 16.9, 0.1);
}

TEST_F(EnergyPlusFixture, ThermalComfort_CalcThermalComfortAdaptiveASH55Test)
{
    // 5381
    state->dataThermalComforts->useEpwData = true;

    state->dataThermalComforts->DailyAveOutTemp(1) = 8.704166667;
    state->dataThermalComforts->DailyAveOutTemp(2) = 9.895833333;
    state->dataThermalComforts->DailyAveOutTemp(3) = 12.2;
    state->dataThermalComforts->DailyAveOutTemp(4) = 8.445833333;
    state->dataThermalComforts->DailyAveOutTemp(5) = 7.8;
    state->dataThermalComforts->DailyAveOutTemp(6) = 7.158333333;
    state->dataThermalComforts->DailyAveOutTemp(7) = 8.0125;
    state->dataThermalComforts->DailyAveOutTemp(8) = 8.279166667;
    state->dataThermalComforts->DailyAveOutTemp(9) = 8.166666667;
    state->dataThermalComforts->DailyAveOutTemp(10) = 7.141666667;
    state->dataThermalComforts->DailyAveOutTemp(11) = 7.433333333;
    state->dataThermalComforts->DailyAveOutTemp(12) = 9.0625;
    state->dataThermalComforts->DailyAveOutTemp(13) = 9.741666667;
    state->dataThermalComforts->DailyAveOutTemp(14) = 9.545833333;
    state->dataThermalComforts->DailyAveOutTemp(15) = 11.43333333;
    state->dataThermalComforts->DailyAveOutTemp(16) = 12.375;
    state->dataThermalComforts->DailyAveOutTemp(17) = 12.59583333;
    state->dataThermalComforts->DailyAveOutTemp(18) = 12.6625;
    state->dataThermalComforts->DailyAveOutTemp(19) = 13.50833333;
    state->dataThermalComforts->DailyAveOutTemp(20) = 12.99583333;
    state->dataThermalComforts->DailyAveOutTemp(21) = 11.58333333;
    state->dataThermalComforts->DailyAveOutTemp(22) = 11.72083333;
    state->dataThermalComforts->DailyAveOutTemp(23) = 9.1875;
    state->dataThermalComforts->DailyAveOutTemp(24) = 6.8;
    state->dataThermalComforts->DailyAveOutTemp(25) = 9.391666667;
    state->dataThermalComforts->DailyAveOutTemp(26) = 8.1125;
    state->dataThermalComforts->DailyAveOutTemp(27) = 8.4;
    state->dataThermalComforts->DailyAveOutTemp(28) = 8.475;
    state->dataThermalComforts->DailyAveOutTemp(29) = 7.941666667;
    state->dataThermalComforts->DailyAveOutTemp(30) = 9.316666667;

    state->dataGlobal->BeginDayFlag = true;

    CalcThermalComfortAdaptiveASH55(*state, false);
    EXPECT_NEAR(state->dataThermalComforts->runningAverageASH, 9.29236111, 0.001);
    state->dataThermalComforts->useEpwData = false;
    state->dataGlobal->BeginDayFlag = false;
}

TEST_F(EnergyPlusFixture, ThermalComfort_CalcIfSetPointMetWithCutoutTest)
{
    state->dataGlobal->NumOfZones = 1;
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand.allocate(state->dataGlobal->NumOfZones);
    state->dataThermalComforts->ThermalComfortSetPoint.allocate(state->dataGlobal->NumOfZones);
    state->dataHeatBalFanSys->TempControlType.allocate(1);
    state->dataRoomAirMod->AirModel.allocate(state->dataGlobal->NumOfZones);
    state->dataRoomAirMod->AirModel(1).AirModelType = DataRoomAirModel::RoomAirModel::Mixing;
    state->dataHeatBalFanSys->ZTAV.allocate(state->dataGlobal->NumOfZones);
    state->dataHeatBalFanSys->ZoneThermostatSetPointLo.allocate(state->dataGlobal->NumOfZones);
    state->dataHeatBalFanSys->ZoneThermostatSetPointHi.allocate(state->dataGlobal->NumOfZones);
    state->dataHeatBalFanSys->ZoneThermostatSetPointLoAver.allocate(state->dataGlobal->NumOfZones);
    state->dataHeatBalFanSys->ZoneThermostatSetPointHiAver.allocate(state->dataGlobal->NumOfZones);
    state->dataThermalComforts->ThermalComfortInASH55.allocate(state->dataGlobal->NumOfZones);
    state->dataThermalComforts->ThermalComfortInASH55(1).ZoneIsOccupied = true;
    state->dataGlobal->TimeStepZone = 0.25;
    state->dataHeatBal->Zone.allocate(state->dataGlobal->NumOfZones);
    state->dataZoneTempPredictorCorrector->NumOnOffCtrZone = 1;

    state->dataHeatBalFanSys->TempControlType(1) = DualSetPointWithDeadBand;

    // heating
    state->dataHeatBalFanSys->ZTAV(1) = 21.1;                                     // 70F
    state->dataHeatBalFanSys->ZoneThermostatSetPointLo(1) = 22.2;                 // 72F
    state->dataHeatBalFanSys->ZoneThermostatSetPointLoAver(1) = 22.2;                 // 72F
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).TotalOutputRequired = 500.0; // must be greater than zero
    CalcIfSetPointMet(*state);
    EXPECT_EQ(state->dataGlobal->TimeStepZone, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetHeating);
    EXPECT_EQ(state->dataGlobal->TimeStepZone, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetHeatingOccupied);
    EXPECT_EQ(0., state->dataThermalComforts->ThermalComfortSetPoint(1).notMetCooling);
    EXPECT_EQ(0., state->dataThermalComforts->ThermalComfortSetPoint(1).notMetCoolingOccupied);

    // cooling
    state->dataHeatBalFanSys->ZTAV(1) = 25.0;                                      // 77F
    state->dataHeatBalFanSys->ZoneThermostatSetPointHi(1) = 23.9;                  // 75F
    state->dataHeatBalFanSys->ZoneThermostatSetPointHiAver(1) = 23.9;                  // 75F
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).TotalOutputRequired = -500.0; // must be less than zero
    CalcIfSetPointMet(*state);
    EXPECT_EQ(0, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetHeating);
    EXPECT_EQ(0, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetHeatingOccupied);
    EXPECT_EQ(state->dataGlobal->TimeStepZone, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetCooling);
    EXPECT_EQ(state->dataGlobal->TimeStepZone, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetCoolingOccupied);

    // no cooling or heating
    state->dataHeatBalFanSys->ZTAV(1) = 23.0;                                      // 73F
    state->dataZoneEnergyDemand->ZoneSysEnergyDemand(1).TotalOutputRequired = 0.0; // must be zero
    CalcIfSetPointMet(*state);
    EXPECT_EQ(0, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetHeating);
    EXPECT_EQ(0, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetHeatingOccupied);
    EXPECT_EQ(0, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetCooling);
    EXPECT_EQ(0, state->dataThermalComforts->ThermalComfortSetPoint(1).notMetCoolingOccupied);
    EXPECT_EQ(state->dataGlobal->TimeStepZone, state->dataThermalComforts->ThermalComfortSetPoint(1).totalNotMetHeating);
    EXPECT_EQ(state->dataGlobal->TimeStepZone, state->dataThermalComforts->ThermalComfortSetPoint(1).totalNotMetHeatingOccupied);
    EXPECT_EQ(state->dataGlobal->TimeStepZone, state->dataThermalComforts->ThermalComfortSetPoint(1).totalNotMetCooling);
    EXPECT_EQ(state->dataGlobal->TimeStepZone, state->dataThermalComforts->ThermalComfortSetPoint(1).totalNotMetCoolingOccupied);

}

TEST_F(EnergyPlusFixture, ThermalComfort_CalcThermalComfortASH55)
{

    // Set the data for the test
    state->dataHeatBal->TotPeople = 1;
    state->dataHeatBal->People.allocate(state->dataHeatBal->TotPeople);
    state->dataThermalComforts->ThermalComfortData.allocate(state->dataHeatBal->TotPeople);
    state->dataGlobal->NumOfZones = 1;
    state->dataHeatBal->Zone.allocate(state->dataGlobal->NumOfZones);
    state->dataHeatBalFanSys->ZTAVComf.allocate(state->dataGlobal->NumOfZones);
    state->dataHeatBal->MRT.allocate(state->dataGlobal->NumOfZones);
    state->dataHeatBalFanSys->ZoneAirHumRatAvgComf.allocate(state->dataGlobal->NumOfZones);
    state->dataRoomAirMod->IsZoneDV.allocate(state->dataGlobal->NumOfZones);
    state->dataRoomAirMod->IsZoneUI.allocate(state->dataGlobal->NumOfZones);
    state->dataHeatBalFanSys->QHTRadSysToPerson.allocate(state->dataGlobal->NumOfZones);
    state->dataHeatBalFanSys->QCoolingPanelToPerson.allocate(state->dataGlobal->NumOfZones);
    state->dataHeatBalFanSys->QHWBaseboardToPerson.allocate(state->dataGlobal->NumOfZones);
    state->dataHeatBalFanSys->QSteamBaseboardToPerson.allocate(state->dataGlobal->NumOfZones);
    state->dataHeatBalFanSys->QElecBaseboardToPerson.allocate(state->dataGlobal->NumOfZones);

    state->dataHeatBal->People(1).ZonePtr = 1;
    state->dataHeatBal->People(1).NumberOfPeoplePtr = -1;
    state->dataHeatBal->People(1).NumberOfPeople = 5.0;
    state->dataHeatBal->People(1).NomMinNumberPeople = 5.0;
    state->dataHeatBal->People(1).NomMaxNumberPeople = 5.0;
    state->dataHeatBal->Zone(state->dataHeatBal->People(1).ZonePtr).TotOccupants = state->dataHeatBal->People(1).NumberOfPeople;
    state->dataHeatBal->People(1).FractionRadiant = 0.3;
    state->dataHeatBal->People(1).FractionConvected = 1.0 - state->dataHeatBal->People(1).FractionRadiant;
    state->dataHeatBal->People(1).UserSpecSensFrac = DataGlobalConstants::AutoCalculate;
    state->dataHeatBal->People(1).CO2RateFactor = 3.82e-8;
    state->dataHeatBal->People(1).Show55Warning = true;
    state->dataHeatBal->People(1).Pierce = true;
    state->dataHeatBal->People(1).MRTCalcType = ZoneAveraged;
    state->dataHeatBal->People(1).WorkEffPtr = 0;
    state->dataHeatBal->People(1).ClothingType = 1;

    state->dataRoomAirMod->IsZoneDV(1) = state->dataRoomAirMod->IsZoneUI(1) = false;
    state->dataHeatBalFanSys->QHTRadSysToPerson(1) = 0.0;
    state->dataHeatBalFanSys->QCoolingPanelToPerson(1) = 0.0;
    state->dataHeatBalFanSys->QHWBaseboardToPerson(1) = 0.0;
    state->dataHeatBalFanSys->QSteamBaseboardToPerson(1) = 0.0;
    state->dataHeatBalFanSys->QElecBaseboardToPerson(1) = 0.0;
    Real64 BodySurfaceArea = 1.8258;
    state->dataEnvrn->OutBaroPress = 101325.;
    Real64 WorkEff = 0.0;
    state->dataHeatBal->People(1).ActivityLevelPtr = 1;
    state->dataHeatBal->People(1).ClothingPtr = 2;
    state->dataHeatBal->People(1).AirVelocityPtr = 3;
    state->dataHeatBal->People(1).AnkleAirVelocityPtr = 4;
    state->dataScheduleMgr->Schedule.allocate(4);

    // Part 1: Test SET calculations.
    // Reference: ANSI/ASHRAE Standard 55-2017 Appendix D - Table D3 Validation Table for SET Computer Model
    const std::vector<double> TAir = {25, 0, 40, 25, 25, 25, 25, 25};
    const std::vector<double> RH = {0.5, 0.5, 0.5, 0.9, 0.5, 0.5, 0.5, 0.5};
    const std::vector<double> Vel = {0.15, 0.15, 0.15, 0.15, 3, 0.15, 0.15, 0.15};
    const std::vector<double> TRad = {25, 25, 25, 25, 25, 40, 25, 25};
    const std::vector<double> MET = {1, 1, 1, 1, 1, 1, 2, 1};
    const std::vector<double> Clo = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 2};
    const std::vector<double> SET = {23.8, 12.3, 34.3, 24.9, 18.8, 31.8, 29.7, 32.5};

    for (unsigned i = 0; i < TAir.size(); i++) {
        Real64 SETResult = CalcStandardEffectiveTemp(*state, TAir[i], TRad[i], RH[i], Vel[i], MET[i], Clo[i], WorkEff);
        EXPECT_NEAR(SETResult, SET[i], 0.1);
    }

    // Part 2: Test cooling effect
    state->dataHeatBal->People(1).CoolingEffectASH55 = true;
    Real64 AirTemp = 25.0;
    Real64 RadTemp = 25.0;
    Real64 RelHum = 0.5;
    Real64 ActMet = 1;
    Real64 CloUnit = 0.5;

    state->dataHeatBalFanSys->ZTAVComf(1) = AirTemp;
    state->dataHeatBal->MRT(1) = RadTemp;
    state->dataHeatBalFanSys->ZoneAirHumRatAvgComf(1) = Psychrometrics::PsyWFnTdbRhPb(*state, state->dataHeatBalFanSys->ZTAVComf(1), RelHum, state->dataEnvrn->OutBaroPress);
    state->dataScheduleMgr->Schedule(1).CurrentValue = ActMet * BodySurfaceArea * state->dataThermalComforts->ActLevelConv;
    state->dataScheduleMgr->Schedule(2).CurrentValue = CloUnit;

    // Test 1 - Air velocity = 0.15 m/s.
    Real64 AirVel = 0.15;
    state->dataScheduleMgr->Schedule(3).CurrentValue = AirVel;
    CalcThermalComfortCoolingEffectASH(*state);
    Real64 CoolingEffect = state->dataThermalComforts->ThermalComfortData(1).CoolingEffectASH55;
    Real64 StillAirVel = 0.1;
    Real64 RelAirVel = CalcRelativeAirVelocity(AirVel, ActMet);
    Real64 InitialSET = CalcStandardEffectiveTemp(*state, AirTemp, RadTemp, RelHum, RelAirVel, ActMet, CloUnit, WorkEff);
    Real64 CoolingEffectSET = CalcStandardEffectiveTemp(*state, AirTemp - CoolingEffect, RadTemp - CoolingEffect, RelHum, StillAirVel, ActMet, CloUnit, WorkEff);
    EXPECT_NEAR(CoolingEffectSET, InitialSET, 0.1);

    // Test 2 - Air velocity = 1 m/s.
    AirVel = 1;
    state->dataScheduleMgr->Schedule(3).CurrentValue = AirVel;
    CalcThermalComfortCoolingEffectASH(*state);
    CoolingEffect = state->dataThermalComforts->ThermalComfortData(1).CoolingEffectASH55;
    CoolingEffectSET = CalcStandardEffectiveTemp(*state, AirTemp - CoolingEffect, RadTemp - CoolingEffect, RelHum, StillAirVel, ActMet, CloUnit, WorkEff);

    RelAirVel = CalcRelativeAirVelocity(AirVel, ActMet);
    InitialSET = CalcStandardEffectiveTemp(*state, AirTemp, RadTemp, RelHum, RelAirVel, ActMet, CloUnit, WorkEff);
    EXPECT_NEAR(CoolingEffectSET, InitialSET, 0.1);

    // Part 3: Test ankle draft PPD.
    state->dataHeatBal->People(1).AnkleDraftASH55 = true;
    AirVel = 0.15;
    Real64 AnkleAirVel = 0.3;
    state->dataScheduleMgr->Schedule(3).CurrentValue = AirVel;
    state->dataScheduleMgr->Schedule(4).CurrentValue = AnkleAirVel;
    CalcThermalComfortAnkleDraftASH(*state);
    EXPECT_NEAR(state->dataThermalComforts->ThermalComfortData(1).AnkleDraftPPDASH55, 25.0, 0.1);
}

TEST_F(EnergyPlusFixture, ThermalComfort_CalcThermalComfortFanger_Correct_TimeStep)
{

    std::string const idf_objects = delimited_string({
        "  People,                                                                 ",
        "    Space People,   !- Name                                      ",
        "    Space,     !- Zone or ZoneList Name                     ",
        "    PeopleSchedule,          !- Number of People Schedule Name            ",
        "    People,                  !- Number of People Calculation Method       ",
        "    5.0,                     !- Number of People                          ",
        "    ,                        !- People per Zone Floor Area {person/m2}    ",
        "    ,                        !- Zone Floor Area per Person {m2/person}    ",
        "    0.3,                     !- Fraction Radiant                          ",
        "    AUTOCALCULATE,           !- Sensible Heat Fraction                    ",
        "    Activity Schedule,       !- Activity Level Schedule Name              ",
        "    ,                        !- Carbon Dioxide Generation Rate {m3/s-W}   ",
        "    Yes,                     !- Enable ASHRAE 55 Comfort Warnings         ",
        "    ZoneAveraged,            !- Mean Radiant Temperature Calculation Type ",
        "    ,                        !- Surface Name/Angle Factor List Name       ",
        "    Work efficiency,         !- Work Efficiency Schedule Name             ",
        "    ClothingInsulationSchedule,  !- Clothing Insulation Calculation Method",
        "    ,                        !- Clothing Insulation Calculation Method Sch",
        "    Clothing Schedule,       !- Clothing Insulation Schedule Name         ",
        "    AirVelocitySchedule,     !- Air Velocity Schedule Name                ",
        "    Fanger;                  !- Thermal Comfort Model 1 Type              ",
        "                                                                          ",
        "  Schedule:Compact,                                                       ",
        "    PeopleSchedule,          !- Name                                      ",
        "    Any Number,              !- Schedule Type Limits Name                 ",
        "    Through: 12/30,          !- Field 1                                   ",
        "    For: AllDays,            !- Field 2                                   ",
        "    Until: 24:00,1.0,        !- Field 3                                   ",
        "    Through: 12/31,          !- Field 1                                   ",
        "    For: AllDays,            !- Field 2                                   ",
        "    Until: 24:00,0.3;        !- Field 3                                   ",
        "                                                                          ",
        "  Schedule:Compact,                                                       ",
        "    Activity Schedule,       !- Name                                      ",
        "    Any Number,              !- Schedule Type Limits Name                 ",
        "    Through: 12/31,          !- Field 1                                   ",
        "    For: AllDays,            !- Field 2                                   ",
        "    Until: 24:00,70;         !- Field 3                                   ",
        "                                                                          ",
        "  Schedule:Compact,                                                       ",
        "    Clothing Schedule,       !- Name                                      ",
        "    Any Number,              !- Schedule Type Limits Name                 ",
        "    Through: 12/31,          !- Field 9                                   ",
        "    For: AllDays,            !- Field 10                                  ",
        "    Until: 24:00,1.0;         !- Field 11                                 ",
        "                                                                          ",
        "  Schedule:Compact,                                                       ",
        "    AirVelocitySchedule,     !- Name                                      ",
        "    Any Number,              !- Schedule Type Limits Name                 ",
        "    Through: 12/31,          !- Field 1                                   ",
        "    For: AllDays,            !- Field 2                                   ",
        "    Until: 24:00,0.0;        !- Field 3                                   ",
        "                                                                          ",
        "  Schedule:Compact,                                                       ",
        "    Work efficiency,         !- Name                                      ",
        "    Any Number,              !- Schedule Type Limits Name                 ",
        "    Through: 12/31,          !- Field 9                                   ",
        "    For: AllDays,            !- Field 10                                  ",
        "    Until: 24:00,0.0;         !- Field 11                                 ",
        "                                                                          ",
        " Output:Diagnostics, DisplayExtraWarnings;",
        " Timestep, 4;",
        " BUILDING, AirloopHVAC_VentilationRateProcedure, 0.0, Suburbs, .04, .4, FullExterior, 25, 6;",
        " SimulationControl, NO, NO, NO, YES, NO;",
        "ScheduleTypeLimits,",
        "  Any Number;              !- Name",
        "  Site:Location,",
        "    Miami Intl Ap FL USA TMY3 WMO=722020E,    !- Name",
        "    25.82,                 !- Latitude {deg}",
        "    -80.30,                !- Longitude {deg}",
        "    -5.00,                 !- Time Zone {hr}",
        "    11;                    !- Elevation {m}",

        "SizingPeriod:DesignDay,",
        " Miami Intl Ap Ann Clg .4% Condns DB/MCWB, !- Name",
        " 7,                        !- Month",
        " 21,                       !- Day of Month",
        " SummerDesignDay,          !- Day Type",
        " 31.7,                     !- Maximum Dry - Bulb Temperature{ C }",
        " 10.0,                      !- Daily Dry - Bulb Temperature Range{ deltaC }",
        " ,                         !- Dry - Bulb Temperature Range Modifier Type",
        " ,                         !- Dry - Bulb Temperature Range Modifier Day Schedule Name",
        " Wetbulb,                  !- Humidity Condition Type",
        " 22.7,                     !- Wetbulb or DewPoint at Maximum Dry - Bulb{ C }",
        " ,                         !- Humidity Condition Day Schedule Name",
        " ,                         !- Humidity Ratio at Maximum Dry - Bulb{ kgWater / kgDryAir }",
        " ,                         !- Enthalpy at Maximum Dry - Bulb{ J / kg }",
        " ,                         !- Daily Wet - Bulb Temperature Range{ deltaC }",
        " 101217.,                  !- Barometric Pressure{ Pa }",
        " 3.8,                      !- Wind Speed{ m / s }",
        " 340,                      !- Wind Direction{ deg }",
        " No,                       !- Rain Indicator",
        " No,                       !- Snow Indicator",
        " No,                       !- Daylight Saving Time Indicator",
        " ASHRAEClearSky,           !- Solar Model Indicator",
        " ,                         !- Beam Solar Day Schedule Name",
        " ,                         !- Diffuse Solar Day Schedule Name",
        " ,                         !- ASHRAE Clear Sky Optical Depth for Beam Irradiance( taub ) { dimensionless }",
        " ,                         !- ASHRAE Clear Sky Optical Depth for Diffuse Irradiance( taud ) { dimensionless }",
        " 1.00;                     !- Sky Clearness",

        "SizingPeriod:DesignDay,",
        " Miami Intl Ap Ann Htg 99.6% Condns DB, !- Name",
        " 1,                        !- Month",
        " 21,                       !- Day of Month",
        " WinterDesignDay,          !- Day Type",
        " 8.7,                      !- Maximum Dry - Bulb Temperature{ C }",
        " 0.0,                      !- Daily Dry - Bulb Temperature Range{ deltaC }",
        " ,                         !- Dry - Bulb Temperature Range Modifier Type",
        " ,                         !- Dry - Bulb Temperature Range Modifier Day Schedule Name",
        " Wetbulb,                  !- Humidity Condition Type",
        " 8.7,                      !- Wetbulb or DewPoint at Maximum Dry - Bulb{ C }",
        " ,                         !- Humidity Condition Day Schedule Name",
        " ,                         !- Humidity Ratio at Maximum Dry - Bulb{ kgWater / kgDryAir }",
        " ,                         !- Enthalpy at Maximum Dry - Bulb{ J / kg }",
        " ,                         !- Daily Wet - Bulb Temperature Range{ deltaC }",
        " 101217.,                  !- Barometric Pressure{ Pa }",
        " 3.8,                      !- Wind Speed{ m / s }",
        " 340,                      !- Wind Direction{ deg }",
        " No,                       !- Rain Indicator",
        " No,                       !- Snow Indicator",
        " No,                       !- Daylight Saving Time Indicator",
        " ASHRAEClearSky,           !- Solar Model Indicator",
        " ,                         !- Beam Solar Day Schedule Name",
        " ,                         !- Diffuse Solar Day Schedule Name",
        " ,                         !- ASHRAE Clear Sky Optical Depth for Beam Irradiance( taub ) { dimensionless }",
        " ,                         !- ASHRAE Clear Sky Optical Depth for Diffuse Irradiance( taud ) { dimensionless }",
        " 0.00;                     !- Sky Clearness",

        "Zone,",
        "  Space,                   !- Name",
        "  0.0000,                  !- Direction of Relative North {deg}",
        "  0.0000,                  !- X Origin {m}",
        "  0.0000,                  !- Y Origin {m}",
        "  0.0000,                  !- Z Origin {m}",
        "  1,                       !- Type",
        "  1,                       !- Multiplier",
        "  2.4,                     !- Ceiling Height {m}",
        "  ,                        !- Volume {m3}",
        "  autocalculate,           !- Floor Area {m2}",
        "  ,                        !- Zone Inside Convection Algorithm",
        "  ,                        !- Zone Outside Convection Algorithm",
        "  Yes;                     !- Part of Total Floor Area",

        "ZoneGroup,",
        " Zone Group,               !- Name",
        " Zone List,                !- Zone List Name",
        " 10;                       !- Zone List Multiplier",

        "ZoneList,",
        " Zone List,                !- Name",
        " Spacex10;                 !- Zone 1 Name",

        "Zone,",
        "  Spacex10,                !- Name",
        "  0.0000,                  !- Direction of Relative North {deg}",
        "  0.0000,                  !- X Origin {m}",
        "  0.0000,                  !- Y Origin {m}",
        "  0.0000,                  !- Z Origin {m}",
        "  1,                       !- Type",
        "  1,                       !- Multiplier",
        "  2.4,                     !- Ceiling Height {m}",
        "  ,                        !- Volume {m3}",
        "  autocalculate,           !- Floor Area {m2}",
        "  ,                        !- Zone Inside Convection Algorithm",
        "  ,                        !- Zone Outside Convection Algorithm",
        "  Yes;                     !- Part of Total Floor Area",

        "People,",
        " Spacex10 People,          !- Name",
        " Spacex10,                 !- Zone or ZoneList Name",
        " OnSched,                  !- Number of People Schedule Name",
        " people,                   !- Number of People Calculation Method",
        " 11,                       !- Number of People",
        " ,                         !- People per Zone Floor Area{ person / m2 }",
        " ,                         !- Zone Floor Area per Person{ m2 / person }",
        " 0.3,                      !- Fraction Radiant",
        " AutoCalculate,            !- Sensible Heat Fraction",
        " Activity Schedule;        !- Activity Level Schedule Name",

        "Lights,",
        " Space Lights,             !- Name",
        " Space,                    !- Zone or ZoneList Name",
        " OnSched,                  !- Schedule Name",
        " Watts/Area,               !- Design Level Calculation Method",
        " ,                         !- Lighting Level{ W }",
        " 10.0,                     !- Watts per Zone Floor Area{ W / m2 }",
        " ,                         !- Watts per Person{ W / person }",
        " 0.0,                      !- Return Air Fraction",
        " 0.59,                     !- Fraction Radiant",
        " 0.2,                      !- Fraction Visible",
        " 0,                        !- Fraction Replaceable",
        " GeneralLights;            !- End - Use Subcategory",

        "Lights,",
        " Space Lights x10,         !- Name",
        " Spacex10,                 !- Zone or ZoneList Name",
        " OnSched,                  !- Schedule Name",
        " Watts/Area,               !- Design Level Calculation Method",
        " ,                         !- Lighting Level{ W }",
        " 10.0,                     !- Watts per Zone Floor Area{ W / m2 }",
        " ,                         !- Watts per Person{ W / person }",
        " 0.0,                      !- Return Air Fraction",
        " 0.59,                     !- Fraction Radiant",
        " 0.2,                      !- Fraction Visible",
        " 0,                        !- Fraction Replaceable",
        " GeneralLights;            !- End - Use Subcategory",

        "ElectricEquipment,",
        " Space ElecEq,             !- Name",
        " Space,                    !- Zone or ZoneList Name",
        " OnSched,                  !- Schedule Name",
        " Watts/Area,               !- Design Level Calculation Method",
        " ,                         !- Design Level{ W }",
        " 20.0,                     !- Watts per Zone Floor Area{ W / m2 }",
        " ,                         !- Watts per Person{ W / person }",
        " 0.1,                      !- Fraction Latent",
        " 0.3,                      !- Fraction Radiant",
        " 0.1;                      !- Fraction Lost",

        "ElectricEquipment,",
        " Space ElecEq x10,         !- Name",
        " Spacex10,                 !- Zone or ZoneList Name",
        " OnSched,                  !- Schedule Name",
        " Watts/Area,               !- Design Level Calculation Method",
        " ,                         !- Design Level{ W }",
        " 20.0,                     !- Watts per Zone Floor Area{ W / m2 }",
        " ,                         !- Watts per Person{ W / person }",
        " 0.1,                      !- Fraction Latent",
        " 0.3,                      !- Fraction Radiant",
        " 0.1;                      !- Fraction Lost",

        "Schedule:Compact,",
        " OnSched,                  !- Name",
        " Fraction,                 !- Schedule Type Limits Name",
        " Through: 12/31,           !- Field 1",
        " For: AllDays,             !- Field 2",
        " Until: 24:00, 1.0;        !- Field 26",

        "ScheduleTypeLimits,",
        " Fraction,                 !- Name",
        " 0.0,                      !- Lower Limit Value",
        " 1.0,                      !- Upper Limit Value",
        " CONTINUOUS;               !- Numeric Type",

        "Construction,",
        " INT-WALL-1,               !- Name",
        " GP02,                     !- Outside Layer",
        " AL21,                     !- Layer 2",
        " GP02;                     !- Layer 3",

        "Material,",
        " GP02,                     !- Name",
        " MediumSmooth,             !- Roughness",
        " 1.5900001E-02,            !- Thickness{ m }",
        " 0.1600000,                !- Conductivity{ W / m - K }",
        " 801.0000,                 !- Density{ kg / m3 }",
        " 837.0000,                 !- Specific Heat{ J / kg - K }",
        " 0.9000000,                !- Thermal Absorptance",
        " 0.7500000,                !- Solar Absorptance",
        " 0.7500000;                !- Visible Absorptance",

        "Material:AirGap,",
        " AL21,                     !- Name",
        " 0.1570000;                !- Thermal Resistance{ m2 - K / W }",

        "Construction,",
        "FLOOR-SLAB-1,              !- Name",
        "CC03,                      !- Outside Layer",
        "CP01;                      !- Layer 2",

        "Material,",
        " CC03,                     !- Name",
        " MediumRough,              !- Roughness",
        " 0.1016000,                !- Thickness{ m }",
        " 1.310000,                 !- Conductivity{ W / m - K }",
        " 2243.000,                 !- Density{ kg / m3 }",
        " 837.0000,                 !- Specific Heat{ J / kg - K }",
        " 0.9000000,                !- Thermal Absorptance",
        " 0.6500000,                !- Solar Absorptance",
        " 0.6500000;                !- Visible Absorptance",

        "Material:NoMass,",
        " CP01,                     !- Name",
        " Rough,                    !- Roughness",
        " 0.3670000,                !- Thermal Resistance{ m2 - K / W }",
        " 0.9000000,                !- Thermal Absorptance",
        " 0.7500000,                !- Solar Absorptance",
        " 0.7500000;                !- Visible Absorptance",

        "Construction,",
        " CLNG-1,                   !- Name",
        " MAT-CLNG-1;               !- Outside Layer",

        "Material:NoMass,",
        " MAT-CLNG-1,               !- Name",
        " Rough,                    !- Roughness",
        " 0.652259290,              !- Thermal Resistance{ m2 - K / W }",
        " 0.65,                     !- Thermal Absorptance",
        " 0.65,                     !- Solar Absorptance",
        " 0.65;                     !- Visible Absorptance",

        "BuildingSurface:Detailed,",
        " FRONT-1,                  !- Name",
        " WALL,                     !- Surface Type",
        " INT-WALL-1,               !- Construction Name",
        " Space,                    !- Zone Name",
        " Outdoors,                 !- Outside Boundary Condition",
        " ,                         !- Outside Boundary Condition Object",
        " SunExposed,               !- Sun Exposure",
        " WindExposed,              !- Wind Exposure",
        " 0.50000,                  !- View Factor to Ground",
        " 4,                        !- Number of Vertices",
        " 0.0, 0.0, 2.4,            !- X, Y, Z == > Vertex 1 {m}",
        " 0.0, 0.0, 0.0,            !- X, Y, Z == > Vertex 2 {m}",
        " 30.5, 0.0, 0.0,           !- X, Y, Z == > Vertex 3 {m}",
        " 30.5, 0.0, 2.4;           !- X, Y, Z == > Vertex 4 {m}",

        "BuildingSurface:Detailed,",
        " C1-1,                     !- Name",
        " CEILING,                  !- Surface Type",
        " CLNG-1,                   !- Construction Name",
        " Space,                    !- Zone Name",
        " Outdoors,                 !- Outside Boundary Condition",
        " ,                         !- Outside Boundary Condition Object",
        " NoSun,                    !- Sun Exposure",
        " NoWind,                   !- Wind Exposure",
        " 0.0,                      !- View Factor to Ground",
        " 4,                        !- Number of Vertices",
        " 3.7, 3.7, 2.4,            !- X, Y, Z == > Vertex 1 {m}",
        " 0.0, 0.0, 2.4,            !- X, Y, Z == > Vertex 2 {m}",
        " 30.5, 0.0, 2.4,           !- X, Y, Z == > Vertex 3 {m}",
        " 26.8, 3.7, 2.4;           !- X, Y, Z == > Vertex 4 {m}",

        "BuildingSurface:Detailed,",
        " F1-1,                     !- Name",
        " FLOOR,                    !- Surface Type",
        " FLOOR-SLAB-1,             !- Construction Name",
        " Space,                    !- Zone Name",
        " Outdoors,                   !- Outside Boundary Condition",
        " ,                         !- Outside Boundary Condition Object",
        " NoSun,                    !- Sun Exposure",
        " NoWind,                   !- Wind Exposure",
        " 0.0,                      !- View Factor to Ground",
        " 4,                        !- Number of Vertices",
        " 26.8, 3.7, 0.0,           !- X, Y, Z == > Vertex 1 {m}",
        " 30.5, 0.0, 0.0,           !- X, Y, Z == > Vertex 2 {m}",
        " 0.0, 0.0, 0.0,            !- X, Y, Z == > Vertex 3 {m}",
        " 3.7, 3.7, 0.0;            !- X, Y, Z == > Vertex 4 {m}",

        "BuildingSurface:Detailed,",
        " SB12,                     !- Name",
        " WALL,                     !- Surface Type",
        " INT-WALL-1,               !- Construction Name",
        " Space,                    !- Zone Name",
        " Adiabatic,                !- Outside Boundary Condition",
        " ,                         !- Outside Boundary Condition Object",
        " NoSun,                    !- Sun Exposure",
        " NoWind,                   !- Wind Exposure",
        " 0.0,                      !- View Factor to Ground",
        " 4,                        !- Number of Vertices",
        " 30.5, 0.0, 2.4,           !- X, Y, Z == > Vertex 1 {m}",
        " 30.5, 0.0, 0.0,           !- X, Y, Z == > Vertex 2 {m}",
        " 26.8, 3.7, 0.0,           !- X, Y, Z == > Vertex 3 {m}",
        " 26.8, 3.7, 2.4;           !- X, Y, Z == > Vertex 4 {m}",

        "BuildingSurface:Detailed,",
        " SB14,                     !- Name",
        " WALL,                     !- Surface Type",
        " INT-WALL-1,               !- Construction Name",
        " Space,                    !- Zone Name",
        " Adiabatic,                !- Outside Boundary Condition",
        " ,                         !- Outside Boundary Condition Object",
        " NoSun,                    !- Sun Exposure",
        " NoWind,                   !- Wind Exposure",
        " 0.0,                      !- View Factor to Ground",
        " 4,                        !- Number of Vertices",
        " 3.7, 3.7, 2.4,            !- X, Y, Z == > Vertex 1 {m}",
        " 3.7, 3.7, 0.0,            !- X, Y, Z == > Vertex 2 {m}",
        " 0.0, 0.0, 0.0,            !- X, Y, Z == > Vertex 3 {m}",
        " 0.0, 0.0, 2.4;            !- X, Y, Z == > Vertex 4 {m}",

        "BuildingSurface:Detailed,",
        " SB15,                     !- Name",
        " WALL,                     !- Surface Type",
        " INT-WALL-1,               !- Construction Name",
        " Space,                    !- Zone Name",
        " Adiabatic,                !- Outside Boundary Condition",
        " ,                         !- Outside Boundary Condition Object",
        " NoSun,                    !- Sun Exposure",
        " NoWind,                   !- Wind Exposure",
        " 0.0,                      !- View Factor to Ground",
        " 4,                        !- Number of Vertices",
        " 26.8, 3.7, 2.4,           !- X, Y, Z == > Vertex 1 {m}",
        " 26.8, 3.7, 0.0,           !- X, Y, Z == > Vertex 2 {m}",
        " 3.7, 3.7, 0.0,            !- X, Y, Z == > Vertex 3 {m}",
        " 3.7, 3.7, 2.4;            !- X, Y, Z == > Vertex 4 {m}",

        "BuildingSurface:Detailed,",
        " FRONT-1x10,               !- Name",
        " WALL,                     !- Surface Type",
        " INT-WALL-1,               !- Construction Name",
        " Spacex10,                 !- Zone Name",
        " Outdoors,                 !- Outside Boundary Condition",
        " ,                         !- Outside Boundary Condition Object",
        " SunExposed,               !- Sun Exposure",
        " WindExposed,              !- Wind Exposure",
        " 0.50000,                  !- View Factor to Ground",
        " 4,                        !- Number of Vertices",
        " 0.0, 0.0, 2.4,            !- X, Y, Z == > Vertex 1 {m}",
        " 0.0, 0.0, 0.0,            !- X, Y, Z == > Vertex 2 {m}",
        " 30.5, 0.0, 0.0,           !- X, Y, Z == > Vertex 3 {m}",
        " 30.5, 0.0, 2.4;           !- X, Y, Z == > Vertex 4 {m}",

        "BuildingSurface:Detailed,",
        " C1-1x10,                  !- Name",
        " CEILING,                  !- Surface Type",
        " CLNG-1,                   !- Construction Name",
        " Spacex10,                 !- Zone Name",
        " Outdoors,                 !- Outside Boundary Condition",
        " ,                         !- Outside Boundary Condition Object",
        " NoSun,                    !- Sun Exposure",
        " NoWind,                   !- Wind Exposure",
        " 0.0,                      !- View Factor to Ground",
        " 4,                        !- Number of Vertices",
        " 3.7, 3.7, 2.4,            !- X, Y, Z == > Vertex 1 {m}",
        " 0.0, 0.0, 2.4,            !- X, Y, Z == > Vertex 2 {m}",
        " 30.5, 0.0, 2.4,           !- X, Y, Z == > Vertex 3 {m}",
        " 26.8, 3.7, 2.4;           !- X, Y, Z == > Vertex 4 {m}",

        "BuildingSurface:Detailed,",
        " F1-1x10,                  !- Name",
        " FLOOR,                    !- Surface Type",
        " FLOOR-SLAB-1,             !- Construction Name",
        " Spacex10,                 !- Zone Name",
        " Outdoors,                   !- Outside Boundary Condition",
        " ,                         !- Outside Boundary Condition Object",
        " NoSun,                    !- Sun Exposure",
        " NoWind,                   !- Wind Exposure",
        " 0.0,                      !- View Factor to Ground",
        " 4,                        !- Number of Vertices",
        " 26.8, 3.7, 0.0,           !- X, Y, Z == > Vertex 1 {m}",
        " 30.5, 0.0, 0.0,           !- X, Y, Z == > Vertex 2 {m}",
        " 0.0, 0.0, 0.0,            !- X, Y, Z == > Vertex 3 {m}",
        " 3.7, 3.7, 0.0;            !- X, Y, Z == > Vertex 4 {m}",

        "BuildingSurface:Detailed,",
        " SB12x10,                  !- Name",
        " WALL,                     !- Surface Type",
        " INT-WALL-1,               !- Construction Name",
        " Spacex10,                 !- Zone Name",
        " Adiabatic,                !- Outside Boundary Condition",
        " ,                         !- Outside Boundary Condition Object",
        " NoSun,                    !- Sun Exposure",
        " NoWind,                   !- Wind Exposure",
        " 0.0,                      !- View Factor to Ground",
        " 4,                        !- Number of Vertices",
        " 30.5, 0.0, 2.4,           !- X, Y, Z == > Vertex 1 {m}",
        " 30.5, 0.0, 0.0,           !- X, Y, Z == > Vertex 2 {m}",
        " 26.8, 3.7, 0.0,           !- X, Y, Z == > Vertex 3 {m}",
        " 26.8, 3.7, 2.4;           !- X, Y, Z == > Vertex 4 {m}",

        "BuildingSurface:Detailed,",
        " SB14x10,                  !- Name",
        " WALL,                     !- Surface Type",
        " INT-WALL-1,               !- Construction Name",
        " Spacex10,                 !- Zone Name",
        " Adiabatic,                !- Outside Boundary Condition",
        " ,                         !- Outside Boundary Condition Object",
        " NoSun,                    !- Sun Exposure",
        " NoWind,                   !- Wind Exposure",
        " 0.0,                      !- View Factor to Ground",
        " 4,                        !- Number of Vertices",
        " 3.7, 3.7, 2.4,            !- X, Y, Z == > Vertex 1 {m}",
        " 3.7, 3.7, 0.0,            !- X, Y, Z == > Vertex 2 {m}",
        " 0.0, 0.0, 0.0,            !- X, Y, Z == > Vertex 3 {m}",
        " 0.0, 0.0, 2.4;            !- X, Y, Z == > Vertex 4 {m}",

        "BuildingSurface:Detailed,",
        " SB15x10,                  !- Name",
        " WALL,                     !- Surface Type",
        " INT-WALL-1,               !- Construction Name",
        " Spacex10,                 !- Zone Name",
        " Adiabatic,                !- Outside Boundary Condition",
        " ,                         !- Outside Boundary Condition Object",
        " NoSun,                    !- Sun Exposure",
        " NoWind,                   !- Wind Exposure",
        " 0.0,                      !- View Factor to Ground",
        " 4,                        !- Number of Vertices",
        " 26.8, 3.7, 2.4,           !- X, Y, Z == > Vertex 1 {m}",
        " 26.8, 3.7, 0.0,           !- X, Y, Z == > Vertex 2 {m}",
        " 3.7, 3.7, 0.0,            !- X, Y, Z == > Vertex 3 {m}",
        " 3.7, 3.7, 2.4;            !- X, Y, Z == > Vertex 4 {m}",

        "Output:Table:SummaryReports,",
        "  AllSummary; !- Report 1 Name",
        " ",

    });

    ASSERT_TRUE(process_idf(idf_objects));

    state->dataGlobal->DDOnlySimulation = true;

    ManageSimulation(*state);

    CalcThermalComfortFanger(*state);

    EXPECT_NEAR(state->dataHeatBalFanSys->ZTAVComf(1), 14.863733439268286, 0.001);

    EXPECT_NEAR(state->dataHeatBalFanSys->ZoneAirHumRatAvgComf(1), 0.010564839505489259, 0.0001);

    EXPECT_NEAR(state->dataThermalComforts->ThermalComfortData(1).FangerPMV, -5.5896341565108720, 0.001);

    EXPECT_NEAR(state->dataThermalComforts->ThermalComfortData(1).FangerPPD, 100.0, 0.001);
}
