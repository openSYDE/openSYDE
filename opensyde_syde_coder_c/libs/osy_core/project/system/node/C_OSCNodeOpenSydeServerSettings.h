//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data class for openSYDE server specific settings (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OSCNODEOPENSYDESERVERSETTINGS_H
#define C_OSCNODEOPENSYDESERVERSETTINGS_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_core
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OSCNodeOpenSYDEServerSettings
{
public:
   C_OSCNodeOpenSYDEServerSettings(void);

   //lint -sem(stw_opensyde_core::C_OSCNodeOpenSYDEServerSettings::Initialize,initializer)
   void Initialize(void);

   void CalcHash(stw_types::uint32 & oru32_HashValue) const;

   stw_types::uint8 u8_MaxClients;               ///< Maximum number of concurrent clients
   stw_types::uint8 u8_MaxParallelTransmissions; ///< Maximum number of parallel cyclic/event driven transmissions
   stw_types::sint16 s16_DPDDataBlockIndex;      ///< Which application is Diagnostic Protocol Driver assigned to?
   ///< -1: Application not assigned
   ///< Else: Assigned application
   stw_types::uint16 u16_MaxMessageBufferTx; ///< CAN Tx routing FIFO size
   ///< (number of messages that can be buffered by in Tx direction)
   stw_types::uint16 u16_MaxRoutingMessageBufferRx; ///< CAN Rx routing FIFO size
   ///< (number of messages that can be buffered by in Rx direction)
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
