//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for handling the process of configuring all nodes. (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVDCWIDGET_HPP
#define C_SYVDCWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>
#include <QTimer>

#include "C_OgePopUpDialog.hpp"
#include "C_SyvDcSequences.hpp"
#include "C_OscSystemBus.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SyvDcWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SyvDcWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvDcWidget(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent, const uint32_t ou32_ViewIndex);
   ~C_SyvDcWidget(void) override;

   void CleanUp(void);
   void InitText(void);

protected:
   void paintEvent(QPaintEvent * const opc_Event) override;
   void keyPressEvent(QKeyEvent * const opc_Event) override;

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
      C_ServerConfStepResult(const uint32_t ou32_Step, const uint8_t ou8_InterfaceType,
                             const uint8_t ou8_InterfaceNumber, const int32_t os32_Result) :
         u32_ConfStep(ou32_Step),
         u8_InterfaceType(ou8_InterfaceType),
         u8_InterfaceNumber(ou8_InterfaceNumber),
         s32_Result(os32_Result)
      {
      }
      uint32_t u32_ConfStep;
      uint8_t u8_InterfaceType;
      uint8_t u8_InterfaceNumber;
      int32_t s32_Result;
   };

   //Avoid call
   C_SyvDcWidget(const C_SyvDcWidget &);
   C_SyvDcWidget & operator =(const C_SyvDcWidget &) &;

   int32_t m_InitSequence(void);

   void m_StartSearchProper(void);
   void m_CleanUpScan(void) const;
   void m_ScanFinished(void);
   bool m_CheckSameSerialNumber(void);
   void m_EnterScanErrorState(void);
   void m_StartConfigProper(void);
   void m_ShowConfigResult(void);
   void m_BackToScan(void);
   void m_ShowConfigInfoOfDevice(const std::vector<stw::opensyde_gui_logic::C_SyvDcDeviceConfiguation> & orc_Config,
                                 const uint32_t ou32_DeviceMaxCount, const uint32_t ou32_DeviceCounter,
                                 QString & orc_Text);
   void m_ShowConfigInfoOfCanInterface(const stw::opensyde_core::C_OscNodeComInterfaceSettings & orc_IntfSetting,
                                       const stw::opensyde_core::C_OscProtocolDriverOsyNode & orc_ServerIdOnUsedBus,
                                       const stw::opensyde_core::C_OscProtocolDriverOsyNode & orc_ServerIdOnConfiguredBus, const uint32_t ou32_Bitrate, const bool oq_SingleNodeOrFirstSubNode, const bool oq_NodeSquad, const QString & orc_SubNodeName, QString & orc_Text);
   void m_ShowConfigInfoOfEthInterface(const stw::opensyde_core::C_OscNodeComInterfaceSettings & orc_IntfSetting,
                                       const stw::opensyde_core::C_OscProtocolDriverOsyNode & orc_ServerIdOnUsedBus,
                                       const stw::opensyde_core::C_OscProtocolDriverOsyNode & orc_ServerIdOnConfiguredBus, const stw::opensyde_core::C_OscNodeComInterfaceSettings::C_IpAddress & orc_IpAddress, const bool oq_SingleNodeOrFirstSubNode, const bool oq_NodeSquad, const QString & orc_SubNodeName, QString & orc_Text);
   void m_ShowConfigInfoOfInterface(const stw::opensyde_core::C_OscNodeComInterfaceSettings & orc_IntfSetting,
                                    const stw::opensyde_core::C_OscProtocolDriverOsyNode & orc_ServerIdOnUsedBus,
                                    const stw::opensyde_core::C_OscProtocolDriverOsyNode & orc_ServerIdOnConfiguredBus,
                                    const bool oq_SingleNodeOrFirstSubNode, const bool oq_NodeSquad,
                                    const QString & orc_SubNodeName, QString & orc_Text, const bool oq_BusConnected,
                                    const bool oq_Configured);
   void m_ResetFlashloaderAfterConfig(const bool oq_SameBitrate);
   int32_t m_GetRelevantConfigInfo(std::vector<stw::opensyde_core::C_OscProtocolDriverOsyNode> & orc_OpenSydeIds,
                                   std::vector<bool> & orc_OpenSydeSnrExtFormat,
                                   std::vector<stw::opensyde_core::C_OscProtocolDriverOsyNode> & orc_StwIds,
                                   uint32_t & oru32_Bitrate);
   void m_ResetNetwork(const bool oq_ToFlashloader);
   void m_ShowReadInfo(const int32_t os32_ActualResult);

   void m_UpdateProgressOfStwFlashloaderConfig(const uint32_t ou32_Progress) const;
   void m_UpdateProgressOfOpenSydeConfig(const uint32_t ou32_Progress) const;

   void m_UpdateStateOfStwFlashloaderConfig(const uint32_t ou32_Step, const int32_t os32_Result,
                                            const uint8_t ou8_BusIdentifier, const uint8_t ou8_NodeIdentifier);
   void m_UpdateStateOfOpenSydeConfig(const uint32_t ou32_Step, const int32_t os32_Result,
                                      const uint8_t ou8_BusIdentifier, const uint8_t ou8_NodeIdentifier,
                                      const uint8_t ou8_InterfaceType, const uint8_t ou8_InterfaceNumber);
   QString m_GetStateStringOfServerStep(const uint32_t ou32_Step,
                                        const stw::opensyde_core::C_OscProtocolDriverOsyNode & orc_Server,
                                        const stw::opensyde_core::C_OscSystemBus::E_Type oe_InterfaceType,
                                        const uint8_t ou8_InterfaceNumber);

   void m_OkClicked(void);
   void m_CancelClicked(void);

   void m_InitScanScreen(void);
   void m_InitAssignmentScreen(void) const;
   void m_InitBitRateComboBox(void);
   void m_InitModeComboBox(void);

   QString m_GetComboBoxString(const uint32_t ou32_Bitrate) const;
   uint32_t m_GetBitrateFromComboBoxString(const QString & orc_Entry) const;

   void m_OnBitRateChanged(void) const;
   void m_OnDeviceConfigModeChanged(void) const;
   int32_t m_GetBitRateValue(uint32_t & oru32_Value) const;

   void m_AssignmentConnect(const uint32_t ou32_NodeIndex,
                            const stw::opensyde_core::C_OscProtocolSerialNumber & orc_SerialNumber,
                            const std::map<uint8_t,
                                           stw::opensyde_gui_logic::C_SyvDcDeviceOldComConfig> & orc_SubNodeIdsToOldNodeIds)
   const;
   void m_AssignmentDisconnect(const uint32_t ou32_NodeIndex,
                               const stw::opensyde_core::C_OscProtocolSerialNumber & orc_SerialNumber) const;
   void m_AssignmentUpdateProgress(void) const;

   void m_Timer(void);

   void m_ClearReportText(void);
   void m_UpdateReportText(const QString & orc_NewTextPart, const QString & orc_TemporaryText = "");

   void m_HandleConfigurationStarted(void);
   void m_HandleDeviceVerificationStart(void);

   void m_HandleMissingDevices(
      const std::vector<stw::opensyde_gui_logic::C_SyvDcDeviceConfiguation> & orc_AllDeviceInfos,
      const std::vector<stw::opensyde_gui_logic::C_SyvDcDeviceInformation> & orc_FoundDeviceInfos,
      QString & orc_ReportText) const;

   bool m_AreAllInterfacesToConfigure(void) const;
   void m_DoCompleteDisconnect(void);
   bool m_CheckQuitPossible(void) const;
   void m_OnCloseIgnored(void);
   void m_InformUserAboutAbortedClose(void) const;
   void m_InformUserAfterScan(const QString & orc_Text);

   Ui::C_SyvDcWidget * mpc_Ui;

   stw::opensyde_gui_elements::C_OgePopUpDialog * const mpc_ParentDialog;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_gui_logic::C_SyvDcSequences * mpc_DcSequences;
   const uint32_t mu32_ViewIndex;
   bool mq_InitializationFinished;
   uint32_t mu32_TempBitrate; // In kbit/s
   bool mq_SameBitrates;
   uint32_t mu32_BusIndex;
   uint8_t mu8_BusId;
   uint64_t mu64_BitRate;
   bool mq_DisconnectNecessary;
   stw::opensyde_core::C_OscSystemBus::E_Type me_BusType;
   std::vector<stw::opensyde_gui_logic::C_SyvDcDeviceInformation> mc_FoundDevices;
   bool mq_SecurityFeatureUsed;

   QTimer mc_Timer;
   E_Step me_Step;
   QString mc_ReportText;
   QMap<stw::opensyde_core::C_OscProtocolDriverOsyNode, std::vector<C_ServerConfStepResult> > mc_ServerStates;

   std::vector<stw::opensyde_gui_logic::C_SyvDcDeviceConfiguation> mc_StwFlashloaderDeviceConfigurations;
   std::vector<stw::opensyde_gui_logic::C_SyvDcDeviceConfiguation> mc_OpenSydeDeviceConfigurations;

   static const int32_t mhs32_INDEX_CONFIGURATION_ALL_CONNECTED_INTERFACES;
   static const int32_t mhs32_INDEX_CONFIGURATION_ONLY_USED_INTERFACES;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
