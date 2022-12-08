//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       short description (implementation)

   detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_OgeTabBar.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeTabBar::C_OgeTabBar(QWidget * const opc_Parent) :
   QTabBar(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Tab size hint

   \param[in] os32_Index Tab index

   \return
   Tab size hint
*/
//----------------------------------------------------------------------------------------------------------------------
QSize C_OgeTabBar::tabSizeHint(const int32_t os32_Index) const
{
   QSize c_Retval = QTabBar::tabSizeHint(os32_Index);

   if (this->count() > 0)
   {
      const int32_t s32_Error = static_cast<int32_t>(this->width() % this->count());
      int32_t s32_Width = static_cast<int32_t>(this->width() / this->count());
      if (s32_Error > 1)
      {
         //Add most to start, some to end
         if (os32_Index == 0)
         {
            s32_Width += s32_Error - 1;
         }
         else if (os32_Index == (this->count() - 1))
         {
            ++s32_Width;
         }
         else
         {
            //No adaptation
         }
      }
      else
      {
         //Add to end
         if (os32_Index == 0)
         {
            s32_Width += s32_Error;
         }
      }
      c_Retval.setWidth(s32_Width);
   }

   return c_Retval;
}
