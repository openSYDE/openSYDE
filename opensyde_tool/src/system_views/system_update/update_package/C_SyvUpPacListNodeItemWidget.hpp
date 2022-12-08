//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing a specific application or data block as part of a node

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPPACLISTNODEITEMWIDGET_H
#define C_SYVUPPACLISTNODEITEMWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QDateTime>

#include "stwtypes.hpp"

#include "C_OgeWiOnlyBackground.hpp"

#include "C_SyvUpPacConfig.hpp"
#include "C_OscNodeApplication.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SyvUpPacListNodeItemWidget;
}

namespace stw::opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpPacListNodeItemWidget :
   public stw::opensyde_gui_elements::C_OgeWiOnlyBackground
{
   Q_OBJECT

public:
   explicit C_SyvUpPacListNodeItemWidget(const uint32_t ou32_ViewIndex, const uint32_t ou32_NodeIndex,
                                         const QString & orc_DeviceName, const bool oq_FileBased,
                                         const bool oq_StwFlashloader, QWidget * const opc_Parent = NULL);
   virtual ~C_SyvUpPacListNodeItemWidget();

   QWidget * GetListParent(void);

   void SetSelected(const bool oq_Selected);
   void SetState(const uint32_t ou32_State);

   void SetOwnerSectionName(const QString & orc_Name);
   void SetAppNumber(const uint32_t ou32_Number);
   void SetAppFile(const QString & orc_File, const bool oq_DefaultFile);
   void SetDefaultFile(const QString & orc_File);
   void SetConnected(const bool oq_Connected);
   void SetSkipOfUpdateFile(const bool oq_Skip);

   virtual uint32_t GetType(void) const = 0;
   uint32_t GetState(void) const;
   uint32_t GetAppNumber(void) const;
   QString GetAppFileName(void) const;
   QString GetAppFilePath(void) const;
   QString GetDefaultFilePath(void) const;
   QString GetAppAbsoluteFilePath(void) const;
   QString GetAppDeviceType(void) const;
   bool GetSkipOfUpdateFile(void) const;
   bool IsAppInfoAmbiguous(void) const;
   bool IsDefaultPathActive(void) const;

   void PrepareExportConfig(stw::opensyde_gui_logic::C_SyvUpPacConfigNodeApp & orc_AppConfig) const;
   void LoadImportConfig(const stw::opensyde_gui_logic::C_SyvUpPacConfigNodeApp & orc_AppConfig);

   int32_t CheckPath(void);

   virtual bool IsFileIdentical(const QString & orc_AppName, const QString & orc_AppVersion,
                                const QString & orc_AppBuildTime, const QString & orc_AppBuildDate) const;
   virtual void ViewFileInfo(void);
   virtual bool IsViewFileInfoPossible(void) const = 0;
   //virtual

   static const uint32_t hu32_STATE_DEFAULT = 0U;
   static const uint32_t hu32_STATE_ERROR = 1U;
   static const uint32_t hu32_STATE_FINISHED = 2U;
   static const uint32_t hu32_STATE_IN_PROGRESS = 3U;
   static const uint32_t hu32_STATE_TO_DO = 4U;
   static const uint32_t hu32_STATE_DISABLED = 5U;

protected:
   void resizeEvent(QResizeEvent * const opc_Event) override;
   bool event(QEvent * const opc_Event) override;

   virtual void m_LoadFileInformation(bool & orq_FileExists, bool & orq_FlashwareWarning, bool & orq_TriggerRemove);
   virtual QString m_CreateToolTipTitle(void) const;
   virtual QString m_CreateToolTipContent(void) const;

   void m_UpateFilePathLabel(void) const;

   Ui::C_SyvUpPacListNodeItemWidget * mpc_Ui;
   const uint32_t mu32_ViewIndex;
   const uint32_t mu32_NodeIndex;
   const QString mc_NodeDeviceType;
   QString mc_AppDeviceType;
   bool mq_AppInfoAmbiguous;
   const bool mq_FileBased;
   bool mq_StwFlashloader;
   bool mq_SkipOfUpdateFile;

   QString mc_OwnerSectionName;
   uint32_t mu32_Number;
   QString mc_FilePath;

   // File information
   QString mc_Date;
   QString mc_Time;
   QString mc_AbsoluteFilePath;

private:
   void m_UpdateAbsolutePath(void);
   void m_UpdateToolTip(void);
   void m_SetSelectColor(const bool oq_Active);

   QWidget * const mpc_ListParent;
   bool mq_Selected;

   QString mc_DefaultFilePath; // Only for comparing with an import configuration
   bool mq_DefaultFilePath;
   bool mq_DefaultFilePathUsed; // Was the default path set at least one time
   uint32_t mu32_State;
   bool mq_Connected;

   uint32_t mu32_FileSize; // In kB

   // File information for change check
   QDateTime mc_FileTimestamp;
   int32_t ms32_PreviousFileResult;

   static const int32_t mhs32_PATH_OFFSET = 60;
};
}

#endif
