//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       List for showing all nodes of the update package

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPUPDATEPACKAGELISTWIDGET_H
#define C_SYVUPUPDATEPACKAGELISTWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QAction>

#include "stwtypes.h"

#include "C_OgeHorizontalListWidget.h"
#include "C_SyvUpUpdatePackageListDelegate.h"
#include "C_SyvUpUpdatePackageNodeWidget.h"
#include "C_SyvUpUpdatePackageListNodeItemWidget.h"

#include "C_OgeContextMenu.h"
#include "C_OSCSuSequences.h"
#include "C_SyvUpDeviceInfo.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpUpdatePackageListWidget :
   public stw_opensyde_gui_elements::C_OgeHorizontalListWidget
{
   Q_OBJECT

public:
   C_SyvUpUpdatePackageListWidget(QWidget * const opc_Parent = NULL);
   virtual ~C_SyvUpUpdatePackageListWidget();

   void SetViewIndex(const stw_types::uint32 ou32_ViewIndex);

   void SetConnected(void);
   void SetUpdateStarted(void) const;
   void SetUpdateApplicationStarted(const stw_types::uint32 ou32_NodeIndex);
   void SetUpdateApplicationFinished(const stw_types::uint32 ou32_NodeIndex) const;
   void SetUpdateApplicationError(const stw_types::uint32 ou32_NodeIndex) const;
   void DiscardApplicationStatus(const stw_types::uint32 ou32_NodeIndex) const;
   void SetUpdateFinished(void) const;
   void SetDisconnected(void);
   void UpdateDeviceInformation(const std::vector<stw_types::uint32> & orc_NodeIndexes,
                                const std::vector<stw_opensyde_gui_logic::C_SyvUpDeviceInfo> & orc_DeviceInformation)
   const;
   void SetNodeProgress(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint8 ou8_Progress) const;
   void RemoveAllFiles(void) const;

   void ExportConfig(void);
   void ImportConfig(void);

   void CreateServiceUpdatePackage(void);

   stw_types::sint32 CheckAllPaths(stw_types::uint32 & oru32_CountFiles, QStringList * const opc_FlashwareWarningsApps,
                                   QStringList * const opc_MissingDataBlocks, QStringList * const opc_MissingParamFiles,
                                   QStringList * const opc_MissingFiles) const;

   stw_types::sint32 GetUpdatePackage(
      std::vector<stw_opensyde_core::C_OSCSuSequences::C_DoFlash> & orc_ApplicationsToWrite,
      std::vector<stw_types::uint32> & orc_NodesOrder,
      std::vector<stw_opensyde_core::C_OSCSuSequences::C_DoFlash> * const opc_AllApplications = NULL) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigStartCheck(const bool oq_Active);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void resizeEvent(QResizeEvent * const opc_Event) override;
   virtual void paintEvent(QPaintEvent * const opc_Event) override;
   virtual void mouseDoubleClickEvent(QMouseEvent * const opc_Event) override;
   //lint -restore

   virtual void m_MoveItem(const stw_types::sintn osn_SourceIndex, const stw_types::sintn osn_TargetIndex) override;
   virtual void m_UpdateNumbers(void) const override;
   virtual void m_DelegateStartPaint(void) override;
   virtual void m_DelegateStopPaint(void) override;

private:
   //Avoid call
   C_SyvUpUpdatePackageListWidget(const C_SyvUpUpdatePackageListWidget &);
   C_SyvUpUpdatePackageListWidget & operator =(const C_SyvUpUpdatePackageListWidget &);

   void m_AddNodeWidget(const stw_types::uint32 ou32_NodeIndex, const QString & orc_NodeName);

   void m_OnCustomContextMenuRequested(const QPoint & orc_Pos);
   void m_OnCustomContextMenuHide(void);
   void m_SetupContextMenu(void);
   void m_AddFileAction(void);
   void m_AddNewFile(const QString & orc_DialogCaption, const QString & orc_DialogFilter);
   void m_SelectFile(void);
   void m_RevertFile(void);
   void m_RemoveFile(void);
   void m_ViewFileInfo(void);
   void m_RemoveAllSectionFiles(void);
   void m_RemoveAllNodeFiles(void);
   void m_HideShowOptionalSections(void);
   void m_ShowInExplorer(void) const;

   void m_AdaptFile(const QString & orc_Path);
   QString m_GetDialogPath(void);

   C_SyvUpUpdatePackageListDelegate mc_Delegate;
   stw_opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;
   C_SyvUpUpdatePackageNodeWidget * mpc_SelectedNode;
   C_SyvUpUpdatePackageListNodeWidget * mpc_SelectedSection;
   C_SyvUpUpdatePackageListNodeItemWidget * mpc_SelectedApp;
   QAction * mpc_AddFileAction;
   QAction * mpc_SelectFileAction;
   QAction * mpc_RevertFileAction;
   QAction * mpc_RemoveFileAction;
   QAction * mpc_ShowFileInfoAction;
   QAction * mpc_RemoveAllSectionFilesAction;
   QAction * mpc_RemoveAllNodeFilesAction;
   QAction * mpc_HideShowOptionalSectionsAction;
   QAction * mpc_ShowInExplorerAction;

   QString mc_LastPath;

   stw_types::uint32 mu32_ViewIndex;
   bool mq_Connected;
   bool mq_EmptyOptionalSectionsVisible;

   static const QString mhc_CONFIG_FILE_TYPE;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
