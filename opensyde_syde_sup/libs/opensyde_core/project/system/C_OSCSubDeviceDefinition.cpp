//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Sub device part of device definition

   Sub device part of device definition

   \copyright   Copyright 2021 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_OSCSubDeviceDefinition.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_core;

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
C_OSCSubDeviceDefinition::C_OSCSubDeviceDefinition()
{
   this->Clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Clear
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OSCSubDeviceDefinition::Clear()
{
   c_SubDeviceName = "";
   c_ConnectedInterfaces.clear();
   c_OtherAcceptedNames.clear();
   q_ProgrammingSupport = false;
   q_DiagnosticProtocolKefex = false;
   q_DiagnosticProtocolOpenSydeCan = false;
   q_DiagnosticProtocolOpenSydeEthernet = false;
   q_FlashloaderStwCan = false;
   q_FlashloaderOpenSydeCan = false;
   q_FlashloaderOpenSydeEthernet = false;
   q_FlashloaderOpenSydeIsFileBased = false;
   u32_UserEepromSizeBytes = 0U;
   //Special default values
   u32_FlashloaderResetWaitTimeNoChangesCan = 1000U;
   u32_FlashloaderResetWaitTimeNoChangesEthernet = 5500U;
   u32_FlashloaderResetWaitTimeNoFundamentalChangesCan = 1000U;
   u32_FlashloaderResetWaitTimeNoFundamentalChangesEthernet = 5500U;
   u32_FlashloaderResetWaitTimeFundamentalChangesCan = 1000U;
   u32_FlashloaderResetWaitTimeFundamentalChangesEthernet = 5500U;
   u32_FlashloaderOpenSydeRequestDownloadTimeout = 20000U;
   u32_FlashloaderOpenSydeTransferDataTimeout = 2000U;
}

//----------------------------------------------------------------------------------------------------------------------

bool C_OSCSubDeviceDefinition::IsUpdateAvailable(const C_OSCSystemBus::E_Type oe_Type) const
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
   default:
      break;
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------

bool C_OSCSubDeviceDefinition::IsRoutingAvailable(const C_OSCSystemBus::E_Type oe_Type) const
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
   default:
      break;
   }
   return q_Retval;
}
//----------------------------------------------------------------------------------------------------------------------

bool C_OSCSubDeviceDefinition::IsDiagnosisAvailable(const C_OSCSystemBus::E_Type oe_Type) const
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
   default:
      break;
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Is connected

   \param[in]  oe_Type              Type
   \param[in]  ou8_InterfaceNumber  Interface number

   \return
   State if interface is connected
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OSCSubDeviceDefinition::IsConnected(const C_OSCSystemBus::E_Type oe_Type,
                                           const stw_types::uint8 ou8_InterfaceNumber) const
{
   bool q_Retval = false;

   const stw_scl::C_SCLString c_Interface = C_OSCSubDeviceDefinition::h_GetInterfaceNameLower(oe_Type,
                                                                                              ou8_InterfaceNumber);
   {
      const std::map<stw_scl::C_SCLString, bool>::const_iterator c_It = this->c_ConnectedInterfaces.find(c_Interface);
      if (c_It != this->c_ConnectedInterfaces.end())
      {
         q_Retval = c_It->second;
      }
   }

   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get interface name (lowercase)

   \param[in]  oe_Type              Type
   \param[in]  ou8_InterfaceNumber  Interface number

   \return
   Interface name (lowercase)
*/
//----------------------------------------------------------------------------------------------------------------------
stw_scl::C_SCLString C_OSCSubDeviceDefinition::h_GetInterfaceNameLower(const C_OSCSystemBus::E_Type oe_Type,
                                                                       const stw_types::uint8 ou8_InterfaceNumber)
{
   stw_scl::C_SCLString c_Interface =
      stw_scl::C_SCLString::IntToStr(static_cast<stw_types::uintn>(ou8_InterfaceNumber) + 1U);

   if (oe_Type == C_OSCSystemBus::eCAN)
   {
      c_Interface = "can" + c_Interface;
   }
   else
   {
      c_Interface = "eth" + c_Interface;
   }
   return c_Interface;
}
