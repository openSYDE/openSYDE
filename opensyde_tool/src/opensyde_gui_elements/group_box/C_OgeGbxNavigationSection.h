//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Group box for miscellaneous navigation section (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     30.07.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGEGBXNAVIGATIONSECTION_H
#define C_OGEGBXNAVIGATIONSECTION_H

/* -- Includes ------------------------------------------------------------- */
#include <QGroupBox>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeGbxNavigationSection :
   public QGroupBox
{
   Q_OBJECT

public:
   explicit C_OgeGbxNavigationSection(QWidget * const opc_Parent = NULL);

   void SetSpecialBackground(const bool oq_Active);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
