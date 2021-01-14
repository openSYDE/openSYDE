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
   stw_types::uint8 GetNodeId1(void) const;
   stw_types::uint32 GetSelectedInterface2(void) const;
   stw_types::uint8 GetNodeId2(void) const;
   void GetComDataPoolConfigurationNode1(bool & orq_ComProtocolL2, bool & orq_ComProtocolECeS,
                                         bool & orq_ComProtocolECoS) const;
   void GetComDataPoolConfigurationNode2(bool & orq_ComProtocolL2, bool & orq_ComProtocolECeS,
                                         bool & orq_ComProtocolECoS) const;
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
   std::vector<stw_types::uint32> mc_UsedNodeIds;
   std::vector<stw_types::uint32> mc_BusIndices;
   bool mq_InteractionPossible;

   void m_OkClicked(void);
   void m_CancelClicked(void);
   void m_InitFromData(void);
   void m_HandleTypeChange(void) const;
   void m_HandleBusChange(void);
   void m_InitDynamicContent(void);
   void m_OnNodeId1Change(const stw_types::sint32 & ors32_Value);
   void m_OnNodeId2Change(const stw_types::sint32 & ors32_Value);
   void m_CheckNodeIds(void);
   void m_OnBusNameChange(const QString & orc_Value) const;
   void m_OnNewOrExistingChange(void);
   void m_OnBusTypeChange(void) const;
   void m_OnExistingBusChange(void);
   void m_CheckTypeRestrictions(bool & orq_NewBusPossible, bool & orq_NewBusRestricted,
                                stw_opensyde_core::C_OSCSystemBus::E_Type & ore_NewBusType,
                                bool & orq_ExistingBusExists, bool & orq_ExistingBusRestricted,
                                stw_opensyde_core::C_OSCSystemBus::E_Type & ore_ExistingBusType) const;
   bool m_CheckDatapoolNumberNodes(void) const;
   bool m_CheckDatapoolNumberNode1(void) const;
   bool m_CheckDatapoolNumberNode2(void) const;

   const QString mc_BUS_TYPE_CAN;
   const QString mc_BUS_TYPE_ETHERNET;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
