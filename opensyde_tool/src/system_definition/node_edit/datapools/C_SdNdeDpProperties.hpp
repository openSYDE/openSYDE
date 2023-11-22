//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Properties dialog for datapool properties (header)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDPPROPERTIES_HPP
#define C_SDNDEDPPROPERTIES_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "stwtypes.hpp"

#include "C_OgePopUpDialog.hpp"
#include "C_OscNodeDataPool.hpp"
#include "C_OscNodeDataPoolId.hpp"
#include "C_OscCanProtocol.hpp"
#include "C_PuiSdNodeDataPool.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeDpProperties;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SdNdeDpProperties :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDpProperties(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                stw::opensyde_core::C_OscNodeDataPool * const opc_OscDataPool,
                                stw::opensyde_gui_logic::C_PuiSdNodeDataPool * const opc_UiDataPool,
                                stw::opensyde_core::C_OscCanProtocol::E_Type * const ope_ComProtocolType,
                                const int32_t os32_DataPoolIndex, const uint32_t & oru32_NodeIndex,
                                const bool oq_SelectName, const bool oq_NodeProgrammingSupport,
                                const stw::opensyde_core::C_OscNodeDataPoolId * const opc_SharedDatapoolId);
   ~C_SdNdeDpProperties(void) override;

   void InitStaticNames(void);
   bool GetIsDatapoolShared() const;
   void SetIsDatapoolShared(const bool o_IsShared);

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   //Avoid call
   C_SdNdeDpProperties(const C_SdNdeDpProperties &);
   C_SdNdeDpProperties & operator =(const C_SdNdeDpProperties &) &;

   static const int32_t mhs32_INDEX_PRIVATE;
   static const int32_t mhs32_INDEX_PUBLIC;

   Ui::C_SdNdeDpProperties * mpc_Ui;
   stw::opensyde_gui_elements::C_OgePopUpDialog * mpc_ParentDialog;
   stw::opensyde_core::C_OscNodeDataPool * const mpc_OscDataPool;
   stw::opensyde_gui_logic::C_PuiSdNodeDataPool * const mpc_UiDataPool;
   stw::opensyde_core::C_OscCanProtocol::E_Type * const mpe_ComProtocolType;
   const uint32_t mu32_NodeIndex;
   const int32_t ms32_DataPoolIndex;
   bool mq_DatapoolAutoNvmStartAddress;

   void m_OkClicked(void);
   void m_CancelClicked(void);
   void m_ApplyType(const bool oq_SharedDatapool);
   void m_LoadCodeGenerationAndApplication(void) const;
   void m_SpinBoxSizeChanged(const int32_t os32_Value) const;
   void m_UpdateSizePrediction(void) const;
   void m_CheckDatapoolName(void) const;
   bool m_CheckDatapoolNameNotDuplicate(std::vector<stw::scl::C_SclString> * const opc_ExistingDatapoolNames) const;
   void m_InitSpinBox(void) const;
   void m_InitComboBoxProtocols(const bool oq_NewDatapool, const stw::opensyde_core::C_OscCanProtocol::E_Type oe_ComProtocolType =
                                   stw::opensyde_core::C_OscCanProtocol::eLAYER2) const;
   stw::opensyde_core::C_OscCanProtocol::E_Type m_GetSelectedProtocol(void) const;
   void m_OnComTypeChange(void) const;
   void m_OnSafetyChange(const bool oq_IsSafety) const;
   void m_HandleDataPoolSafetyAdaptation(void);
   void m_BreakSharedRelation(void);
   bool m_IsRelatedAppValid(const int32_t os32_RelatedDataBlockIndex) const;
   int32_t m_GetCurrentDataBlockIndex(void) const;
   void m_SetCrcVersion();
   void m_GetCrcVersion() const;
   bool mq_IsDatapoolShared;

   static const int32_t mhs32_VERSION_INDEX_V1;
   static const int32_t mhs32_VERSION_INDEX_V2;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
