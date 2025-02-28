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

#ifndef ChilledCeilingPanelSimple_hh_INCLUDED
#define ChilledCeilingPanelSimple_hh_INCLUDED

// ObjexxFCL Headers
#include <ObjexxFCL/Array1D.hh>

// EnergyPlus Headers
#include <EnergyPlus/Data/BaseData.hh>
#include <EnergyPlus/DataGlobals.hh>
#include <EnergyPlus/EnergyPlus.hh>

namespace EnergyPlus {

// Forward declarations
struct EnergyPlusData;

namespace CoolingPanelSimple {

    // Control types:
    enum class Control {
        Unassigned,
        MAT,                // Controls system using mean air temperature
        MRT,                // Controls system using mean radiant temperature
        Operative,          // Controls system using operative temperature
        ODB,                // Controls system using outside air dry-bulb temperature
        OWB,                // Controls system using outside air wet-bulb temperature
        ZoneTotalLoad,      // Controls system using zone total remaining load
        ZoneConvectiveLoad  // Controls system using zone convective remaining load
    };

    // Condensation control types:
    enum class CondCtrl {
        NONE,               // Condensation control--none, so system never shuts down
        SIMPLEOFF,          // Condensation control--simple off, system shuts off when condensation predicted
        VARIEDOFF           // Condensation control--variable off, system modulates to keep running if possible
    };

    struct CoolingPanelParams
    {
        // Members
        std::string EquipID;
        int EquipType;
        std::string Schedule;
        Array1D_string SurfaceName;
        Array1D_int SurfacePtr;
        int ZonePtr;
        int SchedPtr;
        int WaterInletNode;
        int WaterOutletNode;
        int TotSurfToDistrib;
        int ControlCompTypeNum;
        int CompErrIndex;
        Control ControlType;
        std::string ColdSetptSched;
        int ColdSetptSchedPtr;
        CondCtrl CondCtrlType;
        Real64 CondDewPtDeltaT;
        int CondErrIndex;
        Real64 ColdThrottlRange;
        Real64 RatedWaterTemp;
        int CoolingCapMethod;
        Real64 ScaledCoolingCapacity;
        Real64 UA;
        Real64 Offset;
        Real64 WaterMassFlowRate;
        Real64 WaterMassFlowRateMax;
        Real64 RatedWaterFlowRate;
        Real64 WaterVolFlowRateMax;
        Real64 WaterInletTempStd;
        Real64 WaterInletTemp;
        Real64 WaterInletEnthalpy;
        Real64 WaterOutletTempStd;
        Real64 WaterOutletTemp;
        Real64 WaterOutletEnthalpy;
        Real64 RatedZoneAirTemp;
        Real64 FracRadiant;
        Real64 FracConvect;
        Real64 FracDistribPerson;
        Array1D<Real64> FracDistribToSurf;
        Real64 TotPower;
        Real64 Power;
        Real64 ConvPower;
        Real64 RadPower;
        Real64 TotEnergy;
        Real64 Energy;
        Real64 ConvEnergy;
        Real64 RadEnergy;
        int LoopNum;     // plant loop index
        int LoopSideNum; // plant loop side index
        int BranchNum;   // plant loop branch index
        int CompNum;     // plant loop component index
        int CoolingPanelLoadReSimIndex;
        int CoolingPanelMassFlowReSimIndex;
        int CoolingPanelInletTempFlowReSimIndex;

        // Default Constructor
        CoolingPanelParams()
            : EquipType(0), ZonePtr(0), SchedPtr(0), WaterInletNode(0), WaterOutletNode(0), TotSurfToDistrib(0), ControlCompTypeNum(0),
              CompErrIndex(0), ControlType(Control::Unassigned), ColdSetptSchedPtr(0), CondCtrlType(CondCtrl::NONE), CondDewPtDeltaT(0.0), CondErrIndex(0),
              ColdThrottlRange(0.0), RatedWaterTemp(0.0), CoolingCapMethod(0), ScaledCoolingCapacity(0.0), UA(0.0), Offset(0.0), WaterMassFlowRate(0.0),
              WaterMassFlowRateMax(0.0), RatedWaterFlowRate(0.0), WaterVolFlowRateMax(0.0), WaterInletTempStd(0.0), WaterInletTemp(0.0),
              WaterInletEnthalpy(0.0), WaterOutletTempStd(0.0), WaterOutletTemp(0.0), WaterOutletEnthalpy(0.0), RatedZoneAirTemp(0.0),
              FracRadiant(0.0), FracConvect(0.0), FracDistribPerson(0.0), TotPower(0.0), Power(0.0), ConvPower(0.0), RadPower(0.0), TotEnergy(0.0),
              Energy(0.0), ConvEnergy(0.0), RadEnergy(0.0), LoopNum(0), LoopSideNum(0), BranchNum(0), CompNum(0), CoolingPanelLoadReSimIndex(0),
              CoolingPanelMassFlowReSimIndex(0), CoolingPanelInletTempFlowReSimIndex(0)
        {
        }

