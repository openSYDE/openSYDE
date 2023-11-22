//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Node to Node connection setup pop up dialog (header)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNODETONODECONNECTIONSETUPWIDGET_HPP
#define C_SDNODETONODECONNECTIONSETUPWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include <QRadioButton>

#include "stwtypes.hpp"
#include "C_OgePopUpDialog.hpp"
#include "C_OscSystemBus.hpp"
#include "C_OscNodeComInterfaceSettings.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNodeToNodeConnectionSetupWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SdNodeToNodeConnectionSetupWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNodeToNodeConnectionSetupWidget(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                                const uint32_t & oru32_Node1Index, const uint32_t & oru32_Node2Index);
   ~C_SdNodeToNodeConnectionSetupWidget(void) override;

   void InitStaticNames(void) const;
   uint32_t GetSelectedInterface1(void) const;
   uint32_t GetSelectedInterface2(void) const;
   stw::opensyde_core::C_OscSystemBus::E_Type GetBusType(void) const;
   uint32_t GetBusIndex(void) const;
   QString GetBusName(void) const;
   bool CheckIfCreateNew(void) const;
   bool GetInteractionPossible(void) const;

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   //Avoid call
   C_SdNodeToNodeConnectionSetupWidget(const C_SdNodeToNodeConnectionSetupWidget &);
   C_SdNodeToNodeConnectionSetupWidget & operator =(const C_SdNodeToNodeConnectionSetupWidget &) &;

   Ui::C_SdNodeToNodeConnectionSetupWidget * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   const uint32_t mu32_Node1Index;
   const uint32_t mu32_Node2Index;
   uint32_t mu32_NodeId1;
   uint32_t mu32_NodeId2;
   std::vector<uint32_t> mc_UsedNodeIds;
   std::vector<uint32_t> mc_BusIndices;
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
                                stw::opensyde_core::C_OscSystemBus::E_Type & ore_NewBusType,
                                bool & orq_ExistingBusExists, bool & orq_ExistingBusRestricted,
                                stw::opensyde_core::C_OscSystemBus::E_Type & ore_ExistingBusType) const;

   const QString mc_BusTypeCan;
   const QString mc_BusTypeEthernet;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
