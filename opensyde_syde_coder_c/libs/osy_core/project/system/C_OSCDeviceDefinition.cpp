//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       openSYDE device definition container class

   Serves as data class to hold device definition data.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     05.09.2016  STW/A.Stangl
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_OSCDeviceDefinition.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_opensyde_core;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Constructor

   Initialize all class elements with default values

   \created     05.09.2016  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
C_OSCDeviceDefinition::C_OSCDeviceDefinition(void)
{
   this->Clear();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Clear elements

   Sets all numeric elements to zero and all strings to ""

   \created     05.09.2016  STW/A.Stangl
*/
//-----------------------------------------------------------------------------
void C_OSCDeviceDefinition::Clear(void)
{
   c_DeviceName = "";
   c_DeviceNameAlias = "";
   c_DeviceDescription = "";
   c_ImagePath = "";
   c_FilePath = "";

   u8_NumCanBusses = 0U;
   u8_NumEthernetBusses = 0U;
   q_ProgrammingSupport = false;
   c_SupportedBitrates.clear();
   q_DiagnosticProtocolKefex = false;
   q_DiagnosticProtocolOpenSydeCan = false;
   q_DiagnosticProtocolOpenSydeEthernet = false;
   q_FlashloaderStwCan = false;
   q_FlashloaderOpenSydeCan = false;
   q_FlashloaderOpenSydeEthernet = false;
   q_FlashloaderOpenSydeIsFileBased = false;
   u32_FlashloaderOpenSydeRequestDownloadTimeout = 0U;
   u32_FlashloaderOpenSydeTransferDataTimeout = 0;
   u32_UserEepromSizeBytes = 0U;
}

//-----------------------------------------------------------------------------
bool C_OSCDeviceDefinition::IsUpdateAvailable(const C_OSCSystemBus::E_Type oe_Type) const
{
   bool q_Retval = false;

   //flashloader support?
   switch (oe_Type)
   {
   case C_OSCSystemBus::eCAN:
      if (this->q_FlashloaderOpenSydeCan || this->q_FlashloaderStwCan)
      {
         q_Retval = true;
      }
      else
      {
         q_Retval = false;
      }
      break;
   case C_OSCSystemBus::eETHERNET:
      if (this->q_FlashloaderOpenSydeEthernet)
      {
         q_Retval = true;
      }
      else
      {
         q_Retval = false;
      }
      break;
   }
   return q_Retval;
}

//-----------------------------------------------------------------------------
bool C_OSCDeviceDefinition::IsRoutingAvailable(const C_OSCSystemBus::E_Type oe_Type) const
{
   bool q_Retval = false;

   switch (oe_Type)
   {
   case C_OSCSystemBus::eCAN:
      if (this->q_DiagnosticProtocolOpenSydeCan)
      {
         q_Retval = true;
      }
      else
      {
         q_Retval = false;
      }
      break;
   case C_OSCSystemBus::eETHERNET:
      if (this->q_DiagnosticProtocolOpenSydeEthernet)
      {
         q_Retval = true;
      }
      else
      {
         q_Retval = false;
      }
      break;
   }
   return q_Retval;
}
//-----------------------------------------------------------------------------
bool C_OSCDeviceDefinition::IsDiagnosisAvailable(const C_OSCSystemBus::E_Type oe_Type) const
{
   bool q_Retval = false;

   switch (oe_Type)
   {
   case C_OSCSystemBus::eCAN:
      if (this->q_DiagnosticProtocolKefex || this->q_DiagnosticProtocolOpenSydeCan)
      {
         q_Retval = true;
      }
      else
      {
         q_Retval = false;
      }
      break;
   case C_OSCSystemBus::eETHERNET:
      if (this->q_DiagnosticProtocolOpenSydeEthernet)
      {
         q_Retval = true;
      }
      else
      {
         q_Retval = false;
      }
      break;
   }
   return q_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get string to display for this device

   \return
   String to display

   \created     18.10.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
stw_scl::C_SCLString C_OSCDeviceDefinition::GetDisplayName(void) const
{
   stw_scl::C_SCLString c_Retval;
   if (this->c_DeviceNameAlias == "")
   {
      c_Retval = this->c_DeviceName;
   }
   else
   {
      c_Retval = this->c_DeviceNameAlias;
   }
   return c_Retval;
}
//-----------------------------------------------------------------------------
