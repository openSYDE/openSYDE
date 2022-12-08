//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Holds datapool element content and its timestamp (implementation)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "TglTime.hpp"

#include "C_PuiSvDbDataElementContent.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_logic;

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
C_PuiSvDbDataElementContent::C_PuiSvDbDataElementContent(void) :
   C_OscNodeDataPoolContent(),
   mu32_TimeStamp(0U)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Copy constructor

   \param[in]     orc_Source      Reference to source
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDbDataElementContent::C_PuiSvDbDataElementContent(const C_PuiSvDbDataElementContent & orc_Source) :
   C_OscNodeDataPoolContent(orc_Source)
{
   this->mu32_TimeStamp = orc_Source.mu32_TimeStamp;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Copy constructor with base class reference

   Initializing the timestamp value with the actual timestamp

   \param[in]     orc_Source      Reference to source
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDbDataElementContent::C_PuiSvDbDataElementContent(const stw::opensyde_core::C_OscNodeDataPoolContent & orc_Source)
   :
   C_OscNodeDataPoolContent(orc_Source)
{
   this->mu32_TimeStamp = stw::tgl::TglGetTickCount();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Copy operator

   \param[in]     orc_Source      Reference to source

   \return
   Copied element
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDbDataElementContent & C_PuiSvDbDataElementContent::operator =(const C_PuiSvDbDataElementContent & orc_Source) &
{
   if (this != &orc_Source)
   {
      this->mu32_TimeStamp = orc_Source.mu32_TimeStamp;

      C_OscNodeDataPoolContent::operator =(orc_Source);
   }

   return (*this);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the timestamp of the received datapool content

   \param[in]     ou32_Timestamp   Timestamp value in ms
*/
//----------------------------------------------------------------------------------------------------------------------
void C_PuiSvDbDataElementContent::SetTimeStamp(const uint32_t ou32_Timestamp)
{
   this->mu32_TimeStamp = ou32_Timestamp;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Gets the timestamp of the received datapool content

   \return
   Timestamp value in ms
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_PuiSvDbDataElementContent::GetTimeStamp(void) const
{
   return this->mu32_TimeStamp;
}
