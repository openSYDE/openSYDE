//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Base class for graphics items with system definition data elements (implementation)

   Base class for graphics items with system definition data elements

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     14.09.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_PuiSdDataElement.h"
#include "C_PuiSdHandler.h"

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
   \brief   Standard constructor

   \param[in] ors32_Index Index of connected data item
   \param[in] ore_Type    Type of data

   \created     14.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_PuiSdDataElement::C_PuiSdDataElement(const stw_types::sint32 & ors32_Index, const E_Type & ore_Type) :
   C_PuiBsDataElement(ors32_Index),
   me_Type(ore_Type)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   \created     19.10.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_PuiSdDataElement::~C_PuiSdDataElement()
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Adapt index if array was resized

   \param[in]   ore_Type      Type of affected items
   \param[in]   ors32_Index   Index of which segment
   \param[in]   ore_Action    Type of change

   \created     14.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_PuiSdDataElement::SyncIndex(const E_Type & ore_Type, const stw_types::sint32 & ors32_Index,
                                   const E_Action & ore_Action)
{
   if (ore_Type == me_Type)
   {
      if (ore_Action == E_Action::eDELETE)
      {
         if (ors32_Index < ms32_Index)
         {
            --ms32_Index;
         }
      }
      else
      {
         if (ors32_Index <= ms32_Index)
         {
            ++ms32_Index;
         }
      }
   }
}
