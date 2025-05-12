//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Label with custom context menu (implementation)

   Label for external links with context menu in openSYDE style, only entry is "Copy link location".

   No color styling for this label as color is set by hyperlink (cf. C_Uti::h_GetLink())

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QApplication>
#include <QClipboard>

#include "C_OgeLabExternalLink.hpp"
#include "C_Uti.hpp"
#include "constants.hpp"
#include "stwtypes.hpp"
#include "C_GtGetText.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_elements;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeLabExternalLink::C_OgeLabExternalLink(QWidget * const opc_Parent) :
   C_OgeLabToolTipBase(opc_Parent),
   mpc_ContextMenu(NULL)
{
   m_InitContextMenu();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set text and link of this label

   \param[in]  orc_DisplayedText    Displayed text
   \param[in]  orc_LinkText         Link text
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLabExternalLink::SetLink(const QString & orc_DisplayedText, const QString & orc_LinkText)
{
   this->setText(C_Uti::h_GetLink(orc_DisplayedText, mc_STYLE_GUIDE_COLOR_6, orc_LinkText));
   this->mc_LinkText = orc_LinkText;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize custom context menu functionality
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLabExternalLink::m_InitContextMenu(void)
{
   this->setContextMenuPolicy(Qt::CustomContextMenu);
   this->mpc_ContextMenu = new C_OgeContextMenu(this);
   connect(this, &C_OgeLabExternalLink::customContextMenuRequested, this,
           &C_OgeLabExternalLink::m_OnCustomContextMenuRequested);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Setup context menu entries
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLabExternalLink::m_SetupContextMenu(void)
{
   // reset to empty menu
   this->mpc_ContextMenu->clear();

   // add the actions
   this->mpc_ContextMenu->addAction(C_GtGetText::h_GetText("Copy Link Location"),
                                    this, &C_OgeLabExternalLink::m_CopyLinkLocation);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show custom context menu

   \param[in]  orc_Pos  Local context menu position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLabExternalLink::m_OnCustomContextMenuRequested(const QPoint & orc_Pos)
{
   m_SetupContextMenu();

   const QPoint c_PosGlobal = this->mapToGlobal(orc_Pos);
   this->mpc_ContextMenu->popup(c_PosGlobal);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Copy the selected text to clipboard.

   QLabel has no copy action.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeLabExternalLink::m_CopyLinkLocation() const
{
   QClipboard * const pc_Clipboard = QApplication::clipboard();

   pc_Clipboard->setText(this->mc_LinkText);
}
