//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Group box with white background and top border (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     30.11.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_CAMOGEGBXMESSAGEGENERATORBACKGROUND_H
#define C_CAMOGEGBXMESSAGEGENERATORBACKGROUND_H

/* -- Includes ------------------------------------------------------------- */
#include <QGroupBox>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_CamOgeGbxMessageGeneratorBackground :
   public QGroupBox
{
   Q_OBJECT

public:
   C_CamOgeGbxMessageGeneratorBackground(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
