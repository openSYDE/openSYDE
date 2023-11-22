//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Help engine handler (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_HEHANDLER_HPP
#define C_HEHANDLER_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <windows.h>
#include <QWidget>
#include <QString>
#include <QMap>
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

//lint -save -e8080 //using types expected by the library for API compatibility
typedef HWND (WINAPI * PR_HtmlHelp)(HWND opc_Caller, LPCSTR opx_File, UINT ox_Command, DWORD ox_Data);
//lint -restore

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
   C_HeHandler & operator =(const C_HeHandler &) &;

   void m_InitSpecialHelpPages(void);
   QString m_GetHelpLocation(void) const;
   static C_HeHandler mhc_Instance;
   QMap<QString, QString> mc_LookUpHelpPageName;
   HINSTANCE mpc_InstHtmlHelp; ///< Pointer for DLL
   PR_HtmlHelp mpr_HtmlHelp;   ///< Function pointer for HtmlHelp
   QString mc_HelpFileRelPath;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
