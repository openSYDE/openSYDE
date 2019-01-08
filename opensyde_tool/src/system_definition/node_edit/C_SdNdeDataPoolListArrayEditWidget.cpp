//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Data set array edit widget

   Data set array edit widget

   \implementation
   project     opensyde
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     15.03.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "C_SdNdeDataPoolListArrayEditWidget.h"
#include "ui_C_SdNdeDataPoolListArrayEditWidget.h"
#include "C_GtGetText.h"
#include "C_PuiSdHandler.h"

/* -- Used Namespaces ------------------------------------------------------ */

using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_core;

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] orc_Parent          Reference to parent
   \param[in]     oru32_NodeIndex     Node index
   \param[in]     oru32_DataPoolIndex Node data pool index
   \param[in]     oru32_ListIndex     Node data pool list index
   \param[in]     oru32_ElementIndex  Node data pool list element index
   \param[in]     ore_ArrayEditType   Enum for node data pool list element variable
   \param[in]     oru32_DataSetIndex  If min or max use 0
                                  Else use data set index

   \created     15.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdNdeDataPoolListArrayEditWidget::C_SdNdeDataPoolListArrayEditWidget(
   stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent, const uint32 & oru32_NodeIndex,
   const uint32 & oru32_DataPoolIndex, const uint32 & oru32_ListIndex, const uint32 & oru32_ElementIndex,
   const C_SdNdeDataPoolUtil::E_ArrayEditType & ore_ArrayEditType, const uint32 & oru32_DataSetIndex) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SdNdeDataPoolListArrayEditWidget),
   mrc_Parent(orc_Parent),
   mu32_NodeIndex(oru32_NodeIndex),
   mu32_DataPoolIndex(oru32_DataPoolIndex),
   mu32_ListIndex(oru32_ListIndex),
   mu32_ElementIndex(oru32_ElementIndex),
   me_ArrayEditType(ore_ArrayEditType),
   mu32_DataSetIndex(oru32_DataSetIndex)
{
   this->mpc_Ui->setupUi(this);
   this->mrc_Parent.SetWidget(this);
   InitStaticNames();
   this->mpc_Ui->pc_TableView->SetElement(oru32_NodeIndex, oru32_DataPoolIndex, oru32_ListIndex, oru32_ElementIndex,
                                          ore_ArrayEditType, oru32_DataSetIndex);

   //Connects
   connect(this->mpc_Ui->pc_BushButtonOk, &stw_opensyde_gui_elements::C_OgePubDialog::clicked, this,
           &C_SdNdeDataPoolListArrayEditWidget::m_OkClicked);
   connect(this->mpc_Ui->pc_BushButtonCancel, &stw_opensyde_gui_elements::C_OgePubDialog::clicked, this,
           &C_SdNdeDataPoolListArrayEditWidget::m_CancelClicked);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   Clean up.

   \created     15.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SdNdeDataPoolListArrayEditWidget::~C_SdNdeDataPoolListArrayEditWidget(void)
{
   delete mpc_Ui;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize all displayed static names

   \created     15.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListArrayEditWidget::InitStaticNames(void) const
{
   const C_OSCNodeDataPool * const pc_DataPool = C_PuiSdHandler::h_GetInstance()->GetOSCDataPool(this->mu32_NodeIndex,
                                                                                                 this->mu32_DataPoolIndex);
   const C_OSCNodeDataPoolListElement * const pc_Element =
      C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(
         this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex, this->mu32_ElementIndex);
   const C_OSCNodeDataPoolDataSet * const pc_DataSet = C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListDataSet(
      this->mu32_NodeIndex, this->mu32_DataPoolIndex,
      this->mu32_ListIndex,
      this->mu32_DataSetIndex);

   //Build title
   if ((pc_DataPool != NULL) && (pc_Element != NULL))
   {
      const QString c_Type = C_PuiSdHandler::h_GetElementTypeName(pc_DataPool->e_Type);
      QString c_EditType;
      switch (this->me_ArrayEditType)
      {
      case C_SdNdeDataPoolUtil::eARRAY_EDIT_MIN:
         c_EditType = C_GtGetText::h_GetText("Minimum");
         break;
      case C_SdNdeDataPoolUtil::eARRAY_EDIT_MAX:
         c_EditType = C_GtGetText::h_GetText("Maximum");
         break;
      case C_SdNdeDataPoolUtil::eARRAY_EDIT_DATA_SET:
         if (pc_DataSet != NULL)
         {
            //Translation: 1: data set name
            c_EditType = QString(C_GtGetText::h_GetText("Dataset Value"));
         }
         break;
      default:
         c_EditType = C_GtGetText::h_GetText("Unknown");
         break;
      }

      //Translation: 1: Data element type, 2: Data element name, 3: Value type
      this->mrc_Parent.SetTitle(QString(C_GtGetText::h_GetText("%1 %2 (%3)")).arg(c_Type).arg(
                                   pc_Element->c_Name.c_str()).arg(c_EditType));
   }

   this->mrc_Parent.SetSubTitle(QString(C_GtGetText::h_GetText("Array Edit")));
}

//-----------------------------------------------------------------------------
/*!
   \brief   GetUndocommand and take ownership

   Internal undo command is reseted

   \return
   NULL No changes
   Else Undocommand accumulating all changes

   \created     16.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QUndoCommand * C_SdNdeDataPoolListArrayEditWidget::TakeUndoCommand(void) const
{
   return this->mpc_Ui->pc_TableView->TakeUndoCommand();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set model view manager

   \param[in,out] opc_Value Model view manager

   \created     17.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListArrayEditWidget::SetModelViewManager(C_SdNdeDataPoolListModelViewManager * const opc_Value)
const
{
   this->mpc_Ui->pc_TableView->SetModelViewManager(opc_Value);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information

   \created     02.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListArrayEditWidget::keyPressEvent(QKeyEvent * const opc_KeyEvent)
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

//-----------------------------------------------------------------------------
/*!
   \brief   On ok clicked

   \created     15.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListArrayEditWidget::m_OkClicked(void)
{
   mrc_Parent.accept();
}

//-----------------------------------------------------------------------------
/*!
   \brief   On cancel clicked

   \created     15.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdNdeDataPoolListArrayEditWidget::m_CancelClicked(void)
{
   mrc_Parent.reject();
}
