//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Store bus (header)

   Store bus (See .cpp file for full description)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCSYSTEMBUS_HPP
#define C_OSCSYSTEMBUS_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_SclString.hpp"
#include "C_OscCanProtocol.hpp"
#include "C_OscNodeDataPool.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscSystemBus
{
public:
   C_OscSystemBus(void);
   virtual ~C_OscSystemBus(void);

   void CalcHash(uint32_t & oru32_HashValue) const;

   enum E_Type ///< type of bus
   {
      eCAN,     ///< Data element is storing an CAN element
      eETHERNET ///< Data element is storing an ethernet element
   };

   static bool h_CompareNameGreater(const C_OscSystemBus & orc_Bus1, const C_OscSystemBus & orc_Bus2);
   bool CheckErrorBusId(void) const;

   E_Type e_Type;                   ///< type of bus (e.g. CAN / Ethernet)
   stw::scl::C_SclString c_Name;    ///< unique name of bus
   stw::scl::C_SclString c_Comment; ///< user comment
   uint64_t u64_BitRate;            ///< bitrate of bus in bits / second
   bool q_UseCanFd;                 ///< Flag to use CAN-FD on bus
   uint64_t u64_CanFdBitRate;       ///< CAN-FD bitrate of bus in bits / second
   uint8_t u8_BusId;                ///< unique bus id
   uint16_t u16_RxTimeoutOffsetMs;  ///< Delta time to wait for receive before timeout in ms
   bool q_UseableForRouting;        ///< indicator if routing is available for this bus
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
}

#endif
