//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing a specific node with its applications and data blocks as list item

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPPACSECTIONNODEWIDGET_H
#define C_SYVUPPACSECTIONNODEWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include <QMovie>

#include "stwtypes.hpp"

#include "C_SyvUpPacListNodeItemWidget.hpp"
#include "C_SyvUpPacConfig.hpp"
#include "C_SyvUpDeviceInfo.hpp"

#include "C_OscSuSequences.hpp"
#include "C_OscViewNodeUpdate.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SyvUpPacSectionNodeWidget;
}

namespace stw::opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpPacSectionNodeWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvUpPacSectionNodeWidget(QWidget * const opc_Parent = NULL);
   ~C_SyvUpPacSectionNodeWidget() override;

   void InitWidget(const uint32_t ou32_ViewIndex, const uint32_t ou32_PositionNumber, const uint32_t ou32_NodeIndex,
                   const QString & orc_NodeName, const uint32_t ou32_SectionNumber,
                   const uint32_t ou32_DataBlockPathNumber);
   void InitStaticNames(void);

   void SetConnected(void);
   void SetUpdateApplicationStarted(void);
   void SetUpdateApplicationFinished(void);
   void SetUpdateApplicationError(void);
   virtual void SetUpdateFinished(void);
   void DiscardApplicationStatus(void);
   void SetDisconnected(void);
   void UpdatePositionNumber(const uint32_t ou32_PositionNumber);
   virtual void AdaptFile(const QString & orc_File, C_SyvUpPacListNodeItemWidget * const opc_App);
   virtual void SetSkipOfUpdateFile(const bool oq_Skip, C_SyvUpPacListNodeItemWidget * const opc_App);
   virtual void RevertFile(C_SyvUpPacListNodeItemWidget * const opc_App);
   virtual void RemoveFile(C_SyvUpPacListNodeItemWidget * const opc_App);
   void RemoveAllFiles(void);
   virtual void UpdateDeviceInformation(const stw::opensyde_gui_logic::C_SyvUpDeviceInfo & orc_DeviceInformation);
   virtual void OpenPemFileSettings(C_SyvUpPacListNodeItemWidget * const opc_App);
   virtual void PrepareExportConfig(stw::opensyde_gui_logic::C_SyvUpPacConfigNode & orc_NodeConfig) const = 0;
   virtual void LoadImportConfig(const stw::opensyde_gui_logic::C_SyvUpPacConfig & orc_Config) = 0;
   virtual uint32_t Type(void) const = 0;
   int32_t CheckAllFiles(uint32_t & oru32_CountFiles, QStringList * const opc_MissingFiles,
                         QStringList * const opc_MissingParamSetFiles, QStringList * const opc_FlashwareWarningsApps);
   C_SyvUpPacListNodeItemWidget * GetAndSelectApplication(const QPoint & orc_Pos) const;
   C_SyvUpPacListNodeItemWidget * GetApplication(const QPoint & orc_Pos, uint32_t * const opu32_Number = NULL) const;
   void SetApplicationSelect(const uint32_t ou32_Application, const bool oq_Select) const;
   uint32_t GetFileCount(void) const;
   uint32_t GetPrimaryFileCount(void) const;
   uint32_t GetParamSetFileCount(void) const;
   uint32_t GetPemFileCount(void) const;
   uint32_t GetSectionState(void) const;
   bool IsUpdateNecessary(void) const;
   int32_t GetUpdatePackage(stw::opensyde_core::C_OscSuSequences::C_DoFlash & orc_ApplicationsToWrite,
                            stw::opensyde_core::C_OscSuSequences::C_DoFlash * const opc_AllApplications,
                            uint32_t & oru32_FilesUpdated) const;
   virtual bool CheckMime(QStringList & orc_PathList, const QPoint & orc_Pos, QStringList * const opc_RelevantFilePaths,
                          QStringList * const opc_RelevantParamSetImagePaths,
                          QStringList * const opc_RelevantPemFilePaths,
                          C_SyvUpPacListNodeItemWidget ** const oppc_App) const;
   void Expand(const bool oq_Expand);
   bool IsExpanded(void) const;

protected:
   void resizeEvent(QResizeEvent * const opc_Event) override;

   virtual void m_InitSpecificItem(const stw::opensyde_core::C_OscNode & orc_Node,
                                   const stw::opensyde_core::C_OscViewNodeUpdate & orc_UpdateInfo) = 0;
   virtual void m_InitStaticNames(void) = 0;
   virtual void m_UpdateTitle(void);
   virtual uint32_t m_AdaptParamSetNumber(const uint32_t ou32_Number);

   QString m_GetApplicationPath(const uint32_t ou32_Application) const;
   bool m_AreAllFilesSkipped(void) const;
   void m_FileCountChanged(void);
   void m_SetFileState(const uint32_t ou32_File, const uint32_t ou32_State);
   void m_SetState(const uint32_t ou32_State);

   void m_AdaptParamSetFile(const QString & orc_File, C_SyvUpPacListNodeItemWidget * const opc_App);
   int32_t m_GetParamsetFileInfo(const QString & orc_File,
                                 stw::opensyde_core::C_OscViewNodeUpdateParamInfo & orc_ParamFileInfo);

   void mousePressEvent(QMouseEvent * const opc_Event) override;

   static bool mh_IsFileParamSetFile(const QString & orc_File);
   static bool mh_IsFilePemFile(const QString & orc_File);

   Ui::C_SyvUpPacSectionNodeWidget * mpc_Ui;
   uint32_t mu32_ViewIndex;
   uint32_t mu32_NodeIndex;
   uint32_t mu32_SectionNumber;
   QString mc_NodeName;
   QString mc_SectionName; // In case of a datablock the datablock name
   QString mc_DeviceType;
   bool mq_FileBased;
   bool mq_StwFlashloader;
   uint32_t mu32_FileCount;           // Count of all files
   uint32_t mu32_PrimaryFileCount;    // Count of all section specific files without param set and PEM files
   uint32_t mu32_ParamSetFileCount;   // Count of all param set files in the section
   uint32_t mu32_PemFileCount;        // Count of all PEM files in the section
   uint32_t mu32_DataBlockPathNumber; // Index of Data Block path
   QString mc_Title;
   bool mq_ShowAddButton;

private:
   void m_OnExpand(const bool oq_Expand);

   void m_InitItems(void);

   uint32_t m_GetApplicationState(const uint32_t ou32_Application, uint32_t * const opu32_Type = NULL) const;
   void m_SetApplicationConnected(const uint32_t ou32_Application, const bool oq_Connected) const;
   uint32_t m_GetFirstNotFinishedApplication(void) const;

   void m_UpdateNumbers(void);
   void m_ButtonAddNewFile(void);
   void m_RestartMovie(void);
   void m_UpdateLabelNoFiles(void);

   static void mh_FillDoFlashWithPemStates(const C_SyvUpPacListNodeItemWidget * const opc_App,
                                           stw::opensyde_core::C_OscSuSequences::C_DoFlash & orc_DoFlash);

   uint32_t mu32_PositionNumber;
   uint32_t mu32_State;
   bool mq_Connected;
   QMovie * mpc_Movie;

   static const int32_t mhs32_TITLE_OFFSET = 140;
};
}

#endif
