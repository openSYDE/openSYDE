//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing a specific application or data block as part of a node

   \implementation
   project     project name
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     09.02.2018  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SYVUPUPDATEPACKAGELISTNODEAPPWIDGET_H
#define C_SYVUPUPDATEPACKAGELISTNODEAPPWIDGET_H

/* -- Includes ------------------------------------------------------------- */
#include <QDateTime>

#include "stwtypes.h"

#include "C_OgeWiOnlyBackground.h"

#include "C_SyvUpUpdatePackageConfig.h"
#include "C_OSCNodeApplication.h"

/* -- Namespace ------------------------------------------------------------ */

namespace Ui
{
class C_SyvUpUpdatePackageListNodeAppWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SyvUpUpdatePackageListNodeAppWidget :
   public stw_opensyde_gui_elements::C_OgeWiOnlyBackground
{
   Q_OBJECT

public:
   explicit C_SyvUpUpdatePackageListNodeAppWidget(const stw_types::uint32 ou32_ViewIndex,
                                                  const stw_types::uint32 ou32_NodeIndex,
                                                  const QString & orc_DeviceName, const bool oq_FileBased,
                                                  const bool oq_StwFlashloader, QWidget * const opc_Parent = NULL);
   virtual ~C_SyvUpUpdatePackageListNodeAppWidget();

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
   bool IsDefaultPathActive(void) const;
   bool IsAppFileMissing(void) const;

   void PrepareExportConfig(stw_opensyde_gui_logic::C_SyvUpUpdatePackageConfigNodeApp & orc_AppConfig) const;
   void LoadImportConfig(const stw_opensyde_gui_logic::C_SyvUpUpdatePackageConfigNode & orc_NodeConfig);

   stw_types::sint32 CheckPath(void);

   bool IsAppIdentical(const QString & orc_AppName, const QString & orc_AppVersion, const QString & orc_AppBuildTime,
                       const QString & orc_AppBuildDate) const;

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

private:
   //Avoid call
   C_SyvUpUpdatePackageListNodeAppWidget(const C_SyvUpUpdatePackageListNodeAppWidget &);
   C_SyvUpUpdatePackageListNodeAppWidget & operator =(const C_SyvUpUpdatePackageListNodeAppWidget &);

   void m_UpdateAbsolutePath(void);
   void m_UpdateTitle(void);
   bool m_LoadApplicationInformation(void);
   void m_UpdateToolTip(void);
   void m_UpateFilePathLabel(void) const;
   void m_SetSelectColor(const bool oq_Active);

   Ui::C_SyvUpUpdatePackageListNodeAppWidget * mpc_Ui;

   const stw_types::uint32 mu32_ViewIndex;
   const stw_types::uint32 mu32_NodeIndex;
   const QString mc_DeviceName;

   bool mq_Selected;

   QString mc_Name;
   QString mc_Title;
   stw_opensyde_core::C_OSCNodeApplication::E_Type me_Type;
   stw_types::uint32 mu32_Number;
   QString mc_FilePath;
   QString mc_DefaultFilePath; // Only for comparing with an import configuration
   bool mq_DefaultFilePath;
   const bool mq_FileBased;
   bool mq_StwFlashloader;
   stw_types::uint32 mu32_State;
   bool mq_Connected;

   // File information
   QString mc_ProjectName;
   QString mc_Date;
   QString mc_Time;
   QString mc_AbsoluteFilePath;
   QString mc_Version;
   stw_types::uint32 mu32_FileSize; // In KB

   // File information for change check
   QDateTime mc_FileTimestamp;
   stw_types::sint32 ms32_PreviousFileResult;

   static const stw_types::sintn mhsn_TitleOffset = 140;
   static const stw_types::sintn mhsn_PathOffset = 60;
};
}

#endif
