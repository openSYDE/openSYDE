//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Help engine handler (implementation)

   Help engine handler

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     03.11.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

//Html help
#include <windows.h>
#include <htmlhelp.h>
#include <QProcess>
#include <QKeyEvent>
#include <iostream>
#include "C_HeHandler.h"
#include "TGLUtils.h"
#include "C_Uti.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_tgl;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */
C_HeHandler C_HeHandler::mhc_Instance;

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Get current help engine instance

   \return
   Current help engine instance

   \created     03.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_HeHandler & C_HeHandler::GetInstance(void)
{
   return mhc_Instance;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Call help for current widget

   \param[in] opc_Widget Current widget

   \created     03.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_HeHandler::CallSpecificHelpPage(const QString & orc_ClassName)
{
   QString c_PageSearchName = "";

   const QString c_PageName = orc_ClassName;

   //Debug helper
   //std::cout << c_PageName.toStdString().c_str() << std::endl;
   QMap<QString, QString>::const_iterator c_Page = this->mc_LookUpHelpPageName.find(c_PageName);
   if (c_Page != this->mc_LookUpHelpPageName.end())
   {
      //Html help
      c_PageSearchName = c_Page.value();
   }

   //Html help
   tgl_assert(this->mpr_HtmlHelp != NULL);
   if (this->mpr_HtmlHelp != NULL)
   {
      if (c_PageSearchName.compare("") == 0)
      {
         this->mpr_HtmlHelp(GetDesktopWindow(), mh_GetHelpLocation().toStdString().c_str(), HH_DISPLAY_TOPIC, 0);
      }
      else
      {
         //lint -e{1960} All conversions as required by interface
         charn * const pcn_Text = new charn[static_cast<uint32>(c_PageSearchName.count() + 1)];
         HH_AKLINK c_Link;
         for (sint32 s32_It = 0; s32_It < c_PageSearchName.count(); ++s32_It)
         {
            pcn_Text[static_cast<uint32>(s32_It)] = c_PageSearchName.at(s32_It).toLatin1();
         }
         pcn_Text[static_cast<uint32>(c_PageSearchName.count())] = '\0';
         c_Link.cbStruct =     sizeof(HH_AKLINK);
         c_Link.fReserved =    FALSE;
         //Cast char * to wchar * but still interpreted as char * for some reason ...
         //lint -e{926,1924} Required by API interface
         c_Link.pszKeywords =  (LPCTSTR)pcn_Text;
         c_Link.pszUrl =       NULL;
         c_Link.pszMsgText =   NULL;
         c_Link.pszMsgTitle =  NULL;
         c_Link.pszWindow =    NULL;
         c_Link.fIndexOnFail = TRUE;
         //this->mpr_HtmlHelp(GetDesktopWindow(), mh_GetHelpLocation().toStdString().c_str(), HH_DISPLAY_TOPIC, 0);
         //lint -e{923,1924} Required by API interface
         this->mpr_HtmlHelp(GetDesktopWindow(),
                            mh_GetHelpLocation().toStdString().c_str(), HH_KEYWORD_LOOKUP, (DWORD) &c_Link);
         delete[] (pcn_Text);
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Check help key press

   \param[in] opc_KeyEvent Key event information

   \return
   true: Help key press detected
   false: No help key press detected

   \created     04.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_HeHandler::CheckHelpKey(const QKeyEvent * const opc_KeyEvent)
{
   bool q_Retval;

   switch (opc_KeyEvent->key())
   {
   case Qt::Key_F1:
      q_Retval = true;
      break;
   default:
      q_Retval = false;
      break;
   }
   return q_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     03.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_HeHandler::C_HeHandler()
{
   InitSpecialHelpPages();

   //Load DLL
   this->mpv_InstHtmlHelp = LoadLibraryA("HHCtrl.ocx");
   if (this->mpv_InstHtmlHelp != NULL)
   {
      //Load function
      //lint -e{740,929,1924} Working code
      (FARPROC &)mpr_HtmlHelp = GetProcAddress(this->mpv_InstHtmlHelp, "HtmlHelpA");
   }
   else
   {
      this->mpr_HtmlHelp = NULL;
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   \created     03.11.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_HeHandler::~C_HeHandler()
{
   //Free DLL
   if (this->mpv_InstHtmlHelp != NULL)
   {
      FreeLibrary(this->mpv_InstHtmlHelp);
   }
   //lint -e{1540}  DLL freed by function call
}

//-----------------------------------------------------------------------------
/*!
   \brief   Init static map for specific help pages

   \created     29.10.2018  STW/S.Singer
*/
//-----------------------------------------------------------------------------
void C_HeHandler::InitSpecialHelpPages(void)
{
   //System Definition

   //Network Topology
   this->mc_LookUpHelpPageName.insert("stw_opensyde_gui::C_SdHandlerWidget", "System Definition");
   //Node Edit
   this->mc_LookUpHelpPageName.insert("stw_opensyde_gui::C_SdNdeNodeEditWidget", "Node Properties");
   this->mc_LookUpHelpPageName.insert("stw_opensyde_gui::C_SdNdeDbViewWidget", "Datapool");
   this->mc_LookUpHelpPageName.insert("stw_opensyde_gui::C_SdNdeDbProperties", "Data Block Properties");

   //Bus Edit
   this->mc_LookUpHelpPageName.insert("stw_opensyde_gui::C_SdBueBusEditWidget", "Bus Properties");
   this->mc_LookUpHelpPageName.insert("stw_opensyde_gui::C_SdBueComIfDescriptionWidget", "Message Properties");
   //Pop-Ups
   this->mc_LookUpHelpPageName.insert("stw_opensyde_gui::C_SdNodeComIfSetupWidget", "Communication Interface Setup");
   this->mc_LookUpHelpPageName.insert("stw_opensyde_gui::C_SdNodeToNodeConnectionSetupWidget",
                                      "Node to Node - Connection Setup");
   this->mc_LookUpHelpPageName.insert("stw_opensyde_gui::C_SdNdeDatapoolProperties", "Datapool Properties");
   this->mc_LookUpHelpPageName.insert("stw_opensyde_gui::C_SdNdeDataPoolListDataSetWidget", "Datasets");

   //System Commissioning
   this->mc_LookUpHelpPageName.insert("stw_opensyde_gui::C_SyvSeSetupWidget", "Setup");
   this->mc_LookUpHelpPageName.insert("stw_opensyde_gui::C_SyvUpUpdateWidget", "Update");
   this->mc_LookUpHelpPageName.insert("stw_opensyde_gui::C_SyvDaDashboardsWidget", "Dashboards");
   //Pop-Ups
   this->mc_LookUpHelpPageName.insert("stw_opensyde_gui::C_SyvDaPeBase", "Widget Properties");
   this->mc_LookUpHelpPageName.insert("stw_opensyde_gui::C_SyvDaPeUpdateModeConfiguration", "Dashboards Configuration");
   this->mc_LookUpHelpPageName.insert("stw_opensyde_gui::C_SyvDaItPaWriteWidget",
                                      "Parametrization - Write Values to Device");
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get current help location

   \created     16.05.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QString C_HeHandler::mh_GetHelpLocation(void)
{
   return C_Uti::h_GetExePath() + "/Help/openSYDE PC Tool.chm";
}