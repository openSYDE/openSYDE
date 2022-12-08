//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Message receiver edit list (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUEMESSAGERXLIST_H
#define C_SDBUEMESSAGERXLIST_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "stwtypes.hpp"
#include "C_SdBueMessageRxEntry.hpp"
#include "C_PuiSdNodeCanMessage.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdBueMessageRxList;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SdBueMessageRxList :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdBueMessageRxList(QWidget * const opc_Parent = NULL);
   ~C_SdBueMessageRxList(void) override;

   void InitStaticNames(void) const;
   void AddNodes(const std::vector<QString> & orc_EntryNames, const std::vector<QString> & orc_EntryDatapoolNames,
                 const std::vector<uint32_t> & orc_NodeIndexes, const std::vector<uint32_t> & orc_InterfaceIndexes,
                 const std::vector<uint32_t> & orc_DatapoolIndexes,
                 const std::vector<stw::opensyde_gui_logic::C_PuiSdNodeCanMessage::E_RxTimeoutMode> & orc_ReceiveTimeoutModes, const std::vector<uint32_t> & orc_ReceiveTimeoutValues, const bool oq_ReadOnly, const QString & orc_SpecificTooltip);
   void SetLastKnownCycleTimeValue(const uint32_t ou32_Value);
   void SetRxTimeoutPreconditions(const bool oq_TxMethodOnEvent, const bool oq_DisableOptionPossible);
   void SetRxTimeoutConfigurationReadOnly(const bool oq_TimeoutConfigurationReadOnly);
   void CheckNodes(const std::vector<uint32_t> & orc_NodeIndexes, const std::vector<uint32_t> & orc_InterfaceIndexes,
                   const std::vector<uint32_t> & orc_DatapoolIndexes) const;
   void CheckSpecificNode(const uint32_t ou32_NodeIndex, const uint32_t ou32_InterfaceIndex,
                          const uint32_t ou32_DatapoolIndex, const bool oq_Checked) const;
   void SetModeSingleNode(const bool oq_ModeSingleNode);

   void Clear(void);

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
   Ui::C_SdBueMessageRxList * mpc_Ui;
   std::vector<C_SdBueMessageRxEntry *> mc_Entries;
   uint32_t mu32_LastKnownCycleTimeValue;
   bool mq_TxMethodOnEvent;
   bool mq_DisableOptionPossible;
   bool mq_TimeoutConfigurationReadOnly;
   bool mq_ModeSingleNode;

   //Avoid call
   C_SdBueMessageRxList(const C_SdBueMessageRxList &);
   C_SdBueMessageRxList & operator =(const C_SdBueMessageRxList &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
