//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Project bus user settings (implementation)

   Project bus user settings

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_UsCommunication.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_UsCommunication::C_UsCommunication(void) :
   me_SelectedProtocol(C_OSCCanProtocol::eLAYER2),
   mq_MessageSelected(false),
   mq_SignalSelected(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set last selected message

   \param[in] oe_SelectedProtocol     Currently selected protocol
   \param[in] oq_MessageSelected      Set flag if there is a selected message
   \param[in] orc_SelectedMessageName Selected message name if any
   \param[in] oq_SignalSelected       Flag if signal selected
   \param[in] orc_SelectedSignalName  Selected signal name if any
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsCommunication::SetMessageSelected(const C_OSCCanProtocol::E_Type oe_SelectedProtocol,
                                           const bool oq_MessageSelected, const QString & orc_SelectedMessageName,
                                           const bool oq_SignalSelected, const QString & orc_SelectedSignalName)
{
   this->me_SelectedProtocol = oe_SelectedProtocol;
   this->mq_MessageSelected = oq_MessageSelected;
   this->mc_SelectedMessageName = orc_SelectedMessageName;
   this->mq_SignalSelected = oq_SignalSelected;
   this->mc_SelectedSignalName = orc_SelectedSignalName;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get last selected message

   \param[out] ore_SelectedProtocol    Currently selected protocol
   \param[out] orq_MessageSelected     Set flag if there is a selected message
   \param[out] orc_SelectedMessageName Selected message name if any
   \param[out] orq_SignalSelected      Flag if signal selected
   \param[out] orc_SelectedSignalName  Selected signal name if any
*/
//----------------------------------------------------------------------------------------------------------------------
void C_UsCommunication::GetLastSelectedMessage(C_OSCCanProtocol::E_Type & ore_SelectedProtocol,
                                               bool & orq_MessageSelected, QString & orc_SelectedMessageName,
                                               bool & orq_SignalSelected, QString & orc_SelectedSignalName) const
{
   ore_SelectedProtocol = this->me_SelectedProtocol;
   orq_MessageSelected = this->mq_MessageSelected;
   orc_SelectedMessageName = this->mc_SelectedMessageName;
   orq_SignalSelected = this->mq_SignalSelected;
   orc_SelectedSignalName = this->mc_SelectedSignalName;
}
