//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       One message receiver edit entry (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUEMESSAGERXENTRY_H
#define C_SDBUEMESSAGERXENTRY_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QIcon>
#include <QWidget>
#include "stwtypes.h"

#include "C_PuiSdNodeCanMessage.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdBueMessageRxEntry;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SdBueMessageRxEntry :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdBueMessageRxEntry(QWidget * const opc_Parent = NULL);
   ~C_SdBueMessageRxEntry(void);

   void InitStaticNames(void) const;
   void Init(const QString & orc_EntryName, const stw_types::uint32 ou32_NodeIndex,
             const stw_types::uint32 ou32_InterfaceIndex,
             const std::vector<stw_opensyde_gui_logic::C_PuiSdNodeCanMessage::E_RxTimeoutMode> & orc_ReceiveTimeoutModes, const std::vector<stw_types::uint32> & orc_ReceiveTimeoutValues, const std::vector<stw_types::uint32> & orc_DatapoolIndexes, const std::vector<QString> & orc_DatapoolNames, const bool oq_NodeLayer, const bool oq_ReadOnly);
   void SetLastKnownCycleTimeValue(const stw_types::uint32 ou32_Value);
   void SetRxTimeoutPreconditions(const bool oq_TxMethodOnEvent, const bool oq_DisableOptionPossible);
   void SetRxTimeoutConfigurationDisabled(const bool oq_DisableTimeoutConfiguration);
   void SetChecked(const stw_types::uint32 ou32_DatapoolIndex, const bool oq_Checked) const;
   void SetEnabled(const bool oq_Enabled) const;

   bool HasChildren(void) const;

   // Interface usable in case if mq_HasChildren == true
   bool DoesMatch(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_InterfaceIndex) const;
   void SetModeSingleNode(const bool oq_ModeSingleNode);

   // Interface usable in case if mq_HasChildren == false
   bool IsChecked(void) const;
   stw_types::uint32 GetDatapoolIndex(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigNodeToggled(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_InterfaceIndex,
                       const stw_types::uint32 ou32_DatapoolIndex, const bool oq_Checked);
   void SigNodeReceiveTimeoutMode(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_InterfaceIndex,
                                  const stw_types::uint32 ou32_DatapoolIndex,
                                  const stw_opensyde_gui_logic::C_PuiSdNodeCanMessage::E_RxTimeoutMode oe_ReceiveTimeoutMode);
   void SigNodeReceiveTimeout(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_InterfaceIndex,
                              const stw_types::uint32 ou32_DatapoolIndex, const stw_types::uint32 ou32_ReceiveTimeout);

private:
   Ui::C_SdBueMessageRxEntry * mpc_Ui;
   QIcon mc_NodeActive;
   QIcon mc_NodeInactive;
   QIcon mc_DatapoolActive;
   QIcon mc_DatapoolInactive;

   // mq_HasChildren  false: only one Datapool is handled and no sub items for multiple Datapools exists
   //                 true:  multiple Datapools are handled by sub items for each Datapools
   bool mq_HasChildren;
   // mq_NodeLayer: Flag if entry is on node layer or on Datapool layer
   bool mq_NodeLayer;
   bool mq_ModeSingleNode;

   stw_types::uint32 mu32_NodeIndex;
   stw_types::uint32 mu32_InterfaceIndex;

   // Not used in case of mq_HasChildren = true
   stw_types::uint32 mu32_DatapoolIndex;
   stw_types::uint32 mu32_LastKnownCycleTimeValue;
   stw_types::uint32 mu32_AutoReceiveTimeoutValue;
   stw_opensyde_gui_logic::C_PuiSdNodeCanMessage::E_RxTimeoutMode me_ReceiveTimeoutMode;
   stw_types::uint32 mu32_ReceiveTimeoutValue;

   // If entry has sub items (mq_HasChildren == true)
   std::vector<C_SdBueMessageRxEntry *> mc_Entries;

   bool mq_TxMethodOnEvent;
   bool mq_DisableOptionPossible;
   bool mq_DisableTimeoutConfiguration;

   void m_OnCheckBoxStateChanged(const stw_types::sintn osn_CheckState);
   void m_ToggleSubItems(const bool oq_Checked);
   void m_OnNodeDatapoolToggled(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_InterfaceIndex,
                                const stw_types::uint32 ou32_DatapoolIndex, const bool oq_Checked);
   void m_AdaptParentCheckBoxState(void) const;
   void m_ToggleIcon(const bool oq_Checked) const;
   void m_OnTimeoutConfigLinkClicked(void);
   void m_HandleInactiveStates(void) const;
   void m_UpdateAndAdaptRxTimeoutValue(void);
   void m_UpdateTimeoutLink(void) const;

   //Avoid call
   C_SdBueMessageRxEntry(const C_SdBueMessageRxEntry &);
   C_SdBueMessageRxEntry & operator =(const C_SdBueMessageRxEntry &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
