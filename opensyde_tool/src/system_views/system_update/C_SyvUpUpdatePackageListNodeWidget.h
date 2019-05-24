//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing a specific node with its applications and data blocks as list item

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPUPDATEPACKAGELISTNODEWIDGET_H
#define C_SYVUPUPDATEPACKAGELISTNODEWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "stwtypes.h"

#include "C_SyvUpUpdatePackageListNodeItemWidget.h"
#include "C_SyvUpUpdatePackageConfig.h"
#include "C_SyvUpDeviceInfo.h"

#include "C_OSCSuSequences.h"
#include "C_PuiSvNodeUpdate.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SyvUpUpdatePackageListNodeWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpUpdatePackageListNodeWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvUpUpdatePackageListNodeWidget(QWidget * const opc_Parent = NULL);
   virtual ~C_SyvUpUpdatePackageListNodeWidget();

   void InitWidget(const stw_types::uint32 ou32_ViewIndex, const stw_types::uint32 ou32_PositionNumber,
                   const stw_types::uint32 ou32_NodeIndex, const QString & orc_NodeName);
   void InitStaticNames(void) const;

   void SetConnected(void);
   void SetUpdateApplicationStarted(void) const;
   void SetUpdateApplicationFinished(void) const;
   void SetUpdateApplicationError(void) const;
   virtual void SetUpdateFinished(void) const;
   void DiscardApplicationStatus(void) const;
   void SetDisconnected(void);
   void UpdatePositionNumber(const stw_types::uint32 ou32_PositionNumber);
   virtual void AddFile(const QString & orc_File);
   virtual void AdaptFile(const QString & orc_File, C_SyvUpUpdatePackageListNodeItemWidget * const opc_App);
   virtual void RevertFile(C_SyvUpUpdatePackageListNodeItemWidget * const opc_App);
   virtual void RemoveFile(C_SyvUpUpdatePackageListNodeItemWidget * const opc_App);
   void RemoveAllFiles(void);
   virtual void UpdateDeviceInformation(const stw_opensyde_gui_logic::C_SyvUpDeviceInfo & orc_DeviceInformation) const;
   virtual void PrepareExportConfig(stw_opensyde_gui_logic::C_SyvUpUpdatePackageConfigNode & orc_NodeConfig) const = 0;
   virtual void LoadImportConfig(const stw_opensyde_gui_logic::C_SyvUpUpdatePackageConfig & orc_Config) = 0;
   virtual stw_types::uint32 Type(void) const = 0;
   stw_types::sint32 CheckAllFiles(stw_types::uint32 & oru32_CountFiles, QStringList * const opc_MissingApps,
                                   QStringList * const opc_FlashwareWarningsApps);
   C_SyvUpUpdatePackageListNodeItemWidget * GetAndSelectApplication(const QPoint & orc_Pos) const;
   C_SyvUpUpdatePackageListNodeItemWidget * GetApplication(const QPoint & orc_Pos,
                                                           stw_types::uint32 * const opu32_Number = NULL) const;
   void SetApplicationSelect(const stw_types::uint32 ou32_Application, const bool oq_Select) const;
   stw_types::uint32 GetFileCount(void) const;
   bool IsUpdateNecessary(void) const;
   virtual stw_types::sint32 GetUpdatePackage(stw_opensyde_core::C_OSCSuSequences::C_DoFlash & orc_ApplicationsToWrite,
                                              stw_opensyde_core::C_OSCSuSequences::C_DoFlash * const opc_AllApplications,
                                              stw_types::uint32 & oru32_FilesUpdated) const;
   virtual bool CheckMime(QStringList & orc_PathList, const QPoint & orc_Pos, QStringList * const opc_RelevantFilePaths,
                          C_SyvUpUpdatePackageListNodeItemWidget ** const oppc_App) const;
   void Expand(const bool oq_Expand);
   bool IsExpanded(void) const;

protected:
   virtual void m_InitSpecificItem(const stw_opensyde_core::C_OSCNode & orc_Node,
                                   const stw_opensyde_gui_logic::C_PuiSvNodeUpdate & orc_UpdateInfo) = 0;
   virtual void m_InitStaticNames(void) const = 0;

   QString m_GetApplicationPath(const stw_types::uint32 ou32_Application) const;
   void m_FileCountChanged(void);
   void m_SetApplicationState(const stw_types::uint32 ou32_Application, const stw_types::uint32 ou32_State) const;

   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void mousePressEvent(QMouseEvent * const opc_Event) override;
   //lint -restore

   Ui::C_SyvUpUpdatePackageListNodeWidget * mpc_Ui;
   stw_types::uint32 mu32_ViewIndex;
   stw_types::uint32 mu32_NodeIndex;
   QString mc_NodeName;
   QString mc_DeviceType;
   bool mq_FileBased;
   bool mq_StwFlashloader;
   stw_types::uint32 mu32_FileCount;
   bool mq_ShowAddButton;

private:
   //Avoid call
   C_SyvUpUpdatePackageListNodeWidget(const C_SyvUpUpdatePackageListNodeWidget &);
   C_SyvUpUpdatePackageListNodeWidget & operator =(const C_SyvUpUpdatePackageListNodeWidget &);

   void m_OnExpand(const bool oq_Expand);

   void m_InitItems(void);

   stw_types::uint32 m_GetApplicationState(const stw_types::uint32 ou32_Application) const;
   void m_SetApplicationConnected(const stw_types::uint32 ou32_Application, const bool oq_Connected) const;
   stw_types::uint32 m_GetFirstNotFinishedApplication(void) const;

   void m_UpdateNumbers(void) const;
   void m_ButtonAddNewFile(void);

   stw_types::uint32 mu32_PositionNumber;
   bool mq_Connected;
};
}

#endif
