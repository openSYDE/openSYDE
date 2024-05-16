//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for list item
   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_FLAUPLISTITEMWIDGET_H
#define C_FLAUPLISTITEMWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include <QListWidget>
#include <QTimer>
#include <QMovie>

#include "C_FlaUpHexFileInfo.hpp"
#include "C_OgeWiOnlyBackground.hpp"
#include "C_OgeContextMenu.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_FlaUpListItemWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_FlaUpListItemWidget :
   public stw::opensyde_gui_elements::C_OgeWiOnlyBackground
{
   Q_OBJECT

public:
   explicit C_FlaUpListItemWidget(QListWidget * const opc_Parent, C_FlaUpHexFileInfo * const opc_HexFileInfo,
                                  const bool oq_IsRelativePath);
   ~C_FlaUpListItemWidget(void) override;
   void GetHexFileInformation(void);
   int32_t GetApplicationBlockIndex(void) const;
   bool IsFileExistingAndValid(void) const;
   void EnableSettings(const bool oq_Enabled);
   QString GetFileSizeInformation(void) const;
   void SetState(const uint32_t ou32_State);

   C_FlaUpHexFileInfo * pc_HexFileInfo;
   bool q_IsRelativePathToAdd;

   static const uint32_t hu32_STATE_DEFAULT = 0U;
   static const uint32_t hu32_STATE_ERROR = 1U;
   static const uint32_t hu32_STATE_FINISHED = 2U;
   static const uint32_t hu32_STATE_IN_PROGRESS = 3U;
   static const uint32_t hu32_STATE_TO_DO = 4U;
   static const uint32_t hu32_STATE_DISABLED = 5U;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigDeleteItemWidget(int32_t os32_CurrentHexFileIndex, bool oq_NeedToUpdateListIndex);
   void SigSelectFile(int32_t os32_CurrentHexFileIndex);
   void SigReloadCurrentFile(QString oc_FilePath, C_FlaUpHexFileInfo * opc_HexFileInfo);
   void SigDeleteAllItems(void);

protected:
   void resizeEvent(QResizeEvent * const opc_Event) override;

private:
   Ui::C_FlaUpListItemWidget * mpc_Ui;
   stw::opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;
   int32_t ms32_ApplicationBlockIndex;
   QTimer * mpc_FileStatusChecktimer;
   QString mc_CurrentAbsolutePath;
   QString mc_CurrentRelativePath;

   void m_SetupContextMenu();
   void m_OnCustomContextMenuRequested(const QPoint & orc_Pos);
   void m_ViewFileInfo(void);
   void m_ShowInExplorer(void) const;
   QString m_UpdateToolTipContent(const int32_t os32_Index);
   void m_DeleteItem(void);
   void m_DeleteAllItems(void);
   void m_SelectFile(void);
   void m_FileStatusCheck(void);
   void m_SetSizePolicy(void);
   void m_RestartMovie(void);
   void m_GetAbsoluteAndRelativePaths(const QString & orc_Path, const QString & orc_AbsoluteReferenceDir);

   static const int32_t mhs32_TITLE_OFFSET = 140;

   QMovie * mpc_Movie;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
