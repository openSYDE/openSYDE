//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for displaying and editing hardware configuration of a specific channel
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEHALCCHANNELWIDGET_HPP
#define C_SDNDEHALCCHANNELWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "stwtypes.hpp"

#include "C_OscHalcConfigDomain.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeHalcChannelWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeHalcChannelWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeHalcChannelWidget(QWidget * const opc_Parent = NULL);
   ~C_SdNdeHalcChannelWidget(void) override;

   void InitStaticNames(void) const;
   void SetNode(const uint32_t ou32_NodeIndex);
   void SetChannel(const uint32_t ou32_DomainIndex, const uint32_t ou32_ChannelIndex);
   void ReloadChannel(void);
   void SetNameFocus(void) const;
   void LoadUserSettings(void) const;
   void SaveUserSettings(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigUpdateChannel(const uint32_t ou32_DomainIndex, const uint32_t ou32_ChannelIndex,
                         const bool oq_UseChannelIndex);
   void SigChannelSelected(const uint32_t ou32_DomainIndex, const uint32_t ou32_ChannelIndex,
                           const bool oq_UseChannelIndex);

private:
   Ui::C_SdNdeHalcChannelWidget * mpc_Ui;

   uint32_t mu32_NodeIndex;
   uint32_t mu32_DomainIndex;
   uint32_t mu32_ChannelIndex;
   bool mq_UseChannelIndex;
   std::vector<uint32_t> mc_CbxUseCaseIndices;
   int32_t ms32_LastComboboxUseCaseIndex;

   //Avoid call
   C_SdNdeHalcChannelWidget(const C_SdNdeHalcChannelWidget &);
   C_SdNdeHalcChannelWidget & operator =(const C_SdNdeHalcChannelWidget &) &;

   void m_OnNameEdited(void);
   void m_OnCommentEdited(void);
   void m_OnSafetyToggled(const bool oq_Checked) const;
   void m_HandleHalcNvmFlag(void);
   void m_OnUseCaseChanged(const int32_t os32_NewIndex);
   void m_OnLinkedChannelClicked(const QString & orc_LinkedChannelName);
   void m_OnViewDatapoolDetailsClicked(void);
   void m_SetLinkedSymbolAndChannel(const bool oq_IsLinked,
                                    const std::vector<uint32_t> & orc_LinkedChannelIndices) const;
   void m_LoadChannelData(void);
   void m_EmitUpdateSignal(void);
   void m_ConnectWidgets(const bool oq_Connect) const;
   void m_CheckName(const QString & orc_NewName) const;
   bool m_AskUserToContinueLinkingIfNecessary(const bool oq_IsLinkedOld, const bool oq_IsLinkedNew,
                                              const std::vector<QString> & orc_LinkedChannelNamesOld, const
                                              std::vector<QString> & orc_LinkedChannelNamesNew,
                                              const uint32_t ou32_NewUseCaseIndex);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
