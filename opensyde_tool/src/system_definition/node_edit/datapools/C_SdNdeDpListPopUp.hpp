//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       List edit pop up widget (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDPLISTPOPUP_HPP
#define C_SDNDEDPLISTPOPUP_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include <QTreeWidget>
#include "stwtypes.hpp"
#include "C_OgePopUpDialog.hpp"
#include "C_SdNdeDpListModelViewManager.hpp"
#include "C_SdNdeUnoDataPoolManager.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeDpListPopUp;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SdNdeDpListPopUp :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDpListPopUp(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                               const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                               const uint32_t & oru32_ListIndex,
                               stw::opensyde_gui_logic::C_SdNdeDpListModelViewManager * const opc_ModelViewManager,
                               QTreeWidget * const opc_TreeWidget,
                               stw::opensyde_gui_logic::C_SdNdeUnoDataPoolManager * const opc_UndoManager,
                               QWidget * const opc_Parent = NULL);
   ~C_SdNdeDpListPopUp(void) override;
   void InitText(void) const;
   void Clear(void) const;
   void UpdateHeader(void) const;
   bool GetSaveAsAfterClose(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigSave(void);

protected:
   void paintEvent(QPaintEvent * const opc_Event) override;
   void keyPressEvent(QKeyEvent * const opc_Event) override;

private:
   Ui::C_SdNdeDpListPopUp * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_gui_elements::C_OgePopUpDialog & mrc_Parent;
   const uint32_t mu32_NodeIndex;
   const uint32_t mu32_DataPoolIndex;
   const uint32_t mu32_ListIndex;
   stw::opensyde_gui_logic::C_SdNdeDpListModelViewManager * const mpc_ModelViewManager;
   QTreeWidget * const mpc_TreeWidget;
   stw::opensyde_gui_logic::C_SdNdeUnoDataPoolManager * const mpc_UndoManager;
   int32_t ms32_UndoStartCount;
   bool mq_SaveAsAfterClose;

   void m_InitButtonIcons(void) const;
   void m_Close(void);
   void m_HandleSelection(const uint32_t & oru32_ListIndex, const uint32_t & oru32_Count) const;
   void m_HandleButtonChange(const bool & orq_AddActive, const bool & orq_CutActive, const bool & orq_CopyActive,
                             const bool & orq_PasteActive, const bool & orq_DeleteActive,
                             const bool & orq_MoveDownActive, const bool & orq_MoveUpActive) const;
   void m_OpenColorPicker(void);
   //Avoid call
   C_SdNdeDpListPopUp(const C_SdNdeDpListPopUp &);
   C_SdNdeDpListPopUp & operator =(const C_SdNdeDpListPopUp &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif // C_SDNDEDPLISTPOPUP_HPP
