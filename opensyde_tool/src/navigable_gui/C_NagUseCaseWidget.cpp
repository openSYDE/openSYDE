//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Base class widget for use case specific widgets

   The base class for all use case widgets, which will be shown with
   C_NagUseCaseViewWidget.

   \implementation
   project     opensyde
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     11.07.2016  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_NagUseCaseWidget.h"
#include "C_OgePopUpDialog.h"
#include "C_PopSaveAsDialogWidget.h"

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

   \created     11.07.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_NagUseCaseWidget::C_NagUseCaseWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   default destructor

   Clean up.

   \created     11.07.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_NagUseCaseWidget::~C_NagUseCaseWidget()
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Function to set the parent of the widget

   The function is virtual to offer specific widgets the possibility to react
   if the parent was changed

   \param[in,out] opc_Parent Optional pointer to parent

   \created     13.10.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_NagUseCaseWidget::SetParentHook(QWidget * const opc_Parent)
{
   this->setParent(opc_Parent);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the vector with the function names

   \created     12.07.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
QVector<C_NagToolBarButtonProperties> C_NagUseCaseWidget::GetVecToolBarFuncNames(void) const
{
   return this->mc_VecUserInputFuncNames;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Function for toolbar

   \param[in]  ou32_FuncNumber   Number of function

   \created     12.07.2016  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_NagUseCaseWidget::UserInputFunc(const uint32 ou32_FuncNumber)
{
   // empty implementation if the derived class doesn't need function
   // it can not be a pure virtual function
   Q_UNUSED(ou32_FuncNumber)
}

//-----------------------------------------------------------------------------
/*!
   \brief   Optional function to save data

   Default implementation: The widget has nothing to save. The concrete
   implementation must override this function if necessary.

   \created     08.03.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_NagUseCaseWidget::Save(void)
{
   // nothing to do here
}

//-----------------------------------------------------------------------------
/*!
   \brief   Optional function to open a concrete destination or information

   The concrete implementation is widget specific

   \param[in] os32_Index            First index
   \param[in] os32_SubIndex         Second index
   \param[in] os32_SubSubIndex      Third index
   \param[in] os32_SubSubSubIndex   Fourth index
   \param[in] os32_Flag             Optional flag for further information

   \created     16.03.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_NagUseCaseWidget::OpenDetail(const sint32 os32_Index, const sint32 os32_SubIndex, const sint32 os32_SubSubIndex,
                                    const sint32 os32_SubSubSubIndex, const stw_types::sint32 os32_Flag)
{
   // empty implementation if the derived class doesn't need function
   // it can not be a pure virtual function
   Q_UNUSED(os32_Index)
   Q_UNUSED(os32_SubIndex)
   Q_UNUSED(os32_SubSubIndex)
   Q_UNUSED(os32_SubSubSubIndex)
   Q_UNUSED(os32_Flag)
}

//-----------------------------------------------------------------------------
/*!
   \brief   Optional function to prepare closing the widget

   Default implementation: The widget has nothing to prepare. The concrete
   implementation must override this function if necessary.

   \return
   true     Preparation successful. Can be closed.
   false    Preparation not finished. Can not be closed.

   \created     22.03.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
bool C_NagUseCaseWidget::PrepareToClose(void)
{
   // nothing to do here
   return true;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Optional function to set the actual mode of concrete handler

   Do not call this function internally. Use the signal SigChangeMode to
   inform the entire application about the change.

   \param[in]     os32_SubMode     Actual sub mode
   \param[in]     ou32_Index       Index for node or bus
   \param[in]     ou32_Flag        Flag for special functionality

   \created     19.06.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_NagUseCaseWidget::SetSubMode(const sint32 os32_SubMode, const uint32 ou32_Index, const uint32 ou32_Flag)
{
   // empty implementation if the derived class doesn't need function
   // it can not be a pure virtual function
   Q_UNUSED(os32_SubMode)
   Q_UNUSED(ou32_Index)
   Q_UNUSED(ou32_Flag)
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle for global key press event

   \param[in,out] opc_Event Event identification and information

   \return
   True  Handled
   False Not handled

   \created     27.07.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
bool C_NagUseCaseWidget::GlobalUserKeyPress(QKeyEvent * const opc_Event)
{
   // nothing to do here
   Q_UNUSED(opc_Event)

   return false;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle generic push button icon press

   \created     02.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_NagUseCaseWidget::OnPushButtonIconPress(void)
{
   // nothing to do here
}

//-----------------------------------------------------------------------------
/*!
   \brief   Handle save as action

   \created     11.01.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_NagUseCaseWidget::SaveAs(void)
{
   QPointer<stw_opensyde_gui_elements::C_OgePopUpDialog> const c_New =
      new stw_opensyde_gui_elements::C_OgePopUpDialog(this, this);
   C_PopSaveAsDialogWidget * const pc_Dialog = new C_PopSaveAsDialogWidget(*c_New);

   // open save-as dialog
   if (c_New->exec() == static_cast<sintn>(QDialog::Accepted))
   {
      Q_EMIT this->SigNewUserSettings();
   }

   if (c_New != NULL)
   {
      pc_Dialog->SaveUserSettings();
      c_New->HideOverlay();
   }
   //lint -e{429}  no memory leak because of the parent of pc_Dialog and the Qt memory management
}
