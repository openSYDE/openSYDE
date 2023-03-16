//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for adding a new Datapool and configuring if it is a stand alone or shared Datapool

   See cpp file for detailed description

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDPSELECTORADDWIDGET_H
#define C_SDNDEDPSELECTORADDWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include <map>
#include "stwtypes.hpp"

#include "C_OgePopUpDialog.hpp"
#include "C_OscNodeDataPool.hpp"
#include "C_OscNodeDataPoolId.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeDpSelectorAddWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDpSelectorAddWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDpSelectorAddWidget(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                       const uint32_t ou32_NodeIndex,
                                       stw::opensyde_core::C_OscNodeDataPool & orc_OscDataPool);
   ~C_SdNdeDpSelectorAddWidget(void) override;

   void InitStaticNames(void) const;

   bool GetSelectedSharedDatapool(stw::opensyde_core::C_OscNodeDataPoolId & orc_SharedDatapoolId) const;

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_SdNdeDpSelectorAddWidget * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;

   const uint32_t mu32_NodeIndex;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_core::C_OscNodeDataPool & mrc_OscDataPool;

   std::map<QString, stw::opensyde_core::C_OscNodeDataPoolId> mc_AvailableDatapools;

   void m_InitFromData(void);

   void m_OkClicked(void);
   void m_CancelClicked(void);

   void m_OnStandAloneChange(void) const;
   void m_OnSharedChanged(void) const;

   void m_OnSharedDataPoolChanged(void) const;

   //Avoid call
   C_SdNdeDpSelectorAddWidget(const C_SdNdeDpSelectorAddWidget &);
   C_SdNdeDpSelectorAddWidget & operator =(const C_SdNdeDpSelectorAddWidget &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
