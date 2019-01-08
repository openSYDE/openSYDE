//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Help engine handler (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     03.11.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_HEHANDLER_H
#define C_HEHANDLER_H

/* -- Includes ------------------------------------------------------------- */
#include <windows.h>
#include <QWidget>
#include <QString>
#include <QMap>
#include "stwtypes.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */
typedef HWND (WINAPI * PR_HtmlHelp)(HWND, LPCSTR, UINT, DWORD);

class C_HeHandler
{
public:
   static C_HeHandler & GetInstance(void);
   void CallSpecificHelpPage(const QString & orc_ClassName);
   static bool CheckHelpKey(const QKeyEvent * const opc_KeyEvent);

private:
   C_HeHandler();
   virtual ~C_HeHandler();
   void InitSpecialHelpPages(void);
   static QString mh_GetHelpLocation(void);
   static C_HeHandler mhc_Instance;
   QMap<QString, QString> mc_LookUpHelpPageName;
   HINSTANCE mpv_InstHtmlHelp; ///< Pointer for DLL
   PR_HtmlHelp mpr_HtmlHelp;   ///< Function pointer for HtmlHelp
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
