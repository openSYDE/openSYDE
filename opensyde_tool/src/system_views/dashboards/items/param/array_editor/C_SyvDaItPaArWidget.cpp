//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data set array edit widget

   Data set array edit widget

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_GtGetText.hpp"
#include "C_GiSvDaParam.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_PuiSvHandler.hpp"
#include "C_SyvDaItPaArWidget.hpp"
#include "ui_C_SyvDaItPaArWidget.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */

using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_core;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] orc_Parent         Reference to parent
   \param[in]     oru32_ElementIndex Element index
   \param[in]     opc_DataWidget     Data widget
   \param[in]     oq_EcuValues       Optional flag if the shown values are ECU values
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaItPaArWidget::C_SyvDaItPaArWidget(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                         const uint32_t & oru32_ElementIndex,
                                         C_PuiSvDbDataElementHandler * const opc_DataWidget, const bool oq_EcuValues) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SyvDaItPaArWidget),
   mrc_Parent(orc_Parent),
   mq_EcuValues(oq_EcuValues),
   mu32_ElementIndex(oru32_ElementIndex),
   mpc_DataWidget(opc_DataWidget)
{
   this->mpc_Ui->setupUi(this);
   this->mrc_Parent.SetWidget(this);
   InitStaticNames();
   this->mpc_Ui->pc_TableView->SetElement(oru32_ElementIndex, opc_DataWidget, oq_EcuValues);

   //Connects
   connect(this->mpc_Ui->pc_BushButtonOk, &stw::opensyde_gui_elements::C_OgePubDialog::clicked, this,
           &C_SyvDaItPaArWidget::m_OkClicked);
   connect(this->mpc_Ui->pc_BushButtonCancel, &stw::opensyde_gui_elements::C_OgePubDialog::clicked, this,
           &C_SyvDaItPaArWidget::m_CancelClicked);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaItPaArWidget::~C_SyvDaItPaArWidget(void)
{
   delete mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaArWidget::InitStaticNames(void)
{
   int32_t s32_DataSetIndex = -1;
   const C_PuiSvDbNodeDataPoolListElementId * pc_Id = NULL;

   const C_GiSvDaParam * const pc_ParamWidget = dynamic_cast<const C_GiSvDaParam * const>(this->mpc_DataWidget);

   if (pc_ParamWidget != NULL)
   {
      const C_PuiSvDbParam * const pc_Param = pc_ParamWidget->GetParamItem();
      if (pc_Param != NULL)
      {
         if (this->mu32_ElementIndex < pc_Param->c_DataPoolElementsConfig.size())
         {
            const C_PuiSvDbNodeDataElementConfig & rc_Config =
               pc_Param->c_DataPoolElementsConfig[this->mu32_ElementIndex];
            pc_Id = &rc_Config.c_ElementId;
         }
         if (this->mu32_ElementIndex < pc_Param->c_DataSetSelectionIndices.size())
         {
            s32_DataSetIndex = pc_Param->c_DataSetSelectionIndices[this->mu32_ElementIndex];
         }
      }
   }
   if (pc_Id != NULL)
   {
      const C_OscNodeDataPool * const pc_DataPool = C_PuiSdHandler::h_GetInstance()->GetOscDataPool(
         pc_Id->u32_NodeIndex, pc_Id->u32_DataPoolIndex);
      const C_OscNodeDataPoolListElement * const pc_Element =
         C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(
            pc_Id->u32_NodeIndex, pc_Id->u32_DataPoolIndex, pc_Id->u32_ListIndex, pc_Id->u32_ElementIndex);

      //Build title
      if ((pc_DataPool != NULL) && (pc_Element != NULL))
      {
         QString c_EditType;
         const QString c_Type = C_PuiSdHandler::h_GetElementTypeName(pc_DataPool->e_Type);
         if (this->mq_EcuValues == true)
         {
            c_EditType = C_GtGetText::h_GetText("Device Value");
         }
         else
         {
            c_EditType = C_GtGetText::h_GetText("Set Value");
         }

         //Translation: 1: Data element type, 2: Data element name, 3: Value type
         this->mrc_Parent.SetTitle(static_cast<QString>(C_GtGetText::h_GetText("%1 %2 (%3)")).arg(c_Type).arg(
                                      pc_Element->c_Name.c_str()).arg(c_EditType));
      }
   }

   if ((this->mq_EcuValues == true) || (s32_DataSetIndex >= 0L))
   {
      this->mrc_Parent.SetSubTitle(static_cast<QString>(C_GtGetText::h_GetText("Array Editor (Read Only)")));
   }
   else
   {
      this->mrc_Parent.SetSubTitle(static_cast<QString>(C_GtGetText::h_GetText("Array Editor")));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaArWidget::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   bool q_CallOrg = true;

   //Handle all enter key cases manually
   if ((opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_Enter)) ||
       (opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_Return)))
   {
      if (((opc_KeyEvent->modifiers().testFlag(Qt::ControlModifier) == true) &&
           (opc_KeyEvent->modifiers().testFlag(Qt::AltModifier) == false)) &&
          (opc_KeyEvent->modifiers().testFlag(Qt::ShiftModifier) == false))
      {
         this->mrc_Parent.accept();
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
/*! \brief   On ok clicked
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaArWidget::m_OkClicked(void)
{
   mrc_Parent.accept();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   On cancel clicked
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaItPaArWidget::m_CancelClicked(void)
{
   mrc_Parent.reject();
}
