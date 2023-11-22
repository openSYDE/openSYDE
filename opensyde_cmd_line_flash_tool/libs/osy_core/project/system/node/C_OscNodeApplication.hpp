//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for application related information (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCNODEAPPLICATION_HPP
#define C_OSCNODEAPPLICATION_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_SclString.hpp"
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscNodeApplication
{
public:
   C_OscNodeApplication(void);

   void CalcHash(uint32_t & oru32_HashValue) const;

   static const uint16_t hu16_HIGHEST_KNOWN_CODE_VERSION = 6U;
   ///< highest known structure format version in openSYDE core library

   enum E_Type ///< possible application types
   {
      eBINARY = 0,               ///< Type for address based flash access devices. E.g.: Intel Hex file.
      ePROGRAMMABLE_APPLICATION, ///< Type for user programmable applications, i.e. with source code generation support
      ePARAMETER_SET_HALC        ///< Type for parameter set for NVM based hardware configuration, i.e. with SYDE_PSI
                                 // file generation support
   };

   static stw::scl::C_SclString h_ApplicationToString(const C_OscNodeApplication::E_Type & ore_Application);
   static void h_StringToApplication(const stw::scl::C_SclString & orc_String, C_OscNodeApplication::E_Type & ore_Type);

   E_Type e_Type;                                    ///< Application type
   stw::scl::C_SclString c_Name;                     ///< Application name
   stw::scl::C_SclString c_Comment;                  ///< Application comment
   bool q_Active;                                    ///< Application active flag
   uint8_t u8_ProcessId;                             ///< Unique process ID
   stw::scl::C_SclString c_ProjectPath;              ///< Path to data block project
                                                     //(if relative it is meant as relative to *.syde file)
   stw::scl::C_SclString c_IdeCall;                  ///< Complete IDE command line call string
   stw::scl::C_SclString c_CodeGeneratorPath;        ///< Path to file generator
                                                     //(if relative it is meant as relative to openSYDE.exe)
   stw::scl::C_SclString c_GeneratePath;             ///< Path to generate sources for this application in
                                                     //(if relative it is meant as relative to data block project)
   uint16_t u16_GenCodeVersion;                      ///< Version of structure of generated files
   std::vector<stw::scl::C_SclString> c_ResultPaths; ///< Paths to result files of this application
                                                     //(if relative they are meant as relative to data block project)
                                                     //(vector size is either 1 or 2;
                                                     //2 only for PSI file generation, and in this case, the first one
                                                     //corresponds to safe and the second one to non-safe file)
   //Note: all paths and the IDE call can contain placeholder variables!
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
