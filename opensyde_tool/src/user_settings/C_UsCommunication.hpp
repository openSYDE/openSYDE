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

#include "stwtypes.hpp"

#include "C_OscCanProtocol.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_UsCommunication
{
public:
   C_UsCommunication(void);

   const std::vector<int32_t> & GetMessageOverviewColumnWidth(void) const;
   const std::vector<int32_t> & GetSignalOverviewColumnWidth(void) const;
   void SetMessageOverviewColumnWidth(const std::vector<int32_t> & orc_Value);
   void SetSignalOverviewColumnWidth(const std::vector<int32_t> & orc_Value);

   void SetMessageSelected(const stw::opensyde_core::C_OscCanProtocol::E_Type oe_SelectedProtocol,
                           const bool oq_MessageSelected, const QString & orc_SelectedMessageName,
                           const bool oq_SignalSelected, const QString & orc_SelectedSignalName);
   void GetLastSelectedMessage(stw::opensyde_core::C_OscCanProtocol::E_Type & ore_SelectedProtocol,
                               bool & orq_MessageSelected, QString & orc_SelectedMessageName, bool & orq_SignalSelected,
                               QString & orc_SelectedSignalName) const;

private:
   std::vector<int32_t> mc_MessageOverviewColumnWidth;
   std::vector<int32_t> mc_SignalOverviewColumnWidth;
   stw::opensyde_core::C_OscCanProtocol::E_Type me_SelectedProtocol;
   bool mq_MessageSelected;
   QString mc_SelectedMessageName;
   bool mq_SignalSelected;
   QString mc_SelectedSignalName;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
