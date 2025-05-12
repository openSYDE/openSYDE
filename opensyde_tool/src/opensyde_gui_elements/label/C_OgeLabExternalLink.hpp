//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Label with custom context menu (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELABEXTERNALLINK_HPP
#define C_OGELABEXTERNALLINK_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeContextMenu.hpp"
#include "C_OgeLabToolTipBase.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLabExternalLink :
   public C_OgeLabToolTipBase
{
   Q_OBJECT

public:
   explicit C_OgeLabExternalLink(QWidget * const opc_Parent = NULL);

   void SetLink(const QString & orc_DisplayedText, const QString & orc_LinkText);

private:
   stw::opensyde_gui_elements::C_OgeContextMenu * mpc_ContextMenu;
   QString mc_LinkText;
   void m_InitContextMenu();
   void m_SetupContextMenu();
   void m_OnCustomContextMenuRequested(const QPoint & orc_Pos);
   void m_CopyLinkLocation() const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
