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
#include "C_OgePopUpDialog.hpp"

#include "C_OscHalcConfig.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeHalcDefUpdateDialog;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeHalcDefUpdateDialog :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeHalcDefUpdateDialog(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                       const uint32_t ou32_NodeIndex,
                                       const stw::opensyde_core::C_OscHalcConfig & orc_LoadedHalcConfig,
                                       const QString & orc_FilePath);
   ~C_SdNdeHalcDefUpdateDialog(void) noexcept override;

   void InitStaticNames(void) const;

   int32_t GetResult(stw::opensyde_core::C_OscHalcConfig & orc_UpdatedHalcConfig, QString & orc_ErrorDetails) const;

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_SdNdeHalcDefUpdateDialog * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;

   const uint32_t mu32_NodeIndex;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   const stw::opensyde_core::C_OscHalcConfig & mrc_LoadedHalcConfig;

   stw::opensyde_core::C_OscHalcConfig mc_UpdatedHalcConfig;
   int32_t ms32_Result;
   QStringList mc_NewContent;
   QStringList mc_ResetContent;
   QStringList mc_RemovedContent;

   void m_OkClicked(void);
   void m_OnCancel(void);

   void m_CreateReport(const QString & orc_FilePath) const;
   QString m_AddSection(const QString & orc_SectionName, const QStringList & orc_Content) const;
   void m_UpdateHalcConfiguration(void);
   void m_UpdateDomainConfiguration(const stw::opensyde_core::C_OscHalcConfigDomain & orc_CurrentConfig,
                                    stw::opensyde_core::C_OscHalcConfigDomain & orc_UpdatedConfig);
   void m_UpdateChannelConfiguration(const stw::opensyde_core::C_OscHalcConfigDomain & orc_CurrentConfig,
                                     stw::opensyde_core::C_OscHalcConfigDomain & orc_UpdatedConfig,
                                     const stw::scl::C_SclString & orc_PreviousDefChannelName,
                                     const bool oq_IsDomainConfiguration, const uint32_t ou32_CurrentChannelIndex = 0U,
                                     const uint32_t ou32_UpdatedChannelIndex = 0U);
   void m_UpdateSubParameterConfiguration(const stw::opensyde_core::C_OscHalcDefStruct & orc_CurrentParameterDef,
                                          const stw::opensyde_core::C_OscHalcDefStruct & orc_UpdatedParameterDef,
                                          const bool oq_IsDomainConfiguration, const uint32_t ou32_UseCaseIndex,
                                          const stw::opensyde_core::C_OscHalcConfigParameterStruct & orc_CurrentParameterConfig, stw::opensyde_core::C_OscHalcConfigParameterStruct &
                                          orc_UpdatedParameterConfig, const stw::scl::C_SclString & orc_ChannelName);
   void m_UpdateParameterElementConfiguration(const stw::opensyde_core::C_OscHalcDefElement & orc_CurrentParameterDef,
                                              const stw::opensyde_core::C_OscHalcDefElement & orc_UpdatedParameterDef,
                                              const bool oq_IsDomainConfiguration, const uint32_t ou32_UseCaseIndex,
                                              const stw::opensyde_core::C_OscHalcConfigParameter & orc_CurrentParameterConfig, stw::opensyde_core::C_OscHalcConfigParameter &
                                              orc_UpdatedParameterConfig, const stw::scl::C_SclString & orc_ChannelName);
   static bool mh_CheckParameterDefForUseCase(const stw::opensyde_core::C_OscHalcDefElement & orc_ParameterDef,
                                              const bool oq_IsDomainConfiguration, const uint32_t ou32_UseCaseIndex);

   //Avoid call
   C_SdNdeHalcDefUpdateDialog(const C_SdNdeHalcDefUpdateDialog &);
   C_SdNdeHalcDefUpdateDialog & operator =(const C_SdNdeHalcDefUpdateDialog &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
