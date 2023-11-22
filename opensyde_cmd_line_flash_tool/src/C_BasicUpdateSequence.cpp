//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Short description

   Detailed description (optional). The module shall be described detailed if it is not described completely
   by short description.

   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"
#include "stwerrors.h"
#include "stwtypes.hpp"

#include "C_OscLoggingHandler.hpp"
#include "C_BasicUpdateSequence.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::scl;
using namespace stw::opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_BasicUpdateSequence::C_BasicUpdateSequence(void) :
   C_OscBuSequences()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reports some information about the current sequence for a specific server

   Implementation here: print to console and log file. Only show result in error case.

   \param[in]  os32_Result       Result of service
   \param[in]  orc_Information   Text information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_BasicUpdateSequence::m_ReportProgress(const int32_t os32_Result, const stw::scl::C_SclString & orc_Information)
{
   if (os32_Result == C_NO_ERR)
   {
      std::cout << "Progress: " << orc_Information.c_str() << std::endl;
      osc_write_log_info("Progress", orc_Information);
   }
   else if (os32_Result == C_WARN)
   {
      std::cout << "Warning in step: " << orc_Information.c_str() << std::endl;
      osc_write_log_warning("Progress", orc_Information);
   }
   else
   {
      std::cout << "Error in step: " << orc_Information.c_str() << " Result: " << os32_Result <<
         ". See log file for details. " << std::endl;
      osc_write_log_error("Progress", orc_Information);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Reports information read from openSYDE flashloader of node

   Called by ReadDeviceInformation() after it has read information from an openSYDE node.
   Implementation here: print read information to console and log file

   \param[in]  orc_DeviceName    Device name
   \param[in]  orc_Information   Information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_BasicUpdateSequence::m_ReportFlashloaderInformationRead(const C_SclString & orc_DeviceName,
                                                               const C_OscComFlashloaderInformation & orc_Information)
{
   C_SclStringList c_Text;
   const C_SclStringList c_MoreInformation = orc_Information.FlashloaderInformationToText();

   c_Text.Clear();
   c_Text.Add("Device name: " + orc_DeviceName);
   c_Text.AddStrings(&c_MoreInformation);

   std::cout << "openSYDE device information read: " << "\n";
   for (uint32_t u32_Line = 0U; u32_Line < c_Text.GetCount(); u32_Line++)
   {
      std::cout << "  " << c_Text.Strings[u32_Line].c_str() << "\n";
      osc_write_log_info("Flashloader Info", c_Text.Strings[u32_Line]);
   }
}
