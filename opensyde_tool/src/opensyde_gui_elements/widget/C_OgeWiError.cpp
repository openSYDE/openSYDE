//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Custom error widget (implementation)

   Custom error widget

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QDialog>
#include "C_OgeWiError.h"
#include "C_GtGetText.h"
#include "ui_C_OgeWiError.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw_opensyde_gui_elements;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeWiError::C_OgeWiError(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_OgeWiError),
   mpc_ParentDialog(NULL)
{
   mpc_Ui->setupUi(this);

   InitStaticNames();

   //Allow exit on click ok
   connect(this->mpc_Ui->pc_PushButtonOk, &C_OgePubDialog::clicked, this, &C_OgeWiError::m_OkClicked);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540} Never took ownership of parent dialog
C_OgeWiError::~C_OgeWiError()
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiError::InitStaticNames() const
{
   this->mpc_Ui->pc_PushButtonOk->setText(C_GtGetText::h_GetText("OK"));
   this->mpc_Ui->pc_ErrorDescription->setPlainText(C_GtGetText::h_GetText("No error description available."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set error description

   \param[in]  orc_Description   Error description string
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiError::SetErrorDescription(const QString & orc_Description) const
{
   this->mpc_Ui->pc_ErrorDescription->setPlainText(orc_Description);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Remember dialog as parent

   \param[in]  opc_ParentDialog  Parent dialog
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiError::SaveParentDialog(QDialog * const opc_ParentDialog)
{
   this->mpc_ParentDialog = opc_ParentDialog;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Ok button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeWiError::m_OkClicked(void)
{
   if (mpc_ParentDialog != NULL)
   {
      mpc_ParentDialog->accept();
   }
   else
   {
      this->close();
   }
}
