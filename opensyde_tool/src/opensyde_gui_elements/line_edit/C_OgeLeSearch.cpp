//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Line edit field for search (implementation)

   Line edit field for search

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     26.09.2016  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_OgeLeSearch.h"

/* -- Used Namespaces ------------------------------------------------------ */
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

   \created     26.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------

C_OgeLeSearch::C_OgeLeSearch(QWidget * const opc_Parent) :
   C_OgeLeContextMenuBase(opc_Parent)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Apply dark theme

   \created     27.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeLeSearch::SetDarkTheme()
{
   this->setStyleSheet("stw_opensyde_gui_elements--C_OgeLeSearch {"
                       "background-color:transparent;"
                       "color:rgb(211, 211, 216);"
                       "}");
}

//-----------------------------------------------------------------------------
/*!
   \brief   Apply light theme

   \created     27.09.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeLeSearch::SetLightTheme()
{
   this->setStyleSheet("");
}
