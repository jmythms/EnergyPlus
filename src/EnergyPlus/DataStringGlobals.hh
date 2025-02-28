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

#ifndef DataStringGlobals_hh_INCLUDED
#define DataStringGlobals_hh_INCLUDED

// C++ Headers
#include <string>

// EnergyPlus Headers
#include <EnergyPlus/Data/BaseData.hh>
#include <EnergyPlus/EnergyPlus.hh>

namespace EnergyPlus {

namespace DataStringGlobals {

#ifdef _WIN32
    char constexpr pathChar('\\');
    char constexpr altpathChar('/');
#elif __linux__
    char constexpr pathChar('/');
    char constexpr altpathChar('\\');
#elif __unix__
    char constexpr pathChar('/');
    char constexpr altpathChar('\\');
#elif __posix__
    char constexpr pathChar('/');
    char constexpr altpathChar('\\');
#elif __APPLE__
    char constexpr pathChar('/');
    char constexpr altpathChar('\\');
#else
#error "Invalid platform detection in DataStringGlobals."
#endif
    char constexpr CharComma(',');     // comma
    char constexpr CharSemicolon(';'); // semicolon
    char constexpr CharTab('\t');      // tab
    char constexpr CharSpace(' ');     // space

    extern std::string VerString;            // String that represents version information
    extern std::string MatchVersion;         // String to be matched by Version object
    extern std::string PythonAPIVersion;

} // namespace DataStringGlobals

struct DataStringGlobalsData : BaseGlobalStruct {

    std::string outputMddFileName = "eplusout.mdd";
    std::string outputRddFileName = "eplusout.rdd";
    std::string outputShdFileName = "eplusout.shd";
    std::string outputTblCsvFileName = "eplustbl.csv";
    std::string outputTblHtmFileName = "eplustbl.htm";
    std::string outputTblTabFileName = "eplustbl.tab";
    std::string outputTblTxtFileName = "eplustbl.txt";
    std::string outputTblXmlFileName = "eplustbl.xml";
    std::string outputAdsFileName = "eplusADS.out";
    std::string outputGLHEFileName = "eplusout.glhe";
    std::string outputDelightOutFileName = "eplusout.delightout";
    std::string outputIperrFileName = "eplusout.iperr";
    std::string outputPerfLogFileName = "eplusout_perflog.csv";
    std::string outputSqlFileName = "eplusout.sql";
    std::string outputSqliteErrFileName = "eplussqlite.err";
    std::string outputCsvFileName = "eplusout.csv";
    std::string outputMtrCsvFileName = "eplusmtr.csv";
    std::string outputRvauditFileName = "eplusout.rvaudit";
    std::string outputErrFileName;
    std::string eplusADSFileName;
    std::string inputFileName;
    std::string inputIddFileName;
    std::string inputEpJSONSchemaFileName;
    std::string weatherFileNameOnly;
    std::string idfDirPathName;
    std::string outDirPathName;
    std::string idfFileNameOnly;
    std::string inputDirPathName;
    std::string outputDirPathName;
    std::string inputFileNameOnly;
    std::string exeDirectory;
    std::string ProgramPath;          // Path for Program from INI file
    std::string CurrentWorkingFolder; // Current working directory for run
    std::string FullName;             // Full name of file to open, including path
    std::string IDDVerString;         // Version information from the IDD (line 1)
    std::string CurrentDateTime;      // For printing current date and time at start of run

    
    void clear_state() override
    {
        eplusADSFileName.clear();
        idfFileNameOnly.clear();
        idfDirPathName.clear();
        outDirPathName.clear();
        inputFileNameOnly.clear();
        inputDirPathName.clear();
        outputDirPathName.clear();
        exeDirectory.clear();
        inputFileName.clear();
        inputIddFileName.clear();
        inputEpJSONSchemaFileName.clear();
        FullName.clear();
        weatherFileNameOnly.clear();
        ProgramPath.clear();
        CurrentWorkingFolder.clear();
        CurrentDateTime.clear();
        IDDVerString.clear();
    }
};

} // namespace EnergyPlus

#endif
