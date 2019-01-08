//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list element base command (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     25.01.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDNDEUNOLEDATAPOOLLISTELEMENTBASECOMMAND_H
#define C_SDNDEUNOLEDATAPOOLLISTELEMENTBASECOMMAND_H

/* -- Includes ------------------------------------------------------------- */

#include <QString>
#include <QUndoCommand>
#include "C_SdNdeDataPoolListModelViewManager.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SdNdeUnoLeDataPoolListElementBaseCommand :
   public QUndoCommand
{
public:
   C_SdNdeUnoLeDataPoolListElementBaseCommand(const stw_types::uint32 & oru32_NodeIndex,
                                              const stw_types::uint32 & oru32_DataPoolIndex,
                                              const stw_types::uint32 & oru32_DataPoolListIndex,
                                              C_SdNdeDataPoolListModelViewManager * const opc_DataPoolListModelViewManager, const QString & orc_Text = "",
                                              QUndoCommand * const opc_Parent = NULL);

protected:
   const stw_types::uint32 mu32_NodeIndex;
   const stw_types::uint32 mu32_DataPoolIndex;
   const stw_types::uint32 mu32_DataPoolListIndex;
   C_SdNdeDataPoolListModelViewManager * const mpc_DataPoolListModelViewManager;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
