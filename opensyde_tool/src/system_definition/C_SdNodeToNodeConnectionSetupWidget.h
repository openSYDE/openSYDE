//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node to Node connection setup pop up dialog (header)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNODETONODECONNECTIONSETUPWIDGET_H
#define C_SDNODETONODECONNECTIONSETUPWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include <QRadioButton>

#include "stwtypes.h"
#include "C_OgePopUpDialog.h"
#include "C_OSCSystemBus.h"
#include "C_OSCNodeComInterfaceSettings.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNodeToNodeConnectionSetupWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SdNodeToNodeConnectionSetupWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNodeToNodeConnectionSetupWidget(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                                const stw_types::uint32 & oru32_Node1Index,
                                                const stw_types::uint32 & oru32_Node2Index);
   virtual ~C_SdNodeToNodeConnectionSetupWidget(void);

   void InitStaticNames(void) const;
   stw_types::uint32 GetSelectedInterface1(void) const;
   stw_types::uint32 GetSelectedInterface2(void) const;
   stw_opensyde_core::C_OSCSystemBus::E_Type GetBusType(void) const;
   stw_types::uint32 GetBusIndex(void) const;
   QString GetBusName(void) const;
   bool CheckIfCreateNew(void) const;
   bool GetInteractionPossible(void) const;

protected:
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   //Avoid call
   C_SdNodeToNodeConnectionSetupWidget(const C_SdNodeToNodeConnectionSetupWidget &);
   C_SdNodeToNodeConnectionSetupWidget & operator =(const C_SdNodeToNodeConnectionSetupWidget &);

   Ui::C_SdNodeToNodeConnectionSetupWidget * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   const stw_types::uint32 mu32_Node1Index;
   const stw_types::uint32 mu32_Node2Index;
   stw_types::uint32 mu32_NodeID1;
   stw_types::uint32 mu32_NodeID2;
   std::vector<stw_types::uint32> mc_UsedNodeIds;
   std::vector<stw_types::uint32> mc_BusIndices;
   bool mq_InteractionPossible;

   void m_OkClicked(void);
   void m_CancelClicked(void);
   void m_InitFromData(void);
   void m_HandleTypeChange(void) const;
   void m_OnBusNameChange(const QString & orc_Value) const;
   void m_OnNewOrExistingChange(void);
   void m_OnBusTypeChange(void) const;
   void m_OnExistingBusChange(void);
   void m_CheckTypeRestrictions(bool & orq_NewBusPossible, bool & orq_NewBusRestricted,
                                stw_opensyde_core::C_OSCSystemBus::E_Type & ore_NewBusType,
                                bool & orq_ExistingBusExists, bool & orq_ExistingBusRestricted,
                                stw_opensyde_core::C_OSCSystemBus::E_Type & ore_ExistingBusType) const;

   const QString mc_BusTypeCan;
   const QString mc_BusTypeEthernet;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
