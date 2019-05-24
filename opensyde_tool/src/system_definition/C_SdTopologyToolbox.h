//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for toolbox of system definition

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDTOPOLOGYTOOLBOX_H
#define C_SDTOPOLOGYTOOLBOX_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>
#include "C_SdTopologyListWidget.h"
#include "C_OSCDeviceDefinition.h"
#include "C_OSCDeviceGroup.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SdTopologyToolbox;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdTopologyToolbox :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdTopologyToolbox(QWidget * const opc_Parent = NULL);
   virtual ~C_SdTopologyToolbox();

   // slot
   void SearchChanged(const QString & orc_Text);
   void InitStaticNames(void) const;

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void enterEvent(QEvent * const opc_Event) override;
   virtual void leaveEvent(QEvent * const opc_Event) override;
   //lint -restore

private:
   //Avoid call
   C_SdTopologyToolbox(const C_SdTopologyToolbox &);
   C_SdTopologyToolbox & operator =(const C_SdTopologyToolbox &);

   Ui::C_SdTopologyToolbox * mpc_Ui;
   QVector<C_SdTopologyListWidget *> mc_ListWidgets;

   void m_FillToolboxDynamic(void);
   void m_FillToolboxStatic(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
