//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing a specific application or data block as part of a node

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPUPDATEPACKAGELISTNODEITEMWIDGET_H
#define C_SYVUPUPDATEPACKAGELISTNODEITEMWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QDateTime>
#include <QMovie>

#include "stwtypes.h"

#include "C_OgeWiOnlyBackground.h"

#include "C_SyvUpUpdatePackageConfig.h"
#include "C_OSCNodeApplication.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SyvUpUpdatePackageListNodeItemWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpUpdatePackageListNodeItemWidget :
   public stw_opensyde_gui_elements::C_OgeWiOnlyBackground
{
   Q_OBJECT

public:
   explicit C_SyvUpUpdatePackageListNodeItemWidget(const stw_types::uint32 ou32_ViewIndex,
                                                   const stw_types::uint32 ou32_NodeIndex,
                                                   const QString & orc_DeviceName, const bool oq_FileBased,
                                                   const bool oq_StwFlashloader, QWidget * const opc_Parent = NULL);
   virtual ~C_SyvUpUpdatePackageListNodeItemWidget();

   QWidget * GetListParent(void);

   void SetSelected(const bool oq_Selected);
   void SetState(const stw_types::uint32 ou32_State);

   void SetAppName(const QString & orc_Name);
   void SetAppNumber(const stw_types::uint32 ou32_Number);
   void SetAppFile(const QString & orc_File, const bool oq_DefaultFile);
   void SetDefaultFile(const QString & orc_File);
   void SetAppType(const stw_opensyde_core::C_OSCNodeApplication::E_Type oe_Type);
   void SetConnected(const bool oq_Connected);

   stw_types::uint32 GetState(void) const;
   QString GetAppName(void) const;
   stw_types::uint32 GetAppNumber(void) const;
   QString GetAppFilePath(void) const;
   QString GetAppAbsoluteFilePath(void) const;
   QString GetAppDeviceType(void) const;
   bool IsDefaultPathActive(void) const;
   bool IsAppFileMissing(void) const;
   bool IsAppInfoAmbiguous(void) const;

   void PrepareExportConfig(stw_opensyde_gui_logic::C_SyvUpUpdatePackageConfigNodeApp & orc_AppConfig) const;
   void LoadImportConfig(const stw_opensyde_gui_logic::C_SyvUpUpdatePackageConfigNode & orc_NodeConfig);

   stw_types::sint32 CheckPath(void);

   virtual bool IsFileIdentical(const QString & orc_AppName, const QString & orc_AppVersion,
                                const QString & orc_AppBuildTime, const QString & orc_AppBuildDate) const;
   virtual void ViewFileInfo(void);
   virtual bool IsViewFileInfoPossible(void) const = 0;

   static const stw_types::uint32 hu32_STATE_DEFAULT = 0U;
   static const stw_types::uint32 hu32_STATE_ERROR = 1U;
   static const stw_types::uint32 hu32_STATE_FINISHED = 2U;
   static const stw_types::uint32 hu32_STATE_IN_PROGRESS = 3U;
   static const stw_types::uint32 hu32_STATE_TO_DO = 4U;

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void resizeEvent(QResizeEvent * const opc_Event) override;
   virtual bool event(QEvent * const opc_Event) override;
   //lint -restore

   virtual void m_LoadFileInformation(bool & orq_FileExists, bool & orq_FlashwareWarning, bool & orq_TriggerRemove);
   virtual QString m_CreateToolTipContent(void) const;

   void m_UpateFilePathLabel(void) const;

   Ui::C_SyvUpUpdatePackageListNodeItemWidget * mpc_Ui;
   const stw_types::uint32 mu32_ViewIndex;
   const stw_types::uint32 mu32_NodeIndex;
   const QString mc_NodeDeviceType;
   QString mc_AppDeviceType;
   bool mq_AppInfoAmbiguous;
   const bool mq_FileBased;
   bool mq_StwFlashloader;

   stw_types::uint32 mu32_Number;
   QString mc_FilePath;

   // File information
   QString mc_Date;
   QString mc_Time;
   QString mc_AbsoluteFilePath;

private:
   //Avoid call
   C_SyvUpUpdatePackageListNodeItemWidget(const C_SyvUpUpdatePackageListNodeItemWidget &);
   C_SyvUpUpdatePackageListNodeItemWidget & operator =(const C_SyvUpUpdatePackageListNodeItemWidget &);

   void m_UpdateAbsolutePath(void);
   void m_UpdateTitle(void);
   void m_UpdateToolTip(void);
   void m_SetSelectColor(const bool oq_Active);
   void m_RestartMovie(void);

   QWidget * const mpc_ListParent;

   bool mq_Selected;

   QString mc_Name;
   QString mc_Title;
   stw_opensyde_core::C_OSCNodeApplication::E_Type me_Type;

   QString mc_DefaultFilePath; // Only for comparing with an import configuration
   bool mq_DefaultFilePath;
   stw_types::uint32 mu32_State;
   bool mq_Connected;

   stw_types::uint32 mu32_FileSize; // In kB

   // File information for change check
   QDateTime mc_FileTimestamp;
   stw_types::sint32 ms32_PreviousFileResult;

   QMovie * mpc_Movie;

   static const stw_types::sintn mhsn_TitleOffset = 140;
   static const stw_types::sintn mhsn_PathOffset = 60;
};
}

#endif
