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

#include "C_FlaUpHexFileInfo.hpp"
#include "C_FlaUpListWidget.hpp"
#include "C_OgeWiOnlyBackground.hpp"

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
   explicit C_FlaUpListItemWidget(C_FlaUpListWidget * const opc_Parent, C_FlaUpHexFileInfo * const opc_HexFileInfo);
   ~C_FlaUpListItemWidget(void) override;
   void GetHexFileInformation(C_FlaUpHexFileInfo * const opc_HexFileInfo);
   int32_t GetApplicationBlockIndex(void) const;
   bool IsFileExistingAndValid(void) const;
   void EnableSettings(const bool oq_Enabled);
   C_FlaUpHexFileInfo * pc_HexFileInfo;

Q_SIGNALS:
   void SigDeleteItemWidget(void);
   void SigSelectFile(void);
   void SigReloadCurrentFile(void);

protected:
   void resizeEvent(QResizeEvent * const opc_Event) override;

private:
   Ui::C_FlaUpListItemWidget * mpc_Ui;
   stw::opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;
   int32_t ms32_ApplicationBlockIndex;
   QTimer * mpc_FileStatusChecktimer;

   void m_SetupContextMenu();
   void m_OnCustomContextMenuRequested(const QPoint & orc_Pos);
   void m_ViewFileInfo(void);
   void m_ShowInExplorer(void) const;
   QString m_UpdateToolTipContent(const int32_t os32_Index);
   void m_DeleteItem(void);
   void m_SelectFile(void);
   void m_FileStatusCheck(void);

   static const int32_t mhs32_TITLE_OFFSET = 140;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
