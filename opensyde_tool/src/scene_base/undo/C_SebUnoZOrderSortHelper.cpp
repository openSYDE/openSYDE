//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Sorting helper for scene z order

   Sorting helper for scene z order

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_OSCUtils.h"
#include "C_SebUnoZOrderSortHelper.h"

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
/*! \brief  Default constructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SebUnoZOrderSortHelper::C_SebUnoZOrderSortHelper(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Compare item 1 to 2

   Primary sorting criteria: alphabetical

   \param[in] opc_Item1 item 1
   \param[in] opc_Item2 item 2

   \retval   true  item 1 smaller
   \retval   false item 1 greater or equal
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SebUnoZOrderSortHelper::operator ()(const QGraphicsItem * const opc_Item1,
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
      else if (C_OSCUtils::h_IsFloat64NearlyEqual(opc_Item1->zValue(), opc_Item2->zValue()))
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

   \param[in,out] orc_ZValues Items to sort
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SebUnoZOrderSortHelper::h_SortZValues(QList<QGraphicsItem *> & orc_ZValues)
{
   //lint -e{864} Call as expected by interface
   std::sort(orc_ZValues.begin(), orc_ZValues.end(), C_SebUnoZOrderSortHelper());
}
