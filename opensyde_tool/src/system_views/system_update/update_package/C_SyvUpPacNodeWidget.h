//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for one node in the update package

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPPACNODEWIDGET_H
#define C_SYVUPPACNODEWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include <QLabel>
#include <QMimeData>

#include "stwtypes.h"

#include "C_OgeLabToolTipBase.h"
#include "C_SyvUpDeviceInfo.h"
#include "C_SyvUpPacConfig.h"
#include "C_OSCSuSequences.h"
#include "C_SyvUpPacSectionNodeWidget.h"
#include "C_SyvUpPacSectionNodeDatablockWidget.h"
#include "C_SyvUpPacSectionNodeFilesWidget.h"
#include "C_SyvUpPacListNodeItemWidget.h"
#include "C_OgeFraSeparator.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvUpPacNodeWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpPacNodeWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvUpPacNodeWidget(const stw_types::uint32 ou32_ViewIndex, const stw_types::uint32 ou32_PositionNumber,
                                 const stw_types::uint32 ou32_NodeIndex, const QString & orc_NodeName,
                                 QWidget * const opc_Parent = NULL);
   ~C_SyvUpPacNodeWidget(void);

   void SetConnected(void);
   void SetUpdateStarted(void) const;
   void SetUpdateApplicationStarted(void) const;
   void SetUpdateApplicationFinished(void);
   void SetUpdateApplicationError(void) const;
   void SetUpdateFinished(void) const;
   void DiscardApplicationStatus(void) const;
   void SetDisconnected(void);
   void UpdateDeviceInformation(const stw_opensyde_gui_logic::C_SyvUpDeviceInfo & orc_DeviceInformation) const;

   void SetProgress(const stw_types::uint32 ou32_Percentage) const;
   void UpdatePositionNumber(const stw_types::uint32 ou32_PositionNumber);

   void AddNewFile(const QString & orc_File, const bool oq_Paramset, const bool oq_PemFile);
   void AdaptFile(const QString & orc_File, C_SyvUpPacListNodeItemWidget * const opc_App);
   void RevertFile(C_SyvUpPacListNodeItemWidget * const opc_App) const;
   void RemoveFile(C_SyvUpPacListNodeItemWidget * const opc_App) const;
   void RemoveAllFiles(void) const;

   void PrepareExportConfig(stw_opensyde_gui_logic::C_SyvUpPacConfigNode & orc_NodeConfig) const;
   void LoadImportConfig(const stw_opensyde_gui_logic::C_SyvUpPacConfig & orc_Config) const;

   stw_types::sint32 CheckAllFiles(stw_types::uint32 & oru32_CountFiles,
                                   QStringList * const opc_FlashwareWarningsApps = NULL,
                                   QStringList * const opc_MissingApps = NULL,
                                   QStringList * const opc_MissingParamFiles = NULL,
                                   QStringList * const opc_MissingFiles = NULL) const;

   C_SyvUpPacSectionNodeWidget * GetSectionList(const QPoint & orc_Pos) const;
   C_SyvUpPacListNodeItemWidget * GetAndSelectApplication(const QPoint & orc_Pos) const;
   C_SyvUpPacListNodeItemWidget * GetApplication(const QPoint & orc_Pos,
                                                 stw_types::uint32 * const opu32_Number = NULL) const;

   stw_types::uint32 GetNodeIndex(void) const;
   stw_types::uint32 GetViewIndex(void) const;
   QString GetNodeName(void) const;
   stw_types::uint32 GetPositionNumber(void) const;

   bool IsFileBased(void) const;
   bool IsStwFlashloader(void) const;

   stw_types::sint32 GetUpdatePackage(stw_opensyde_core::C_OSCSuSequences::C_DoFlash & orc_ApplicationsToWrite,
                                      stw_opensyde_core::C_OSCSuSequences::C_DoFlash * const opc_AllApplications);

   void CollapseAll(void) const;
   void SetEmptyOptionalSectionsVisible(const bool oq_Visible);
   void UpdateSectionsVisibility(void) const;

protected:
   virtual void resizeEvent(QResizeEvent * const opc_Event) override;
   virtual void dragEnterEvent(QDragEnterEvent * const opc_Event) override;
   virtual void dragMoveEvent(QDragMoveEvent * const opc_Event) override;
   virtual void dragLeaveEvent(QDragLeaveEvent * const opc_Event) override;
   virtual void dropEvent(QDropEvent * const opc_Event) override;

private:
   Ui::C_SyvUpPacNodeWidget * mpc_Ui;

   stw_types::uint32 mu32_ViewIndex;
   stw_types::uint32 mu32_NodeIndex;
   QString mc_NodeName;
   QString mc_DeviceType;

   bool mq_FileBased;
   bool mq_NvmHalcBased;
   bool mq_StwFlashloader;
   bool mq_Connected;
   bool mq_EmptyOptionalSectionsVisible;

   stw_types::uint32 mu32_PositionNumber;
   stw_types::uint32 mu32_FilesUpdated;

   QVector<C_SyvUpPacSectionNodeDatablockWidget *> mc_DatablockWidgets;
   C_SyvUpPacSectionNodeFilesWidget * mpc_FilesWidget;
   stw_opensyde_gui_elements::C_OgeFraSeparator * mpc_FilesWidgetSeparator;

   //Avoid call
   C_SyvUpPacNodeWidget(const C_SyvUpPacNodeWidget &);
   C_SyvUpPacNodeWidget & operator =(const C_SyvUpPacNodeWidget &);

   void m_Init(void);
   void m_AddSeparatorToScrollArea(const bool oq_FilesWidgetSeparator);
   void m_UpdateTitle(void) const;

   stw_types::uint32 m_GetFileCount(void) const;
   C_SyvUpPacSectionNodeWidget * m_GetNextListInUpdateOrder(void) const;
   void m_SetApplicationsUnselected(const C_SyvUpPacSectionNodeWidget * const opc_List) const;
   C_SyvUpPacSectionNodeWidget * m_GetAppParentList(C_SyvUpPacListNodeItemWidget * const opc_App) const;
   bool m_CheckFileAlreadyContained(const QString & orc_File);
   bool m_CheckMime(const QMimeData * const opc_Mime, const QPoint & orc_Pos,
                    QStringList * const opc_FilePathsDatablocks = NULL,
                    QStringList * const opc_FilePathsParamsets = NULL,
                    QStringList * const opc_FilePathsFileBased = NULL,
                    QStringList * const opc_RelevantPemFilePaths = NULL,
                    C_SyvUpPacListNodeItemWidget ** const oppc_App = NULL) const;

   static const stw_types::sint32 mhs32_LAYOUT_THRESHOLD;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
