//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree delegate for HALC use case configuration.

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_SdNdeHalcConfigTreeDelegate.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeHalcConfigTreeDelegate::C_SdNdeHalcConfigTreeDelegate(QObject * const opc_Parent) :
   C_TblDelegate(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create spin box

   \param[in,out] opc_Parent Parent

   \return
   HALC spin box interaction element
   TODO return correctly styled spin box
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeWiSpinBoxGroup * C_SdNdeHalcConfigTreeDelegate::m_CreateSpinBox(QWidget * const opc_Parent) const
{
   return new C_OgeWiSpinBoxGroup(opc_Parent);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create line edit

   \param[in,out] opc_Parent Parent

   \return
   HALC line edit interaction element
   TODO return correctly styled spin box
*/
//----------------------------------------------------------------------------------------------------------------------
C_TblEditLineEditBase * C_SdNdeHalcConfigTreeDelegate::m_CreateLineEdit(QWidget * const opc_Parent) const
{
   return new C_TblEditLineEditBase(opc_Parent);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create combo box

   \param[in,out] opc_Parent Parent

   \return
   HALC combo box interaction element
   TODO return correctly styled spin box
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeCbxTableBase * C_SdNdeHalcConfigTreeDelegate::m_CreateComboBox(QWidget * const opc_Parent) const
{
   return new C_OgeCbxTableBase(opc_Parent);
}
