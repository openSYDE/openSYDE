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
#include "stwtypes.h"
#include "C_SdBueMessageRxEntry.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdBueMessageRxList;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */
class C_SdBueMessageRxList :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdBueMessageRxList(QWidget * const opc_Parent = NULL);
   ~C_SdBueMessageRxList(void);

   void InitStaticNames(void) const;
   void AddNodes(const std::vector<QString> & orc_EntryNames, const std::vector<stw_types::uint32> & orc_NodeIndexes,
                 const std::vector<stw_types::uint32> & orc_InterfaceIndexes,
                 const std::vector<bool> & orc_UseAutoReceiveTimeoutFlags,
                 const std::vector<stw_types::uint32> & orc_ReceiveTimeoutValues);
   void SetLastKnownCycleTimeValue(const stw_types::uint32 ou32_Value);
   void SetAlwaysHideTimeout(const bool oq_Hide);
   void CheckNodes(const std::vector<stw_types::uint32> & orc_NodeIndexes,
                   const std::vector<stw_types::uint32> & orc_InterfaceIndexes) const;
   void CheckSpecificNode(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_InterfaceIndex,
                          const bool oq_Checked) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigNodeToggled(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_InterfaceIndex,
                       const bool oq_Checked);
   void SigNodeUseReceiveTimeout(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_InterfaceIndex,
                                 const bool oq_UseReceiveTimeout);
   void SigNodeReceiveTimeout(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_InterfaceIndex,
                              const stw_types::uint32 ou32_ReceiveTimeout);

private:
   Ui::C_SdBueMessageRxList * mpc_Ui;
   std::vector<C_SdBueMessageRxEntry *> mc_Entries;
   stw_types::uint32 mu32_LastKnownCycleTimeValue;
   bool mq_AlwaysHide;

   void m_Clear(void);
   //Avoid call
   C_SdBueMessageRxList(const C_SdBueMessageRxList &);
   C_SdBueMessageRxList & operator =(const C_SdBueMessageRxList &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
