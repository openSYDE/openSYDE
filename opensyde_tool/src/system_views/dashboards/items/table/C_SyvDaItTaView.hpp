//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Table view for dashboard table widget (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDAITTAVIEW_HPP
#define C_SYVDAITTAVIEW_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_TblViewScroll.hpp"
#include "C_SyvDaItTaModel.hpp"
#include "C_PuiSvDbWidgetBase.hpp"
#include "C_SyvDaItTaDelegate.hpp"
#include "C_PuiSvDbDataElementHandler.hpp"
#include "C_SyvDaItTableHeaderView.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvDaItTaView :
   public C_TblViewScroll
{
   Q_OBJECT

public:
   C_SyvDaItTaView(stw::opensyde_gui_logic::C_PuiSvDbDataElementHandler * const opc_Data = NULL,
                   QWidget * const opc_Parent = NULL);
   ~C_SyvDaItTaView(void) override;

   void UpdateStaticValues(void);
   void UpdateValue(void);
   void UpdateError(void);
   void UpdateTransparency(const uint32_t ou32_DataElementIndex, const int32_t os32_Value);

   void AddItem(const stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId & orc_DataPoolElementId);
   void MoveSelected(const bool oq_Up);
   void RemoveSelectedItems(
      std::vector<stw::opensyde_gui_logic::C_PuiSvDbNodeDataPoolListElementId> & orc_RemovedDataElements);
   std::vector<uint32_t> GetUniqueSelectedRows(void) const;
   std::vector<uint32_t> GetUniqueAndValidSelectedRows(void) const;
   void SetCurrentColumnWidths(const std::vector<int32_t> & orc_ColumnWidths);
   std::vector<int32_t> GetCurrentColumnWidths(void) const;

   void SetSelectionAvailable(const bool oq_Active, const bool oq_SelectFirst);
   void SetDisplayStyle(const stw::opensyde_gui_logic::C_PuiSvDbWidgetBase::E_Style oe_Style, const bool oq_DarkMode);
   void SetCursorHandlingActive(const bool oq_Active);
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
   void keyPressEvent(QKeyEvent * const opc_Event) override;
   void mouseDoubleClickEvent(QMouseEvent * const opc_Event) override;

private:
   stw::opensyde_gui_logic::C_SyvDaItTaModel mc_Model;
   C_SyvDaItTaDelegate mc_Delegate;
   C_SyvDaItTableHeaderView * mpc_TableHeaderView;

   //Avoid call
   C_SyvDaItTaView(const C_SyvDaItTaView &);
   C_SyvDaItTaView & operator =(const C_SyvDaItTaView &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
