//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Holds datapool element content and its timestamp (implementation)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     23.08.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "TGLTime.h"

#include "C_PuiSvDbDataElementContent.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \created     23.08.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_PuiSvDbDataElementContent::C_PuiSvDbDataElementContent(void) :
   C_OSCNodeDataPoolContent(),
   mu32_TimeStamp(0U)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Copy constructor

   \param[in]     orc_Source      Reference to source

   \created     23.08.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_PuiSvDbDataElementContent::C_PuiSvDbDataElementContent(const C_PuiSvDbDataElementContent & orc_Source) :
   C_OSCNodeDataPoolContent(orc_Source)
{
   this->mu32_TimeStamp = orc_Source.mu32_TimeStamp;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Copy constructor with base class reference

   Initializing the timestamp value with the actual timestamp

   \param[in]     orc_Source      Reference to source

   \created     23.08.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_PuiSvDbDataElementContent::C_PuiSvDbDataElementContent(const stw_opensyde_core::C_OSCNodeDataPoolContent & orc_Source)
   :
   C_OSCNodeDataPoolContent(orc_Source)
{
   this->mu32_TimeStamp = stw_tgl::TGL_GetTickCount();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Copy operator

   \param[in]     orc_Source      Reference to source

   \created     23.08.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_PuiSvDbDataElementContent & C_PuiSvDbDataElementContent::operator =(const C_PuiSvDbDataElementContent & orc_Source)
{
   if (this != &orc_Source)
   {
      this->mu32_TimeStamp = orc_Source.mu32_TimeStamp;

      C_OSCNodeDataPoolContent::operator =(orc_Source);
   }

   return (*this);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the timestamp of the received datapool content

   \param[in]     ou32_Timestamp   Timestamp value in ms

   \created     23.08.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_PuiSvDbDataElementContent::SetTimeStamp(const stw_types::uint32 ou32_Timestamp)
{
   this->mu32_TimeStamp = ou32_Timestamp;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Gets the timestamp of the received datapool content

   \return
   Timestamp value in ms

   \created     23.08.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
stw_types::uint32 C_PuiSvDbDataElementContent::GetTimeStamp(void) const
{
   return this->mu32_TimeStamp;
}
