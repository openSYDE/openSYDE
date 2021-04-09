//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base widget for style setup to customize graphic elements

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include <QVBoxLayout>

#include "TGLUtils.h"
#include "C_GiSyBaseWidget.h"
#include "C_OgeWiUtil.h"
#include "ui_C_GiSyBaseWidget.h"

#include "C_GtGetText.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;
using namespace stw_types;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] orc_Parent  Reference to parent
   \param[in]     orc_Name    Name of the bus for the title
   \param[in]     oq_DarkMode Optional flag if dark mode active
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiSyBaseWidget::C_GiSyBaseWidget(C_OgePopUpDialog & orc_Parent, const QString & orc_Name, const bool oq_DarkMode) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_GiSyBaseWidget()),
   mpc_ParentDialog(&orc_Parent)
{
   const QSize c_Size = h_GetSceneViewSize();

   // init UI
   mpc_Ui->setupUi(this);

   // register the widget for showing
   this->mpc_ParentDialog->SetWidget(this);

   // set main title
   this->mpc_ParentDialog->SetTitle(orc_Name);

   // create scene for graphics view
   this->mpc_Scene = new C_SdTopologyScene(false, NULL);

   // configure background drawing
   this->mpc_Scene->setSceneRect(0.0, 0.0,
                                 static_cast<float64>(c_Size.width()),
                                 static_cast<float64>(c_Size.height()));
   this->mpc_Ui->pc_GraphicsView->setScene(this->mpc_Scene);
   this->mpc_Ui->pc_GraphicsView->setEnabled(false);
   this->mpc_Ui->pc_GraphicsView->SetSubtleSurroundGradient(true);
   this->mpc_Scene->SetDarkModeActive(oq_DarkMode);
   this->mpc_Ui->pc_GraphicsView->SetDarkMode(oq_DarkMode);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_GroupBox, "Edit", true);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_GroupBox, "DarkMode", oq_DarkMode);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_PreviewFrame, "NoBorder", oq_DarkMode);

   InitStaticNames();

   // connects
   connect(this->mpc_Ui->pc_BushButtonOk, &QPushButton::clicked, this, &C_GiSyBaseWidget::m_OkClicked);
   connect(this->mpc_Ui->pc_BushButtonCancel, &QPushButton::clicked,
           this, &C_GiSyBaseWidget::m_CancelClicked);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because the ownership of these objects was never transferred to this class
C_GiSyBaseWidget::~C_GiSyBaseWidget(void)
{
   delete mpc_Ui;
   delete mpc_Scene;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyBaseWidget::InitStaticNames(void)
{
   this->mpc_ParentDialog->SetSubTitle(C_GtGetText::h_GetText("Setup Style"));
   this->mpc_Ui->pc_BushButtonOk->setText(C_GtGetText::h_GetText("OK"));
   this->mpc_Ui->pc_BushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));
   this->mpc_Ui->pc_LabelStyle->setText(C_GtGetText::h_GetText("Style"));
   this->mpc_Ui->pc_LabelPreview->setText(C_GtGetText::h_GetText("Example Preview"));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Place widget in pop up dialog

   \param[in,out] opc_Widget Widget to place in pop up dialog
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyBaseWidget::SetWidget(QWidget * const opc_Widget)
{
   if (opc_Widget != NULL)
   {
      stw_types::sintn sn_Index;
      QVBoxLayout * const pc_Layout = new QVBoxLayout(this->mpc_Ui->pc_SettingsShowWidget);

      opc_Widget->setParent(this);
      this->mpc_Ui->pc_SettingsShowWidget->setLayout(pc_Layout);
      pc_Layout->addWidget(opc_Widget);
      sn_Index = pc_Layout->indexOf(opc_Widget);
      pc_Layout->setStretch(sn_Index, 1);
      pc_Layout->setMargin(0);
   } //lint !e429  //no memory leak because of the parent of pc_Layout and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the preview scene

   \return     Pointer to preview scene
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdTopologyScene * C_GiSyBaseWidget::GetPreviewScene(void)
{
   return this->mpc_Scene;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get scene view size for improved item placement

   \return
   Scene view size
*/
//----------------------------------------------------------------------------------------------------------------------
QSize C_GiSyBaseWidget::h_GetSceneViewSize(void)
{
   return QSize(296, 168);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyBaseWidget::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   bool q_CallOrg = true;

   //Handle all enter key cases manually
   if ((opc_KeyEvent->key() == static_cast<sintn>(Qt::Key_Enter)) ||
       (opc_KeyEvent->key() == static_cast<sintn>(Qt::Key_Return)))
   {
      if (((opc_KeyEvent->modifiers().testFlag(Qt::ControlModifier) == true) &&
           (opc_KeyEvent->modifiers().testFlag(Qt::AltModifier) == false)) &&
          (opc_KeyEvent->modifiers().testFlag(Qt::ShiftModifier) == false))
      {
         if (this->mpc_ParentDialog != NULL)
         {
            this->mpc_ParentDialog->accept();
         }
      }
      else
      {
         q_CallOrg = false;
      }
   }
   if (q_CallOrg == true)
   {
      QWidget::keyPressEvent(opc_KeyEvent);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Ok button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyBaseWidget::m_OkClicked(void)
{
   tgl_assert(this->mpc_ParentDialog != NULL);
   if (this->mpc_ParentDialog != NULL)
   {
      this->mpc_ParentDialog->accept();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Cancel button
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSyBaseWidget::m_CancelClicked(void)
{
   tgl_assert(this->mpc_ParentDialog != NULL);
   if (this->mpc_ParentDialog != NULL)
   {
      this->mpc_ParentDialog->reject();
   }
}
