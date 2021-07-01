//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for application related information (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCNODEAPPLICATION_H
#define C_OSCNODEAPPLICATION_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "CSCLString.h"
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCNodeApplication
{
public:
   C_OSCNodeApplication(void);

   void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   static const stw_types::uint16 hu16_HIGHEST_KNOWN_CODE_VERSION = 5U;
   ///< highest known structure format version in openSYDE core library

   enum E_Type ///< possible application types
   {
      eBINARY = 0,               ///< Type for address based flash access devices. E.g.: Intel Hex file.
      ePROGRAMMABLE_APPLICATION, ///< Type for user programmable applications, i.e. with source code generation support
      ePARAMETER_SET_HALC        ///< Type for parameter set for NVM based hardware configuration, i.e. with SYDE_PSI
                                 // file generation support
   };

   static stw_scl::C_SCLString h_ApplicationToString(const C_OSCNodeApplication::E_Type & ore_Application);
   static void h_StringToApplication(const stw_scl::C_SCLString & orc_String, C_OSCNodeApplication::E_Type & ore_Type);

   E_Type e_Type;                                   ///< Application type
   stw_scl::C_SCLString c_Name;                     ///< Application name
   stw_scl::C_SCLString c_Comment;                  ///< Application comment
   bool q_Active;                                   ///< Application active flag
   stw_types::uint8 u8_ProcessId;                   ///< Unique process ID
   stw_scl::C_SCLString c_ProjectPath;              ///< Path to data block project
                                                    //(if relative it is meant as relative to *.syde file)
   stw_scl::C_SCLString c_IDECall;                  ///< Complete IDE command line call string
   stw_scl::C_SCLString c_CodeGeneratorPath;        ///< Path to file generator
                                                    //(if relative it is meant as relative to openSYDE.exe)
   stw_scl::C_SCLString c_GeneratePath;             ///< Path to generate sources for this application in
                                                    //(if relative it is meant as relative to data block project)
   stw_types::uint16 u16_GenCodeVersion;            ///< Version of structure of generated files
   std::vector<stw_scl::C_SCLString> c_ResultPaths; ///< Paths to result files of this application
                                                    //(if relative they are meant as relative to data block project)
                                                    //(vector size is either 1 or 2;
                                                    //2 only for PSI file generation, and in this case, the first one
                                                    //corresponds to safe and the second one to non-safe file)
   //Note: all paths and the IDE call can contain placeholder variables!
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
