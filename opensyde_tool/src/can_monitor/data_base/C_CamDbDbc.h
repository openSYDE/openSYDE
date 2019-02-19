//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Handler class for DBC data (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     17.01.2019  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_CAMDBDBC_H
#define C_CAMDBDBC_H

/* -- Includes ------------------------------------------------------------- */
#include <QMap>
#include "C_CieConverter.h"
#include "C_CamDbDbcMessageId.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_CamDbDbc
{
public:
   C_CamDbDbc();

   void SetActive(const bool oq_Active);
   void SetData(const C_CieConverter::C_CIECommDefinition & orc_Data);

   void FindAllMessages(void);
   stw_types::sint32 FindMessageById(const stw_types::uint32 ou32_Id, QString & orc_Message) const;
   stw_types::sint32 FindMessage(const QString & orc_Message);

   bool GetActive(void) const;
   const QMap<QString, C_CamDbDbcMessageId> & GetFoundMessages(void) const;
   const C_CieConverter::C_CIECanMessage * GetDBCMessage(const QString & orc_Message) const;

private:
   bool mq_Active;
   bool mq_FoundAll;
   C_CieConverter::C_CIECommDefinition mc_Data;
   QMap<QString, C_CamDbDbcMessageId> mc_FoundMessages;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
