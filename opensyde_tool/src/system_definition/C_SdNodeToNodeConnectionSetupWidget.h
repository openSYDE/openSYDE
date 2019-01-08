//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Node to Node connection setup pop up dialog (header)

   \implementation
   project     opensyde
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     07.06.2017  STW/S.Singer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDNODETONODECONNECTIONSETUPWIDGET_H
#define C_SDNODETONODECONNECTIONSETUPWIDGET_H

/* -- Includes ------------------------------------------------------------- */
#include <QWidget>
#include <QRadioButton>

#include "stwtypes.h"
#include "C_OgePopUpDialog.h"
#include "C_OSCSystemBus.h"
#include "C_OSCNodeComInterfaceSettings.h"

/* -- Namespace ------------------------------------------------------------ */
namespace Ui
{
class C_SdNodeToNodeConnectionSetupWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */
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
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   //lint -restore

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
   bool mq_RestrictType;
   stw_opensyde_core::C_OSCSystemBus::E_Type me_RestrictedType;

   void m_OkClicked(void);
   void m_CancelClicked(void);
   void m_InitFromData(void);
   void m_HandleTypeChange(void);
   void m_HandleBusChange(void);
   void m_InitDynamicContent(void);
   void m_OnNodeId1Change(const stw_types::sint32 & ors32_Value);
   void m_OnNodeId2Change(const stw_types::sint32 & ors32_Value);
   void m_CheckNodeIds(void);
   void m_OnBusNameChange(const QString & orc_Value) const;
   void m_OnNewOrExistingChange(void);
   void m_OnBusTypeChange(void);
   void m_OnExistingBusChange(void);
   bool m_CheckTypeRestrictions(stw_opensyde_core::C_OSCSystemBus::E_Type & ore_BusType) const;
   bool m_CheckDatapoolNumberNodes(void) const;
   bool m_CheckDatapoolNumberNode1(void) const;
   bool m_CheckDatapoolNumberNode2(void) const;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
