//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       List widget for the toolbox (header)

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDTOPOLOGYLISTWIDGET_H
#define C_SDTOPOLOGYLISTWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeListWidgetToolTipBase.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdTopologyListWidget :
   public stw_opensyde_gui_elements::C_OgeListWidgetToolTipBase
{
   Q_OBJECT

public:
   explicit C_SdTopologyListWidget(QWidget * const opc_Parent = NULL);
   virtual ~C_SdTopologyListWidget();

   void SetMaximumHeightAdaption(const bool oq_Active);
   void ApplyDarkMode(const bool oq_Active) const;
   void SetGroupName(const QString & orc_Name);

   static const QString hc_GroupName;

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void startDrag(const Qt::DropActions oc_SupportedActions) override;
   virtual void resizeEvent(QResizeEvent * const opc_Event) override;
   //lint -restore

private:
   //Avoid call
   C_SdTopologyListWidget(const C_SdTopologyListWidget &);
   C_SdTopologyListWidget & operator =(const C_SdTopologyListWidget &);

   bool mq_MinimumSize;
   bool mq_AdaptMaximumHeight;
   QString mc_Name;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
