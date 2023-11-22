//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for communication interface settings (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCNODECOMINTERFACESETTINGS_HPP
#define C_OSCNODECOMINTERFACESETTINGS_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "stwtypes.hpp"
#include "C_SclString.hpp"
#include "C_OscSystemBus.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscNodeComInterfaceSettings
{
public:
   class C_IpAddress
   {
   public:
      C_IpAddress(void);

      uint8_t au8_IpAddress[4];      //IP V4 address
      uint8_t au8_NetMask[4];        //IP V4 net mask
      uint8_t au8_DefaultGateway[4]; //IP V4 default gateway

      static const uint8_t hu8_IP_FIRST_BYTE;
      static const uint8_t hu8_IP_SECOND_BYTE;
      static const uint8_t hu8_IP_THIRD_BYTE;
      static const uint8_t hu8_IP_FOURTH_BYTE;
   };

   C_OscNodeComInterfaceSettings(void);
   ~C_OscNodeComInterfaceSettings(void);

   void CalcHash(uint32_t & oru32_HashValue) const;

   C_OscSystemBus::E_Type e_InterfaceType; ///< e.g. CAN, ethernet
   uint8_t u8_InterfaceNumber;             ///< 0 = first interface for this type
   uint8_t u8_NodeId;                      ///< Node ID on bus

   C_IpAddress c_Ip; ///< only used with interface type ethernet

   bool q_IsUpdateEnabled;    ///< Flag for update enabled using this bus
   bool q_IsRoutingEnabled;   ///< Flag for routing enabled using this bus
   bool q_IsDiagnosisEnabled; ///< Flag for diagnosis enabled using this bus
   uint32_t u32_BusIndex;     ///< Target bus index this interface connects to
   ///< (index in system definition)
   ///< Only valid if bus flag q_IsBusConnected is true

   void AddConnection(const uint32_t & oru32_BusIndex);
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
}
} //end of namespace

#endif
