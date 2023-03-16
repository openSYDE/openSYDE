//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Text browser with custom context menu (implementation)

   Text browser with custom context menu in openSYDE style.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QScrollBar>
#include <QClipboard>
#include <QApplication>
#include <QDesktopServices>

#include "stwtypes.hpp"
#include "C_OgeTebContextMenuBase.hpp"
#include "C_GtGetText.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_elements;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor.

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeTebContextMenuBase::C_OgeTebContextMenuBase(QWidget * const opc_Parent) :
   QTextBrowser(opc_Parent),
   mpc_ContextMenu(NULL)
{
   m_InitContextMenu();

   this->setOpenLinks(false); // do not open link in text browser but open file
   connect(this, &QTextBrowser::anchorClicked, this, &C_OgeTebContextMenuBase::m_LinkClicked);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize custom context menu functionality
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeTebContextMenuBase::m_InitContextMenu(void)
{
   this->setContextMenuPolicy(Qt::CustomContextMenu);
   this->mpc_ContextMenu = new C_OgeContextMenu(this);
   connect(this, &C_OgeTebContextMenuBase::customContextMenuRequested, this,
           &C_OgeTebContextMenuBase::m_OnCustomContextMenuRequested);

   // Deactivate custom context menu of scroll bar
   this->verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
   this->horizontalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Setup context menu entries
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeTebContextMenuBase::m_SetupContextMenu(const QPoint & orc_Pos)
{
   // reset to empty menu
   this->mpc_ContextMenu->clear();

   // add the actions
   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Copy"),
                                    this,
                                    &C_OgeTebContextMenuBase::copy,
                                    static_cast<int32_t>(Qt::CTRL) + static_cast<int32_t>(Qt::Key_C));

   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Copy Link Location"),
                                    this,
                                    &C_OgeTebContextMenuBase::m_CopyHyperLink);

   this->mpc_ContextMenu->addSeparator();

   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Select All"),
                                    this,
                                    &C_OgeTebContextMenuBase::selectAll,
                                    static_cast<int32_t>(Qt::CTRL) + static_cast<int32_t>(Qt::Key_A));

   // store link text for copying
   mc_LinkText = this->anchorAt(orc_Pos);

   // disable "Copy link location" if the context menu was not requested by clicking on a link
   if ((mc_LinkText == "") && (this->mpc_ContextMenu->actions().length() >= 2))
   {
      this->mpc_ContextMenu->actions().at(1)->setEnabled(false);
   }

   // disable "Copy" if selection is empty
   if ((this->textCursor().selectedText() == "") && (this->mpc_ContextMenu->actions().length() >= 1))
   {
      this->mpc_ContextMenu->actions().at(0)->setEnabled(false);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show custom context menu

   \param[in] orc_Pos Local context menu position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeTebContextMenuBase::m_OnCustomContextMenuRequested(const QPoint & orc_Pos)
{
   m_SetupContextMenu(orc_Pos);
   QPoint c_PosGlobal = this->mapToGlobal(orc_Pos);
   c_PosGlobal = c_PosGlobal + QPoint(8, 5); // little coordination correction of popup needed

   this->mpc_ContextMenu->popup(c_PosGlobal);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Copy link location to clipboard
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeTebContextMenuBase::m_CopyHyperLink(void) const
{
   // get link location
   const QString c_FilePath = mc_LinkText;

   if (c_FilePath != "")
   {
      // copy to clipboard
      QClipboard * const pc_Clipboard = QApplication::clipboard();

      if (pc_Clipboard != NULL)
      {
         //Copy to clipboard
         pc_Clipboard->clear();
         pc_Clipboard->setText(c_FilePath);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Open document on link click.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeTebContextMenuBase::m_LinkClicked(const QUrl & orc_Link) const
{
   QDesktopServices::openUrl(orc_Link);
}
