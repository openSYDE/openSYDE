//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for communication interface settings (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCNODECOMINTERFACESETTINGS_H
#define C_OSCNODECOMINTERFACESETTINGS_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "stwtypes.h"
#include "CSCLString.h"
#include "C_OSCSystemBus.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCNodeComInterfaceSettings
{
public:
   class C_IpAddress
   {
   public:
      C_IpAddress(void);

      stw_types::uint8 au8_IpAddress[4];      //IP V4 address
      stw_types::uint8 au8_NetMask[4];        //IP V4 net mask
      stw_types::uint8 au8_DefaultGateway[4]; //IP V4 default gateway
   };

   C_OSCNodeComInterfaceSettings(void);
   ~C_OSCNodeComInterfaceSettings(void);

   void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   C_OSCSystemBus::E_Type e_InterfaceType; ///< e.g. CAN, ethernet
   stw_types::uint8 u8_InterfaceNumber;    ///< 0 = first interface for this type
   stw_types::uint8 u8_NodeID;             ///< Node ID on bus

   C_IpAddress c_Ip; ///< only used with interface type ethernet

   bool q_IsUpdateEnabled;         ///< Flag for update enabled using this bus
   bool q_IsRoutingEnabled;        ///< Flag for routing enabled using this bus
   bool q_IsDiagnosisEnabled;      ///< Flag for diagnosis enabled using this bus
   stw_types::uint32 u32_BusIndex; ///< Target bus index this interface connects to
   ///< (index in system definition)
   ///< Only valid if bus flag q_IsBusConnected is true

   void AddConnection(const stw_types::uint32 & oru32_BusIndex);
   void RemoveConnection(void);

   bool GetBusConnected(void) const;
   bool GetBusConnectedRawValue(void) const;
   void SetBusConnected(const bool oq_Value);
   bool GetInterfaceConnectedInDeviceRawValue(void) const;
   void SetInterfaceConnectedInDevice(const bool oq_Value);

private:
   bool mq_IsBusConnected;               ///< Flag if there is a valid bus connected to this interface
   bool mq_IsInterfaceConnectedInDevice; ///< Flag if the interface is active in device
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
