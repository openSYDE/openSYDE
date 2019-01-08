//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Widget element for the QTreeWidget based C_OgeTreeWidgetToolBarSearch painted in delegate

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     16.03.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_OgeTreeWidgetToolBarSearchItemWidget.h"

#include "ui_C_OgeTreeWidgetToolBarSearchItemWidget.h"

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

   \param[in] opc_Parent  Optional parent

   \created     16.03.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_OgeTreeWidgetToolBarSearchItemWidget::C_OgeTreeWidgetToolBarSearchItemWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_OgeTreeWidgetToolBarSearchItemWidget)
{
   mpc_Ui->setupUi(this);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   \param[in] opc_Parent        Optional parent
   \param[in] orc_Title         New title
   \param[in] orc_Subtitle      New subtitle

   \created     16.03.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_OgeTreeWidgetToolBarSearchItemWidget::C_OgeTreeWidgetToolBarSearchItemWidget(const QString & orc_Title,
                                                                               const QString & orc_Subtitle,
                                                                               QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_OgeTreeWidgetToolBarSearchItemWidget)
{
   mpc_Ui->setupUi(this);

   this->mpc_Ui->pc_LabelTitle->setText(orc_Title);

   if (orc_Subtitle != "")
   {
      this->mpc_Ui->pc_LabelSubtitle->setVisible(true);
      this->mpc_Ui->pc_LabelSubtitle->setText(orc_Subtitle);
   }
   else
   {
      this->mpc_Ui->pc_LabelSubtitle->setVisible(false);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   default destructor

   Clean up.

   \created     06.07.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_OgeTreeWidgetToolBarSearchItemWidget::~C_OgeTreeWidgetToolBarSearchItemWidget()
{
   delete mpc_Ui;
}
