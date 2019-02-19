//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing a specific node with its applications and data blocks as list item

   \implementation
   project     project name
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     09.02.2018  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SYVUPUPDATEPACKAGELISTNODEWIDGET_H
#define C_SYVUPUPDATEPACKAGELISTNODEWIDGET_H

/* -- Includes ------------------------------------------------------------- */
#include <QWidget>
#include <QLabel>
#include <QMimeData>

#include "stwtypes.h"

#include "C_SyvUpUpdatePackageListNodeAppWidget.h"
#include "C_SyvUpUpdatePackageConfig.h"

#include "C_OSCSuSequences.h"
#include "C_SyvUpDeviceInfo.h"

/* -- Namespace ------------------------------------------------------------ */

namespace Ui
{
class C_SyvUpUpdatePackageListNodeWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SyvUpUpdatePackageListNodeWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvUpUpdatePackageListNodeWidget(const stw_types::uint32 ou32_ViewIndex,
                                               const stw_types::uint32 ou32_PositionNumber,
                                               const stw_types::uint32 ou32_NodeIndex, const QString & orc_NodeName,
                                               QWidget * const opc_Parent = NULL);
   virtual ~C_SyvUpUpdatePackageListNodeWidget();

   void InitStaticNames(void) const;

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
   void AddFile(const QString & orc_File);
   void AdaptFile(const QString & orc_File, C_SyvUpUpdatePackageListNodeAppWidget * const opc_App) const;
   void RevertFile(C_SyvUpUpdatePackageListNodeAppWidget * const opc_App) const;
   void RemoveFile(C_SyvUpUpdatePackageListNodeAppWidget * const opc_App);
   void RemoveAllFiles(void);

   void PrepareExportConfig(stw_opensyde_gui_logic::C_SyvUpUpdatePackageConfigNode & orc_NodeConfig) const;
   void LoadImportConfig(const stw_opensyde_gui_logic::C_SyvUpUpdatePackageConfig & orc_Config);

   stw_types::sint32 CheckAllFiles(stw_types::uint32 & oru32_CountMissingFiles, stw_types::uint32 & oru32_CountFiles);

   C_SyvUpUpdatePackageListNodeAppWidget * GetAndSelectApplication(const QPoint & orc_Pos) const;
   C_SyvUpUpdatePackageListNodeAppWidget * GetApplication(const QPoint & orc_Pos,
                                                          stw_types::uint32 * const opu32_Number = NULL) const;

   stw_types::uint32 GetViewIndex(void) const;
   QString GetNodeName(void) const;
   stw_types::uint32 GetPositionNumber(void) const;
   stw_types::uint32 GetNodeIndex(void) const;
   bool IsFileBased(void) const;

   stw_types::sint32 GetUpdatePackage(stw_opensyde_core::C_OSCSuSequences::C_DoFlash & orc_ApplicationsToWrite,
                                      stw_opensyde_core::C_OSCSuSequences::C_DoFlash * const opc_AllApplications);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigButtonAddFileClicked(C_SyvUpUpdatePackageListNodeWidget * const opc_Sender);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void resizeEvent(QResizeEvent * const opc_Event) override;
   virtual void dragEnterEvent(QDragEnterEvent * const opc_Event) override;
   virtual void dragMoveEvent(QDragMoveEvent * const opc_Event) override;
   virtual void dragLeaveEvent(QDragLeaveEvent * const opc_Event) override;
   virtual void dropEvent(QDropEvent * const opc_Event) override;
   //lint -restore

private:
   //Avoid call
   C_SyvUpUpdatePackageListNodeWidget(const C_SyvUpUpdatePackageListNodeWidget &);
   C_SyvUpUpdatePackageListNodeWidget & operator =(const C_SyvUpUpdatePackageListNodeWidget &);

   void m_InitApplications(void);

   void m_UpdateTitle(void) const;
   void m_SetApplicationState(const stw_types::uint32 ou32_Application, const stw_types::uint32 ou32_State) const;
   stw_types::uint32 m_GetApplicationState(const stw_types::uint32 ou32_Application) const;
   void m_SetApplicationSelect(const stw_types::uint32 ou32_Application, const bool oq_Select) const;
   QString m_GetApplicationPath(const stw_types::uint32 ou32_Application) const;
   void m_SetApplicationConnected(const stw_types::uint32 ou32_Application, const bool oq_Connected) const;
   stw_types::uint32 m_GetFirstNotFinishedApplication(void) const;
   bool m_CheckMime(const QMimeData * const opc_Mime, const QPoint & orc_Pos, QStringList * const opc_FilePaths = NULL,
                    C_SyvUpUpdatePackageListNodeAppWidget ** const oppc_App = NULL) const;
   void m_UpdateNumbers(void) const;
   void m_ButtonAddNewFile(void);

   Ui::C_SyvUpUpdatePackageListNodeWidget * mpc_Ui;
   QLabel * mpc_LabelStateImg;
   stw_types::uint32 mu32_ViewIndex;
   stw_types::uint32 mu32_NodeIndex;
   QString mc_NodeName;
   QString mc_DeviceType;
   bool mq_FileBased;
   bool mq_StwFlashloader;
   stw_types::uint32 mu32_PositionNumber;
   stw_types::uint32 mu32_AppCount;
   stw_types::uint32 mu32_AppsUpdated;
   bool mq_Connected;
};
}

#endif
