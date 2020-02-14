//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for title, about and help elements
 (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMTITLEBARWIDGET_H
#define C_CAMTITLEBARWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeWiOnlyBackground.h"
#include "C_CamOgeMuRecentProjects.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_CamTitleBarWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamTitleBarWidget :
   public stw_opensyde_gui_elements::C_OgeWiOnlyBackground
{
   Q_OBJECT

public:
   explicit C_CamTitleBarWidget(QWidget * const opc_Parent = NULL);
   ~C_CamTitleBarWidget(void);

   void InitStaticNames(void) const;
   void UpdateRecentProjectsAndWindowTitle(void);
   bool HandleProjectComparison(void);
   void SaveConfig(void);
   void SaveAsConfig(void);
   stw_types::sint32 LoadConfig(const QString & orc_FilePath);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigPrepareForSave(void);
   void SigSavedAsNew(void);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void resizeEvent(QResizeEvent * const opc_Event) override;
   //lint -restore

private:
   Ui::C_CamTitleBarWidget * mpc_Ui;
   stw_opensyde_gui_elements::C_CamOgeMuRecentProjects * mpc_Menu;
   static const QString mhc_FILTER;
   static const QString mhc_NORECENTPROJECT;

   //Avoid call
   C_CamTitleBarWidget(const C_CamTitleBarWidget &);
   C_CamTitleBarWidget & operator =(const C_CamTitleBarWidget &);

   void m_ShowAbout(void);
   void m_TriggerHelp(void);
   void m_NewConfig(void);
   void m_DoSaveToFileAction(const QString & orc_File);
   void m_OnOpenProjectClicked(void);
   void m_OnRecentProjectSelected(const QAction * const opc_Action);
   void m_RemapOnSaveAs(const QString & orc_NewFileName) const;
   void m_SetButtonsText(const bool oq_IconOnly) const;

   void m_OnAboutToShowMenu(void) const;
   void m_OnAboutToHideMenu(void) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
