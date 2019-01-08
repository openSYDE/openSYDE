//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing the update package UI

   \implementation
   project     project name
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     09.02.2018  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SYVUPUPDATEPACKAGEWIDGET_H
#define C_SYVUPUPDATEPACKAGEWIDGET_H

/* -- Includes ------------------------------------------------------------- */

#include <QWidget>
#include <QTimer>

#include "stwtypes.h"

#include "C_OSCSuSequences.h"
#include "C_SyvUpDeviceInfo.h"

/* -- Namespace ------------------------------------------------------------ */

namespace Ui
{
class C_SyvUpUpdatePackageWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SyvUpUpdatePackageWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvUpUpdatePackageWidget(QWidget * const opc_Parent = NULL);
   virtual ~C_SyvUpUpdatePackageWidget();

   void InitText(void) const;
   void SetViewIndex(const stw_types::uint32 ou32_ViewIndex);

   void SetConnected(void);
   void SetUpdateStarted(void) const;
   void SetUpdateApplicationStarted(const stw_types::uint32 ou32_NodeIndex) const;
   void SetUpdateApplicationFinished(const stw_types::uint32 ou32_NodeIndex) const;
   void SetUpdateApplicationError(const stw_types::uint32 ou32_NodeIndex) const;
   void DiscardApplicationStatus(const stw_types::uint32 ou32_NodeIndex) const;
   void SetUpdateFinished(void) const;
   void SetDisconnected(void);
   void UpdateDeviceInformation(const std::vector<stw_types::uint32> & orc_NodeIndexes,
                                const std::vector<stw_opensyde_gui_logic::C_SyvUpDeviceInfo> & orc_DeviceInformation)
   const;
   void SetNodeProgress(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint8 ou8_Progress) const;

   stw_types::sint32 GetUpdatePackage(
      std::vector<stw_opensyde_core::C_OSCSuSequences::C_DoFlash> & orc_ApplicationsToWrite,
      std::vector<stw_types::uint32> & orc_NodesOrder) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigUpdatePackageState(const stw_types::sint32 os32_State) const;

private:
   //Avoid call
   C_SyvUpUpdatePackageWidget(const C_SyvUpUpdatePackageWidget &);
   C_SyvUpUpdatePackageWidget & operator =(const C_SyvUpUpdatePackageWidget &);

   void m_ButtonRightClicked(void) const;
   void m_ButtonLeftClicked(void) const;
   void m_ButtonClearAll(void) const;
   void m_ButtonImport(void) const;
   void m_ButtonExport(void) const;
   void m_ButtonCreatePackage(void) const;

   void m_UpdateWidget(void);
   void m_FileCheckTimer(void);
   void m_StartCheck(const bool oq_Active);
   void m_SetUpdatePackageStatusOk(void) const;
   void m_SetUpdatePackageStatusLocked(void) const;
   void m_SetUpdatePackageStatusWarning(const std::vector<QString> & orc_InvalidNodeNames) const;

   Ui::C_SyvUpUpdatePackageWidget * mpc_Ui;
   QTimer mc_Timer;

   static const stw_types::uint32 mhu32_FILE_CHECK_INTERVAL = 500U;
};
}

#endif
