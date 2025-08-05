//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree delegate for HALC use case configuration.

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_SdNdeHalcConfigTreeDelegate.hpp"

#include "C_OgeCbxMultiSelectTableHalc.hpp"
#include "C_OgeCbxTableHalc.hpp"
#include "C_OgeSpxTableHalc.hpp"
#include "C_OgeLeTableHalc.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeHalcConfigTreeDelegate::C_SdNdeHalcConfigTreeDelegate(QObject * const opc_Parent) :
   C_TblDelegate(opc_Parent)
{
   this->mq_InitialSelection = false;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create spin box

   \param[in,out]  opc_Parent    Parent

   \return
   HALC spin box interaction element
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeWiSpinBoxGroup * C_SdNdeHalcConfigTreeDelegate::m_CreateSpinBox(QWidget * const opc_Parent) const
{
   return new C_OgeSpxTableHalc(opc_Parent);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create line edit

   \param[in,out]  opc_Parent    Parent

   \return
   HALC line edit interaction element
*/
//----------------------------------------------------------------------------------------------------------------------
C_TblEditLineEditBase * C_SdNdeHalcConfigTreeDelegate::m_CreateLineEdit(QWidget * const opc_Parent) const
{
   return new C_OgeLeTableHalc(opc_Parent);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create combo box

   \param[in,out]  opc_Parent    Parent

   \return
   HALC combo box interaction element
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeCbxTableBase * C_SdNdeHalcConfigTreeDelegate::m_CreateComboBox(QWidget * const opc_Parent) const
{
   return new C_OgeCbxTableHalc(opc_Parent);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Create multi select combo box

   \param[in,out]  opc_Parent    Parent

   \return
   HALC multi select combo box interaction element
*/
//----------------------------------------------------------------------------------------------------------------------
C_OgeCbxMultiSelect * C_SdNdeHalcConfigTreeDelegate::m_CreateMultiSelectComboBox(QWidget * const opc_Parent) const
{
   return new C_OgeCbxMultiSelectTableHalc(opc_Parent);
}
