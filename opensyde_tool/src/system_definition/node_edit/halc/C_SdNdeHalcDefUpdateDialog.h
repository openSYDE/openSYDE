//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Dialog for updating an existing HALC definition
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEHALCDEFUPDATEDIALOG_H
#define C_SDNDEHALCDEFUPDATEDIALOG_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "C_OgePopUpDialog.h"

#include "C_OSCHalcConfig.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeHalcDefUpdateDialog;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeHalcDefUpdateDialog :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeHalcDefUpdateDialog(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                       const stw_types::uint32 ou32_NodeIndex,
                                       const stw_opensyde_core::C_OSCHalcConfig & orc_LoadedHalcConfig,
                                       const QString & orc_FilePath);
   ~C_SdNdeHalcDefUpdateDialog(void);

   void InitStaticNames(void) const;

   stw_types::sint32 GetResult(stw_opensyde_core::C_OSCHalcConfig & orc_UpdatedHalcConfig,
                               QString & orc_ErrorDetails) const;

protected:
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_SdNdeHalcDefUpdateDialog * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;

   const stw_types::uint32 mu32_NodeIndex;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   const stw_opensyde_core::C_OSCHalcConfig & mrc_LoadedHalcConfig;

   stw_opensyde_core::C_OSCHalcConfig mc_UpdatedHalcConfig;
   stw_types::sint32 ms32_Result;
   QStringList mc_NewContent;
   QStringList mc_ResetContent;
   QStringList mc_RemovedContent;

   void m_OkClicked(void);
   void m_OnCancel(void);

   void m_CreateReport(const QString & orc_FilePath) const;
   QString m_AddSection(const QString & orc_SectionName, const QStringList & orc_Content) const;
   void m_UpdateHalcConfiguration(void);
   void m_UpdateDomainConfiguration(const stw_opensyde_core::C_OSCHalcConfigDomain & orc_CurrentConfig,
                                    stw_opensyde_core::C_OSCHalcConfigDomain & orc_UpdatedConfig);
   void m_UpdateChannelConfiguration(const stw_opensyde_core::C_OSCHalcConfigDomain & orc_CurrentConfig,
                                     stw_opensyde_core::C_OSCHalcConfigDomain & orc_UpdatedConfig,
                                     const stw_scl::C_SCLString & orc_PreviousDefChannelName,
                                     const bool oq_IsDomainConfiguration,
                                     const stw_types::uint32 ou32_CurrentChannelIndex = 0U,
                                     const stw_types::uint32 ou32_UpdatedChannelIndex = 0U);
   void m_UpdateSubParameterConfiguration(const stw_opensyde_core::C_OSCHalcDefStruct & orc_CurrentParameterDef,
                                          const stw_opensyde_core::C_OSCHalcDefStruct & orc_UpdatedParameterDef,
                                          const bool oq_IsDomainConfiguration,
                                          const stw_types::uint32 ou32_UseCaseIndex,
                                          const stw_opensyde_core::C_OSCHalcConfigParameterStruct & orc_CurrentParameterConfig, stw_opensyde_core::C_OSCHalcConfigParameterStruct &
                                          orc_UpdatedParameterConfig, const stw_scl::C_SCLString & orc_ChannelName);
   void m_UpdateParameterElementConfiguration(const stw_opensyde_core::C_OSCHalcDefElement & orc_CurrentParameterDef,
                                              const stw_opensyde_core::C_OSCHalcDefElement & orc_UpdatedParameterDef,
                                              const bool oq_IsDomainConfiguration,
                                              const stw_types::uint32 ou32_UseCaseIndex,
                                              const stw_opensyde_core::C_OSCHalcConfigParameter & orc_CurrentParameterConfig, stw_opensyde_core::C_OSCHalcConfigParameter &
                                              orc_UpdatedParameterConfig, const stw_scl::C_SCLString & orc_ChannelName);
   static bool mh_CheckParameterDefForUseCase(const stw_opensyde_core::C_OSCHalcDefElement & orc_ParameterDef,
                                              const bool oq_IsDomainConfiguration,
                                              const stw_types::uint32 ou32_UseCaseIndex);

   //Avoid call
   C_SdNdeHalcDefUpdateDialog(const C_SdNdeHalcDefUpdateDialog &);
   C_SdNdeHalcDefUpdateDialog & operator =(const C_SdNdeHalcDefUpdateDialog &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
