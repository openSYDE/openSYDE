//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for device configuration information
   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCDCDEVICEINFORMATION_H
#define C_OSCDCDEVICEINFORMATION_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OscProtocolSerialNumber.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

///Basic information about one device
class C_OscDcDeviceInformation
{
public:
   C_OscDcDeviceInformation(void);

   void SetDeviceName(const stw::scl::C_SclString & orc_DeviceName);
   void SetNodeId(const uint8_t ou8_NodeId);
   void SetIpAddress(const uint8_t (&orau8_IpAddress)[4]);
   void SetSerialNumber(const stw::opensyde_core::C_OscProtocolSerialNumber & orc_SerialNumber);
   void SetExtendedInfo(const uint8_t ou8_SubNodeId, const bool oq_SecurityActivated);

   bool IsSerialNumberIdentical(const C_OscDcDeviceInformation & orc_Cmp) const;

   stw::scl::C_SclString c_DeviceName;
   bool q_DeviceNameValid;

   uint8_t u8_NodeId;
   bool q_NodeIdValid;

   uint8_t au8_IpAddress[4];
   bool q_IpAddressValid;

   stw::opensyde_core::C_OscProtocolSerialNumber c_SerialNumber;

   // Extended specific information
   uint8_t u8_SubNodeId;
   bool q_SecurityActivated;
   bool q_ExtendedInfoValid;
};

///Old communication configuration of a node
class C_OscDcDeviceOldComConfig
{
public:
   C_OscDcDeviceOldComConfig(void);
   C_OscDcDeviceOldComConfig(const C_OscDcDeviceOldComConfig & orc_Source);
   C_OscDcDeviceOldComConfig & operator =(const C_OscDcDeviceOldComConfig & orc_Source);

   void SetContent(const uint8_t ou8_OldNodeId, const bool oq_OldIpAddressValid,
                   const uint8_t * const opu8_OldIpAddress = NULL);

   // Old node id which was used and detected by getting serial number
   uint8_t u8_OldNodeId;
   uint8_t au8_OldIpAddress[4];
   bool q_OldIpAddressValid;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
