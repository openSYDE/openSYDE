//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Add and delete base class (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SEBUNOADDDELETEBASECOMMAND_H
#define C_SEBUNOADDDELETEBASECOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SebUnoBaseCommand.h"
#include "C_PuiBsTemporaryDataID.h"
#include "C_PuiBsElements.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SebUnoAddDeleteBaseCommand :
   public C_SebUnoBaseCommand
{
public:
   C_SebUnoAddDeleteBaseCommand(QGraphicsScene * const opc_Scene, const std::vector<stw_types::uint64> & orc_IDs,
                                const QString & orc_Text, QUndoCommand * const opc_Parent = NULL);
   virtual void undo(void) override = 0;
   virtual void redo(void) override = 0;

protected:
   virtual void m_DeleteSave(void) = 0;
   virtual void m_Restore(void) = 0;
   void m_StoreCommon(C_PuiBsElements & orc_Storage, QMap<C_PuiBsTemporaryDataID,
                                                          stw_types::uint64> & orc_MapTypeAndIndexToID,
                      const C_PuiBsElements & orc_AllElements) const;

   virtual stw_types::sint32 m_GetBoundaryType(void) const = 0;
   virtual stw_types::sint32 m_GetImageType(void) const = 0;
   virtual stw_types::sint32 m_GetLineArrowType(void) const = 0;
   virtual stw_types::sint32 m_GetTextElementType(void) const = 0;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
