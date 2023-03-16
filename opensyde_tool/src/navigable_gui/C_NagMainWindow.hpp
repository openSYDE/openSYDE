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

#include "precomp_headers.hpp"
#include <QMainWindow>
#include <QWidget>
#include <QMimeData>
#include <vector>

#include "stwtypes.hpp"
#include "C_SyvManager.hpp"
#include "C_NagMainWidget.hpp"
#include "C_NagUseCaseViewWidget.hpp"
#include "C_NagUseCaseWidget.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_NagMainWindow;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_NagMainWindow :
   public QMainWindow
{
   Q_OBJECT

public:
   explicit C_NagMainWindow(const uint16_t ou16_Timer);
   ~C_NagMainWindow() override;

private Q_SLOTS:
   void m_ShowStartView(void);
   void m_ChangeUseCase(const int32_t os32_Mode, const int32_t os32_SubMode, const bool oq_ChangeUseCase = false);
   void m_HandleSysDefRequest(const int32_t os32_Mode, const int32_t os32_SubMode);
   bool m_ChangeMode(const int32_t os32_Mode, const int32_t os32_SubMode, const uint32_t ou32_Index = 0U,
                     const QString & orc_Name = "", const QString & orc_SubSubName = "", const uint32_t ou32_Flag = 0U,
                     const bool oq_ChangeUseCase = false);
   void m_OpenDetail(const int32_t os32_Index, const int32_t os32_SubIndex, const int32_t os32_SubSubIndex,
                     const int32_t os32_SubSubSubIndex, const int32_t os32_Flag);

protected:
   void showEvent(QShowEvent * const opc_Event) override;
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   void closeEvent(QCloseEvent * const opc_Event) override;
   void dragEnterEvent(QDragEnterEvent * const opc_Event) override;
   void dragMoveEvent(QDragMoveEvent * const opc_Event) override;
   void dropEvent(QDropEvent * const opc_Event) override;

private:
   //Avoid call
   C_NagMainWindow(const C_NagMainWindow &);
   C_NagMainWindow & operator =(const C_NagMainWindow &) &;

   void m_LoadInitialProject(void);
   void m_PrepareForSpecificWidget(const bool oq_DeleteActualWidget = true);
   void m_SetNewSpecificWidget(const int32_t os32_Mode, const int32_t os32_SubMode = 0,
                               const QString & orc_ItemName = "", const QString & orc_SubSubModeName = "",
                               const uint32_t ou32_Index = 0);
   void m_RemoveUseCaseWidget(void);
   void m_AdaptParameter(const int32_t os32_Mode, int32_t & ors32_SubMode, uint32_t & oru32_Index, QString & orc_Name,
                         QString & orc_SubItemName, uint32_t & oru32_Flag, const bool oq_ChangeUseCase);
   static void mh_GetHeadingNames(const int32_t os32_Mode, const int32_t & ors32_SubMode, const uint32_t ou32_Index,
                                  QString & orc_SubMode, QString & orc_SubSubMode);
   void m_ShowSysDefItem(const int32_t os32_SubMode, const uint32_t ou32_Index = 0U, const QString & orc_Name = "",
                         const QString & orc_SubName = "", const uint32_t ou32_Flag = 0U);
   void m_ShowSysViewItem(int32_t & ors32_SubMode, const uint32_t ou32_Index = 0U, const QString & orc_Name = "",
                          const QString & orc_SubSubModeName = "", const uint32_t ou32_Flag = 0U);
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
   void m_HandleRenameView(const uint32_t ou32_Index, const QString & orc_Name) const;
   void m_UpdateTitle(void) const;
   void m_HandleMoveViewRequest(const uint32_t ou32_StartIndex, const uint32_t ou32_TargetIndex);
   void m_HandleDeleteSysViewRequest(const uint32_t ou32_Index, const int32_t os32_SelectedSubMode,
                                     const uint32_t ou32_SelectedIndex);
   void m_HandleDuplicateSysViewRequest(const uint32_t ou32_Index);
   void m_HandleServiceMode(void) const;

   Ui::C_NagMainWindow * mpc_Ui;

   C_NagMainWidget * mpc_MainWidget;
   C_NagUseCaseViewWidget * mpc_UseCaseWidget;
   C_NagUseCaseWidget * mpc_ActiveWidget;
   stw::opensyde_gui_logic::C_SyvManager mc_SystemViewManager;

   bool mq_InitialProjectLoaded;
   bool mq_BlockDragAndDrop;
   bool mq_StartView;
   int32_t ms32_Mode;
   int32_t ms32_SubMode;
   uint32_t mu32_Index;

   // Special case: Switching from main widget to system definition, restore previous position
   int32_t ms32_SdSubMode;
   uint32_t mu32_SdIndex;
   uint32_t mu32_SdFlag;

   // Special case: Switching from main widget to system commissioning, restore previous position
   int32_t ms32_SvSubMode;
   uint32_t mu32_SvIndex;
   uint32_t mu32_SvFlag;

   const uint16_t mu16_Timer;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
