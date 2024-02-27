//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for general node properties (implementation)

   Data class for general node properties

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_OscNodeProperties.hpp"

#include "C_SclChecksums.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_core;

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
C_OscNodeProperties::C_OscNodeProperties(void)
{
   this->Initialize();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscNodeProperties::~C_OscNodeProperties(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize class content

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeProperties::Initialize(void)
{
   c_Name = "Default Node name";
   c_Comment = "";
   e_DiagnosticServer = eDS_NONE;
   e_FlashLoader = eFL_NONE;
   c_ComInterfaces.resize(0);
   c_OpenSydeServerSettings.Initialize();
   c_StwFlashloaderSettings.Initialize();
   c_CodeExportSettings.Initialize();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Calculates the hash value over all data

   The hash value is a 32 bit CRC value.
   It is not endian-safe, so it should only be used on the same system it is created on.

   \param[in,out]  oru32_HashValue  Hash value with initial [in] value and result [out] value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeProperties::CalcHash(uint32_t & oru32_HashValue) const
{
   uint32_t u32_Counter;

   stw::scl::C_SclChecksums::CalcCRC32(this->c_Name.c_str(), this->c_Name.Length(), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(this->c_Comment.c_str(), this->c_Comment.Length(), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->e_DiagnosticServer, sizeof(this->e_DiagnosticServer), oru32_HashValue);
   stw::scl::C_SclChecksums::CalcCRC32(&this->e_FlashLoader, sizeof(this->e_FlashLoader), oru32_HashValue);

   for (u32_Counter = 0U; u32_Counter < this->c_ComInterfaces.size(); ++u32_Counter)
   {
      this->c_ComInterfaces[u32_Counter].CalcHash(oru32_HashValue);
   }

   this->c_OpenSydeServerSettings.CalcHash(oru32_HashValue);
   this->c_StwFlashloaderSettings.CalcHash(oru32_HashValue);
   this->c_CodeExportSettings.CalcHash(oru32_HashValue);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get interface matching the specification

   \param[in]  oe_InterfaceType     Interface type
   \param[in]  ou8_InterfaceNumber  Interface number

   \return
   Match for requested interface
   Else NULL
*/
//----------------------------------------------------------------------------------------------------------------------
const C_OscNodeComInterfaceSettings * C_OscNodeProperties::GetComInterface(
   const C_OscSystemBus::E_Type oe_InterfaceType, const uint8_t ou8_InterfaceNumber) const
{
   const C_OscNodeComInterfaceSettings * pc_Retval = NULL;

   for (uint32_t u32_Index = 0; (u32_Index < this->c_ComInterfaces.size()) && (pc_Retval == NULL); ++u32_Index)
   {
      const C_OscNodeComInterfaceSettings & rc_CurComInterface = this->c_ComInterfaces[u32_Index];
      //Check if match
      if ((rc_CurComInterface.e_InterfaceType == oe_InterfaceType) &&
          (ou8_InterfaceNumber == rc_CurComInterface.u8_InterfaceNumber))
      {
         pc_Retval = &this->c_ComInterfaces[u32_Index];
      }
   }
   return pc_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set com interface

   \param[in]  orc_ComInterface  Com interface replacement
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeProperties::SetComInterface(const C_OscNodeComInterfaceSettings & orc_ComInterface)
{
   for (uint32_t u32_Index = 0; u32_Index < this->c_ComInterfaces.size(); ++u32_Index)
   {
      const C_OscNodeComInterfaceSettings & rc_CurComInterface = this->c_ComInterfaces[u32_Index];
      //Check if match
      if ((rc_CurComInterface.e_InterfaceType == orc_ComInterface.e_InterfaceType) &&
          (orc_ComInterface.u8_InterfaceNumber == rc_CurComInterface.u8_InterfaceNumber))
      {
         this->c_ComInterfaces[u32_Index] = orc_ComInterface;
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Disconnect bus of specified interface

   \param[in]  oe_InterfaceType     Interface type
   \param[in]  ou8_InterfaceNumber  Interface number
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeProperties::DisconnectComInterface(const C_OscSystemBus::E_Type oe_InterfaceType,
                                                 const uint8_t ou8_InterfaceNumber)
{
   const C_OscNodeComInterfaceSettings * const pc_Interface = GetComInterface(oe_InterfaceType, ou8_InterfaceNumber);

   if (pc_Interface != NULL)
   {
      C_OscNodeComInterfaceSettings c_Tmp = *pc_Interface;
      c_Tmp.RemoveConnection();
      this->SetComInterface(c_Tmp);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize com interfaces using the specified device definition

   \param[in]  orc_Device           Device definition to use as base for com interfaces
   \param[in]  ou32_SubDeviceIndex  Sub device index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscNodeProperties::CreateComInterfaces(const C_OscDeviceDefinition & orc_Device,
                                              const uint32_t ou32_SubDeviceIndex)
{
   bool q_Found;

   //Check missing can
   for (uint8_t u8_ItCan = 0U; u8_ItCan < orc_Device.u8_NumCanBusses; ++u8_ItCan)
   {
      q_Found = false;
      for (uint32_t u32_CurInterface = 0U; u32_CurInterface < this->c_ComInterfaces.size(); ++u32_CurInterface)
      {
         C_OscNodeComInterfaceSettings & rc_CurInterface = this->c_ComInterfaces[u32_CurInterface];
         if ((rc_CurInterface.e_InterfaceType == C_OscSystemBus::eCAN) &&
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
               if (ou32_SubDeviceIndex < orc_Device.c_SubDevices.size())
               {
                  rc_CurInterface.SetInterfaceConnectedInDevice(orc_Device.c_SubDevices[ou32_SubDeviceIndex].IsConnected(
                                                                   C_OscSystemBus::eCAN,
                                                                   u8_ItCan));
               }
            }
         }
      }
      if (q_Found == false)
      {
         C_OscNodeComInterfaceSettings c_NewInterface;
         c_NewInterface.e_InterfaceType    = C_OscSystemBus::eCAN;
         c_NewInterface.u8_InterfaceNumber = u8_ItCan;
         if (ou32_SubDeviceIndex < orc_Device.c_SubDevices.size())
         {
            c_NewInterface.SetInterfaceConnectedInDevice(orc_Device.c_SubDevices[ou32_SubDeviceIndex].IsConnected(
                                                            C_OscSystemBus::eCAN,
                                                            u8_ItCan));
         }
         this->c_ComInterfaces.push_back(c_NewInterface);
      }
   }

   //Check missing Ethernet
   for (uint8_t u8_ItEth = 0; u8_ItEth < orc_Device.u8_NumEthernetBusses; ++u8_ItEth)
   {
      q_Found = false;
      for (uint32_t u32_CurInterface = 0; u32_CurInterface < this->c_ComInterfaces.size(); ++u32_CurInterface)
      {
         C_OscNodeComInterfaceSettings & rc_CurInterface = this->c_ComInterfaces[u32_CurInterface];
         if ((rc_CurInterface.e_InterfaceType == C_OscSystemBus::eETHERNET) &&
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
               if (ou32_SubDeviceIndex < orc_Device.c_SubDevices.size())
               {
                  rc_CurInterface.SetInterfaceConnectedInDevice(orc_Device.c_SubDevices[ou32_SubDeviceIndex].IsConnected(
                                                                   C_OscSystemBus::eETHERNET,
                                                                   u8_ItEth));
               }
            }
         }
      }
      if (q_Found == false)
      {
         C_OscNodeComInterfaceSettings c_NewInterface;
         c_NewInterface.e_InterfaceType    = C_OscSystemBus::eETHERNET;
         c_NewInterface.u8_InterfaceNumber = u8_ItEth;
         if (ou32_SubDeviceIndex < orc_Device.c_SubDevices.size())
         {
            c_NewInterface.SetInterfaceConnectedInDevice(orc_Device.c_SubDevices[ou32_SubDeviceIndex].IsConnected(
                                                            C_OscSystemBus::eETHERNET,
                                                            u8_ItEth));
         }
         this->c_ComInterfaces.push_back(c_NewInterface);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get all CAN interfaces that are stored in c_ComInterfaces

   \return
   std::vector<C_OscNodeComInterfaceSettings>
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<C_OscNodeComInterfaceSettings> C_OscNodeProperties::GetCanInterfaces() const
{
   const C_OscSystemBus::E_Type e_CAN = C_OscSystemBus::eCAN;

   std::vector<C_OscNodeComInterfaceSettings> c_CanInterfaces = m_GetInterfaces(e_CAN);

   return c_CanInterfaces;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get all Ethernet interfaces that are stored in c_ComInterfaces

   \return
   std::vector<C_OscNodeComInterfaceSettings>
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<C_OscNodeComInterfaceSettings> C_OscNodeProperties::GetEthernetInterfaces() const
{
   const C_OscSystemBus::E_Type e_ETHERNET = C_OscSystemBus::eETHERNET;

   std::vector<C_OscNodeComInterfaceSettings> c_EthernetInterfaces = m_GetInterfaces(e_ETHERNET);

   return c_EthernetInterfaces;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get the interfaces according to the argument

   \param[in]  ore_Type           Type of the interface that is searched

   \return
   std::vector<C_OscNodeComInterfaceSettings>
*///----------------------------------------------------------------------------------------------------------------------
std::vector<C_OscNodeComInterfaceSettings> C_OscNodeProperties::m_GetInterfaces(const C_OscSystemBus::E_Type oe_Type)
const
{
   const uint32_t u32_ComInterfacesSize = static_cast<uint32_t>(c_ComInterfaces.size());

   std::vector<C_OscNodeComInterfaceSettings> c_Interfaces;

   for (uint32_t u32_ComInterfacesIndex = 0; u32_ComInterfacesIndex < u32_ComInterfacesSize; ++u32_ComInterfacesIndex)
   {
      const C_OscNodeComInterfaceSettings c_OscNodeComInterfaceSettings = c_ComInterfaces[u32_ComInterfacesIndex];
      if (c_OscNodeComInterfaceSettings.e_InterfaceType == oe_Type)
      {
         c_Interfaces.push_back(c_OscNodeComInterfaceSettings);
      }
   }
   return c_Interfaces;
}
