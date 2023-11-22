//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handler class for DBC data (header)

   See cpp file for detailed description

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMDBDBC_HPP
#define C_CAMDBDBC_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QMap>
#include "C_CieConverter.hpp"
#include "C_CamDbDbcMessageId.hpp"
#include "C_CamDbDbcUnmappedMessageId.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamDbDbc
{
public:
   C_CamDbDbc();

   void SetActive(const bool oq_Active);
   void SetData(const C_CieConverter::C_CieCommDefinition & orc_Data);

   void FindAllMessages(void);
   int32_t FindMessageById(const uint32_t ou32_Id, const bool oq_IsExtended, QString & orc_Message) const;
   int32_t FindMessage(const QString & orc_Message);

   bool GetActive(void) const;
   const std::vector<QString> GetFoundMessages(void) const;
   const C_CieConverter::C_CieCanMessage * GetDbcMessage(const QString & orc_Message, const bool oq_UseHash,
                                                         const uint32_t ou32_Hash) const;
   bool CheckHashForMessage(const QString & orc_Message, const uint32_t ou32_Hash) const;

private:
   bool mq_Active;
   bool mq_FoundAll;
   C_CieConverter::C_CieCommDefinition mc_Data;
   QMap<QString, C_CamDbDbcMessageId> mc_FoundMessagesNodes;
   QMap<QString, C_CamDbDbcUnmappedMessageId> mc_FoundMessagesUnmapped;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
