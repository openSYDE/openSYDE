//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handler class for OSY data (header)

   See cpp file for detailed description

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMDBOSY_H
#define C_CAMDBOSY_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QMap>
#include "C_CamDbOsyListId.h"
#include "C_CamDbOsyMessageId.h"
#include "C_OSCNodeDataPoolListElementId.h"
#include "C_OSCCanMessageIdentificationIndices.h"
#include "C_OSCComMessageLoggerOsySysDefConfig.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamDbOsy
{
public:
   C_CamDbOsy();

   void SetActive(const bool oq_Active);
   void SetData(const stw_opensyde_core::C_OSCComMessageLoggerOsySysDefConfig & orc_Data);

   void FindAllMessages(void);
   stw_types::sint32 FindMessageById(const stw_types::uint32 ou32_Id, const bool oq_IsExtended,
                                     QString & orc_Message) const;
   stw_types::sint32 FindMessage(const QString & orc_Message);

   bool GetActive(void) const;
   const QMap<QString, C_CamDbOsyMessageId> & GetFoundMessages(void) const;
   const stw_opensyde_core::C_OSCCanMessage * GetOSCMessage(const QString & orc_Message, const bool oq_UseHash,
                                                            const stw_types::uint32 ou32_Hash,
                                                            stw_opensyde_core::C_OSCCanProtocol::E_Type * const ope_ProtocolType)
   const;
   const stw_opensyde_core::C_OSCNodeDataPoolList * GetOSCList(const QString & orc_Message, const bool oq_UseHash,
                                                               const stw_types::uint32 ou32_Hash) const;

   bool ReplaceOsyBusIndex(const stw_types::uint32 ou32_BusIndex);
   bool CheckHashForMessage(const QString & orc_Message, const stw_types::uint32 ou32_Hash) const;

private:
   bool mq_Active;
   bool mq_FoundAll;
   stw_opensyde_core::C_OSCComMessageLoggerOsySysDefConfig mc_Data;
   QMap<QString, C_CamDbOsyListId> mc_FoundMessagesListId;
   QMap<QString, C_CamDbOsyMessageId> mc_FoundMessagesId;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
