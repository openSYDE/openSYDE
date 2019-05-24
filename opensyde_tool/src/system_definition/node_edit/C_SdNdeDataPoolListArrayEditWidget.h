//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data set array edit widget (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDATAPOOLLISTARRAYEDITWIDGET_H
#define C_SDNDEDATAPOOLLISTARRAYEDITWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>
#include <QUndoCommand>
#include "stwtypes.h"
#include "C_OgePopUpDialog.h"
#include "C_SdNdeDataPoolUtil.h"
#include "C_SdNdeDataPoolListModelViewManager.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SdNdeDataPoolListArrayEditWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDataPoolListArrayEditWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDataPoolListArrayEditWidget(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                               const stw_types::uint32 & oru32_NodeIndex,
                                               const stw_types::uint32 & oru32_DataPoolIndex,
                                               const stw_types::uint32 & oru32_ListIndex,
                                               const stw_types::uint32 & oru32_ElementIndex,
                                               const stw_opensyde_gui_logic::C_SdNdeDataPoolUtil::E_ArrayEditType & ore_ArrayEditType,
                                               const stw_types::uint32 & oru32_DataSetIndex);
   ~C_SdNdeDataPoolListArrayEditWidget(void);
   void InitStaticNames(void) const;
   QUndoCommand * TakeUndoCommand(void) const;
   void SetModelViewManager(stw_opensyde_gui_logic::C_SdNdeDataPoolListModelViewManager * const opc_Value) const;

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   //lint -restore

private:
   Ui::C_SdNdeDataPoolListArrayEditWidget * const mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_elements::C_OgePopUpDialog & mrc_Parent;
   const stw_types::uint32 mu32_NodeIndex;
   const stw_types::uint32 mu32_DataPoolIndex;
   const stw_types::uint32 mu32_ListIndex;
   const stw_types::uint32 mu32_ElementIndex;
   const stw_opensyde_gui_logic::C_SdNdeDataPoolUtil::E_ArrayEditType me_ArrayEditType;
   const stw_types::uint32 mu32_DataSetIndex;

   void m_OkClicked(void);
   void m_CancelClicked(void);
   //Avoid call
   C_SdNdeDataPoolListArrayEditWidget(const C_SdNdeDataPoolListArrayEditWidget &);
   C_SdNdeDataPoolListArrayEditWidget & operator =(const C_SdNdeDataPoolListArrayEditWidget &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
