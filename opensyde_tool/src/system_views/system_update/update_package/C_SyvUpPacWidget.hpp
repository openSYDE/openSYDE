//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing the update package UI

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPPACWIDGET_HPP
#define C_SYVUPPACWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>
#include <QTimer>

#include "stwtypes.hpp"

#include "C_OscSuSequences.hpp"
#include "C_SyvUpDeviceInfo.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SyvUpPacWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpPacWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvUpPacWidget(QWidget * const opc_Parent = NULL);
   ~C_SyvUpPacWidget() override;

   void InitText(void) const;
   void SetViewIndex(const uint32_t ou32_ViewIndex);

   void SetConnected(void);
   void SetUpdateStarted(void) const;
   void SetUpdateApplicationStarted(const uint32_t ou32_NodeIndex) const;
   void SetUpdateApplicationFinished(const uint32_t ou32_NodeIndex) const;
   void SetUpdateApplicationError(const uint32_t ou32_NodeIndex) const;
   void DiscardApplicationStatus(const uint32_t ou32_NodeIndex) const;
   void SetUpdateFinished(void) const;
   void SetDisconnected(void);
   void UpdateDeviceInformation(const std::vector<uint32_t> & orc_NodeIndexes,
                                const std::vector<stw::opensyde_gui_logic::C_SyvUpDeviceInfo> & orc_DeviceInformation)
   const;
   void DisableUpdatePackage(void);
   void SetNodeProgress(const uint32_t ou32_NodeIndex, const uint8_t ou8_Progress) const;

   int32_t GetUpdatePackage(std::vector<stw::opensyde_core::C_OscSuSequences::C_DoFlash> & orc_ApplicationsToWrite,
                            std::vector<uint32_t> & orc_NodesOrder, std::vector<stw::opensyde_core::C_OscSuSequences::C_DoFlash> * const opc_AllApplications =
                               NULL) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigUpdatePackageState(const int32_t os32_State) const;

private:
   //Avoid call
   C_SyvUpPacWidget(const C_SyvUpPacWidget &);
   C_SyvUpPacWidget & operator =(const C_SyvUpPacWidget &) &;

   void m_ButtonRightClicked(void) const;
   void m_ButtonLeftClicked(void) const;
   void m_ButtonClearAll(void) const;
   void m_ButtonImport(void) const;
   void m_ButtonExport(void) const;
   void m_ButtonCreatePackage(void) const;

   void m_UpdateWidget(void);
   void m_FileCheckTimer(void);
   void m_StartCheck(const bool oq_Active);
   void m_SetUpdatePackageStatusNotLocked(const QStringList & orc_MissingDataBlocks,
                                          const QStringList & orc_MissingParamFiles,
                                          const QStringList & orc_MissingFiles,
                                          const QStringList & orc_FlashwareWarningsApps);

   Ui::C_SyvUpPacWidget * mpc_Ui;
   QTimer mc_Timer;
   QStringList mc_MissingDataBlocks;
   QStringList mc_MissingParamFiles;
   QStringList mc_MissingFiles;
   QStringList mc_FlashwareWarningsApps;
};
}
}

#endif
