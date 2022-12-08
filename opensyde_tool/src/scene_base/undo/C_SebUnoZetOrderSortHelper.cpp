//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Sorting helper for scene z order

   Sorting helper for scene z order

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_OscUtils.hpp"
#include "C_SebUnoZetOrderSortHelper.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_core;
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
C_SebUnoZetOrderSortHelper::C_SebUnoZetOrderSortHelper(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Compare item 1 to 2

   Primary sorting criteria: alphabetical

   \param[in] opc_Item1 item 1
   \param[in] opc_Item2 item 2

   \retval   true  item 1 smaller
   \retval   false item 1 greater or equal
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SebUnoZetOrderSortHelper::operator ()(const QGraphicsItem * const opc_Item1,
                                             const QGraphicsItem * const opc_Item2) const
{
   //Default: messages equal
   bool q_Retval = false;

   if ((opc_Item1 != NULL) && (opc_Item2 != NULL))
   {
      //Use z values as first order priority
      if (opc_Item1->zValue() < opc_Item2->zValue())
      {
         q_Retval = true;
      }
      else if (C_OscUtils::h_IsFloat64NearlyEqual(opc_Item1->zValue(), opc_Item2->zValue()))
      {
         //Special handling if values equal
         if (opc_Item1->isObscuredBy(opc_Item2))
         {
            q_Retval = true;
         }
      }
      else
      {
         //Definitely item 2 smaller
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Sort items via z order

   first: lowest z order
   last: highest z order

   \param[in,out] orc_ZetValues Items to sort
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoZetOrderSortHelper::h_SortZetValues(QList<QGraphicsItem *> & orc_ZetValues)
{
   std::sort(orc_ZetValues.begin(), orc_ZetValues.end(), C_SebUnoZetOrderSortHelper());
}
