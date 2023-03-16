//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for application related information (implementation)

   Data class for application related information

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwerrors.hpp"

#include "C_OscNodeApplication.hpp"

#include "C_SclChecksums.hpp"
#include "C_OscLoggingHandler.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::errors;
using namespace stw::opensyde_core;
using namespace stw::scl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscNodeApplication::C_OscNodeApplication(void) :
   e_Type(eBINARY),
   c_Name(""),
   c_Comment(""),
   q_Active(true),
   u8_ProcessId(0),
   c_ProjectPath(""),
   c_IdeCall(""),
   c_CodeGeneratorPath(""),
   c_GeneratePath(""),
   u16_GenCodeVersion(0)
{
   // default: one output file
   c_ResultPaths.resize(1);
   c_ResultPaths[0] = "";
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.
   It is not endian-safe, so it should only be used on the same system it is created on.

   \param[in,out] oru32_HashValue    Hash value with initial [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeApplication::CalcHash(uint32_t & oru32_HashValue) const
{
   stw::scl::C_SclChecksums::CalcCRC32(&this->e_Type, sizeof(this->e_Type), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(this->c_Name.c_str(), this->c_Name.Length(), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(this->c_Comment.c_str(), this->c_Comment.Length(), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->q_Active, sizeof(this->q_Active), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->u8_ProcessId, sizeof(this->u8_ProcessId), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(this->c_ProjectPath.c_str(), this->c_ProjectPath.Length(), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(this->c_IdeCall.c_str(), this->c_IdeCall.Length(), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(this->c_CodeGeneratorPath.c_str(),
                                       this->c_CodeGeneratorPath.Length(), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(this->c_GeneratePath.c_str(), this->c_GeneratePath.Length(), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->u16_GenCodeVersion, sizeof(this->u16_GenCodeVersion), oru32_HashValue);
   for (uint32_t u32_Counter = 0U; u32_Counter < this->c_ResultPaths.size(); ++u32_Counter)
   {
      stw::scl::C_SclChecksums::CalcCRC32(this->c_ResultPaths[u32_Counter].c_str(),
                                          this->c_ResultPaths[u32_Counter].Length(), oru32_HashValue);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform application type to string

   \param[in] ore_Application Application type

   \return
   Stringified application type
*/
//----------------------------------------------------------------------------------------------------------------------
C_SclString C_OscNodeApplication::h_ApplicationToString(const C_OscNodeApplication::E_Type & ore_Application)
{
   C_SclString c_Retval;

   switch (ore_Application)
   {
   case C_OscNodeApplication::eBINARY:
      c_Retval = "binary";
      break;
   case C_OscNodeApplication::ePROGRAMMABLE_APPLICATION:
      c_Retval = "programming_application";
      break;
   case C_OscNodeApplication::ePARAMETER_SET_HALC:
      c_Retval = "parameter_set_halc";
      break;
   default:
      c_Retval = "invalid";
      break;
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Transform string to application type

   \param[in]  orc_String String to interpret
   \param[out] ore_Type   Application type
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeApplication::h_StringToApplication(const C_SclString & orc_String,
                                                 C_OscNodeApplication::E_Type & ore_Type)
{
   if (orc_String == "binary")
   {
      ore_Type = C_OscNodeApplication::eBINARY;
   }
   else if (orc_String == "programming_application")
   {
      ore_Type = C_OscNodeApplication::ePROGRAMMABLE_APPLICATION;
   }
   else if (orc_String == "parameter_set_halc")
   {
      ore_Type = C_OscNodeApplication::ePARAMETER_SET_HALC;
   }
   else
   {
      //set binary if unknown type read
      ore_Type = C_OscNodeApplication::eBINARY;
      osc_write_log_warning("Loading node definition",
                            "Invalid value for application type. Unknown type: " + orc_String + ". "
                            "Type is replaced with: " + h_ApplicationToString(C_OscNodeApplication::eBINARY));
   }
}
