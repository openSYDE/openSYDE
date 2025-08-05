//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Creating Pem files widget

   Creating widget to add PEM files in to List

   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_SyvUpPacPemFilesWidget.hpp"
#include "ui_C_SyvUpPacPemFilesWidget.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpPacPemFilesWidget::C_SyvUpPacPemFilesWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SyvUpPacPemFilesWidget)
{
   this->mpc_Ui->setupUi(this);

   this->setAcceptDrops(true);
   this->setMouseTracking(true);

   connect(this->mpc_Ui->pc_ListWidget, &C_SyvUpPacPemFilesListWidget::SigUpdateFileCounter, this,
           &C_SyvUpPacPemFilesWidget::SigUpdateFileCounter);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpPacPemFilesWidget::~C_SyvUpPacPemFilesWidget()
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update PEM files widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacPemFilesWidget::UpdatePemFileWidget()
{
   if (this->mpc_Ui->pc_ListWidget->count() <= 0)
   {
      this->mpc_Ui->pc_GbxNoPEMFiles->setVisible(true);
      this->mpc_Ui->pc_ListWidget->setVisible(false);
   }
   else
   {
      this->mpc_Ui->pc_GbxNoPEMFiles->setVisible(false);
      this->mpc_Ui->pc_ListWidget->setVisible(true);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Return count of PEM files in list

   \return
   int32_t   count of files in list
*/
//----------------------------------------------------------------------------------------------------------------------
int32_t C_SyvUpPacPemFilesWidget::GetFilesCount() const
{
   return this->mpc_Ui->pc_ListWidget->count();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Saves last loaded PEM files
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacPemFilesWidget::SaveUserSettings()
{
   this->mpc_Ui->pc_ListWidget->SaveUserSettings();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle adding PEM file to list
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacPemFilesWidget::HandleAddPemFile()
{
   this->mpc_Ui->pc_ListWidget->AddFileAction();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle clearing all PEM files from the list
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacPemFilesWidget::HandleClearAllPemFiles()
{
   this->mpc_Ui->pc_ListWidget->DeleteAllItems();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get all loaded PEM file paths

   \return
   std::vector<stw::scl::C_SclString>   returns vector of Pem Files
*/
//----------------------------------------------------------------------------------------------------------------------
QStringList C_SyvUpPacPemFilesWidget::GetPemFilePaths() const
{
   return this->mpc_Ui->pc_ListWidget->GetPemFilePaths();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overridden dropEvent slot
 *
   \param[in]  opc_Event  Event identification and information

   \return
   void
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacPemFilesWidget::dropEvent(QDropEvent * const opc_Event)
{
   this->mpc_Ui->pc_ListWidget->DropEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overridden dragEnterEvent slot
 *
   \param[in]  opc_Event  Event identification and information

   \return
   void
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacPemFilesWidget::dragEnterEvent(QDragEnterEvent * const opc_Event)
{
   this->mpc_Ui->pc_ListWidget->DragEnterEvent(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overridden dragMoveEvent slot
 *
   \param[in]  opc_Event  Event identification and information

   \return
   void
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacPemFilesWidget::dragMoveEvent(QDragMoveEvent * const opc_Event)
{
   this->mpc_Ui->pc_ListWidget->DragMoveEvent(opc_Event);
}
