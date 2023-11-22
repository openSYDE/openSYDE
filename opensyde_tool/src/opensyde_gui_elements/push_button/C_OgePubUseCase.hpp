//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Button in material design of main view

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

#ifndef C_OGEPUBUSECASE_HPP
#define C_OGEPUBUSECASE_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "precomp_headers.hpp"
#include "C_OgePubStandard.hpp"
#include <QWidget>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace stw
{
namespace opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgePubUseCase :
   public C_OgePubStandard
{
   Q_OBJECT

public:
   explicit C_OgePubUseCase(QWidget * const opc_Parent = NULL);

   void SetDisabledIcon(const QString & orc_Path);

protected:
   void paintEvent(QPaintEvent * const opc_Event) override;

private:
   QString mc_DisabledIconPath;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
