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
#include "C_SebUnoSetupStyleCommand.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdManUnoTopologySetupStyleCommand :
   public C_SebUnoSetupStyleCommand
{
public:
   C_SdManUnoTopologySetupStyleCommand(QGraphicsScene * const opc_Scene, const std::vector<stw_types::uint64> & orc_IDs,
                                       const bool oq_DarkMode, QUndoCommand * const opc_Parent = NULL);
   virtual ~C_SdManUnoTopologySetupStyleCommand(void);

protected:
   virtual void m_Restore(const QMap<stw_types::uint64, C_PuiBsTemporaryDataID> & orc_MapIDToTypeAndIndex,
                          const C_PuiBsElements * const opc_Snapshot);
   virtual void m_CreateMapAndSaveState(const std::vector<QGraphicsItem *> & orc_Items, QMap<stw_types::uint64,
                                                                                             C_PuiBsTemporaryDataID> & orc_Map,
                                        C_PuiBsElements * const opc_Snapshot) const;

private:
   //Avoid call
   C_SdManUnoTopologySetupStyleCommand(const C_SdManUnoTopologySetupStyleCommand &);
   C_SdManUnoTopologySetupStyleCommand & operator =(const C_SdManUnoTopologySetupStyleCommand &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
