//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Help engine handler (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_HEHANDLER_H
#define C_HEHANDLER_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <windows.h>
#include <QWidget>
#include <QString>
#include <QMap>
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
typedef HWND (WINAPI * PR_HtmlHelp)(HWND, LPCSTR, UINT, DWORD);

class C_HeHandler
{
public:
   static C_HeHandler & h_GetInstance(void);
   void CallSpecificHelpPage(const QString & orc_ClassName);
   static bool h_CheckHelpKey(const QKeyEvent * const opc_KeyEvent);
   void SetHelpFileRelPath(const QString & orc_RelPath);

private:
   C_HeHandler();
   virtual ~C_HeHandler();

   //Avoid call
   C_HeHandler(const C_HeHandler &);
   C_HeHandler & operator =(const C_HeHandler &);

   void m_InitSpecialHelpPages(void);
   QString m_GetHelpLocation(void) const;
   static C_HeHandler mhc_Instance;
   QMap<QString, QString> mc_LookUpHelpPageName;
   HINSTANCE mpv_InstHtmlHelp; ///< Pointer for DLL
   PR_HtmlHelp mpr_HtmlHelp;   ///< Function pointer for HtmlHelp
   QString mc_HelpFileRelPath;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
