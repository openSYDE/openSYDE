//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Setup style undo command (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SEBUNOSETUPSTYLECOMMAND_H
#define C_SEBUNOSETUPSTYLECOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SebUnoBaseCommand.hpp"
#include "C_PuiBsElements.hpp"
#include "C_PuiBsTemporaryDataId.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SebUnoSetupStyleCommand :
   public C_SebUnoBaseCommand
{
public:
   C_SebUnoSetupStyleCommand(QGraphicsScene * const opc_Scene, const std::vector<uint64_t> & orc_Ids,
                             const bool oq_DarkMode, QUndoCommand * const opc_Parent = NULL);
   ~C_SebUnoSetupStyleCommand(void) override;
   void InitPrevious(void);
   void InitNext(void);
   void undo(void) override;
   void redo(void) override;

protected:
   C_PuiBsElements * mpc_PreviousState;
   C_PuiBsElements * mpc_NextState;

   virtual void m_Restore(const QMap<uint64_t, C_PuiBsTemporaryDataId> & orc_MapIdToTypeAndIndex,
                          const C_PuiBsElements * const opc_Snapshot);
   virtual void m_CreateMapAndSaveState(const std::vector<QGraphicsItem *> & orc_Items, QMap<uint64_t,
                                                                                             C_PuiBsTemporaryDataId> & orc_Map,
                                        C_PuiBsElements * const opc_Snapshot) const;

private:
   QMap<uint64_t, C_PuiBsTemporaryDataId> mc_MapIdToTypeAndIndexPrevious;
   QMap<uint64_t, C_PuiBsTemporaryDataId> mc_MapIdToTypeAndIndexNext;
   bool mq_DarkMode;
   //Avoid call
   C_SebUnoSetupStyleCommand(const C_SebUnoSetupStyleCommand &);
   C_SebUnoSetupStyleCommand & operator =(const C_SebUnoSetupStyleCommand &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