        void CalcCoolingPanel(EnergyPlusData &state, int const CoolingPanelNum);

        void SetCoolingPanelControlTemp(EnergyPlusData &state, Real64 &ControlTemp, int ZoneNum);

        bool SizeCoolingPanelUA(EnergyPlusData &state);

        void ReportCoolingPanel(EnergyPlusData &state);
    };

    struct CoolingPanelSysNumericFieldData
    {
        // Members
        Array1D_string FieldNames;

        // Default Constructor
        CoolingPanelSysNumericFieldData() = default;

        // Destructor
        ~CoolingPanelSysNumericFieldData() = default;
    };

    void SimCoolingPanel(EnergyPlusData &state, std::string const &EquipName,
                         int ActualZoneNum,
                         int ControlledZoneNum,
                         bool FirstHVACIteration,
                         Real64 &PowerMet,
                         int &CompIndex);

    void GetCoolingPanelInput(EnergyPlusData &state);

    void InitCoolingPanel(EnergyPlusData &state, int CoolingPanelNum, int ControlledZoneNumSub, bool FirstHVACIteration);

    void SizeCoolingPanel(EnergyPlusData &state, int CoolingPanelNum);

    void UpdateCoolingPanel(EnergyPlusData &state, int CoolingPanelNum);

    void UpdateCoolingPanelSourceValAvg(EnergyPlusData &state,
                                        bool &CoolingPanelSysOn); // .TRUE. if the radiant system has run this zone time step

    void DistributeCoolingPanelRadGains(EnergyPlusData &state);

    Real64 SumHATsurf(EnergyPlusData &state, int ZoneNum); // Zone number

} // namespace CoolingPanelSimple

    struct ChilledCeilingPanelSimpleData : BaseGlobalStruct {

        bool GetInputFlag = true;
        bool MyOneTimeFlag = true;
        int NumCoolingPanels = 0;
        Array1D<Real64> CoolingPanelSource;   // Need to keep the last value in case we are still iterating
        Array1D<Real64> CoolingPanelSrcAvg;   // Need to keep the last value in case we are still iterating
        Array1D<Real64> ZeroSourceSumHATsurf; // Equal to the SumHATsurf for all the walls in a zone with no source

        // Record keeping variables used to calculate CoolingPanelSrcAvg locally
        Array1D<Real64> LastCoolingPanelSrc; // Need to keep the last value in case we are still iterating
        Array1D<Real64> LastSysTimeElapsed;  // Need to keep the last value in case we are still iterating
        Array1D<Real64> LastTimeStepSys;     // Need to keep the last value in case we are still iterating
        Array1D_bool CheckEquipName;
        Array1D_bool SetLoopIndexFlag; // get loop number flag

        // Autosizing variables
        Array1D_bool MySizeFlagCoolPanel;

        Array1D<CoolingPanelSimple::CoolingPanelParams> CoolingPanel;
        Array1D<CoolingPanelSimple::CoolingPanelSysNumericFieldData> CoolingPanelSysNumericFields;

        bool ZoneEquipmentListChecked = false;
        Array1D_bool MyEnvrnFlag;
        void clear_state() override
        {
            this->GetInputFlag = true;
            this->MyOneTimeFlag = true;
            this->NumCoolingPanels = 0;
            this->CoolingPanelSource.deallocate();
            this->CoolingPanelSrcAvg.deallocate();
            this->ZeroSourceSumHATsurf.deallocate();
            this->LastCoolingPanelSrc.deallocate();
            this->LastSysTimeElapsed.deallocate();
            this->LastTimeStepSys.deallocate();
            this->CheckEquipName.deallocate();
            this->SetLoopIndexFlag.deallocate();
            this->MySizeFlagCoolPanel.deallocate();
            this->CoolingPanel.deallocate();
            this->CoolingPanelSysNumericFields.deallocate();
            this->ZoneEquipmentListChecked = false;
            this->MyEnvrnFlag.clear();
        }
    };

} // namespace EnergyPlus

#endif
