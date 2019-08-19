//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Menu with sections that have a title.
   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEMUSECTIONS_H
#define C_OGEMUSECTIONS_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QMenu>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeMuSections :
   public QMenu
{
   Q_OBJECT

public:
   explicit C_OgeMuSections(QWidget * const opc_Parent = NULL);

   void AddCustomSection(const QString & orc_SectionTitle);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
