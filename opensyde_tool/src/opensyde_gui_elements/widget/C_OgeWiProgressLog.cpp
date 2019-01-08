//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Widget for ProgressLog

   Widget for ProgressLog

   \implementation
   project     opensyde
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     06.07.2016  STW/S.Singer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_OgeWiProgressLog.h"
#include "ui_C_OgeWiProgressLog.h"

/* -- Used Namespaces ------------------------------------------------------ */

using namespace stw_opensyde_gui;

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

   \created     06.07.2016  STW/S.Singer
*/
//-----------------------------------------------------------------------------
C_OgeWiProgressLog::C_OgeWiProgressLog(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_OgeWiProgressLog)
{
   this->mpc_Ui->setupUi(this);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   \created     06.07.2016  STW/S.Singer
*/
//-----------------------------------------------------------------------------
C_OgeWiProgressLog::~C_OgeWiProgressLog()
{
   delete mpc_Ui;
}
