//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Label with adaptive size (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_OGELABADAPTIVESIZE_H
#define C_OGELABADAPTIVESIZE_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QLabel>
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_OgeLabAdaptiveSize :
   public QLabel
{
public:
   C_OgeLabAdaptiveSize(QWidget * const opc_Parent = NULL);

   void ResetFont(void);

   virtual void AdjustFontToSize(void);
   void AdjustFontToSpecificSize(const QSize & orc_Size);

   void SetAllowAutomatedAdaptation(const bool oq_Value);

protected:
   virtual void showEvent(QShowEvent * const opc_Event) override;
   virtual void resizeEvent(QResizeEvent * const opc_Event) override;

   bool mq_AllowAutomatedAdaptation;
   QFont mc_LastFont;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
