//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       One message receiver edit entry (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUEMESSAGERXENTRY_HPP
#define C_SDBUEMESSAGERXENTRY_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QIcon>
#include <QWidget>
#include <QRadioButton>
#include "stwtypes.hpp"

#include "C_PuiSdNodeCanMessage.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdBueMessageRxEntry;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SdBueMessageRxEntry :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdBueMessageRxEntry(QWidget * const opc_Parent = NULL);
   ~C_SdBueMessageRxEntry(void) override;

   void InitStaticNames(void) const;
   void Init(const QString & orc_EntryName, const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex,
             const std::vector<stw::opensyde_gui_logic::C_PuiSdNodeCanMessage::E_RxTimeoutMode> & orc_ReceiveTimeoutModes, const std::vector<uint32_t> & orc_ReceiveTimeoutValues, const std::vector<uint32_t> & orc_DatapoolIndexes, const std::vector<QString> & orc_DatapoolNames, const bool oq_NodeLayer, const bool oq_ReadOnly);
   void SetLastKnownCycleTimeValue(const uint32_t ou32_Value);
   void SetRxTimeoutPreconditions(const bool oq_TxMethodOnEvent, const bool oq_DisableOptionPossible);
   void SetRxTimeoutConfigurationReadOnly(const bool oq_TimeoutConfigurationReadOnly);
   void SetChecked(const bool oq_Checked, const bool oq_AllDatapoolIndexes, const uint32_t ou32_DatapoolIndex) const;
   void SetEnabled(const bool oq_Enabled) const;
   void SetSpecificToolTip(const QString & orc_Tooltip);
   void SetExclusiveMode(const bool oq_Active);

   QRadioButton * GetRadioButton(void) const;

   bool HasChildren(void) const;

   // Interface usable in case if mq_HasChildren == true
   bool DoesMatch(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex) const;
   void SetModeSingleNode(const bool oq_ModeSingleNode);

   // Interface usable in case if mq_HasChildren == false
   bool IsChecked(void) const;
   uint32_t GetDatapoolIndex(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigNodeToggled(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex,
                       const uint32_t ou32_DatapoolIndex, const bool oq_Checked);
   void SigNodeReceiveTimeoutMode(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex,
                                  const uint32_t ou32_DatapoolIndex,
                                  const stw::opensyde_gui_logic::C_PuiSdNodeCanMessage::E_RxTimeoutMode oe_ReceiveTimeoutMode);
   void SigNodeReceiveTimeout(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex,
                              const uint32_t ou32_DatapoolIndex, const uint32_t ou32_ReceiveTimeout);

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

   uint32_t mu32_NodeIndex;
   uint32_t mu32_InterfaceIndex;

   // Not used in case of mq_HasChildren = true
   uint32_t mu32_DatapoolIndex;
   uint32_t mu32_LastKnownCycleTimeValue;
   uint32_t mu32_AutoReceiveTimeoutValue;
   stw::opensyde_gui_logic::C_PuiSdNodeCanMessage::E_RxTimeoutMode me_ReceiveTimeoutMode;
   uint32_t mu32_ReceiveTimeoutValue;

   // If entry has sub items (mq_HasChildren == true)
   std::vector<C_SdBueMessageRxEntry *> mc_Entries;

   bool mq_TxMethodOnEvent;
   bool mq_DisableOptionPossible;
   bool mq_TimeoutConfigurationReadOnly;
   bool mq_ExclusiveMode;

   void m_OnCheckBoxStateChanged(const int32_t os32_CheckState);
   void m_OnRadioButtonStateChanged(const bool oq_Checked);
   void m_ToggleSubItems(const bool oq_Checked);
   void m_OnNodeDatapoolToggled(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex,
                                const uint32_t ou32_DatapoolIndex, const bool oq_Checked);
   void m_AdaptParentCheckBoxState(void) const;
   void m_ToggleIcon(const bool oq_Checked) const;
   void m_OnTimeoutConfigLinkClicked(void);
   void m_HandleInactiveStates(void) const;
   void m_UpdateAndAdaptRxTimeoutValue(void);
   void m_UpdateTimeoutLink(void) const;
   void m_HandleWidgetSize(const uint32_t ou32_NumEntries);

   //Avoid call
   C_SdBueMessageRxEntry(const C_SdBueMessageRxEntry &);
   C_SdBueMessageRxEntry & operator =(const C_SdBueMessageRxEntry &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
