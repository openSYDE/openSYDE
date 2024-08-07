//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for device configuration information

   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <cstring>
#include "stwtypes.hpp"
#include "C_OscDcDeviceInformation.hpp"

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
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscDcDeviceInformation::C_OscDcDeviceInformation(void) :
   c_DeviceName(""),
   q_DeviceNameValid(false),
   u8_NodeId(0U),
   q_NodeIdValid(false),
   q_IpAddressValid(false),
   u8_SubNodeId(0U),
   q_SecurityActivated(false),
   q_ExtendedInfoValid(false)
{
   (void)memset(&au8_IpAddress[0], 0U, sizeof(au8_IpAddress));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a device name and its valid flag

   \param[in]  orc_DeviceName    New device name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscDcDeviceInformation::SetDeviceName(const C_SclString & orc_DeviceName)
{
   this->c_DeviceName = orc_DeviceName;
   this->q_DeviceNameValid = true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a node id and its valid flag

   \param[in]  ou8_NodeId  New node id
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscDcDeviceInformation::SetNodeId(const uint8_t ou8_NodeId)
{
   this->u8_NodeId = ou8_NodeId;
   this->q_NodeIdValid = true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a ip address and its valid flag

   \param[in]  orau8_IpAddress   New node id
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscDcDeviceInformation::SetIpAddress(const uint8_t (&orau8_IpAddress)[4])
{
   (void)memcpy(&this->au8_IpAddress[0], &orau8_IpAddress[0], sizeof(this->au8_IpAddress));
   this->q_IpAddressValid = true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a serial number and its valid flag

   \param[in]  orc_SerialNumber   New serial number
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscDcDeviceInformation::SetSerialNumber(const stw::opensyde_core::C_OscProtocolSerialNumber & orc_SerialNumber)
{
   this->c_SerialNumber = orc_SerialNumber;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets a extended specific information of a node and its valid flag

   \param[in]  ou8_SubNodeId         New sub node id
   \param[in]  oq_SecurityActivated  New security activated flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscDcDeviceInformation::SetExtendedInfo(const uint8_t ou8_SubNodeId, const bool oq_SecurityActivated)
{
   this->u8_SubNodeId = ou8_SubNodeId;
   this->q_SecurityActivated = oq_SecurityActivated;
   this->q_ExtendedInfoValid = true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Comparison of serial number and all relevant members

   \param[in] orc_Cmp Compared instance

   \return
   true     Current serial number equals orc_Cmp
   false    Else
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_OscDcDeviceInformation::IsSerialNumberIdentical(const C_OscDcDeviceInformation & orc_Cmp) const
{
   const bool q_Return = (this->c_SerialNumber == orc_Cmp.c_SerialNumber);

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscDcDeviceOldComConfig::C_OscDcDeviceOldComConfig(void) :
   u8_OldNodeId(0U),
   q_OldIpAddressValid(false)
{
   memset(&au8_OldIpAddress, 0U, 4);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief    Assignment operator.

   \param[in]  orc_Source  instance to assign

   \return
   reference to new instance
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscDcDeviceOldComConfig & C_OscDcDeviceOldComConfig::operator =(const C_OscDcDeviceOldComConfig & orc_Source)
{
   if (this != &orc_Source)
   {
      this->u8_OldNodeId = orc_Source.u8_OldNodeId;
      this->q_OldIpAddressValid = orc_Source.q_OldIpAddressValid;
      memcpy(&this->au8_OldIpAddress, &orc_Source.au8_OldIpAddress, 4);
   }

   return (*this);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the content

   \param[in]       ou8_OldNodeId        Old node id of node
   \param[in]       oq_OldIpAddressValid Flag if IP address is valid
   \param[in]       opu8_OldIpAddress    Optional pointer to old IP address
                                         If pointer != NULL, q_OldIpAddressValid will be set to oq_OldIpAddressValid
                                         If pointer == NULL, q_OldIpAddressValid will be set to false
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OscDcDeviceOldComConfig::SetContent(const uint8_t ou8_OldNodeId, const bool oq_OldIpAddressValid,
                                           const uint8_t * const opu8_OldIpAddress)
{
   this->u8_OldNodeId = ou8_OldNodeId;
   if (opu8_OldIpAddress != NULL)
   {
      this->q_OldIpAddressValid = oq_OldIpAddressValid;
      if (this->q_OldIpAddressValid == true)
      {
         memcpy(&this->au8_OldIpAddress[0], opu8_OldIpAddress, 4);
      }
   }
   else
   {
      this->q_OldIpAddressValid = false;
   }
}
