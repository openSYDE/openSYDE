//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handler class for DBC data (header)

   See cpp file for detailed description

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMDBDBC_H
#define C_CAMDBDBC_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QMap>
#include "C_CieConverter.h"
#include "C_CamDbDbcMessageId.h"
#include "C_CamDbDbcUnmappedMessageId.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamDbDbc
{
public:
   C_CamDbDbc();

   void SetActive(const bool oq_Active);
   void SetData(const C_CieConverter::C_CIECommDefinition & orc_Data);

   void FindAllMessages(void);
   stw_types::sint32 FindMessageById(const stw_types::uint32 ou32_Id, const bool oq_IsExtended,
                                     QString & orc_Message) const;
   stw_types::sint32 FindMessage(const QString & orc_Message);

   bool GetActive(void) const;
   const std::vector<QString> GetFoundMessages(void) const;
   const C_CieConverter::C_CIECanMessage * GetDBCMessage(const QString & orc_Message, const bool oq_UseHash,
                                                         const stw_types::uint32 ou32_Hash) const;
   bool CheckHashForMessage(const QString & orc_Message, const stw_types::uint32 ou32_Hash) const;

private:
   bool mq_Active;
   bool mq_FoundAll;
   C_CieConverter::C_CIECommDefinition mc_Data;
   QMap<QString, C_CamDbDbcMessageId> mc_FoundMessagesNodes;
   QMap<QString, C_CamDbDbcUnmappedMessageId> mc_FoundMessagesUnmapped;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
