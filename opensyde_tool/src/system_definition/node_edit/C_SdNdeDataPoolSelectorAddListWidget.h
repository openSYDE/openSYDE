//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing already shared datapools (header)

   See cpp file for detailed description

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDATAPOOLSELECTORADDLISTWIDGET_H
#define C_SDNDEDATAPOOLSELECTORADDLISTWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QListWidget>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDataPoolSelectorAddListWidget :
   public QListWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDataPoolSelectorAddListWidget(QWidget * const opc_Parent = NULL);
   virtual ~C_SdNdeDataPoolSelectorAddListWidget(void);

private:
   //Avoid call
   C_SdNdeDataPoolSelectorAddListWidget(const C_SdNdeDataPoolSelectorAddListWidget &);
   C_SdNdeDataPoolSelectorAddListWidget & operator =(const C_SdNdeDataPoolSelectorAddListWidget &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
