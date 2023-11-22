//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for one node in the update package

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPPACNODEWIDGET_HPP
#define C_SYVUPPACNODEWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include <QLabel>
#include <QMimeData>

#include "stwtypes.hpp"

#include "C_OgeLabToolTipBase.hpp"
#include "C_SyvUpDeviceInfo.hpp"
#include "C_SyvUpPacConfig.hpp"
#include "C_OscSuSequences.hpp"
#include "C_SyvUpPacSectionNodeWidget.hpp"
#include "C_SyvUpPacSectionNodeDatablockWidget.hpp"
#include "C_SyvUpPacSectionNodeFilesWidget.hpp"
#include "C_SyvUpPacListNodeItemWidget.hpp"
#include "C_OgeFraSeparator.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SyvUpPacNodeWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpPacNodeWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvUpPacNodeWidget(const uint32_t ou32_ViewIndex, const uint32_t ou32_PositionNumber,
                                 const uint32_t ou32_NodeIndex, const QString & orc_NodeName,
                                 QWidget * const opc_Parent = NULL);
   ~C_SyvUpPacNodeWidget(void) override;

   void SetConnected(void);
   void SetUpdateStarted(void) const;
   void SetUpdateApplicationStarted(void) const;
   void SetUpdateApplicationFinished(void);
   void SetUpdateApplicationError(void) const;
   void SetUpdateFinished(void) const;
   void DiscardApplicationStatus(void) const;
   void SetDisconnected(void);
   void UpdateDeviceInformation(const stw::opensyde_gui_logic::C_SyvUpDeviceInfo & orc_DeviceInformation) const;

   void SetProgress(const uint32_t ou32_Percentage) const;
   void UpdatePositionNumber(const uint32_t ou32_PositionNumber);

   void AddNewFile(const QString & orc_File, const bool oq_Paramset, const bool oq_PemFile);
   void AdaptFile(const QString & orc_File, C_SyvUpPacListNodeItemWidget * const opc_App);
   void RevertFile(C_SyvUpPacListNodeItemWidget * const opc_App) const;
   void RemoveFile(C_SyvUpPacListNodeItemWidget * const opc_App) const;
   void RemoveAllFiles(void) const;

   void PrepareExportConfig(stw::opensyde_gui_logic::C_SyvUpPacConfigNode & orc_NodeConfig) const;
   void LoadImportConfig(const stw::opensyde_gui_logic::C_SyvUpPacConfig & orc_Config) const;

   int32_t CheckAllFiles(uint32_t & oru32_CountFiles, QStringList * const opc_FlashwareWarningsApps = NULL,
                         QStringList * const opc_MissingApps = NULL, QStringList * const opc_MissingParamFiles = NULL,
                         QStringList * const opc_MissingFiles = NULL) const;

   C_SyvUpPacSectionNodeWidget * GetSectionList(const QPoint & orc_Pos) const;
   C_SyvUpPacListNodeItemWidget * GetAndSelectApplication(const QPoint & orc_Pos) const;
   C_SyvUpPacListNodeItemWidget * GetApplication(const QPoint & orc_Pos, uint32_t * const opu32_Number = NULL) const;

   uint32_t GetNodeIndex(void) const;
   uint32_t GetViewIndex(void) const;
   QString GetNodeName(void) const;
   uint32_t GetPositionNumber(void) const;

   bool IsFileBased(void) const;
   bool IsStwFlashloader(void) const;

   int32_t GetUpdatePackage(stw::opensyde_core::C_OscSuSequences::C_DoFlash & orc_ApplicationsToWrite,
                            stw::opensyde_core::C_OscSuSequences::C_DoFlash * const opc_AllApplications);

   void CollapseAll(void) const;
   void SetEmptyOptionalSectionsVisible(const bool oq_Visible);
   void UpdateSectionsVisibility(void) const;

protected:
   void resizeEvent(QResizeEvent * const opc_Event) override;
   void dragEnterEvent(QDragEnterEvent * const opc_Event) override;
   void dragMoveEvent(QDragMoveEvent * const opc_Event) override;
   void dragLeaveEvent(QDragLeaveEvent * const opc_Event) override;
   void dropEvent(QDropEvent * const opc_Event) override;

private:
   Ui::C_SyvUpPacNodeWidget * mpc_Ui;

   uint32_t mu32_ViewIndex;
   uint32_t mu32_NodeIndex;
   QString mc_NodeName;
   QString mc_DeviceType;

   bool mq_FileBased;
   bool mq_NvmHalcBased;
   bool mq_StwFlashloader;
   bool mq_Connected;
   bool mq_EmptyOptionalSectionsVisible;

   uint32_t mu32_PositionNumber;
   uint32_t mu32_FilesUpdated;

   QVector<C_SyvUpPacSectionNodeDatablockWidget *> mc_DatablockWidgets;
   C_SyvUpPacSectionNodeFilesWidget * mpc_FilesWidget;
   stw::opensyde_gui_elements::C_OgeFraSeparator * mpc_FilesWidgetSeparator;

   //Avoid call
   C_SyvUpPacNodeWidget(const C_SyvUpPacNodeWidget &);
   C_SyvUpPacNodeWidget & operator =(const C_SyvUpPacNodeWidget &) &;

   void m_Init(void);
   void m_AddSeparatorToScrollArea(const bool oq_FilesWidgetSeparator);
   void m_UpdateTitle(void) const;

   uint32_t m_GetFileCount(void) const;
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

   static const int32_t mhs32_LAYOUT_THRESHOLD;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
