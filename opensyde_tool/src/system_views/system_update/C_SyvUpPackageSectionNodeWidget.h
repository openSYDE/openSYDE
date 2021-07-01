//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing a specific node with its applications and data blocks as list item

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPPACKAGESECTIONNODEWIDGET_H
#define C_SYVUPPACKAGESECTIONNODEWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include <QMovie>

#include "stwtypes.h"

#include "C_SyvUpPackageListNodeItemWidget.h"
#include "C_SyvUpUpdatePackageConfig.h"
#include "C_SyvUpDeviceInfo.h"

#include "C_OSCSuSequences.h"
#include "C_PuiSvNodeUpdate.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SyvUpPackageSectionNodeWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpPackageSectionNodeWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvUpPackageSectionNodeWidget(QWidget * const opc_Parent = NULL);
   virtual ~C_SyvUpPackageSectionNodeWidget();

   void InitWidget(const stw_types::uint32 ou32_ViewIndex, const stw_types::uint32 ou32_PositionNumber,
                   const stw_types::uint32 ou32_NodeIndex, const QString & orc_NodeName,
                   const stw_types::uint32 ou32_SectionNumber);
   void InitStaticNames(void);

   void SetConnected(void);
   void SetUpdateApplicationStarted(void);
   void SetUpdateApplicationFinished(void);
   void SetUpdateApplicationError(void);
   virtual void SetUpdateFinished(void);
   void DiscardApplicationStatus(void);
   void SetDisconnected(void);
   void UpdatePositionNumber(const stw_types::uint32 ou32_PositionNumber);
   virtual void AdaptFile(const QString & orc_File, C_SyvUpPackageListNodeItemWidget * const opc_App);
   virtual void SetSkipOfUpdateFile(const bool oq_Skip, C_SyvUpPackageListNodeItemWidget * const opc_App);
   virtual void RevertFile(C_SyvUpPackageListNodeItemWidget * const opc_App);
   virtual void RemoveFile(C_SyvUpPackageListNodeItemWidget * const opc_App);
   void RemoveAllFiles(void);
   virtual void UpdateDeviceInformation(const stw_opensyde_gui_logic::C_SyvUpDeviceInfo & orc_DeviceInformation);
   virtual void PrepareExportConfig(stw_opensyde_gui_logic::C_SyvUpUpdatePackageConfigNode & orc_NodeConfig) const = 0;
   virtual void LoadImportConfig(const stw_opensyde_gui_logic::C_SyvUpUpdatePackageConfig & orc_Config) = 0;
   virtual stw_types::uint32 Type(void) const = 0;
   stw_types::sint32 CheckAllFiles(stw_types::uint32 & oru32_CountFiles, QStringList * const opc_MissingFiles,
                                   QStringList * const opc_MissingParamSetFiles,
                                   QStringList * const opc_FlashwareWarningsApps);
   C_SyvUpPackageListNodeItemWidget * GetAndSelectApplication(const QPoint & orc_Pos) const;
   C_SyvUpPackageListNodeItemWidget * GetApplication(const QPoint & orc_Pos,
                                                     stw_types::uint32 * const opu32_Number = NULL) const;
   void SetApplicationSelect(const stw_types::uint32 ou32_Application, const bool oq_Select) const;
   stw_types::uint32 GetFileCount(void) const;
   stw_types::uint32 GetPrimaryFileCount(void) const;
   stw_types::uint32 GetParamSetFileCount(void) const;
   stw_types::uint32 GetSectionState(void) const;
   bool IsUpdateNecessary(void) const;
   stw_types::sint32 GetUpdatePackage(stw_opensyde_core::C_OSCSuSequences::C_DoFlash & orc_ApplicationsToWrite,
                                      stw_opensyde_core::C_OSCSuSequences::C_DoFlash * const opc_AllApplications,
                                      stw_types::uint32 & oru32_FilesUpdated) const;
   virtual bool CheckMime(QStringList & orc_PathList, const QPoint & orc_Pos, QStringList * const opc_RelevantFilePaths,
                          QStringList * const opc_RelevantParamSetImagePaths,
                          C_SyvUpPackageListNodeItemWidget ** const oppc_App) const;
   void Expand(const bool oq_Expand);
   bool IsExpanded(void) const;

protected:
   virtual void resizeEvent(QResizeEvent * const opc_Event) override;

   virtual void m_InitSpecificItem(const stw_opensyde_core::C_OSCNode & orc_Node,
                                   const stw_opensyde_gui_logic::C_PuiSvNodeUpdate & orc_UpdateInfo) = 0;
   virtual void m_InitStaticNames(void) = 0;
   virtual void m_UpdateTitle(void);
   virtual stw_types::uint32 m_AdaptParamSetNumber(const stw_types::uint32 ou32_Number);

   QString m_GetApplicationPath(const stw_types::uint32 ou32_Application) const;
   void m_FileCountChanged(void);
   void m_SetFileState(const stw_types::uint32 ou32_File, const stw_types::uint32 ou32_State);
   void m_SetState(const stw_types::uint32 ou32_State);

   void m_AdaptParamSetFile(const QString & orc_File, C_SyvUpPackageListNodeItemWidget * const opc_App);
   stw_types::sint32 m_GetParamsetFileInfo(const QString & orc_File,
                                           stw_opensyde_gui_logic::C_PuiSvNodeUpdateParamInfo & orc_ParamFileInfo);

   virtual void mousePressEvent(QMouseEvent * const opc_Event) override;

   static bool mh_IsFileParamSetFile(const QString & orc_File);

   Ui::C_SyvUpPackageSectionNodeWidget * mpc_Ui;
   stw_types::uint32 mu32_ViewIndex;
   stw_types::uint32 mu32_NodeIndex;
   stw_types::uint32 mu32_SectionNumber;
   QString mc_NodeName;
   QString mc_SectionName; // In case of a datablock the datablock name
   QString mc_DeviceType;
   bool mq_FileBased;
   bool mq_StwFlashloader;
   stw_types::uint32 mu32_FileCount;         // Count of all files
   stw_types::uint32 mu32_PrimaryFileCount;  // Count of all section specific files without param set files
   stw_types::uint32 mu32_ParamSetFileCount; // Count of all param set files in the section
   QString mc_Title;
   bool mq_ShowAddButton;

private:
   void m_OnExpand(const bool oq_Expand);

   void m_InitItems(void);

   stw_types::uint32 m_GetApplicationState(const stw_types::uint32 ou32_Application) const;
   void m_SetApplicationConnected(const stw_types::uint32 ou32_Application, const bool oq_Connected) const;
   stw_types::uint32 m_GetFirstNotFinishedApplication(void) const;

   void m_UpdateNumbers(void);
   void m_ButtonAddNewFile(void);
   void m_RestartMovie(void);

   stw_types::uint32 mu32_PositionNumber;
   stw_types::uint32 mu32_State;
   bool mq_Connected;
   QMovie * mpc_Movie;

   static const stw_types::sintn mhsn_TitleOffset = 140;
};
}

#endif
