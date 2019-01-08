//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Setup style undo command (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     25.07.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SEBUNOSETUPSTYLECOMMAND_H
#define C_SEBUNOSETUPSTYLECOMMAND_H

/* -- Includes ------------------------------------------------------------- */
#include "C_SebUnoBaseCommand.h"
#include "C_PuiBsElements.h"
#include "C_PuiBsTemporaryDataID.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SebUnoSetupStyleCommand :
   public C_SebUnoBaseCommand
{
public:
   C_SebUnoSetupStyleCommand(QGraphicsScene * const opc_Scene, const std::vector<stw_types::uint64> & orc_IDs,
                             const bool oq_DarkMode, QUndoCommand * const opc_Parent = NULL);
   virtual ~C_SebUnoSetupStyleCommand(void);
   void InitPrevious(void);
   void InitNext(void);
   virtual void undo(void) override;
   virtual void redo(void) override;

protected:
   C_PuiBsElements * mpc_PreviousState;
   C_PuiBsElements * mpc_NextState;

   virtual void m_Restore(const QMap<stw_types::uint64, C_PuiBsTemporaryDataID> & orc_MapIDToTypeAndIndex,
                          const C_PuiBsElements * const opc_Snapshot);
   virtual void m_CreateMapAndSaveState(const std::vector<QGraphicsItem *> & orc_Items, QMap<stw_types::uint64,
                                                                                             C_PuiBsTemporaryDataID> & orc_Map,
                                        C_PuiBsElements * const opc_Snapshot) const;

private:
   QMap<stw_types::uint64, C_PuiBsTemporaryDataID> mc_MapIDToTypeAndIndexPrevious;
   QMap<stw_types::uint64, C_PuiBsTemporaryDataID> mc_MapIDToTypeAndIndexNext;
   bool mq_DarkMode;
   //Avoid call
   C_SebUnoSetupStyleCommand(const C_SebUnoSetupStyleCommand &);
   C_SebUnoSetupStyleCommand & operator =(const C_SebUnoSetupStyleCommand &);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
