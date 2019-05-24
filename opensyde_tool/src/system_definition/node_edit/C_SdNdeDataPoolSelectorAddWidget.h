//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for adding a new Datapool and configuring if it is a stand alone or shared Datapool

   See cpp file for detailed description

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDATAPOOLSELECTORADDWIDGET_H
#define C_SDNDEDATAPOOLSELECTORADDWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include <map>
#include "stwtypes.h"

#include "C_OgePopUpDialog.h"
#include "C_OSCNodeDataPool.h"
#include "C_OSCNodeDataPoolId.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeDataPoolSelectorAddWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDataPoolSelectorAddWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDataPoolSelectorAddWidget(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                             const stw_types::uint32 ou32_NodeIndex,
                                             stw_opensyde_core::C_OSCNodeDataPool & orc_OSCDataPool);
   ~C_SdNdeDataPoolSelectorAddWidget(void);

   void InitStaticNames(void) const;

   bool GetSelectedSharedDatapool(stw_opensyde_core::C_OSCNodeDataPoolId & orc_SharedDatapoolId) const;

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   //lint -restore

private:
   Ui::C_SdNdeDataPoolSelectorAddWidget * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;

   const stw_types::uint32 mu32_NodeIndex;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_core::C_OSCNodeDataPool & mrc_OSCDataPool;

   std::map<QString, stw_opensyde_core::C_OSCNodeDataPoolId> mc_AvailableDatapools;

   void m_InitFromData(void);

   void m_OkClicked(void);
   void m_CancelClicked(void);

   void m_OnStandAloneChange(void) const;
   void m_OnSharedChanged(void) const;

   void m_OnSharedDataPoolChanged(void) const;

   //Avoid call
   C_SdNdeDataPoolSelectorAddWidget(const C_SdNdeDataPoolSelectorAddWidget &);
   C_SdNdeDataPoolSelectorAddWidget & operator =(const C_SdNdeDataPoolSelectorAddWidget &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
