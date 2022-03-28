//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Table view for dashboard table widget (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAITTAVIEW_H
#define C_SYVDAITTAVIEW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_TblViewScroll.h"
#include "C_SyvDaItTaModel.h"
#include "C_PuiSvDbWidgetBase.h"
#include "C_SyvDaItTaDelegate.h"
#include "C_PuiSvDbDataElementHandler.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaItTaView :
   public C_TblViewScroll
{
   Q_OBJECT

public:
   C_SyvDaItTaView(stw_opensyde_gui_logic::C_PuiSvDbDataElementHandler * const opc_Data = NULL,
                   QWidget * const opc_Parent = NULL);
   ~C_SyvDaItTaView(void);

   void UpdateStaticValues(void);
   void UpdateValue(void);
   void UpdateError(void);
   void UpdateTransparency(const stw_types::uint32 ou32_DataElementIndex, const stw_types::sintn osn_Value);

   void AddItem(const stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId);
   void MoveSelected(const bool oq_Up);
   void RemoveSelectedItems(
      std::vector<stw_opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId> & orc_RemovedDataElements);
   std::vector<stw_types::uint32> GetUniqueSelectedRows(void) const;
   std::vector<stw_types::uint32> GetUniqueAndValidSelectedRows(void) const;
   void SetCurrentColumnWidths(const std::vector<stw_types::sint32> & orc_ColumnWidths);
   std::vector<stw_types::sint32> GetCurrentColumnWidths(void) const;

   void SetSelectionAvailable(const bool oq_Active, const bool oq_SelectFirst);
   void SetDisplayStyle(const stw_opensyde_gui_logic::C_PuiSvDbWidgetBase::E_Style oe_Style, const bool oq_DarkMode);
   void HideToolTip(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigTriggerEdit(void);
   void SigAddDataElement(void);
   void SigMoveDataElementUp(void);
   void SigMoveDataElementDown(void);
   void SigRemoveDataElement(void);

protected:
   virtual void keyPressEvent(QKeyEvent * const opc_Event) override;
   virtual void mouseDoubleClickEvent(QMouseEvent * const opc_Event) override;

private:
   stw_opensyde_gui_logic::C_SyvDaItTaModel mc_Model;
   C_SyvDaItTaDelegate mc_Delegate;

   //Avoid call
   C_SyvDaItTaView(const C_SyvDaItTaView &);
   C_SyvDaItTaView & operator =(const C_SyvDaItTaView &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
