//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Group box for miscellaneous navigation section (implementation)

   Group box for miscellaneous navigation section.

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     30.07.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_OgeWiUtil.h"
#include "C_OgeGbxNavigationSection.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_opensyde_gui_logic;
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

   \created     30.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_OgeGbxNavigationSection::C_OgeGbxNavigationSection(QWidget * const opc_Parent) :
   QGroupBox(opc_Parent)
{
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "SpecialBackground", false);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set special background active

   \param[in] oq_Active Special background active

   \created     31.07.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_OgeGbxNavigationSection::SetSpecialBackground(const bool oq_Active)
{
   C_OgeWiUtil::h_ApplyStylesheetProperty(this, "SpecialBackground", oq_Active);
}
