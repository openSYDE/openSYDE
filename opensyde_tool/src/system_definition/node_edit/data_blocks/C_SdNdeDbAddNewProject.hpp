//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for new project import/creation (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDBADDNEWPROJECT_HPP
#define C_SDNDEDBADDNEWPROJECT_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "C_OgePopUpDialog.hpp"
#include "C_OscNodeApplication.hpp"
#include "C_OscTargetSupportPackageV2.hpp"
#include "C_OscNode.hpp"
#include "C_PuiSdNode.hpp"
#include "C_OscTargetSupportPackage.hpp"
#include "C_PuiSdNode.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeDbAddNewProject;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDbAddNewProject :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDbAddNewProject(const uint32_t ou32_NodeIndex,
                                   stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                   const bool oq_IsCurrentNodeNew);
   ~C_SdNdeDbAddNewProject(void) noexcept override;

   void InitStaticNames(void) const;
   QString GetTspPath(void) const;
   void SetTspPath(const QString & orc_New);
   uint32_t GetTspApplicationCount(void) const;
   void AddSelectedProject(const uint32_t ou32_TspIndex, stw::opensyde_core::C_OscNodeApplication & orc_Application,
                           QString & orc_Warnings) const;
   void HandleCodeGenerationConfig(void) const;
   QString GetHalcDefinitionFileName(void);
   QString GetProcessedHalcDefinitionPath(void);
   void ApplyV3Content(void);
   bool q_IsVersion3;

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_SdNdeDbAddNewProject * mpc_Ui;
   int32_t ms32_TspReadResult;
   const uint32_t mu32_NodeIndex;
   stw::opensyde_core::C_OscTargetSupportPackageV2 mc_Package;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_core::C_OscTargetSupportPackage mc_Tsp;
   stw::opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   static const QString mhc_START_TD;
   static const QString mhc_CONTINUE_TD;
   static const QString mhc_SUFFIX;
   stw::opensyde_core::C_OscNode mc_OscNode;
   stw::opensyde_gui_logic::C_PuiSdNode mc_UiNode;
   bool mq_IsCurrentNodeNew;

   void m_OkClicked(void);
   void m_CancelClicked(void);
   void m_TspButtonClicked(void);
   void m_CreateInButtonClicked(void);
   void m_OnDroppedCreatinPath(void);
   void m_SetCreateInPath(const QString & orc_New);
   void m_OnLoadTsp(void);
   void m_AddTopSection(QString & orc_Content) const;
   void m_AddTemplateSection(QString & orc_Content) const;
   void m_AddV3TopSection(QString & orc_Content) const;
   void m_AddV3TemplateSection(QString & orc_Content) const;
   void m_Init(const uint32_t ou32_NodeIndex) const;
   static void mh_KeepTspProperties(const opensyde_core::C_OscNode & orc_PreviousCoreNode,
                                    stw::opensyde_core::C_OscNode & orc_NewCoreNode,
                                    const stw::opensyde_gui_logic::C_PuiSdNode & orc_PreviousUiNode,
                                    stw::opensyde_gui_logic::C_PuiSdNode & orc_NewUiNode);
   void m_ApplyV2PathAdaptationToV3(void);

   //Avoid call
   C_SdNdeDbAddNewProject(const C_SdNdeDbAddNewProject &);
   C_SdNdeDbAddNewProject & operator =(const C_SdNdeDbAddNewProject &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
