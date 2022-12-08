//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handler class for OSY data (header)

   See cpp file for detailed description

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMDBOSY_HPP
#define C_CAMDBOSY_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QMap>
#include "C_CamDbOsyListId.hpp"
#include "C_CamDbOsyMessageId.hpp"
#include "C_OscNodeDataPoolListElementId.hpp"
#include "C_OscCanMessageIdentificationIndices.hpp"
#include "C_OscComMessageLoggerOsySysDefConfig.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamDbOsy
{
public:
   C_CamDbOsy();

   void SetActive(const bool oq_Active);
   void SetData(const stw::opensyde_core::C_OscComMessageLoggerOsySysDefConfig & orc_Data);

   void FindAllMessages(void);
   int32_t FindMessageById(const uint32_t ou32_Id, const bool oq_IsExtended, QString & orc_Message) const;
   int32_t FindMessage(const QString & orc_Message);

   bool GetActive(void) const;
   const QMap<QString, C_CamDbOsyMessageId> & GetFoundMessages(void) const;
   const stw::opensyde_core::C_OscCanMessage * GetOscMessage(const QString & orc_Message, const bool oq_UseHash,
                                                             const uint32_t ou32_Hash,
                                                             stw::opensyde_core::C_OscCanProtocol::E_Type * const ope_ProtocolType)
   const;
   const stw::opensyde_core::C_OscNodeDataPoolList * GetOscList(const QString & orc_Message, const bool oq_UseHash,
                                                                const uint32_t ou32_Hash) const;

   bool ReplaceOsyBusIndex(const uint32_t ou32_BusIndex);
   bool CheckHashForMessage(const QString & orc_Message, const uint32_t ou32_Hash) const;

private:
   bool mq_Active;
   bool mq_FoundAll;
   stw::opensyde_core::C_OscComMessageLoggerOsySysDefConfig mc_Data;
   QMap<QString, C_CamDbOsyListId> mc_FoundMessagesListId;
   QMap<QString, C_CamDbOsyMessageId> mc_FoundMessagesId;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
