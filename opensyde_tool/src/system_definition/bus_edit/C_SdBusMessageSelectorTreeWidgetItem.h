//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       short description (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUSMESSAGESELECTORTREEWIDGETITEM_H
#define C_SDBUSMESSAGESELECTORTREEWIDGETITEM_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QTreeWidgetItem>
#include <QIcon>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBusMessageSelectorTreeWidgetItem :
   public QTreeWidgetItem
{
public:
   C_SdBusMessageSelectorTreeWidgetItem(const bool & orq_IsMessage = false);

   void SetError(const bool & orq_Value);
   void SetAlwaysActive(const bool & orq_Value);

private:
   void m_UpdateIcon(void);

   const bool mq_IsMessage;
   bool mq_HasError;
   bool mq_IsAlwaysActive;

   QIcon mc_IconMessage;
   QIcon mc_IconMessageError;
   QIcon mc_IconMessageAlwaysActive;
   QIcon mc_IconMessageAlwaysActiveError;
   QIcon mc_IconSignal;
   QIcon mc_IconSignalError;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
