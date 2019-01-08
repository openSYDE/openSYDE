//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       short description (implementation)

   detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     20.02.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_OgeTabBar.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent

   \created     20.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OgeTabBar::C_OgeTabBar(QWidget * const opc_Parent) :
   QTabBar(opc_Parent)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Tab size hint

   \param[in] osn_Index Tab index

   \return
   Tab size hint

   \created     20.02.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QSize C_OgeTabBar::tabSizeHint(const sintn osn_Index) const
{
   QSize c_Retval = QTabBar::tabSizeHint(osn_Index);

   if (this->count() > 0)
   {
      const sint32 s32_Error = static_cast<sint32>(this->width() % this->count());
      sint32 s32_Width = static_cast<sint32>(this->width() / this->count());
      if (s32_Error > 1)
      {
         //Add most to start, some to end
         if (osn_Index == 0)
         {
            s32_Width += s32_Error - 1;
         }
         else if (osn_Index == (this->count() - 1))
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
         if (osn_Index == 0)
         {
            s32_Width += s32_Error;
         }
      }
      c_Retval.setWidth(s32_Width);
   }

   return c_Retval;
}
