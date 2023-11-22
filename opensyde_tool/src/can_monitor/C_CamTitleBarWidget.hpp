//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for title, about and help elements
 (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMTITLEBARWIDGET_HPP
#define C_CAMTITLEBARWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeWiOnlyBackground.hpp"
#include "C_CamOgeMuRecentProjects.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_CamTitleBarWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamTitleBarWidget :
   public stw::opensyde_gui_elements::C_OgeWiOnlyBackground
{
   Q_OBJECT

public:
   explicit C_CamTitleBarWidget(QWidget * const opc_Parent = NULL);
   ~C_CamTitleBarWidget(void) override;

   void InitStaticNames(void) const;
   void UpdateRecentProjectsAndWindowTitle(void);
   bool HandleProjectComparison(void);
   bool SaveConfig(void);
   bool SaveAsConfig(void);
   int32_t LoadConfig(const QString & orc_FilePath);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigPrepareForSave(void);
   void SigSavedAsNew(void);

protected:
   void resizeEvent(QResizeEvent * const opc_Event) override;

private:
   Ui::C_CamTitleBarWidget * mpc_Ui;
   stw::opensyde_gui_elements::C_CamOgeMuRecentProjects * mpc_Menu;
   static const QString mhc_FILTER;
   static const QString mhc_NORECENTPROJECT;

   //Avoid call
   C_CamTitleBarWidget(const C_CamTitleBarWidget &);
   C_CamTitleBarWidget & operator =(const C_CamTitleBarWidget &) &;

   void m_ShowAbout(void);
   void m_TriggerHelp(void);
   void m_NewConfig(void);
   bool m_DoSaveToFileAction(const QString & orc_File);
   void m_OnOpenProjectClicked(void);
   void m_OnRecentProjectSelected(const QAction * const opc_Action);
   void m_RemapOnSaveAs(const QString & orc_NewFileName) const;
   void m_SetButtonsText(const bool oq_IconOnly) const;

   void m_OnAboutToShowMenu(void) const;
   void m_OnAboutToHideMenu(void) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
