//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing any file as part of a file based node

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "stwtypes.hpp"
#include "constants.hpp"

#include "C_SyvUpPacListNodeItemFileWidget.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   Set up GUI with all elements.

   \param[in]     ou32_ViewIndex       View index
   \param[in]     ou32_NodeIndex       Node index
   \param[in]     orc_DeviceName       Name of device for comparing with application block
   \param[in]     oq_FileBased         Flag if node is a file based device
   \param[in]     oq_StwFlashloader    Flag if node has not an openSYDE Flashloader but a STW Flashloader
   \param[in,out] opc_Parent           Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvUpPacListNodeItemFileWidget::C_SyvUpPacListNodeItemFileWidget(const uint32_t ou32_ViewIndex,
                                                                   const uint32_t ou32_NodeIndex,
                                                                   const QString & orc_DeviceName,
                                                                   const bool oq_FileBased,
                                                                   const bool oq_StwFlashloader,
                                                                   QWidget * const opc_Parent) :
   C_SyvUpPacListNodeItemWidget(ou32_ViewIndex, ou32_NodeIndex, orc_DeviceName, oq_FileBased,
                                oq_StwFlashloader, opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the type of the item

   \return
   Type of return values, e.g. STW error codes
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SyvUpPacListNodeItemFileWidget::GetType(void) const
{
   return mu32_UPDATE_PACKAGE_NODE_SECTION_TYPE_FILE;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Returns the flag if it is possible to show file information

   File based devices have no additional information

   \retval   true   View file info can show information
   \retval   false  View file info can not show information
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvUpPacListNodeItemFileWidget::IsViewFileInfoPossible(void) const
{
   return false;
}
