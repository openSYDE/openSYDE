//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       List for showing all nodes of the update package

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPPACLISTWIDGET_H
#define C_SYVUPPACLISTWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QAction>

#include "stwtypes.hpp"

#include "C_OgeHorizontalListWidget.hpp"
#include "C_SyvUpPacListDelegate.hpp"
#include "C_SyvUpPacNodeWidget.hpp"
#include "C_SyvUpPacListNodeItemWidget.hpp"

#include "C_OgeContextMenu.hpp"
#include "C_OscSuSequences.hpp"
#include "C_SyvUpDeviceInfo.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpPacListWidget :
   public stw::opensyde_gui_elements::C_OgeHorizontalListWidget
{
   Q_OBJECT

public:
   C_SyvUpPacListWidget(QWidget * const opc_Parent = NULL);
   ~C_SyvUpPacListWidget() override;

   void SetViewIndex(const uint32_t ou32_ViewIndex);

   void SetConnected(void);
   void SetUpdateStarted(void) const;
   void SetUpdateApplicationStarted(const uint32_t ou32_NodeIndex);
   void SetUpdateApplicationFinished(const uint32_t ou32_NodeIndex) const;
   void SetUpdateApplicationError(const uint32_t ou32_NodeIndex) const;
   void DiscardApplicationStatus(const uint32_t ou32_NodeIndex) const;
   void SetUpdateFinished(void) const;
   void SetDisconnected(void);
   void UpdateDeviceInformation(const std::vector<uint32_t> & orc_NodeIndexes,
                                const std::vector<stw::opensyde_gui_logic::C_SyvUpDeviceInfo> & orc_DeviceInformation)
   const;
   void SetNodeProgress(const uint32_t ou32_NodeIndex, const uint8_t ou8_Progress) const;
   void RemoveAllFiles(void) const;

   void ExportConfig(void);
   void ImportConfig(void);

   void CreateServiceUpdatePackage(const bool oq_SaveAsFile);

   int32_t CheckAllPaths(uint32_t & oru32_CountFiles, QStringList * const opc_FlashwareWarningsApps,
                         QStringList * const opc_MissingDataBlocks, QStringList * const opc_MissingParamFiles,
                         QStringList * const opc_MissingFiles) const;

   int32_t GetUpdatePackage(std::vector<stw::opensyde_core::C_OscSuSequences::C_DoFlash> & orc_ApplicationsToWrite,
                            std::vector<uint32_t> & orc_NodesOrder, std::vector<stw::opensyde_core::C_OscSuSequences::C_DoFlash> * const opc_AllApplications =
                               NULL) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigStartCheck(const bool oq_Active);

protected:
   void resizeEvent(QResizeEvent * const opc_Event) override;
   void paintEvent(QPaintEvent * const opc_Event) override;
   void mouseDoubleClickEvent(QMouseEvent * const opc_Event) override;

   void m_MoveItem(const int32_t os32_SourceIndex, const int32_t os32_TargetIndex) override;
   void m_UpdateNumbers(void) const override;
   void m_DelegateStartPaint(void) override;
   void m_DelegateStopPaint(void) override;

private:
   //Avoid call
   C_SyvUpPacListWidget(const C_SyvUpPacListWidget &);
   C_SyvUpPacListWidget & operator =(const C_SyvUpPacListWidget &) &;

   void m_AddNodeWidget(const uint32_t ou32_NodeIndex, const QString & orc_NodeName);

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
   void m_SkipUpdateOfFile(void);
   void m_ShowInExplorer(void) const;
   void m_OpenPemFileSettings(void);

   void m_AdaptFile(const QString & orc_Path);
   QString m_GetDialogPath(void);

   C_SyvUpPacListDelegate mc_Delegate;
   stw::opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;
   C_SyvUpPacNodeWidget * mpc_SelectedNode;
   C_SyvUpPacSectionNodeWidget * mpc_SelectedSection;
   C_SyvUpPacListNodeItemWidget * mpc_SelectedApp;
   QAction * mpc_AddFileAction;
   QAction * mpc_SelectFileAction;
   QAction * mpc_RevertFileAction;
   QAction * mpc_RemoveFileAction;
   QAction * mpc_ShowFileInfoAction;
   QAction * mpc_RemoveAllSectionFilesAction;
   QAction * mpc_RemoveAllNodeFilesAction;
   QAction * mpc_HideShowOptionalSectionsAction;
   QAction * mpc_SkipUpdateOfFile;
   QAction * mpc_ShowInExplorerAction;
   QAction * mpc_PemFileSettings;

   QString mc_LastPath;

   uint32_t mu32_ViewIndex;
   bool mq_Connected;
   bool mq_EmptyOptionalSectionsVisible;

   static const QString mhc_CONFIG_FILE_TYPE;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
