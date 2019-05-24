//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing search results out of the toolbar (implementation)

   Tool bar search result window

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_NagToolBarSearchResults.h"
#include "ui_C_NagToolBarSearchResults.h"

#include "C_GtGetText.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   \param[in] opc_Parent  Optional parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_NagToolBarSearchResults::C_NagToolBarSearchResults(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_NagToolBarSearchResults)
{
   this->mpc_Ui->setupUi(this);

   //this->setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);
   this->setWindowFlags(Qt::FramelessWindowHint);
   //this->setAttribute(Qt::WA_TranslucentBackground);

   this->mpc_Ui->pc_LabelNoSearchResultsFound->setVisible(false);

   this->mpc_Ui->pc_LabelNoSearchResultsFound->setText(C_GtGetText::h_GetText("    No results found"));

   // Forwarding the signal
   connect(this->mpc_Ui->pc_SearchTreeWidget, &C_SdSearchTreeWidget::SigChangeMode, this,
           &C_NagToolBarSearchResults::SigChangeMode);
   connect(this->mpc_Ui->pc_SearchTreeWidget, &C_SdSearchTreeWidget::SigOpenDetail, this,
           &C_NagToolBarSearchResults::SigOpenDetail);
   connect(this->mpc_Ui->pc_SearchTreeWidget, &C_SdSearchTreeWidget::SigHide, this,
           &C_NagToolBarSearchResults::SigHide);
   connect(this->mpc_Ui->pc_SearchTreeWidget, &C_SdSearchTreeWidget::SigFocusOut, this,
           &C_NagToolBarSearchResults::SigFocusOut);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_NagToolBarSearchResults::~C_NagToolBarSearchResults()
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Searching for substring in the names of system definition items

   \param[in]     orc_SearchString   Search substring
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagToolBarSearchResults::StartSearch(const QString & orc_SearchString) const
{
   bool q_ResultsFound;

   q_ResultsFound = this->mpc_Ui->pc_SearchTreeWidget->Search(orc_SearchString);

   //hide/show no results label
   this->mpc_Ui->pc_LabelNoSearchResultsFound->setVisible(!q_ResultsFound);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Removes all search results
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagToolBarSearchResults::ClearResult(void) const
{
   this->mpc_Ui->pc_SearchTreeWidget->ClearResult();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns a flag if search results exist

   \return
   true     Results found
   false    No results
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_NagToolBarSearchResults::HasResults(void) const
{
   return this->mpc_Ui->pc_SearchTreeWidget->HasResults();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the focus to the search result tree widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagToolBarSearchResults::SetSearchResultFocus(void) const
{
   this->mpc_Ui->pc_SearchTreeWidget->SetSearchResultFocus();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns a flag if the search tree widget has the focus

   \return
   true     Has focus
   false    Has no focus
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_NagToolBarSearchResults::HasChildFocus(void) const
{
   return this->mpc_Ui->pc_SearchTreeWidget->hasFocus();
}
