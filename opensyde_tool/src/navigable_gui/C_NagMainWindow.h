//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Main window with all sub widgets

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

#ifndef C_NAGMAINWINDOW_H
#define C_NAGMAINWINDOW_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "precomp_headers.h"
#include <QMainWindow>
#include <QWidget>
#include <QMimeData>
#include <vector>

#include "stwtypes.h"
#include "C_SyvManager.h"
#include "C_NagMainWidget.h"
#include "C_NagUseCaseViewWidget.h"
#include "C_NagUseCaseWidget.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_NagMainWindow;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_NagMainWindow :
   public QMainWindow
{
   Q_OBJECT

public:
   explicit C_NagMainWindow(void);
   virtual ~C_NagMainWindow();

private Q_SLOTS:
   void m_ShowStartView(void);
   void m_ChangeUseCase(const stw_types::sint32 os32_Mode, const stw_types::sint32 os32_SubMode);
   bool m_ChangeMode(const stw_types::sint32 os32_Mode, const stw_types::sint32 os32_SubMode,
                     const stw_types::uint32 ou32_Index = 0U, const QString & orc_Name = "",
                     const QString & orc_SubSubName = "", const stw_types::uint32 ou32_Flag = 0U);
   void m_OpenDetail(const stw_types::sint32 os32_Index, const stw_types::sint32 os32_SubIndex,
                     const stw_types::sint32 os32_SubSubIndex, const stw_types::sint32 os32_SubSubSubIndex,
                     const stw_types::sint32 os32_Flag);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void showEvent(QShowEvent * const opc_Event) override;
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   virtual void closeEvent(QCloseEvent * const opc_Event) override;
   virtual void dragEnterEvent(QDragEnterEvent * const opc_Event) override;
   virtual void dragMoveEvent(QDragMoveEvent * const opc_Event) override;
   virtual void dropEvent(QDropEvent * const opc_Event) override;
   //lint -restore

private:
   //Avoid call
   C_NagMainWindow(const C_NagMainWindow &);
   C_NagMainWindow & operator =(const C_NagMainWindow &);

   void m_LoadInitialProject(void);
   void m_PrepareForSpecificWidget(const bool oq_DeleteActualWidget = true);
   void m_SetNewSpecificWidget(const stw_types::sint32 os32_Mode, const stw_types::sint32 os32_SubMode = 0,
                               QString oc_ItemName = "", const QString & orc_SubSubModeName = "",
                               const stw_types::uint32 ou32_Index = 0);
   void m_RemoveUseCaseWidget(void);
   void m_AdaptParameter(const stw_types::sint32 os32_Mode, stw_types::sint32 & ors32_SubMode,
                         stw_types::uint32 & oru32_Index, QString & orc_Name, QString & orc_SubItemName,
                         stw_types::uint32 & oru32_Flag);
   void m_ShowSysDefItem(const stw_types::sint32 os32_SubMode, const stw_types::uint32 ou32_Index = 0U,
                         const QString & orc_Name = "", const QString & orc_SubName = "",
                         const stw_types::uint32 ou32_Flag = 0U);
   void m_ShowSysViewItem(stw_types::sint32 & ors32_SubMode, const stw_types::uint32 ou32_Index = 0U,
                          const QString & orc_Name = "", const QString & orc_SubSubModeName = "",
                          const stw_types::uint32 ou32_Flag = 0U);
   void m_SaveScreenProperties(void) const;
   static bool mh_CheckMime(const QMimeData * const opc_Mime, QString * const opc_FilePath = NULL);
   void m_BlockDragAndDrop(const bool oq_Block);

   bool m_CheckProjectForChanges(void) const;
   void m_PrepareProjectLoad(void);
   void m_CloseAndPrepareProjectLoad(void);
   void m_ProjectLoaded(const bool & orq_SwitchToLastKnownMode);

   //Generic push button
   void m_SetInteractionWidget(QWidget * const opc_Widget);

   void m_HandleAddViewRequest(void);
   void m_HandleRenameView(const stw_types::uint32 ou32_Index, const QString & orc_Name) const;
   void m_UpdateTitle(void) const;
   void m_HandleMoveViewRequest(const stw_types::uint32 ou32_StartIndex, const stw_types::uint32 ou32_TargetIndex);
   void m_HandleDeleteSysViewRequest(const stw_types::uint32 ou32_Index, const stw_types::sint32 os32_SelectedSubMode,
                                     const stw_types::uint32 ou32_SelectedIndex);
   void m_HandleDuplicateSysViewRequest(const stw_types::uint32 ou32_Index);

   Ui::C_NagMainWindow * mpc_Ui;

   C_NagMainWidget * mpc_MainWidget;
   C_NagUseCaseViewWidget * mpc_UseCaseWidget;
   C_NagUseCaseWidget * mpc_ActiveWidget;
   stw_opensyde_gui_logic::C_SyvManager mc_SystemViewManager;

   bool mq_InitialProjectLoaded;
   bool mq_BlockDragAndDrop;
   bool mq_StartView;
   bool mq_ChangeUseCase;
   stw_types::sint32 ms32_Mode;
   stw_types::sint32 ms32_SubMode;
   stw_types::uint32 mu32_Index;

   // Special case: Switching from main widget to system definition, restore previous position
   stw_types::sint32 ms32_SdSubMode;
   stw_types::uint32 mu32_SdIndex;
   stw_types::uint32 mu32_SdFlag;

   // Special case: Switching from main widget to system commissioning, restore previous position
   stw_types::sint32 ms32_SvSubMode;
   stw_types::uint32 mu32_SvIndex;
   stw_types::uint32 mu32_SvFlag;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
