//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Small utility data structure to group all imported message data for one specific direction (TX/RX)

   Small utility data structure to group all imported message data for one specific direction (TX/RX)

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_CieImportedMessageVectorData.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_CieImportedMessageVectorData::C_CieImportedMessageVectorData()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Data initialization constructor

   \param[in]  orc_OscMessageData               Osc message data
   \param[in]  orc_OscSignalData                Osc signal data
   \param[in]  orc_UiMessageData                Ui message data
   \param[in]  orc_UiSignalData                 Ui signal data
   \param[in]  opc_WarningMessagesPerMessage    Warning messages per message
*/
//----------------------------------------------------------------------------------------------------------------------
C_CieImportedMessageVectorData::C_CieImportedMessageVectorData(
   const std::vector<stw::opensyde_core::C_OscCanMessage> & orc_OscMessageData,
   const std::vector<stw::opensyde_core::C_OscNodeDataPoolListElement> & orc_OscSignalData,
   const std::vector<C_PuiSdNodeCanMessage> & orc_UiMessageData,
   const std::vector<C_PuiSdNodeDataPoolListElement> & orc_UiSignalData,
   const std::vector<QString> * const opc_WarningMessagesPerMessage) :
   c_OscMessageData(orc_OscMessageData),
   c_OscSignalData(orc_OscSignalData),
   c_UiMessageData(orc_UiMessageData),
   c_UiSignalData(orc_UiSignalData)
{
   if (opc_WarningMessagesPerMessage != NULL)
   {
      c_WarningMessagesPerMessage = *opc_WarningMessagesPerMessage;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Clear
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieImportedMessageVectorData::Clear()
{
   c_OscMessageData.clear();
   c_OscSignalData.clear();
   c_UiMessageData.clear();
   c_UiSignalData.clear();
   c_WarningMessagesPerMessage.clear();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Write back

   \param[in,out]  orc_OscMessageData              Osc message data
   \param[in,out]  orc_OscSignalData               Osc signal data
   \param[in,out]  orc_UiMessageData               Ui message data
   \param[in,out]  orc_UiSignalData                Ui signal data
   \param[in,out]  orc_WarningMessagesPerMessage   Warning messages per message
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CieImportedMessageVectorData::WriteBack(std::vector<stw::opensyde_core::C_OscCanMessage> & orc_OscMessageData,
                                               std::vector<stw::opensyde_core::C_OscNodeDataPoolListElement> & orc_OscSignalData, std::vector<C_PuiSdNodeCanMessage> & orc_UiMessageData, std::vector<C_PuiSdNodeDataPoolListElement> & orc_UiSignalData,
                                               std::vector<QString> & orc_WarningMessagesPerMessage) const
{
   orc_OscMessageData = c_OscMessageData;
   orc_OscSignalData = c_OscSignalData;
   orc_UiMessageData = c_UiMessageData;
   orc_UiSignalData = c_UiSignalData;
   orc_WarningMessagesPerMessage = c_WarningMessagesPerMessage;
}
