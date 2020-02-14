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

   static const stw_types::uint16 hu16_HIGHEST_KNOWN_CODE_VERSION = 4U;
   ///< highest known code format version in openSYDE core library

   enum E_Type ///< possible application types
   {
      eBINARY = 0,              ///< Type for address based flash access devices. E.g.: Intel Hex file.
      ePROGRAMMABLE_APPLICATION ///< Type for user programmable applications
   };

   static stw_scl::C_SCLString h_ApplicationToString(const C_OSCNodeApplication::E_Type & ore_Application);
   static stw_types::sint32 h_StringToApplication(const stw_scl::C_SCLString & orc_String,
                                                  C_OSCNodeApplication::E_Type & ore_Type);

   E_Type e_Type;                            ///< Application type
   stw_scl::C_SCLString c_Name;              ///< Application name
   stw_scl::C_SCLString c_Comment;           ///< Application comment
   bool q_Active;                            ///< Application active flag
   stw_types::uint8 u8_ProcessId;            ///< Unique process ID
   stw_scl::C_SCLString c_ProjectPath;       ///< Path to data block project
   stw_scl::C_SCLString c_IDECall;           ///< Complete IDE command line call string
   stw_scl::C_SCLString c_CodeGeneratorPath; ///< Path to code generator
   stw_scl::C_SCLString c_GeneratePath;      ///< Path to generate sources for this application in
                                             //(can be relative to data block project)
   stw_types::uint16 u16_GenCodeVersion;     ///< Version of structure of generated code
   stw_scl::C_SCLString c_ResultPath;        ///< Path to result file of this application
                                             //(can be relative to data block project)
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
