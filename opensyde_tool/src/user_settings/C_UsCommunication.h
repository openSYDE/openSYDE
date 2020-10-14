//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Project bus user settings (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_USCOMMUNICATION_H
#define C_USCOMMUNICATION_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QString>

#include "stwtypes.h"

#include "C_OSCCanProtocol.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_UsCommunication
{
public:
   C_UsCommunication(void);

   const std::vector<stw_types::sint32> & GetMessageOverviewColumnWidth(void) const;
   const std::vector<stw_types::sint32> & GetSignalOverviewColumnWidth(void) const;
   void SetMessageOverviewColumnWidth(const std::vector<stw_types::sint32> & orc_Value);
   void SetSignalOverviewColumnWidth(const std::vector<stw_types::sint32> & orc_Value);

   void SetMessageSelected(const stw_opensyde_core::C_OSCCanProtocol::E_Type oe_SelectedProtocol,
                           const bool oq_MessageSelected, const QString & orc_SelectedMessageName,
                           const bool oq_SignalSelected, const QString & orc_SelectedSignalName);
   void GetLastSelectedMessage(stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_SelectedProtocol,
                               bool & orq_MessageSelected, QString & orc_SelectedMessageName, bool & orq_SignalSelected,
                               QString & orc_SelectedSignalName) const;

private:
   std::vector<stw_types::sint32> mc_MessageOverviewColumnWidth;
   std::vector<stw_types::sint32> mc_SignalOverviewColumnWidth;
   stw_opensyde_core::C_OSCCanProtocol::E_Type me_SelectedProtocol;
   bool mq_MessageSelected;
   QString mc_SelectedMessageName;
   bool mq_SignalSelected;
   QString mc_SelectedSignalName;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
