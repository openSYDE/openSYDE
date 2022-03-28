//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for handling the process of configuring all nodes. (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDCWIDGET_H
#define C_SYVDCWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>
#include <QTimer>

#include "C_OgePopUpDialog.h"
#include "C_SyvDcSequences.h"
#include "C_OSCSystemBus.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SyvDcWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SyvDcWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDcWidget(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                          const stw_types::uint32 ou32_ViewIndex);
   ~C_SyvDcWidget(void);

   void CleanUp(void);

   void InitText(void);

protected:
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
   virtual void keyPressEvent(QKeyEvent * const opc_Event) override;

private:
   enum E_Step
   {
      eSCANCANENTERFLASHLOADER,
      eSCANCANGETINFOFROMOPENSYDEDEVICES,
      eSCANCANGETINFOFROMSTWFLASHLOADERDEVICES,
      eSCANETHGETINFOFROMOPENSYDEDEVICES,
      eCONFCANSTWFLASHLOADERDEVICES,
      eCONFCANOPENSYDEDEVICES,
      eCONFETHOPENSYDEDEVICES,
      eREADBACKCAN,
      eREADBACKETH
   };

   class C_ServerConfStepResult
   {
   public:
      C_ServerConfStepResult(const stw_types::uint32 ou32_Step, const stw_types::uint8 ou8_InterfaceType,
                             const stw_types::uint8 ou8_InterfaceNumber, const stw_types::sint32 os32_Result) :
         u32_ConfStep(ou32_Step),
         u8_InterfaceType(ou8_InterfaceType),
         u8_InterfaceNumber(ou8_InterfaceNumber),
         s32_Result(os32_Result)
      {
      }
      stw_types::uint32 u32_ConfStep;
      stw_types::uint8 u8_InterfaceType;
      stw_types::uint8 u8_InterfaceNumber;
      stw_types::sint32 s32_Result;
   };

   //Avoid call
   C_SyvDcWidget(const C_SyvDcWidget &);
   C_SyvDcWidget & operator =(const C_SyvDcWidget &);

   stw_types::sint32 m_InitSequence(void);

   void m_StartSearchProper(void);
   void m_CleanUpScan(void) const;
   void m_ScanFinished(void);
   bool m_CheckSameSerialNumber(void);
   void m_EnterScanErrorState(void);
   void m_StartConfigProper(void);
   void m_ShowConfigResult(void);
   void m_BackToScan(void);
   void m_ShowConfigInfoOfDevice(const std::vector<stw_opensyde_gui_logic::C_SyvDcDeviceConfiguation> & orc_Config,
                                 const stw_types::uint32 ou32_DeviceMaxCount,
                                 const stw_types::uint32 ou32_DeviceCounter, QString & orc_Text);
   void m_ShowConfigInfoOfCanInterface(const stw_opensyde_core::C_OSCNodeComInterfaceSettings & orc_IntfSetting,
                                       const stw_opensyde_core::C_OSCProtocolDriverOsyNode & orc_ServerIdOnUsedBus,
                                       const stw_opensyde_core::C_OSCProtocolDriverOsyNode & orc_ServerIdOnConfiguredBus, const stw_types::uint32 ou32_Bitrate, const bool oq_SingleNodeOrFirstSubNode, const bool oq_NodeSquad, const QString & orc_SubNodeName, QString & orc_Text);
   void m_ShowConfigInfoOfEthInterface(const stw_opensyde_core::C_OSCNodeComInterfaceSettings & orc_IntfSetting,
                                       const stw_opensyde_core::C_OSCProtocolDriverOsyNode & orc_ServerIdOnUsedBus,
                                       const stw_opensyde_core::C_OSCProtocolDriverOsyNode & orc_ServerIdOnConfiguredBus, const stw_opensyde_core::C_OSCNodeComInterfaceSettings::C_IpAddress & orc_IpAddress, const bool oq_SingleNodeOrFirstSubNode, const bool oq_NodeSquad, const QString & orc_SubNodeName, QString & orc_Text);
   void m_ShowConfigInfoOfInterface(const stw_opensyde_core::C_OSCNodeComInterfaceSettings & orc_IntfSetting,
                                    const stw_opensyde_core::C_OSCProtocolDriverOsyNode & orc_ServerIdOnUsedBus,
                                    const stw_opensyde_core::C_OSCProtocolDriverOsyNode & orc_ServerIdOnConfiguredBus,
                                    const bool oq_SingleNodeOrFirstSubNode, const bool oq_NodeSquad,
                                    const QString & orc_SubNodeName, QString & orc_Text, const bool oq_BusConnected,
                                    const bool oq_Configured);
   void m_ResetFlashloaderAfterConfig(const bool oq_SameBitrate);
   stw_types::sint32 m_GetRelevantConfigInfo(
      std::vector<stw_opensyde_core::C_OSCProtocolDriverOsyNode> & orc_OpenSydeIds,
      std::vector<bool> & orc_OpenSydeSnrExtFormat,
      std::vector<stw_opensyde_core::C_OSCProtocolDriverOsyNode> & orc_StwIds, stw_types::uint32 & oru32_Bitrate);
   void m_ResetNetwork(const bool oq_ToFlashloader);
   void m_ShowReadInfo(const stw_types::sint32 os32_ActualResult);

   void m_UpdateProgressOfStwFlashloaderConfig(const stw_types::uint32 ou32_Progress) const;
   void m_UpdateProgressOfOpenSydeConfig(const stw_types::uint32 ou32_Progress) const;

   void m_UpdateStateOfStwFlashloaderConfig(const stw_types::uint32 ou32_Step, const stw_types::sint32 os32_Result,
                                            const stw_types::uint8 ou8_BusIdentifier,
                                            const stw_types::uint8 ou8_NodeIdentifier);
   void m_UpdateStateOfOpenSydeConfig(const stw_types::uint32 ou32_Step, const stw_types::sint32 os32_Result,
                                      const stw_types::uint8 ou8_BusIdentifier,
                                      const stw_types::uint8 ou8_NodeIdentifier,
                                      const stw_types::uint8 ou8_InterfaceType,
                                      const stw_types::uint8 ou8_InterfaceNumber);
   QString m_GetStateStringOfServerStep(const stw_types::uint32 ou32_Step,
                                        const stw_opensyde_core::C_OSCProtocolDriverOsyNode & orc_Server,
                                        const stw_opensyde_core::C_OSCSystemBus::E_Type oe_InterfaceType,
                                        const stw_types::uint8 ou8_InterfaceNumber);

   void m_OkClicked(void);
   void m_CancelClicked(void);

   void m_InitScanScreen(void);
   void m_InitAssignmentScreen(void) const;
   void m_InitBitRateComboBox(void);
   void m_InitModeComboBox(void);

   QString m_GetComboBoxString(const stw_types::uint32 ou32_Bitrate) const;
   stw_types::uint32 m_GetBitrateFromComboBoxString(const QString & orc_Entry) const;

   void m_OnBitRateChanged(void) const;
   void m_OnDeviceConfigModeChanged(void) const;
   stw_types::sint32 m_GetBitRateValue(stw_types::uint32 & oru32_Value) const;

   void m_AssignmentConnect(const stw_types::uint32 ou32_NodeIndex,
                            const stw_opensyde_core::C_OSCProtocolSerialNumber & orc_SerialNumber,
                            const std::map<stw_types::uint8,
                                           stw_opensyde_gui_logic::C_SyvDcDeviceOldComConfig> & orc_SubNodeIdsToOldNodeIds)
   const;
   void m_AssignmentDisconnect(const stw_types::uint32 ou32_NodeIndex,
                               const stw_opensyde_core::C_OSCProtocolSerialNumber & orc_SerialNumber) const;
   void m_AssignmentUpdateProgress(void) const;

   void m_Timer(void);

   void m_ClearReportText(void);
   void m_UpdateReportText(const QString & orc_NewTextPart, const QString & orc_TemporaryText = "");

   void m_HandleConfigurationStarted(void);
   void m_HandleDeviceVerificationStart(void);

   void m_HandleMissingDevices(
      const std::vector<stw_opensyde_gui_logic::C_SyvDcDeviceConfiguation> & orc_AllDeviceInfos,
      const std::vector<stw_opensyde_gui_logic::C_SyvDcDeviceInformation> & orc_FoundDeviceInfos,
      QString & orc_ReportText) const;

   bool m_AreAllInterfacesToConfigure(void) const;
   void m_DoCompleteDisconnect(void);
   bool m_CheckQuitPossible(void) const;
   void m_OnCloseIgnored(void);
   void m_InformUserAboutAbortedClose(void) const;
   void m_InformUserAfterScan(const QString & orc_Text);

   Ui::C_SyvDcWidget * mpc_Ui;

   stw_opensyde_gui_elements::C_OgePopUpDialog * const mpc_ParentDialog;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_logic::C_SyvDcSequences * mpc_DcSequences;
   const stw_types::uint32 mu32_ViewIndex;
   bool mq_InitializationFinished;
   stw_types::uint32 mu32_TempBitrate; // In kbit/s
   bool mq_SameBitrates;
   stw_types::uint32 mu32_BusIndex;
   stw_types::uint8 mu8_BusId;
   stw_types::uint64 mu64_BitRate;
   bool mq_DisconnectNecessary;
   stw_opensyde_core::C_OSCSystemBus::E_Type me_BusType;
   std::vector<stw_opensyde_gui_logic::C_SyvDcDeviceInformation> mc_FoundDevices;
   bool mq_SecurityFeatureUsed;

   QTimer mc_Timer;
   E_Step me_Step;
   QString mc_ReportText;
   QMap<stw_opensyde_core::C_OSCProtocolDriverOsyNode, std::vector<C_ServerConfStepResult> > mc_ServerStates;

   std::vector<stw_opensyde_gui_logic::C_SyvDcDeviceConfiguation> mc_StwFlashloaderDeviceConfigurations;
   std::vector<stw_opensyde_gui_logic::C_SyvDcDeviceConfiguation> mc_OpenSydeDeviceConfigurations;

   static const stw_types::sint32 mhs32_INDEX_CONFIGURATION_ALL_CONNECTED_INTERFACES;
   static const stw_types::sint32 mhs32_INDEX_CONFIGURATION_ONLY_USED_INTERFACES;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
