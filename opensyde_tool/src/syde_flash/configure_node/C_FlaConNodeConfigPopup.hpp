//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Dialog for Node Configuration. For more details see implementation file description.
   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_FLACONNODECONFIGPOPUP_H
#define C_FLACONNODECONFIGPOPUP_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include <QTimer>

#include "C_OgePopUpDialog.hpp"
#include "C_FlaSenDcBasicSequences.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_FlaConNodeConfigPopup;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_FlaConNodeConfigPopup :
   public QWidget
{
   Q_OBJECT

public:
   //TODO PTH: flashloader reset timeout as param
   explicit C_FlaConNodeConfigPopup(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent, const uint8_t ou8_NodeId,
                                    const uint32_t ou32_Bitrate, const uint32_t ou32_FlashloaderResetWaitTime,
                                    const QString & orc_CanDllPath);
   ~C_FlaConNodeConfigPopup(void) noexcept override;

   uint8_t GetNodeId(void) const;
   uint32_t GetBitrate(void) const;
   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigReportProgress(const QString & orc_Information);

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_FlaConNodeConfigPopup * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   stw::opensyde_gui_logic::C_FlaSenDcBasicSequences * mpc_DcSequences;

   stw::opensyde_core::C_OscProtocolSerialNumber mc_SnrToConfigureNode;
   int32_t ms32_SequenceResult;            //global result used in m_MonitorSequence to trigger different stuff
   uint8_t mu8_CurrentNodeId;              //original from properties widget
   uint32_t mu32_CurrentBitrate;           //original from properties widget
   uint32_t mu32_FlashloaderResetWaitTime; //from settings widget
   const QString mc_CanDllPath;
   QTimer mc_Timer;

   void m_InitStaticNames(void) const;
   void m_InitStaticGuiElements(void) const;
   int32_t m_InitDcSequence(void);
   void m_CleanUpDcSequence(void);

   void m_OkClicked(void);
   void m_OnCancel(void);
   void m_MonitorSequence();
   void m_FinishConfig(const int32_t os32_SequenceResult);
   void m_DeviceInfoRead(const std::vector<stw::opensyde_core::C_OscDcDeviceInformation> & orc_DeviceInfoResult,
                         const bool oq_SecurityFeatureUsed);
   void m_EnableUi(const bool oq_Enabled);
   QString m_GetErrorAsString(const int32_t os32_SequenceResult);
   //Avoid call
   C_FlaConNodeConfigPopup(const C_FlaConNodeConfigPopup &);
   C_FlaConNodeConfigPopup & operator =(const C_FlaConNodeConfigPopup &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
