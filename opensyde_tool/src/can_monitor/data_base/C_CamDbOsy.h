//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Handler class for OSY data (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     17.01.2019  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_CAMDBOSY_H
#define C_CAMDBOSY_H

/* -- Includes ------------------------------------------------------------- */
#include <QMap>
#include "C_OSCNodeDataPoolListElementId.h"
#include "C_OSCCanMessageIdentificationIndices.h"
#include "C_OSCComMessageLoggerOsySysDefConfig.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_CamDbOsy
{
public:
   C_CamDbOsy();

   void SetActive(const bool oq_Active);
   void SetData(const stw_opensyde_core::C_OSCComMessageLoggerOsySysDefConfig & orc_Data);

   void FindAllMessages(void);
   stw_types::sint32 FindMessageById(const stw_types::uint32 ou32_Id, QString & orc_Message) const;
   stw_types::sint32 FindMessage(const QString & orc_Message);

   bool GetActive(void) const;
   const QMap<QString, stw_opensyde_core::C_OSCCanMessageIdentificationIndices> & GetFoundMessages(void) const;
   const stw_opensyde_core::C_OSCCanMessage * GetOSCMessage(const QString & orc_Message) const;
   const stw_opensyde_core::C_OSCNodeDataPoolList * GetOSCList(const QString & orc_Message) const;

   bool ReplaceOsyBusIndex(const stw_types::uint32 ou32_BusIndex);

private:
   bool mq_Active;
   bool mq_FoundAll;
   stw_opensyde_core::C_OSCComMessageLoggerOsySysDefConfig mc_Data;
   QMap<QString, stw_opensyde_core::C_OSCNodeDataPoolListId> mc_FoundMessagesListId;
   QMap<QString, stw_opensyde_core::C_OSCCanMessageIdentificationIndices> mc_FoundMessagesId;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
