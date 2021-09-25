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

// EnergyPlus::UserDefinedComponents unit tests

// Google Test Headers
#include "Fixtures/EnergyPlusFixture.hh"
#include <EnergyPlus/Data/EnergyPlusData.hh>
#include <EnergyPlus/EMSManager.hh>
#include <EnergyPlus/ScheduleManager.hh>
#include <EnergyPlus/UserDefinedComponents.hh>
#include <gtest/gtest.h>

namespace EnergyPlus {

TEST_F(EnergyPlusFixture, UserCoilComponentStructReport)
{

    std::string const idf_objects = delimited_string({

        "SetpointManager:Scheduled,",
        "OA Air Temp Manager 1,   !- Name",
        "Temperature,             !- Control Variable",
        "OA Cooling Supply Air Temp Sch,  !- Schedule Name",
        "OA Mixing Box 2 Inlet Node;  !- Setpoint Node or NodeList Name",

        "Schedule:Compact,",
        "OA Cooling Supply Air Temp Sch,  !- Name",
        "Temperature,             !- Schedule Type Limits Name",
        "Through: 12/31,          !- Field 1",
        "For: AllDays,            !- Field 2",
        "Until: 24:00,11.5;       !- Field 3",

        "Schedule:Compact,",
        "OA Heating Supply Air Temp Sch,  !- Name",
        "Temperature,             !- Schedule Type Limits Name",
        "Through: 12/31,          !- Field 1",
        "For: AllDays,            !- Field 2",
        "Until: 24:00,4.5;        !- Field 3",

        "SetpointManager:Scheduled,",
        "OA Air Temp Manager 2,   !- Name",
        "Temperature,             !- Control Variable",
        "OA Heating Supply Air Temp Sch,  !- Schedule Name",
        "OA Heating Coil 2 Air Outlet Node;  !- Setpoint Node or NodeList Name",

        "EnergyManagementSystem:Program,",
        "OA_CWCoil_ModelInput,    !- Name",
        "Set Air_Mdot       = OACoil_Air_Mdot,  !- Program Line 1",
        "Set Air_Tsp        = OACoil_Air_Tsp,  !- Program Line 2",
        "Set Water_Mdot     = OACoil_Water_Mdot,  !- <none>",
        "Set Water_cp       = OACoil_Water_cp,  !- <none>",
        "Set Air_Tinlet     = OACoil_Air_Tinlet,  !- <none>",
        "Set Water_Tinlet   = OACoil_Water_Tinlet,  !- <none>",
        "Set Air_Winlet     = OACoil_Air_Winlet,  !- <none>",
        "Set Water_Mdot_Max = OACoil_Mdot_Max,  !- <none>",
        "Set Coil_Eff       = 0.68;  !- <none>",

        "EnergyManagementSystem:Program,",
        "CW_Coil_Sim_Model,       !- Name",
        "IF Air_Mdot == 0.0,      !- Program Line 1",
        "SET Air_Tout = Air_Tinlet,  !- Program Line 2",
        "SET Air_Wout = Air_Winlet,  !- <none>",
        "Set Air_MdotOut = Air_Mdot,  !- <none>",
        "Set tot_cool_Power = 0.0,!- <none>",
        "Set Water_Tout = Water_Tinlet,  !- <none>",
        "Set Water_MdotRequest = 0.0,  !- <none>",
        "RETURN,                  !- <none>",
        "ENDIF,                   !- <none>",
        "Set AirIn_Enth     = @HFnTdbW Air_Tinlet Air_Winlet,  !- <none>",
        "Set Air_Wout_Limit = @WFnTdbRhPb Water_Tinlet 1 OA_Press,  !- <none>",
        "Set AirOut_Enth_Limit = @HFnTdbW Water_Tinlet Air_Wout_Limit,  !- <none>",
        "Set MaxQdot      = Coil_Eff * Air_Mdot * (AirIn_Enth - AirOut_Enth_Limit),  !- <none>",
        "IF MaxQdot <= 0.0,       !- <none>",
        "SET Air_Tout          = Air_Tinlet,  !- <none>",
        "SET Air_Wout          = Air_Winlet,  !- <none>",
        "Set Air_MdotOut       = Air_Mdot,  !- <none>",
        "Set tot_cool_Power    = 0.0,  !- <none>",
        "Set Water_Tout        = Water_Tinlet,  !- <none>",
        "Set Water_MdotRequest = 0.0,  !- <none>",
        "RETURN,                  !- <none>",
        "ENDIF,                   !- <none>",
        "Set AirIn_dewpoint = @TdpFnWpb Air_Winlet OA_Press,  !- <none>",
        "IF  AirIn_dewpoint < Air_Tsp,  !- <none>",
        "Set Air_Wout = Air_Winlet,  !- <none>",
        "Set Air_HoutSP = @HFnTdbW Air_Tsp Air_Wout,  !- <none>",
        "Set Qdot_SP    = Air_Mdot * (AirIn_Enth - Air_HoutSP),  !- <none>",
        "IF Qdot_SP > MaxQdot,    !- <none>",
        "Set Air_Hout = AirIn_Enth - (MaxQdot/Air_Mdot),  !- <none>",
        "ELSE,                    !- <none>",
        "Set Air_Hout = Air_HoutSP,  !- <none>",
        "ENDIF,                   !- <none>",
        "ELSEIF AirIn_dewpoint > Air_Tsp,  !- <none>",
        "Set Air_WoutSP = @WFnTdbRhPb Air_Tsp 1 OA_Press,  !- <none>",
        "Set Air_HoutSP = @HFnTdbW Air_Tsp Air_WoutSP,  !- <none>",
        "Set Qdot_SP    = Air_Mdot * (AirIn_Enth - Air_HoutSP),  !- <none>",
        "IF Qdot_SP > MaxQdot,    !- <none>",
        "set Air_Hout = AirIn_Enth - (MaxQdot/Air_Mdot),  !- <none>",
        "set Air_Tsat = @TsatFnHPb Air_Hout OA_Press,  !- <none>",
        "SEt Air_Wout = @WFnTdbRhPb Air_Tsat 1 OA_Press,  !- <none>",
        "ELSE,                    !- <none>",
        "set Air_Hout = Air_HoutSP,  !- <none>",
        "SEt Air_Wout = Air_WoutSP,  !- <none>",
        "ENDIF,                   !- <none>",
        "ENDIF,                   !- <none>",
        "Set Air_Tout = @TdbFnHW Air_Hout Air_Wout,  !- <none>",
        "Set ActualQdot  = Air_Mdot * (AirIn_Enth   - Air_Hout),  !- <none>",
        "Set Water_partLoad = ActualQdot / MaxQdot,  !- <none>",
        "Set Water_MdotRequest = Water_Mdot_Max * Water_PartLoad,  !- <none>",
        "IF (Water_Mdot > 0.0),   !- <none>",
        "Set CW_tempRise = ActualQdot/(Water_Mdot * Water_cp),  !- <none>",
        "ELSE,                    !- <none>",
        "Set CW_tempRise = ActualQdot/(Water_MdotRequest * Water_cp),  !- <none>",
        "ENDIF,                   !- <none>",
        "Set Water_Tout = Water_Tinlet + CW_tempRise,  !- <none>",
        "Set tot_cool_Power = ActualQdot;  !- <none>",

        "EnergyManagementSystem:Program,",
        "OA_CWCoil_ModelOuput,    !- Name",
        "Set OACoil_Water_MdotRequest = Water_MdotRequest,  !- Program Line 1",
        "Set OACoil_Air_Tout          = Air_Tout,  !- Program Line 2",
        "Set OACoil_Air_Wout          = Air_Wout,  !- <none>",
        "Set OACoil_tot_cool_Power    = Tot_Cool_Power,  !- <none>",
        "Set OACoil_tot_cool_Energy   = Tot_Cool_Power * SystemTimestep * 3600,  !- <none>",
        "Set OACoil_Air_MdotOut       = Air_Mdot,  !- <none>",
        "Set OACoil_Water_Tout        = Water_Tout;  !- <none>",

        "EnergyManagementSystem:Program,",
        "InitOASimpleCoilModel,   !- Name",
        "Set CWLoopExitTemp = 7.0,!- Program Line 1",
        "Set CWLoopDeltaTemp = 0.5 * 4.0,  !- Program Line 2",
        "Set CW_CP = @CPCW CWLoopExitTemp,  !- <none>",
        "Set CW_rho = @RhoH2O CWLoopExitTemp,  !- <none>",
        "Set Dummy = OA_Design_Tdb,  !- <none>",
        "Set Dummy = OA_Design_W, !- <none>",
        "Set Dummy = OA_Design_Vdot,  !- <none>",
        "Set Houtdoor = @HFnTdbW OA_Design_Tdb OA_Design_W,  !- <none>",
        "Set OA_rho = @RhoAirFnPbTdbW OA_Press OA_Design_Tdb OA_Design_W,  !- <none>",
        "Set OA_mdot = OA_Design_Vdot * OA_rho,  !- <none>",
        "Set Hleaving = @HFnTdbW 11.0 0.008,  !- <none>",
        "Set OACoil_CoolDes_Qdot = OA_mdot * ( Houtdoor - Hleaving),  !- <none>",
        "Set OACoil_Water_DesignVdot = OACoil_CoolDes_Qdot / (( CW_rho * CW_CP) * CWLoopDeltaTemp),  !- <none>",
        "Set OACoil_Mdot_Min = 0.0,  !- <none>",
        "Set OACoil_Mdot_Max = OACoil_Water_DesignVdot * CW_rho;  !- <none>",

        "EnergyManagementSystem:ProgramCallingManager,",
        "OA Cooling Coil Init Program Manager,  !- Name",
        "UserDefinedComponentModel,  !- EnergyPlus Model Calling Point",
        "InitOASimpleCoilModel;   !- Program Name 1",

        "EnergyManagementSystem:ProgramCallingManager,",
        "OA Cooling Coil Model Program Manager,  !- Name",
        "UserDefinedComponentModel,  !- EnergyPlus Model Calling Point",
        "OA_CWCoil_ModelInput,    !- Program Name 1",
        "CW_Coil_Sim_Model,       !- Program Name 2",
        "OA_CWCoil_ModelOuput;    !- Program Name 3",

        // Just to offset Coil:UserDefined by one - Doesn't work
        //        "Coil:UserDefined,",
        //        "OA Cooling Coil 1,       !- Name",
        //        "OA Cooling Coil Model Program Manager,  !- Overall Model Simulation Program Calling Manager Name",
        //        "OA Cooling Coil Init Program Manager,  !- Model Setup and Sizing Program Calling Manager Name",
        //        "1,                         !- Number of Air Connections",
        //        "OA Heating Coil 1 Air Outlet Node,                         !- Air Connection 1 Inlet Node Name",
        //        "OA Mixing Box 1 Inlet Node,                         !- Air Connection 1 Outlet Node Name",
        //        ",                        !- Air Connection 2 Inlet Node Name,",
        //        ",                        !- Air Connection 2 Outlet Node Name,",
        //        "Yes,                     !- Plant Connection is Used",
        //        "OA Cooling Coil 1 Water Inlet Node,  !- Plant Connection Inlet Node Name",
        //        "OA Cooling Coil 1 Water Outlet Node;  !- Plant Connection Outlet Node Name",

        "Coil:UserDefined,",
        "OA Cooling Coil 2,       !- Name",
        "OA Cooling Coil Model Program Manager,  !- Overall Model Simulation Program Calling Manager Name",
        "OA Cooling Coil Init Program Manager,  !- Model Setup and Sizing Program Calling Manager Name",
        "1,                         !- Number of Air Connections",
        "OA Heating Coil 2 Air Outlet Node,                         !- Air Connection 1 Inlet Node Name",
        "OA Mixing Box 2 Inlet Node,                         !- Air Connection 1 Outlet Node Name",
        "C,                        !- Air Connection 2 Inlet Node Name,",
        "D,                        !- Air Connection 2 Outlet Node Name,",
        "Yes,                     !- Plant Connection is Used",
        "OA Cooling Coil 1 Water Inlet Node,  !- Plant Connection Inlet Node Name",
        "OA Cooling Coil 1 Water Outlet Node;  !- Plant Connection Outlet Node Name",

    });

    ASSERT_TRUE(process_idf(idf_objects));

    EMSManager::CheckIfAnyEMS(*state);
    EMSManager::GetEMSInput(*state);
    UserDefinedComponents::GetUserDefinedComponents(*state);
    ScheduleManager::ProcessScheduleInput(*state); // read schedules

    auto num = state->dataUserDefinedComponents->UserCoil.size();
    state->dataUserDefinedComponents->UserCoil(1).report(*state);
}

TEST_F(EnergyPlusFixture, GetUserDefinedCoilIndexTest)
{

    std::string const idf_objects = delimited_string({

        "EnergyManagementSystem:Program,",
        "OA_CWCoil_ModelInput,    !- Name",
        "Set Air_Mdot       = OACoil_Air_Mdot,  !- Program Line 1",
        "Set Air_Tsp        = OACoil_Air_Tsp,  !- Program Line 2",
        "Set Water_Mdot     = OACoil_Water_Mdot,  !- <none>",
        "Set Water_cp       = OACoil_Water_cp,  !- <none>",
        "Set Air_Tinlet     = OACoil_Air_Tinlet,  !- <none>",
        "Set Water_Tinlet   = OACoil_Water_Tinlet,  !- <none>",
        "Set Air_Winlet     = OACoil_Air_Winlet,  !- <none>",
        "Set Water_Mdot_Max = OACoil_Mdot_Max,  !- <none>",
        "Set Coil_Eff       = 0.68;  !- <none>",

        "EnergyManagementSystem:Program,",
        "CW_Coil_Sim_Model,       !- Name",
        "IF Air_Mdot == 0.0,      !- Program Line 1",
        "SET Air_Tout = Air_Tinlet,  !- Program Line 2",
        "SET Air_Wout = Air_Winlet,  !- <none>",
        "Set Air_MdotOut = Air_Mdot,  !- <none>",
        "Set tot_cool_Power = 0.0,!- <none>",
        "Set Water_Tout = Water_Tinlet,  !- <none>",
        "Set Water_MdotRequest = 0.0,  !- <none>",
        "RETURN,                  !- <none>",
        "ENDIF,                   !- <none>",
        "Set AirIn_Enth     = @HFnTdbW Air_Tinlet Air_Winlet,  !- <none>",
        "Set Air_Wout_Limit = @WFnTdbRhPb Water_Tinlet 1 OA_Press,  !- <none>",
        "Set AirOut_Enth_Limit = @HFnTdbW Water_Tinlet Air_Wout_Limit,  !- <none>",
        "Set MaxQdot      = Coil_Eff * Air_Mdot * (AirIn_Enth - AirOut_Enth_Limit),  !- <none>",
        "IF MaxQdot <= 0.0,       !- <none>",
        "SET Air_Tout          = Air_Tinlet,  !- <none>",
        "SET Air_Wout          = Air_Winlet,  !- <none>",
        "Set Air_MdotOut       = Air_Mdot,  !- <none>",
        "Set tot_cool_Power    = 0.0,  !- <none>",
        "Set Water_Tout        = Water_Tinlet,  !- <none>",
        "Set Water_MdotRequest = 0.0,  !- <none>",
        "RETURN,                  !- <none>",
        "ENDIF,                   !- <none>",
        "Set AirIn_dewpoint = @TdpFnWpb Air_Winlet OA_Press,  !- <none>",
        "IF  AirIn_dewpoint < Air_Tsp,  !- <none>",
        "Set Air_Wout = Air_Winlet,  !- <none>",
        "Set Air_HoutSP = @HFnTdbW Air_Tsp Air_Wout,  !- <none>",
        "Set Qdot_SP    = Air_Mdot * (AirIn_Enth - Air_HoutSP),  !- <none>",
        "IF Qdot_SP > MaxQdot,    !- <none>",
        "Set Air_Hout = AirIn_Enth - (MaxQdot/Air_Mdot),  !- <none>",
        "ELSE,                    !- <none>",
        "Set Air_Hout = Air_HoutSP,  !- <none>",
        "ENDIF,                   !- <none>",
        "ELSEIF AirIn_dewpoint > Air_Tsp,  !- <none>",
        "Set Air_WoutSP = @WFnTdbRhPb Air_Tsp 1 OA_Press,  !- <none>",
        "Set Air_HoutSP = @HFnTdbW Air_Tsp Air_WoutSP,  !- <none>",
        "Set Qdot_SP    = Air_Mdot * (AirIn_Enth - Air_HoutSP),  !- <none>",
        "IF Qdot_SP > MaxQdot,    !- <none>",
        "set Air_Hout = AirIn_Enth - (MaxQdot/Air_Mdot),  !- <none>",
        "set Air_Tsat = @TsatFnHPb Air_Hout OA_Press,  !- <none>",
        "SEt Air_Wout = @WFnTdbRhPb Air_Tsat 1 OA_Press,  !- <none>",
        "ELSE,                    !- <none>",
        "set Air_Hout = Air_HoutSP,  !- <none>",
        "SEt Air_Wout = Air_WoutSP,  !- <none>",
        "ENDIF,                   !- <none>",
        "ENDIF,                   !- <none>",
        "Set Air_Tout = @TdbFnHW Air_Hout Air_Wout,  !- <none>",
        "Set ActualQdot  = Air_Mdot * (AirIn_Enth   - Air_Hout),  !- <none>",
        "Set Water_partLoad = ActualQdot / MaxQdot,  !- <none>",
        "Set Water_MdotRequest = Water_Mdot_Max * Water_PartLoad,  !- <none>",
        "IF (Water_Mdot > 0.0),   !- <none>",
        "Set CW_tempRise = ActualQdot/(Water_Mdot * Water_cp),  !- <none>",
        "ELSE,                    !- <none>",
        "Set CW_tempRise = ActualQdot/(Water_MdotRequest * Water_cp),  !- <none>",
        "ENDIF,                   !- <none>",
        "Set Water_Tout = Water_Tinlet + CW_tempRise,  !- <none>",
        "Set tot_cool_Power = ActualQdot;  !- <none>",

        "EnergyManagementSystem:Program,",
        "OA_CWCoil_ModelOuput,    !- Name",
        "Set OACoil_Water_MdotRequest = Water_MdotRequest,  !- Program Line 1",
        "Set OACoil_Air_Tout          = Air_Tout,  !- Program Line 2",
        "Set OACoil_Air_Wout          = Air_Wout,  !- <none>",
        "Set OACoil_tot_cool_Power    = Tot_Cool_Power,  !- <none>",
        "Set OACoil_tot_cool_Energy   = Tot_Cool_Power * SystemTimestep * 3600,  !- <none>",
        "Set OACoil_Air_MdotOut       = Air_Mdot,  !- <none>",
        "Set OACoil_Water_Tout        = Water_Tout;  !- <none>",

        "EnergyManagementSystem:Program,",
        "InitOASimpleCoilModel,   !- Name",
        "Set CWLoopExitTemp = 7.0,!- Program Line 1",
        "Set CWLoopDeltaTemp = 0.5 * 4.0,  !- Program Line 2",
        "Set CW_CP = @CPCW CWLoopExitTemp,  !- <none>",
        "Set CW_rho = @RhoH2O CWLoopExitTemp,  !- <none>",
        "Set Dummy = OA_Design_Tdb,  !- <none>",
        "Set Dummy = OA_Design_W, !- <none>",
        "Set Dummy = OA_Design_Vdot,  !- <none>",
        "Set Houtdoor = @HFnTdbW OA_Design_Tdb OA_Design_W,  !- <none>",
        "Set OA_rho = @RhoAirFnPbTdbW OA_Press OA_Design_Tdb OA_Design_W,  !- <none>",
        "Set OA_mdot = OA_Design_Vdot * OA_rho,  !- <none>",
        "Set Hleaving = @HFnTdbW 11.0 0.008,  !- <none>",
        "Set OACoil_CoolDes_Qdot = OA_mdot * ( Houtdoor - Hleaving),  !- <none>",
        "Set OACoil_Water_DesignVdot = OACoil_CoolDes_Qdot / (( CW_rho * CW_CP) * CWLoopDeltaTemp),  !- <none>",
        "Set OACoil_Mdot_Min = 0.0,  !- <none>",
        "Set OACoil_Mdot_Max = OACoil_Water_DesignVdot * CW_rho;  !- <none>",

        "EnergyManagementSystem:ProgramCallingManager,",
        "OA Cooling Coil Init Program Manager,  !- Name",
        "UserDefinedComponentModel,  !- EnergyPlus Model Calling Point",
        "InitOASimpleCoilModel;   !- Program Name 1",

        "EnergyManagementSystem:ProgramCallingManager,",
        "OA Cooling Coil Model Program Manager,  !- Name",
        "UserDefinedComponentModel,  !- EnergyPlus Model Calling Point",
        "OA_CWCoil_ModelInput,    !- Program Name 1",
        "CW_Coil_Sim_Model,       !- Program Name 2",
        "OA_CWCoil_ModelOuput;    !- Program Name 3",

        "Coil:UserDefined,",
        "OA Cooling Coil 1,       !- Name",
        "OA Cooling Coil Model Program Manager,  !- Overall Model Simulation Program Calling Manager Name",
        "OA Cooling Coil Init Program Manager,  !- Model Setup and Sizing Program Calling Manager Name",
        "1,                         !- Number of Air Connections",
        "OA Heating Coil 1 Air Outlet Node,                         !- Air Connection 1 Inlet Node Name",
        "OA Mixing Box 1 Inlet Node,                         !- Air Connection 1 Outlet Node Name",
        ",                        !- Air Connection 2 Inlet Node Name,",
        ",                        !- Air Connection 2 Outlet Node Name,",
        "Yes,                     !- Plant Connection is Used",
        "OA Cooling Coil 1 Water Inlet Node,  !- Plant Connection Inlet Node Name",
        "OA Cooling Coil 1 Water Outlet Node;  !- Plant Connection Outlet Node Name",

        "Coil:UserDefined,",
        "OA Cooling Coil 2,       !- Name",
        "OA Cooling Coil Model Program Manager,  !- Overall Model Simulation Program Calling Manager Name",
        "OA Cooling Coil Init Program Manager,  !- Model Setup and Sizing Program Calling Manager Name",
        "1,                         !- Number of Air Connections",
        "A,                         !- Air Connection 1 Inlet Node Name",
        "B,                         !- Air Connection 1 Outlet Node Name",
        "C,                        !- Air Connection 2 Inlet Node Name,",
        "D,                        !- Air Connection 2 Outlet Node Name,",
        "Yes,                     !- Plant Connection is Used",
        "OA Cooling Coil 1 Water Inlet Node,  !- Plant Connection Inlet Node Name",
        "OA Cooling Coil 1 Water Outlet Node;  !- Plant Connection Outlet Node Name",

        /// Take 2

    });

    ASSERT_TRUE(process_idf(idf_objects));
    EMSManager::CheckIfAnyEMS(*state);
    EMSManager::GetEMSInput(*state);

    int ExpectedCoilIndex{2};

    std::string CoilName{"OA Cooling Coil 3"};
    int CoilIndex{}; // return value
    bool errFlag{};  // Default value as false
    std::string cCurrentModuleObject{"Coil:UserDefined"};

    UserDefinedComponents::GetUserDefinedCoilIndex(*state, CoilName, CoilIndex, errFlag, cCurrentModuleObject);

    std::string const error_string = delimited_string({
        "   ** Severe  ** Coil:UserDefined, GetUserDefinedCoilIndex: User Defined Cooling Coil not found=OA Cooling Coil 3",
    });

    EXPECT_TRUE(compare_err_stream(error_string, true));

    CoilName = "OA Cooling Coil 2";
    errFlag = false;
    UserDefinedComponents::GetUserDefinedCoilIndex(*state, CoilName, CoilIndex, errFlag, cCurrentModuleObject);

    EXPECT_EQ(CoilIndex, ExpectedCoilIndex);

    int CoilInletAirNode{-95555};
    int ExpectedCoilInletAirNode{5};
    UserDefinedComponents::GetUserDefinedCoilAirInletNode(*state, CoilName, CoilInletAirNode, errFlag, cCurrentModuleObject);

    EXPECT_EQ(CoilInletAirNode, ExpectedCoilInletAirNode);

    int CoilAirOutletNode{75};
    int ExpectedCoilAirOutletNode{6};
    UserDefinedComponents::GetUserDefinedCoilAirOutletNode(*state, CoilName, CoilAirOutletNode, errFlag, cCurrentModuleObject);

    EXPECT_EQ(CoilAirOutletNode, ExpectedCoilAirOutletNode);
    EXPECT_FALSE(errFlag);
}
} // namespace EnergyPlus