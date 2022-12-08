//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Base class widget for use case specific widgets

   The base class for all use case widgets, which will be shown with
   C_NagUseCaseViewWidget.

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "constants.hpp"
#include "C_NagUseCaseWidget.hpp"
#include "C_OgePopUpDialog.hpp"
#include "C_PopSaveAsDialogWidget.hpp"
#include "C_GiSyColorSelectWidget.hpp"

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

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_NagUseCaseWidget::C_NagUseCaseWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
C_NagUseCaseWidget::~C_NagUseCaseWidget(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Function to set the parent of the widget

   The function is virtual to offer specific widgets the possibility to react
   if the parent was changed

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagUseCaseWidget::SetParentHook(QWidget * const opc_Parent)
{
   this->setParent(opc_Parent);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the vector with the function names

   \return
   Vector with function names
*/
//----------------------------------------------------------------------------------------------------------------------
QVector<C_NagToolBarButtonProperties> C_NagUseCaseWidget::GetVecToolBarFuncNames(void) const
{
   return this->mc_VecUserInputFuncNames;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Function for toolbar

   \param[in]  ou32_FuncNumber   Number of function
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{9175}  //intentionally no functionality in default implementation
void C_NagUseCaseWidget::UserInputFunc(const uint32_t ou32_FuncNumber)
{
   // empty implementation if the derived class doesn't need function
   // it can not be a pure virtual function
   Q_UNUSED(ou32_FuncNumber)
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Optional function to save data

   Default implementation: The widget has nothing to save. The concrete
   implementation must override this function if necessary.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{9175}  //intentionally no functionality in default implementation
void C_NagUseCaseWidget::Save(void)
{
   // nothing to do here
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Optional function to open a concrete destination or information

   The concrete implementation is widget specific

   \param[in]  os32_Index           First index
   \param[in]  os32_SubIndex        Second index
   \param[in]  os32_SubSubIndex     Third index
   \param[in]  os32_SubSubSubIndex  Fourth index
   \param[in]  os32_Flag            Optional flag for further information
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{9175}  //intentionally no functionality in default implementation
void C_NagUseCaseWidget::OpenDetail(const int32_t os32_Index, const int32_t os32_SubIndex,
                                    const int32_t os32_SubSubIndex, const int32_t os32_SubSubSubIndex,
                                    const int32_t os32_Flag)
{
   // empty implementation if the derived class doesn't need function
   // it can not be a pure virtual function
   Q_UNUSED(os32_Index)
   Q_UNUSED(os32_SubIndex)
   Q_UNUSED(os32_SubSubIndex)
   Q_UNUSED(os32_SubSubSubIndex)
   Q_UNUSED(os32_Flag)
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Optional function to prepare closing the widget

   Default implementation: The widget has nothing to prepare. The concrete
   implementation must override this function if necessary.

   \return
   true     Preparation successful. Can be closed.
   false    Preparation not finished. Can not be closed.
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_NagUseCaseWidget::PrepareToClose(void)
{
   // nothing to do here
   return true;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Optional function to set the actual mode of concrete handler

   Do not call this function internally. Use the signal SigChangeMode to
   inform the entire application about the change.

   \param[in]  os32_SubMode   Actual sub mode
   \param[in]  ou32_Index     Index for node or bus
   \param[in]  ou32_Flag      Flag for special functionality
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{9175}  //intentionally no functionality in default implementation
void C_NagUseCaseWidget::SetSubMode(const int32_t os32_SubMode, const uint32_t ou32_Index, const uint32_t ou32_Flag)
{
   // empty implementation if the derived class doesn't need function
   // it can not be a pure virtual function
   Q_UNUSED(os32_SubMode)
   Q_UNUSED(ou32_Index)
   Q_UNUSED(ou32_Flag)
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle for global key press event

   \param[in,out]  opc_Event  Event identification and information

   \return
   True  Handled
   False Not handled
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_NagUseCaseWidget::GlobalUserKeyPress(QKeyEvent * const opc_Event)
{
   // nothing to do here
   Q_UNUSED(opc_Event)

   return false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle generic push button icon press
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{9175}  //intentionally no functionality in default implementation
void C_NagUseCaseWidget::OnPushButtonIconPress(void)
{
   // nothing to do here
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle save as action
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagUseCaseWidget::SaveAs(void)
{
   const QPointer<stw::opensyde_gui_elements::C_OgePopUpDialog> c_New =
      new stw::opensyde_gui_elements::C_OgePopUpDialog(this, this);
   C_PopSaveAsDialogWidget * const pc_Dialog = new C_PopSaveAsDialogWidget(*c_New);

   // open save-as dialog
   if (c_New->exec() == static_cast<int32_t>(QDialog::Accepted))
   {
      Q_EMIT this->SigNewUserSettings();
   }

   if (c_New != NULL)
   {
      pc_Dialog->SaveUserSettings();
      c_New->HideOverlay();
   }
} //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle open color picker action
*/
//----------------------------------------------------------------------------------------------------------------------
void C_NagUseCaseWidget::OpenColorPicker(void)
{
   const QPointer<stw::opensyde_gui_elements::C_OgePopUpDialog> c_Popup =
      new stw::opensyde_gui_elements::C_OgePopUpDialog(this, this);
   C_GiSyColorSelectWidget * const pc_ColorWidget = new C_GiSyColorSelectWidget(*c_Popup, mc_STYLE_GUIDE_COLOR_7);

   //Resize
   c_Popup->SetSize(QSize(412, 620));

   // open color picker dialog
   if (c_Popup->exec() == static_cast<int32_t>(QDialog::Accepted))
   {
      pc_ColorWidget->ChooseSelectedColor();
   }

   if (c_Popup != NULL)
   {
      c_Popup->HideOverlay();
   }
} //lint !e429  //no memory leak because of the parent of pc_ColorWidget and the Qt memory management
