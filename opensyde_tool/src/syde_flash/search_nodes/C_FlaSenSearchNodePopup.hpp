//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Dialog for scanning available nodes. For more details see implementation file description.
   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_FLASENSEARCHNODEPOPUP_H
#define C_FLASENSEARCHNODEPOPUP_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include <QTimer>

#include "C_OgePopUpDialog.hpp"
#include "C_FlaSenDcBasicSequences.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_FlaSenSearchNodePopup;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_FlaSenSearchNodePopup :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_FlaSenSearchNodePopup(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent);
   ~C_FlaSenSearchNodePopup(void) override;

   void InitStaticNames(void) const;
   int32_t StartSearch(const QString & orc_CanDllPath, const int32_t os32_CanBitrate,
                       const uint32_t ou32_FlashloaderResetWaitTime);
   uint8_t GetSelectedNodeId(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigReportProgress(const QString & orc_Information);

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_FlaSenSearchNodePopup * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   stw::opensyde_gui_logic::C_FlaSenDcBasicSequences * mpc_DcSequences;
   QTimer mc_Timer;

   int32_t m_InitDcSequence(const QString & orc_CanDllPath, const int32_t os32_CanBitrate);
   void m_CleanupDcSequence(void);
   void m_ApplyClicked(void);
   void m_OnCancel(void);
   void m_ScanNodes(const uint32_t ou32_FlashloaderResetWaitTime);
   void m_DeviceInfoReceived(const std::vector<stw::opensyde_core::C_OscDcDeviceInformation> & orc_DeviceInfoResult,
                             const bool oq_SecurityFeatureUsed);
   void m_Timer(void);
   void m_ShowErrorNoDevices(void);

   //Avoid call
   C_FlaSenSearchNodePopup(const C_FlaSenSearchNodePopup &);
   C_FlaSenSearchNodePopup & operator =(const C_FlaSenSearchNodePopup &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
