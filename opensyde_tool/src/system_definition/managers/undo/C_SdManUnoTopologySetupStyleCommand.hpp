//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Setup style undo command (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDMANUNOTOPOLOGYSETUPSTYLECOMMAND_H
#define C_SDMANUNOTOPOLOGYSETUPSTYLECOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SebUnoSetupStyleCommand.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdManUnoTopologySetupStyleCommand :
   public C_SebUnoSetupStyleCommand
{
public:
   C_SdManUnoTopologySetupStyleCommand(QGraphicsScene * const opc_Scene, const std::vector<uint64_t> & orc_Ids,
                                       const bool oq_DarkMode, QUndoCommand * const opc_Parent = NULL);
   ~C_SdManUnoTopologySetupStyleCommand(void) override;

protected:
   void m_Restore(const QMap<uint64_t, C_PuiBsTemporaryDataId> & orc_MapIdToTypeAndIndex,
                  const C_PuiBsElements * const opc_Snapshot) override;
   void m_CreateMapAndSaveState(const std::vector<QGraphicsItem *> & orc_Items, QMap<uint64_t,
                                                                                     C_PuiBsTemporaryDataId> & orc_Map,
                                C_PuiBsElements * const opc_Snapshot) const override;

private:
   //Avoid call
   C_SdManUnoTopologySetupStyleCommand(const C_SdManUnoTopologySetupStyleCommand &);
   C_SdManUnoTopologySetupStyleCommand & operator =(const C_SdManUnoTopologySetupStyleCommand &) &; //lint !e1511 //we
                                                                                                    // want to hide the
                                                                                                    // base
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
