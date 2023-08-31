//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Button in material design without borders, with icon and handle mouseClickEvents

   \copyright   Copyright 2023 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGEPUBICONEVENTS_H
#define C_OGEPUBICONEVENTS_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"
#include "C_OgePubIconText.hpp"
#include "stwtypes.hpp"
#include "C_Uti.hpp"
#include <QMouseEvent>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgePubIconEvents :
   public C_OgePubIconText
{
   Q_OBJECT

public:
   explicit C_OgePubIconEvents(QWidget * const opc_Parent = NULL);

protected:
   void mousePressEvent(QMouseEvent * const opc_Event) override;

Q_SIGNALS:
   void SigPubIconClicked(bool);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
