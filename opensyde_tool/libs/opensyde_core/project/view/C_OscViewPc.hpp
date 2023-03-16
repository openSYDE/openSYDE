//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       System view PC item data element (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCVIEWPC_HPP
#define C_OSCVIEWPC_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"
#include "C_SclString.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OscViewPc
{
public:
   C_OscViewPc(void);

   void CalcHash(uint32_t & oru32_HashValue) const;

   bool GetConnected(void) const;
   uint32_t GetBusIndex(void) const;
   void SetConnected(const bool oq_Connected, const uint32_t ou32_BusIndex, const bool & orq_ForceSimpleSet = false);

   //Sync to system definition
   void OnSyncBusAdded(const uint32_t ou32_Index);
   void OnSyncBusDeleted(const uint32_t ou32_Index);
   bool CheckIndirectDisconnection(void) const;

private:
   bool mq_Connected;      ///< Flag if there is a valid bus connected to this PC
   uint32_t mu32_BusIndex; ///< Target bus index this PC connects to
   ///< Only valid if bus flag q_IsBusConnected is true
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
