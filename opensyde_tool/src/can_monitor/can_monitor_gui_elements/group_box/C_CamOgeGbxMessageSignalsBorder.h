//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Group box with top blue border (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     14.12.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_CAMOGEGBXMESSAGESIGNALSBORDER_H
#define C_CAMOGEGBXMESSAGESIGNALSBORDER_H

/* -- Includes ------------------------------------------------------------- */
#include <QGroupBox>

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_CamOgeGbxMessageSignalsBorder :
   public QGroupBox
{
   Q_OBJECT

public:
   explicit C_CamOgeGbxMessageSignalsBorder(QWidget * const opc_Parent = NULL);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
