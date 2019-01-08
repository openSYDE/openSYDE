//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget base for dashboard widgets

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     10.08.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "stwtypes.h"
#include "C_SyvDaItDashboardBaseWidget.h"
#include "ui_C_SyvDaItDashboardBaseWidget.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
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

   \created     10.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SyvDaItDashboardBaseWidget::C_SyvDaItDashboardBaseWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SyvDaItDashboardBaseWidget),
   mpc_Widget(NULL)
{
   mpc_Ui->setupUi(this);

   this->setAttribute(Qt::WA_TranslucentBackground);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   Clean up.

   \created     10.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SyvDaItDashboardBaseWidget::~C_SyvDaItDashboardBaseWidget(void)
{
   delete mpc_Ui;
   //lint -e{1740}  no memory leak because of the parent of mpc_Widget and the Qt memory management
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set content widget

   \param[in,out] opc_Content New content widget

   \created     10.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaItDashboardBaseWidget::SetWidget(QWidget * const opc_Content)
{
   if (opc_Content != NULL)
   {
      this->mpc_Widget = opc_Content;
      opc_Content->setParent(this);
      this->mpc_Ui->pc_VerticalLayout->addWidget(opc_Content, 0);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten resize event slot

   Here: Update integrated widget size

   \param[in,out] opc_Event Event identification and information

   \created     16.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaItDashboardBaseWidget::resizeEvent(QResizeEvent * const opc_Event)
{
   QWidget::resizeEvent(opc_Event);
   if (this->mpc_Widget != NULL)
   {
      const QMargins c_Margins1 = this->mpc_Ui->pc_VerticalLayout_2->contentsMargins();
      const QMargins c_Margins2 = this->mpc_Ui->pc_VerticalLayout->contentsMargins();
      const sintn sn_Width = this->size().width() -
                             (c_Margins1.right() + c_Margins1.left() + c_Margins2.right() + c_Margins2.left());
      const sintn sn_Height = this->size().height() -
                              (c_Margins1.top() + c_Margins1.bottom() + c_Margins2.top() + c_Margins2.bottom());
      this->mpc_Widget->setMaximumSize(sn_Width, sn_Height);
   }
}
