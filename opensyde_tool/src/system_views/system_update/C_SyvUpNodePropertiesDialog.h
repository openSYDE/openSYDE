//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Dialog for system update node properties view (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPNODEPROPERTIESDIALOG_H
#define C_SYVUPNODEPROPERTIESDIALOG_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "stwtypes.h"
#include "C_SyvUtil.h"
#include "C_OgePopUpDialog.h"
#include "C_SyvUpDeviceInfo.h"
#include "C_PuiSvNodeUpdateParamInfo.h"

namespace Ui
{
class C_SyvUpNodePropertiesDialog;
}
/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpNodePropertiesDialog :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvUpNodePropertiesDialog(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                        const stw_types::uint32 ou32_NodeIndex, const bool oq_UpdateFailed,
                                        const stw_types::uint32 ou32_FailedApplicationIndex);
   ~C_SyvUpNodePropertiesDialog(void);

   void InitStaticNames(void) const;
   void SetStatus(const stw_opensyde_gui_logic::C_SyvUtil::E_NodeUpdateInitialStatus oe_Status,
                  const std::vector<stw_diag_lib::C_XFLECUInformation> * const opc_HexFileInfos,
                  const std::vector<bool> * const opc_HexAppInfoAmbiguous,
                  const std::vector<QString> * const opc_ParamFileInfos,
                  const std::vector<QString> * const opc_FileInfos,
                  const stw_opensyde_core::C_OSCSuSequences::C_XflDeviceInformation * const opc_STWDevice,
                  const stw_opensyde_core::C_OSCSuSequences::C_OsyDeviceInformation * const opc_OSYDevice,
                  const bool oq_UpdateSuccessfull, const bool oq_ValidStatus);
   stw_opensyde_gui_logic::C_SyvUtil::E_NodeUpdateInitialStatus GetStatus(void) const;
   const stw_opensyde_core::C_OSCSuSequences::C_XflDeviceInformation * GetSTWDevice(void) const;
   const stw_opensyde_core::C_OSCSuSequences::C_OsyDeviceInformation * GetOSYDevice(void) const;

   bool GetDiscardedStatus(void) const;

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   //lint -restore

private:
   Ui::C_SyvUpNodePropertiesDialog * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   const stw_types::uint32 mu32_NodeIndex;
   stw_opensyde_gui_logic::C_SyvUtil::E_NodeUpdateInitialStatus me_Status;
   const std::vector<stw_diag_lib::C_XFLECUInformation> * mpc_HexFileInfos;
   const std::vector<bool> * mpc_HexAppInfoAmbiguous;
   const std::vector<QString> * mpc_ParamFileInfos;
   const std::vector<QString> * mpc_FileInfos;
   const stw_opensyde_core::C_OSCSuSequences::C_XflDeviceInformation * mpc_STWDevice;
   const stw_opensyde_core::C_OSCSuSequences::C_OsyDeviceInformation * mpc_OSYDevice;
   const bool mq_UpdateFailed;
   bool mq_UpdateSuccessfull;
   bool mq_Discarded;
   bool mq_ValidStatus;
   const stw_types::uint32 mu32_FailedApplicationIndex;

   void m_OkClicked(void);
   void m_InitStatus(void) const;
   void m_InitDataBlockTable(void) const;
   void m_InitFlashloaderTable(void) const;
   void m_OnDiscard(void);
   void m_ReInitStatus(void) const;
   //Avoid call
   C_SyvUpNodePropertiesDialog(const C_SyvUpNodePropertiesDialog &);
   C_SyvUpNodePropertiesDialog & operator =(const C_SyvUpNodePropertiesDialog &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
