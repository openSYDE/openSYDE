//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Creating PEM File entry

   Creating PEM File entry

   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_SyvUpPacPemFileEntry.hpp"
#include "ui_C_SyvUpPacPemFileEntry.h"
#include "C_Uti.hpp"
#include "C_GtGetText.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_elements;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  opc_Parent                Optional pointer to parent
   \param[in]  orc_File                      Pem file
   \param[in]  os32_CurrentPemFileIndex      Current pem file widget index
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpPacPemFileEntry::C_SyvUpPacPemFileEntry(QWidget * const opc_Parent, const QString & orc_File,
                                               const int32_t os32_CurrentPemFileIndex) :
   stw::opensyde_gui_elements::C_OgeWiWithToolTip(opc_Parent),
   c_FilePath(orc_File),
   s32_CurrentPemFileIndex(os32_CurrentPemFileIndex),
   mpc_Ui(new Ui::C_SyvUpPacPemFileEntry)
{
   this->mpc_Ui->setupUi(this);

   this->mpc_Ui->pc_PushButtonDelete->SetSvg("://images/main_page_and_navi_bar/Icon_delete.svg");
   this->mpc_Ui->pc_PushButtonDelete->SetToolTipInformation(C_GtGetText::h_GetText("Delete file"),
                                                            C_GtGetText::h_GetText(
                                                               "Remove PEM file from list"));

   this->m_LoadPemFileInfo();

   connect(this->mpc_Ui->pc_PushButtonDelete, &QPushButton::clicked, this, &C_SyvUpPacPemFileEntry::m_DeleteItem);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpPacPemFileEntry::~C_SyvUpPacPemFileEntry()
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Loading Pem file info
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacPemFileEntry::m_LoadPemFileInfo()
{
   this->mpc_Ui->pc_LabelPemFilePath->setText(C_Uti::h_MinimizePath(c_FilePath,
                                                                    this->mpc_Ui->pc_LabelPemFilePath->font(),
                                                                    775, 0U));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  emit signal to delete item widget
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvUpPacPemFileEntry::m_DeleteItem()
{
   Q_EMIT this->SigDeleteItemWidget(this->s32_CurrentPemFileIndex, true);
}
