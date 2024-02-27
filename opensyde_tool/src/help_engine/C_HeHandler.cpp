//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Help engine handler (implementation)

   Help engine handler

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

//Html help
#include <windows.h>
#include <htmlhelp.h>
#include <QProcess>
#include <QKeyEvent>
#include <iostream>
#include "C_HeHandler.hpp"
#include "TglUtils.hpp"
#include "C_Uti.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */
C_HeHandler C_HeHandler::mhc_Instance;

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current help engine instance

   \return
   Current help engine instance
*/
//----------------------------------------------------------------------------------------------------------------------
C_HeHandler & C_HeHandler::h_GetInstance(void)
{
   return mhc_Instance;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Call help for current widget

   \param[in]  orc_ClassName   Class name
*/
//----------------------------------------------------------------------------------------------------------------------
void C_HeHandler::CallSpecificHelpPage(const QString & orc_ClassName)
{
   QString c_PageSearchName = "";

   const QString c_PageName = orc_ClassName;

   //Debug helper: Which class calls this function?
   //std::cout << c_PageName.toStdString().c_str() << &std::endl;
   const QMap<QString, QString>::const_iterator c_Page = this->mc_LookUpHelpPageName.find(c_PageName);

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
         this->mpr_HtmlHelp(GetDesktopWindow(), this->m_GetHelpLocation().toStdString().c_str(), HH_DISPLAY_TOPIC, 0);
      }
      else
      {
         char_t * const pcn_Text = new char_t[static_cast<uint32_t>(c_PageSearchName.count() + 1)];
         HH_AKLINK c_Link;
         for (int32_t s32_It = 0; s32_It < c_PageSearchName.count(); ++s32_It)
         {
            pcn_Text[static_cast<uint32_t>(s32_It)] = c_PageSearchName.at(s32_It).toLatin1();
         }
         pcn_Text[static_cast<uint32_t>(c_PageSearchName.count())] = '\0';
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
         //lint -e{923,1924,9091} Required by API interface
         this->mpr_HtmlHelp(GetDesktopWindow(),
                            this->m_GetHelpLocation().toStdString().c_str(), HH_KEYWORD_LOOKUP, (DWORD) &c_Link);
         delete[] (pcn_Text);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check help key press

   \param[in] opc_KeyEvent Key event information

   \return
   true: Help key press detected
   false: No help key press detected
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_HeHandler::h_CheckHelpKey(const QKeyEvent * const opc_KeyEvent)
{
   bool q_Retval;

   if (static_cast<Qt::Key>(opc_KeyEvent->key()) == Qt::Key_F1)
   {
      q_Retval = true;
   }
   else
   {
      q_Retval = false;
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check help key press

   \param[in] orc_RelPath    Relative path to help file
*/
//----------------------------------------------------------------------------------------------------------------------
void C_HeHandler::SetHelpFileRelPath(const QString & orc_RelPath)
{
   this->mc_HelpFileRelPath = orc_RelPath;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_HeHandler::C_HeHandler() :
   //set default path
   mc_HelpFileRelPath("/Help/openSYDE PC Tool.chm")
{
   m_InitSpecialHelpPages();

   //Load DLL
   this->mpc_InstHtmlHelp = LoadLibraryA("HHCtrl.ocx");
   if (this->mpc_InstHtmlHelp != NULL)
   {
      //Load function
      //lint -e{740,929,1924,9176} Working code
      (FARPROC &)mpr_HtmlHelp = GetProcAddress(this->mpc_InstHtmlHelp, "HtmlHelpA");
   }
   else
   {
      this->mpr_HtmlHelp = NULL;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  DLL freed by function call
C_HeHandler::~C_HeHandler()
{
   //Free DLL
   if (this->mpc_InstHtmlHelp != NULL)
   {
      FreeLibrary(this->mpc_InstHtmlHelp);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Init static map for specific help pages
*/
//----------------------------------------------------------------------------------------------------------------------
void C_HeHandler::m_InitSpecialHelpPages(void)
{
   //System Definition

   //Network Topology
   this->mc_LookUpHelpPageName.insert("stw::opensyde_gui::C_SdHandlerWidget", "System Definition");
   //Node Edit
   this->mc_LookUpHelpPageName.insert("stw::opensyde_gui::C_SdNdeNodeEditWidget", "Node Properties");
   this->mc_LookUpHelpPageName.insert("stw::opensyde_gui::C_SdNdeDbViewWidget", "Datapool");
   this->mc_LookUpHelpPageName.insert("stw::opensyde_gui::C_SdNdeDbProperties", "Data Block Properties");
   this->mc_LookUpHelpPageName.insert("stw::opensyde_gui::C_SdNdeHalcChannelWidget", "Hardware Configurator");

   //Bus Edit
   this->mc_LookUpHelpPageName.insert("stw::opensyde_gui::C_SdBueBusEditWidget", "Bus Properties");
   this->mc_LookUpHelpPageName.insert("stw::opensyde_gui::C_SdBueComIfDescriptionWidget", "Message Properties");
   //Pop-Ups
   this->mc_LookUpHelpPageName.insert("stw::opensyde_gui::C_SdNodeComIfSetupWidget", "Communication Interface Setup");
   this->mc_LookUpHelpPageName.insert("stw::opensyde_gui::C_SdNodeToNodeConnectionSetupWidget",
                                      "Node to Node - Connection Setup");
   this->mc_LookUpHelpPageName.insert("stw::opensyde_gui::C_SdNdeDpProperties", "Datapool Properties");
   this->mc_LookUpHelpPageName.insert("stw::opensyde_gui::C_SdNdeDpListDataSetWidget", "Datasets");
   this->mc_LookUpHelpPageName.insert("stw::opensyde_gui::C_SdNdeProgrammingOptions", "File Generation - Settings");

   //System Commissioning
   this->mc_LookUpHelpPageName.insert("stw::opensyde_gui::C_SyvSeSetupWidget", "Setup");
   this->mc_LookUpHelpPageName.insert("stw::opensyde_gui::C_SyvUpUpdateWidget", "Update");
   this->mc_LookUpHelpPageName.insert("stw::opensyde_gui::C_SyvDaDashboardsWidget", "Dashboards");
   //Pop-Ups
   this->mc_LookUpHelpPageName.insert("stw::opensyde_gui::C_SyvDaPeBase", "Widget Properties");
   this->mc_LookUpHelpPageName.insert("stw::opensyde_gui::C_SyvDaPeUpdateModeConfiguration",
                                      "Dashboards Configuration");
   this->mc_LookUpHelpPageName.insert("stw::opensyde_gui::C_SyvDaItPaWriteWidget",
                                      "Parametrization - Write Values to Device");
   this->mc_LookUpHelpPageName.insert("stw::opensyde_gui::C_SyvDcWidget", "Configure Device");

   //openSYDE CAN Monitor
   this->mc_LookUpHelpPageName.insert("stw::opensyde_gui::C_CamMainWindow", "openSYDE CAN Monitor");
   this->mc_LookUpHelpPageName.insert("stw::opensyde_gui::C_CamMosFilterPopup", "openSYDE CAN Monitor");
   this->mc_LookUpHelpPageName.insert("stw::opensyde_gui::C_CamTitleBarWidget", "openSYDE CAN Monitor");

   //SYDEflash
   this->mc_LookUpHelpPageName.insert("stw::opensyde_gui::C_FlaMainWindow", "SYDEflash");
   this->mc_LookUpHelpPageName.insert("stw::opensyde_gui::C_FlaConNodeConfigPopup", "SYDEflash");
   this->mc_LookUpHelpPageName.insert("stw::opensyde_gui::C_FlaSenSearchNodePopup", "SYDEflash");
   this->mc_LookUpHelpPageName.insert("stw::opensyde_gui::C_NagAboutDialog", "SYDEflash");
   this->mc_LookUpHelpPageName.insert("stw::opensyde_gui::C_FlaTitleBarWidget", "SYDEflash");
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current help location
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_HeHandler::m_GetHelpLocation(void) const
{
   return C_Uti::h_GetExePath() + mc_HelpFileRelPath;
}
