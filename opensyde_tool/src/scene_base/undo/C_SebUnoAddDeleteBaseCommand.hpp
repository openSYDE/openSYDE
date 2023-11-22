//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Add and delete base class (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SEBUNOADDDELETEBASECOMMAND_HPP
#define C_SEBUNOADDDELETEBASECOMMAND_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_SebUnoBaseCommand.hpp"
#include "C_PuiBsTemporaryDataId.hpp"
#include "C_PuiBsElements.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SebUnoAddDeleteBaseCommand :
   public C_SebUnoBaseCommand
{
public:
   C_SebUnoAddDeleteBaseCommand(QGraphicsScene * const opc_Scene, const std::vector<uint64_t> & orc_Ids,
                                const QString & orc_Text, QUndoCommand * const opc_Parent = NULL);
   void undo(void) override = 0;
   void redo(void) override = 0;

protected:
   virtual void m_DeleteSave(void) = 0;
   virtual void m_Restore(void) = 0;
   void m_StoreCommon(C_PuiBsElements & orc_Storage, QMap<C_PuiBsTemporaryDataId,
                                                          uint64_t> & orc_MapTypeAndIndexToId,
                      const C_PuiBsElements & orc_AllElements) const;

   virtual int32_t m_GetBoundaryType(void) const = 0;
   virtual int32_t m_GetImageType(void) const = 0;
   virtual int32_t m_GetLineArrowType(void) const = 0;
   virtual int32_t m_GetTextElementType(void) const = 0;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
