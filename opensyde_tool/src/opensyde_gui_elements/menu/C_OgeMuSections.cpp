//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Menu with sections that have a title.

   Menu with sections that have a title. Created for path variables menu.

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QWidgetAction>

#include "C_OgeMuSections.hpp"
#include "C_OgeLabGenericNoPaddingNoMargins.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui_elements;

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
C_OgeMuSections::C_OgeMuSections(QWidget * const opc_Parent) :
   QMenu(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Add section to menu as QWidgetaction.

   Similar to QMenu::addSection, but with this the text is not visible on Windows.

   \param[in]       orc_SectionTitle     Title of new section
*/
//----------------------------------------------------------------------------------------------------------------------
void C_OgeMuSections::AddCustomSection(const QString & orc_SectionTitle)
{
   QWidgetAction * const pc_Action = new QWidgetAction(this);
   C_OgeLabGenericNoPaddingNoMargins * const pc_Label = new C_OgeLabGenericNoPaddingNoMargins(this);

   // label style is handled in style sheets: stw--opensyde_gui_elements--C_OgeMuSections QLabel
   pc_Label->setText(orc_SectionTitle);
   pc_Action->setDefaultWidget(pc_Label);
   this->addAction(pc_Action);
} //lint !e429  //no memory leak because of the parent of pc_Label and pc_Action and the Qt memory management
