//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Handle clipboard load and save (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     24.07.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SYVCLIPBOARDHELPER_H
#define C_SYVCLIPBOARDHELPER_H

/* -- Includes ------------------------------------------------------------- */
#include "C_UtiClipBoardHelper.h"
#include "C_PuiSvDashboard.h"
#include "C_PuiSvReadDataConfiguration.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SyvClipBoardHelper :
   public C_UtiClipBoardHelper
{
public:
   static void h_StoreDashboardToClipboard(const C_PuiSvDashboard & orc_Data,
                                           const QMap<stw_opensyde_core::C_OSCNodeDataPoolListElementId,
                                                      C_PuiSvReadDataConfiguration> & orc_Rails,
                                           const QString & orc_GenericTagName);
   static stw_types::sint32 h_LoadDashboardFromClipboard(C_PuiSvDashboard & orc_Data,
                                                         const QString & orc_GenericTagName);
   static stw_types::sint32 h_LoadRailsFromClipboard(QMap<stw_opensyde_core::C_OSCNodeDataPoolListElementId,
                                                          stw_opensyde_gui_logic::C_PuiSvReadDataConfiguration> & orc_Rails, const QString & orc_GenericTagName);

private:
   C_SyvClipBoardHelper(void);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
