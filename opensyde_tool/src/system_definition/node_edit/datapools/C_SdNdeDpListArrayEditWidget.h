//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data set array edit widget (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDPLISTARRAYEDITWIDGET_H
#define C_SDNDEDPLISTARRAYEDITWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>
#include <QUndoCommand>
#include "stwtypes.h"
#include "C_OgePopUpDialog.h"
#include "C_SdNdeDpUtil.h"
#include "C_SdNdeDpListModelViewManager.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SdNdeDpListArrayEditWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDpListArrayEditWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDpListArrayEditWidget(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                         const stw_types::uint32 & oru32_NodeIndex,
                                         const stw_types::uint32 & oru32_DataPoolIndex,
                                         const stw_types::uint32 & oru32_ListIndex,
                                         const stw_types::uint32 & oru32_ElementIndex,
                                         const stw_opensyde_gui_logic::C_SdNdeDpUtil::E_ArrayEditType & ore_ArrayEditType,
                                         const stw_types::uint32 & oru32_DataSetIndex);
   ~C_SdNdeDpListArrayEditWidget(void);
   void InitStaticNames(void) const;
   QUndoCommand * TakeUndoCommand(void) const;
   void SetModelViewManager(stw_opensyde_gui_logic::C_SdNdeDpListModelViewManager * const opc_Value) const;

protected:
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_SdNdeDpListArrayEditWidget * const mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_elements::C_OgePopUpDialog & mrc_Parent;
   const stw_types::uint32 mu32_NodeIndex;
   const stw_types::uint32 mu32_DataPoolIndex;
   const stw_types::uint32 mu32_ListIndex;
   const stw_types::uint32 mu32_ElementIndex;
   const stw_opensyde_gui_logic::C_SdNdeDpUtil::E_ArrayEditType me_ArrayEditType;
   const stw_types::uint32 mu32_DataSetIndex;

   void m_OkClicked(void);
   void m_CancelClicked(void);
   //Avoid call
   C_SdNdeDpListArrayEditWidget(const C_SdNdeDpListArrayEditWidget &);
   C_SdNdeDpListArrayEditWidget & operator =(const C_SdNdeDpListArrayEditWidget &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
