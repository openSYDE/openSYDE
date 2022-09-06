//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Pop up dialog widget for adding a device in can open manager.
   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDECOADDDEVICEDIALOG_H
#define C_SDNDECOADDDEVICEDIALOG_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include <QFileInfo>
#include "C_OgePopUpDialog.h"
#include "stwtypes.h"
#include "TGLUtils.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeCoAddDeviceDialog;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeCoAddDeviceDialog :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeCoAddDeviceDialog(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                     const stw_types::uint32 ou32_NodeIndex, const stw_types::uint8 ou8_InterfaceId);
   ~C_SdNdeCoAddDeviceDialog(void);

   void InitStaticNames(void) const;
   stw_types::sint32 GetNodeSelection(stw_types::uint32 & oru32_NodeIndex, stw_types::uint32 & oru32_InterfaceIndex,
                                      QString & orc_EdsPath) const;
   stw_scl::C_SCLString GetEDSFile(void);

protected:
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_SdNdeCoAddDeviceDialog * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;

   stw_types::uint32 mu32_BusIndex;
   stw_types::uint32 mu32_NodeIndex;
   stw_types::uint8 mu8_InterfaceId;
   std::vector<stw_types::uint32> mc_NodeIndexes;
   std::vector<stw_types::uint32> mc_InterfaceIndexes;

   static const QString mhc_SUFFIX;

   void m_OkClicked(void);
   void m_CancelClicked(void);
   void m_EDSPathButtonClicked(void);
   void m_SetEDSPath(const QString & orc_New);
   void m_LoadPicture(const QFileInfo oc_FileInfo);

   void m_FillUpComboBox(const stw_types::uint32 ou32_BusIndex, const QString oc_NodeName);
   bool m_CheckIfNodeHasCANopenManager(const stw_types::uint32 ou32_NodeIndex);
   void m_OnLoadEDS(void);

   //Avoid call
   C_SdNdeCoAddDeviceDialog(const C_SdNdeCoAddDeviceDialog &);
   C_SdNdeCoAddDeviceDialog & operator =(const C_SdNdeCoAddDeviceDialog &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
