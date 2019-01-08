//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing system view update overview

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     19.06.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#include "C_SyvUpOverviewWidget.h"
#include "ui_C_SyvUpOverviewWidget.h"

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

   \param[in,out] opc_parent Optional pointer to parent

   \created     19.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SyvUpOverviewWidget::C_SyvUpOverviewWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SyvUpOverviewWidget)
{
   mpc_Ui->setupUi(this);
}

//-----------------------------------------------------------------------------
/*!
   \brief   default destructor

   Clean up.

   \created     19.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SyvUpOverviewWidget::~C_SyvUpOverviewWidget()
{
   delete mpc_Ui;
}
