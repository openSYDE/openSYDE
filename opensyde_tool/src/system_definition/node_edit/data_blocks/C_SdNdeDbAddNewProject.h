//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for new project import/creation (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDBADDNEWPROJECT_H
#define C_SDNDEDBADDNEWPROJECT_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "C_OgePopUpDialog.h"
#include "C_OSCNodeApplication.h"
#include "C_OSCTargetSupportPackage.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeDbAddNewProject;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDbAddNewProject :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDbAddNewProject(const stw_types::uint32 ou32_NodeIndex,
                                   stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent);
   ~C_SdNdeDbAddNewProject(void);

   void InitStaticNames(void) const;
   QString GetTSPPath(void) const;
   void SetTSPPath(const QString & orc_New);
   stw_types::uint32 GetTSPApplicationCount(void) const;
   void AddSelectedProject(const stw_types::uint32 ou32_TSPIndex,
                           stw_opensyde_core::C_OSCNodeApplication & orc_Application, QString & orc_Warnings) const;
   void HandleCodeGenerationConfig(void) const;

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   //lint -restore

private:
   Ui::C_SdNdeDbAddNewProject * mpc_Ui;
   stw_types::sint32 ms32_TSPReadResult;
   const stw_types::uint32 mu32_NodeIndex;
   stw_opensyde_core::C_OSCTargetSupportPackage mc_Package;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   static const QString mhc_StartTD;
   static const QString mhc_ContinueTD;

   void m_OkClicked(void);
   void m_CancelClicked(void);
   void m_TSPButtonClicked(void);
   void m_CreateInButtonClicked(void);
   void m_OnLoadTSP(void);
   void m_AddTopSection(QString & orc_Content) const;
   void m_AddTemplateSection(QString & orc_Content) const;
   void m_Init(const stw_types::uint32 ou32_NodeIndex) const;

   //Avoid call
   C_SdNdeDbAddNewProject(const C_SdNdeDbAddNewProject &);
   C_SdNdeDbAddNewProject & operator =(const C_SdNdeDbAddNewProject &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
