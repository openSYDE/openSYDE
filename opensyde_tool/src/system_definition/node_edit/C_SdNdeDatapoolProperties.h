//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Properties dialog for datapool properties (header)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDATAPOOLPROPERTIES_H
#define C_SDNDEDATAPOOLPROPERTIES_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "stwtypes.h"

#include "C_OgePopUpDialog.h"
#include "C_OSCNodeDataPool.h"
#include "C_OSCNodeDataPoolId.h"
#include "C_OSCCanProtocol.h"
#include "C_PuiSdNodeDataPool.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeDatapoolProperties;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SdNdeDatapoolProperties :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDatapoolProperties(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                      const QString & orc_Name,
                                      stw_opensyde_core::C_OSCNodeDataPool * const opc_OSCDataPool,
                                      stw_opensyde_gui_logic::C_PuiSdNodeDataPool * const pc_UiDataPool,
                                      stw_opensyde_core::C_OSCCanProtocol::E_Type * const ope_ComProtocolType,
                                      const stw_opensyde_core::C_OSCNodeDataPool::E_Type oe_DatapoolType,
                                      const stw_types::sint32 os32_DataPoolIndex,
                                      const stw_types::uint32 & oru32_NodeIndex, const bool oq_SelectName,
                                      const bool oq_ShowApplicationSection,
                                      const stw_opensyde_core::C_OSCNodeDataPoolId * const opc_SharedDatapoolId);
   virtual ~C_SdNdeDatapoolProperties(void);

   void InitStaticNames(void);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   //lint -restore

private:
   //Avoid call
   C_SdNdeDatapoolProperties(const C_SdNdeDatapoolProperties &);
   C_SdNdeDatapoolProperties & operator =(const C_SdNdeDatapoolProperties &);

   Ui::C_SdNdeDatapoolProperties * mpc_Ui;
   stw_opensyde_gui_elements::C_OgePopUpDialog * mpc_ParentDialog;
   stw_opensyde_core::C_OSCNodeDataPool::E_Type me_DatapoolType;
   stw_opensyde_core::C_OSCNodeDataPool * const mpc_OSCDataPool;
   stw_opensyde_gui_logic::C_PuiSdNodeDataPool * const mpc_UiDataPool;
   stw_opensyde_core::C_OSCCanProtocol::E_Type * const mpe_ComProtocolType;
   const stw_types::uint32 mu32_NodeIndex;
   const stw_types::sint32 ms32_DataPoolIndex;

   void m_OkClicked(void);
   void m_CancelClicked(void);
   void m_ApplyType(const bool oq_SharedDatapool);
   void m_LoadCodeGenerationAndApplication(void) const;
   void m_SpinBoxChanged(const stw_types::sintn osn_Value) const;
   void m_UpdateSizePrediction(void) const;
   void m_CheckDatapoolName(void) const;
   bool m_CheckDatapoolNameNotDuplicate(std::vector<stw_scl::C_SCLString> * const opc_ExistingDatapoolNames) const;
   void m_InitSpinBox(void) const;
   void m_InitComboBoxProtocols(const bool oq_NewDataPool, const stw_opensyde_core::C_OSCCanProtocol::E_Type oe_ComProtocolType =
                                   stw_opensyde_core::C_OSCCanProtocol::eLAYER2) const;
   bool m_IsProtocolSelectable(const stw_opensyde_core::C_OSCCanProtocol::E_Type oe_ComProtocolType) const;
   stw_opensyde_core::C_OSCCanProtocol::E_Type m_GetSelectedProtocol(void) const;
   void m_OnComTypeChange(void) const;
   void m_OnSafetyChange(const bool oq_IsSafety) const;
   void m_HandleDataPoolSafetyAdaptation(void);
   void m_BreakSharedRelation(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
