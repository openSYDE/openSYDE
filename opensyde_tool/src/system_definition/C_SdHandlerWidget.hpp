//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for system definition user

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

#ifndef C_SDHANDLERWIDGET_HPP
#define C_SDHANDLERWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "stwtypes.hpp"

#include "C_NagUseCaseWidget.hpp"

#include "C_SdTopologyWidget.hpp"
#include "C_SdNdeNodeEditWidget.hpp"
#include "C_SdBueBusEditWidget.hpp"
#include "C_OgePopUpDialog.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SdHandlerWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdHandlerWidget :
   public C_NagUseCaseWidget
{
   Q_OBJECT

public:
   explicit C_SdHandlerWidget(QWidget * const opc_Parent = NULL);
   ~C_SdHandlerWidget() override;

   void SetParentHook(QWidget * const opc_Parent) override;
   void UserInputFunc(const uint32_t ou32_FuncNumber) override;
   void Save(void) override;
   void OpenDetail(const int32_t os32_Index, const int32_t os32_SubIndex, const int32_t os32_SubSubIndex,
                   const int32_t os32_SubSubSubIndex, const int32_t os32_Flag) override;
   bool PrepareToClose(void) override;

   void SetSubMode(const int32_t os32_SubMode, const uint32_t ou32_Index, const uint32_t ou32_Flag) override;
   bool GlobalUserKeyPress(QKeyEvent * const opc_Event) override;
   void CallHelp(void) override;

private:
   //Avoid call
   C_SdHandlerWidget(const C_SdHandlerWidget &);
   C_SdHandlerWidget & operator =(const C_SdHandlerWidget &) &; //lint !e1511 //we want to hide the base

   void m_DataChanged(void);
   void m_NodeChanged(const uint32_t ou32_Index);
   void m_BusChanged(const uint32_t ou32_Index);

   void m_SwitchToBus(const uint32_t ou32_Index, const QString & orc_BusName);
   void m_SwitchToBusProtocol(const uint32_t ou32_Index, const QString & orc_BusName,
                              const stw::opensyde_core::C_OscCanProtocol::E_Type oe_ProtocolType);
   void m_SwitchToBusProtocolMessage(const uint32_t ou32_Index, const QString & orc_BusName,
                                     const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId);
   void m_SwitchToBusProperties(const uint32_t ou32_Index, const QString & orc_BusName);
   void m_SwitchToCoManager(const uint32_t ou32_ManagerNodeIndex, const QString & orc_ManagerNodeName,
                            const uint8_t ou8_InterfaceNumber);
   void m_SwitchToDeviceNodeInCoManager(const uint32_t ou32_ManagerNodeIndex, const QString & orc_ManagerNodeName,
                                        const uint32_t ou32_DeviceNodeIndex);
   void m_SetFlag(const uint32_t ou32_Flag) const;
   void m_ErrorChange(void);
   void m_GenerateCode(void) const;
   void m_Export(void);
   void m_TriggerImport(void);
   void m_ContinueImporting(const opensyde_core::C_OscCanProtocol::E_Type & ore_Protocol);
   void m_RtfExport(void);
   bool m_TspImport(const bool oq_IsNodeNew);
   void m_TspImportForNewNode();
   void m_GenerateHalcDatapools(void) const;
   opensyde_gui_elements::C_OgeWiCustomMessage * m_ShowWarningUnstoredProjectPopupMessage();
   void m_SwitchProtocolTab(const opensyde_core::C_OscCanProtocol::E_Type & ore_Protocol) const;
   void m_Import(void);
   bool m_CheckImportPossible(void);

   Ui::C_SdHandlerWidget * mpc_Ui;
   C_SdTopologyWidget * mpc_Topology;

   C_SdNdeNodeEditWidget * mpc_ActNodeEdit;
   C_SdBueBusEditWidget * mpc_ActBusEdit;

   int32_t ms32_NodeEditTabIndex;
   int32_t ms32_BusEditTabIndex;
   int32_t ms32_SubMode;
   uint32_t mu32_Index;
   bool mq_DataChanged;

   static const uint32_t mhu32_USER_INPUT_FUNC_APPLY;
   static const uint32_t mhu32_USER_INPUT_FUNC_GENERATE_CODE;
   static const uint32_t mhu32_USER_INPUT_FUNC_IMPORT;
   static const uint32_t mhu32_USER_INPUT_FUNC_EXPORT;
   static const uint32_t mhu32_USER_INPUT_FUNC_RTF_EXPORT;
   static const uint32_t mhu32_USER_INPUT_FUNC_TSP_IMPORT;

   const QString mc_TooltipGenerateCodeHeading;
   const QString mc_TooltipGenerateCodeContentSysdef;
   const QString mc_TooltipGenerateCodeContentNode;
   const QPointer<stw::opensyde_gui_elements::C_OgePopUpDialog> m_CreateImportPopupDialog(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
