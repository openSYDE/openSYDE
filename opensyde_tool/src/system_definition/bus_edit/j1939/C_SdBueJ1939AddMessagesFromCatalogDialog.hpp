//----------------------------------------------------------------------------------------------------------------------
/*!
   \file          C_SdBueJ1939AddMessagesFromCatalogDialog.cpp
   \brief         Pop up dialog for adding Pg messages from J1939 catalog
   \copyright     Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUEJ1939ADDMESSAGESFROMCATALOGDIALOG_HPP
#define C_SDBUEJ1939ADDMESSAGESFROMCATALOGDIALOG_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "stwerrors.hpp"
#include "C_OgePopUpDialog.hpp"
#include "C_CieConverter.hpp"
#include "C_OgeContextMenu.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdBueJ1939AddMessagesFromCatalogDialog;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueJ1939AddMessagesFromCatalogDialog :
   public QWidget
{
   Q_OBJECT

public:
   // Message modes (sequence of the combo box index)
   enum E_MessageMode
   {
      eALL_MESSAGES = 0,
      eJ1939_SPECIFIC
   };

   explicit C_SdBueJ1939AddMessagesFromCatalogDialog(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent);
   ~C_SdBueJ1939AddMessagesFromCatalogDialog(void) noexcept override;

   void InitStaticNames(void) const;
   std::vector<stw::opensyde_gui_logic::C_CieConverter::C_CieNodeMessage> GetMessagesImportedFromCatalog(void) const;
   const QString GetCatalogFilePath() const;

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   void m_AddClicked(void);
   void m_CancelClicked(void);
   void m_CatalogPathButtonClicked();
   void m_OnModeChanged();
   void m_OnSearch(const QString & orc_Text) const;
   void m_SetupContextMenu(void);
   void m_OnCustomContextMenuRequested(const QPoint & orc_Pos);
   void m_LoadCatalog();
   void m_SetStatus() const;
   void m_UpdateUi() const;
   void m_UpdateSelection(const uint32_t ou32_SelectionCount) const;
   void m_ShowCatalogImportError();
   void m_ExtractMessagesFromDbc(const stw::opensyde_gui_logic::C_CieConverter::C_CieCommDefinition & orc_CieCommDef);
   QString m_GetCatalogFilePath(QWidget * const opc_BaseWidget);
   void m_RemoveDuplicateMessages();
   void m_LinkClicked(const QUrl & orc_Link) const;
   void m_ShowNoMessagesSelectedPopup();
   void m_CatalogPathEntered();
   void m_ProcessCatalogPath(const bool oq_PathManuallyEntered);
   void m_FilterJ1939SpecificMessages(
      std::vector<stw::opensyde_gui_logic::C_CieConverter::C_CieNodeMessage> & orc_FilteredMessages);
   void m_SetMessageMode();
   void m_SelectAllMessages();

   //Avoid call
   C_SdBueJ1939AddMessagesFromCatalogDialog(const C_SdBueJ1939AddMessagesFromCatalogDialog &);
   C_SdBueJ1939AddMessagesFromCatalogDialog & operator =(const C_SdBueJ1939AddMessagesFromCatalogDialog &) &;

   Ui::C_SdBueJ1939AddMessagesFromCatalogDialog * mpc_Ui;
   stw::opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   QString mc_CatalogFilePath;
   int32_t ms32_ImportCatalogReturn;
   std::vector<stw::opensyde_gui_logic::C_CieConverter::C_CieNodeMessage> mc_MessagesImportedFromCatalog;
   E_MessageMode me_Mode;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
