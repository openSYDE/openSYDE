//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for general node properties (implementation)

   Data class for general node properties

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_OSCNodeProperties.h"

#include "CSCLChecksums.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_opensyde_core;
using namespace stw_types;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//-----------------------------------------------------------------------------
C_OSCNodeProperties::C_OSCNodeProperties(void)
{
   this->Initialize();
}

//-----------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//-----------------------------------------------------------------------------
C_OSCNodeProperties::~C_OSCNodeProperties(void)
{
}

//-----------------------------------------------------------------------------
/*! \brief   Initialize class content

   Clean up.
*/
//-----------------------------------------------------------------------------
void C_OSCNodeProperties::Initialize(void)
{
   c_Name = "Default Node name";
   c_Comment = "";
   e_DiagnosticServer = eDS_NONE;
   e_FlashLoader = eFL_NONE;
   c_ComInterfaces.resize(0);
   c_OpenSYDEServerSettings.Initialize();
   c_STWFlashloaderSettings.Initialize();
}

//-----------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.
   It is not endian-safe, so it should only be used on the same system it is created on.

   \param[in,out] oru32_HashValue    Hash value with initial [in] value and result [out] value
*/
//-----------------------------------------------------------------------------
void C_OSCNodeProperties::CalcHash(uint32 & oru32_HashValue) const
{
   uint32 u32_Counter;

   stw_scl::C_SCLChecksums::CalcCRC32(this->c_Name.c_str(), this->c_Name.Length(), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(this->c_Comment.c_str(), this->c_Comment.Length(), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->e_DiagnosticServer, sizeof(this->e_DiagnosticServer), oru32_HashValue);
   stw_scl::C_SCLChecksums::CalcCRC32(&this->e_FlashLoader, sizeof(this->e_FlashLoader), oru32_HashValue);

   for (u32_Counter = 0U; u32_Counter < this->c_ComInterfaces.size(); ++u32_Counter)
   {
      this->c_ComInterfaces[u32_Counter].CalcHash(oru32_HashValue);
   }

   this->c_OpenSYDEServerSettings.CalcHash(oru32_HashValue);
   this->c_STWFlashloaderSettings.CalcHash(oru32_HashValue);
}

//-----------------------------------------------------------------------------
/*! \brief   Get interface matching the specification

   \param[in] oe_InterfaceType    Interface type
   \param[in] ou8_InterfaceNumber Interface number

   \return
   Match for requested interface
   Else NULL
*/
//-----------------------------------------------------------------------------
const C_OSCNodeComInterfaceSettings * C_OSCNodeProperties::GetComInterface(
   const C_OSCSystemBus::E_Type oe_InterfaceType, const stw_types::uint8 ou8_InterfaceNumber) const
{
   const C_OSCNodeComInterfaceSettings * pc_Retval = NULL;

   for (uint32 u32_Index = 0; (u32_Index < this->c_ComInterfaces.size()) && (pc_Retval == NULL); ++u32_Index)
   {
      const C_OSCNodeComInterfaceSettings & rc_CurComInterface = this->c_ComInterfaces[u32_Index];
      //Check if match
      if ((rc_CurComInterface.e_InterfaceType == oe_InterfaceType) &&
          (ou8_InterfaceNumber == rc_CurComInterface.u8_InterfaceNumber))
      {
         pc_Retval = &this->c_ComInterfaces[u32_Index];
      }
   }
   return pc_Retval;
}

//-----------------------------------------------------------------------------
/*! \brief   Set com interface

   \param[in] orc_ComInterface  Com interface replacement
*/
//-----------------------------------------------------------------------------
void C_OSCNodeProperties::SetComInterface(const C_OSCNodeComInterfaceSettings & orc_ComInterface)
{
   for (uint32 u32_Index = 0; u32_Index < this->c_ComInterfaces.size(); ++u32_Index)
   {
      const C_OSCNodeComInterfaceSettings & rc_CurComInterface = this->c_ComInterfaces[u32_Index];
      //Check if match
      if ((rc_CurComInterface.e_InterfaceType == orc_ComInterface.e_InterfaceType) &&
          (orc_ComInterface.u8_InterfaceNumber == rc_CurComInterface.u8_InterfaceNumber))
      {
         this->c_ComInterfaces[u32_Index] = orc_ComInterface;
      }
   }
}

//-----------------------------------------------------------------------------
/*! \brief   Disconnect bus of specified interface

   \param[in] oe_InterfaceType    Interface type
   \param[in] ou8_InterfaceNumber Interface number
*/
//-----------------------------------------------------------------------------
void C_OSCNodeProperties::DisconnectComInterface(const C_OSCSystemBus::E_Type oe_InterfaceType,
                                                 const uint8 ou8_InterfaceNumber)
{
   const C_OSCNodeComInterfaceSettings * const pc_Interface = GetComInterface(oe_InterfaceType, ou8_InterfaceNumber);

   if (pc_Interface != NULL)
   {
      C_OSCNodeComInterfaceSettings c_Tmp = *pc_Interface;
      c_Tmp.q_IsBusConnected = false;
      c_Tmp.u32_BusIndex = 0;
      this->SetComInterface(c_Tmp);
   }
}

//-----------------------------------------------------------------------------
/*! \brief   Initialize com interfaces using the specified device definition

   \param[in] orc_Device Device definition to use as base for com interfaces
*/
//-----------------------------------------------------------------------------
void C_OSCNodeProperties::CreateComInterfaces(const C_OSCDeviceDefinition & orc_Device)
{
   bool q_Found;

   //Check missing can
   for (uint8 u8_ItCan = 0U; u8_ItCan < orc_Device.u8_NumCanBusses; ++u8_ItCan)
   {
      q_Found = false;
      for (uint32 u32_CurInterface = 0U; u32_CurInterface < this->c_ComInterfaces.size(); ++u32_CurInterface)
      {
         C_OSCNodeComInterfaceSettings & rc_CurInterface = this->c_ComInterfaces[u32_CurInterface];
         if ((rc_CurInterface.e_InterfaceType == C_OSCSystemBus::eCAN) &&
             (rc_CurInterface.u8_InterfaceNumber == u8_ItCan))
         {
            if (q_Found == true)
            {
               //Remove duplicates
               this->c_ComInterfaces.erase(this->c_ComInterfaces.begin() + u32_CurInterface);
               //lint -e850 Do not do an iterate step (There is a new element at the current position
               --u32_CurInterface;
            }
            else
            {
               //First
               q_Found = true;
            }
         }
      }
      if (q_Found == false)
      {
         C_OSCNodeComInterfaceSettings c_NewInterface;
         c_NewInterface.e_InterfaceType    = C_OSCSystemBus::eCAN;
         c_NewInterface.u8_InterfaceNumber = u8_ItCan;
         this->c_ComInterfaces.push_back(c_NewInterface);
      }
   }

   //Check missing Ethernet
   for (uint8 u8_ItEth = 0; u8_ItEth < orc_Device.u8_NumEthernetBusses; ++u8_ItEth)
   {
      q_Found = false;
      for (uint32 u32_CurInterface = 0; u32_CurInterface < this->c_ComInterfaces.size(); ++u32_CurInterface)
      {
         C_OSCNodeComInterfaceSettings & rc_CurInterface = this->c_ComInterfaces[u32_CurInterface];
         if ((rc_CurInterface.e_InterfaceType == C_OSCSystemBus::eETHERNET) &&
             (rc_CurInterface.u8_InterfaceNumber == u8_ItEth))
         {
            if (q_Found == true)
            {
               //Remove duplicates
               this->c_ComInterfaces.erase(this->c_ComInterfaces.begin() + u32_CurInterface);
               //Do not do an iterate step (There is a new element at the current position
               --u32_CurInterface;
            }
            else
            {
               //First
               q_Found = true;
            }
         }
      }
      if (q_Found == false)
      {
         C_OSCNodeComInterfaceSettings c_NewInterface;
         c_NewInterface.e_InterfaceType    = C_OSCSystemBus::eETHERNET;
         c_NewInterface.u8_InterfaceNumber = u8_ItEth;
         this->c_ComInterfaces.push_back(c_NewInterface);
      }
   }
}
